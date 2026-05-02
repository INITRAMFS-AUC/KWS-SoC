# NNoM Conv1D Accelerator Integration Plan

**Status**: Design Phase - Ready for Implementation  
**Branch**: conv1d-accelerator-integration  
**Date**: May 2, 2026

---

## Executive Summary

The Conv1D accelerator (4-lane packed-int8 MAC, K=3) can accelerate 3 layers in the mel_compact_4blk_ch36 KWS model:
- CONV2D_1: [1, 250, 16] → [1, 250, 36], K=3, SAME padding
- CONV2D_2: [1, 250, 36] → [1, 250, 36], K=3, SAME padding  
- CONV2D_3: [1, 250, 36] → [1, 250, 36], K=3, SAME padding

All layers use int8 quantization compatible with the accelerator.

---

## Model Analysis

### Candidate Conv1D Layers

| Layer | Input Shape | Kernel | In_Ch | Out_Ch | Compatible |
|-------|-------------|--------|-------|--------|------------|
| CONV2D_0 | [1, 8000, 1] | {1,65} | 1 | 16 | ❌ K=65 |
| CONV2D_1 | [1, 250, 16] | {1,3} | 16 | 36 | ✅ Yes |
| CONV2D_2 | [1, 250, 36] | {1,3} | 36 | 36 | ✅ Yes |
| CONV2D_3 | [1, 250, 36] | {1,3} | 36 | 36 | ✅ Yes |

### Quantization Compatibility

✅ All target layers support:
- int8 input (Q7 format)
- int8 weights (per-axis quantized)
- int32 bias
- int8 output (saturated)
- Configurable output_shift
- Optional ReLU activation

### Tensor Layout

**NNoM format**: HWC [height, width, channels]
- Input: [1, length, in_ch]
- Output: [1, length, out_ch]
- Weights: [1, 3, in_ch, out_ch]

**Accelerator format**: Length-major, packed int8
- Input: [length, in_ch]
- Output: [length, out_ch]  
- Weights: [out_ch, 3, in_ch] (requires transpose)

---

## Integration Architecture

### Hook Point: Layer Callback

NNoM supports post-layer callbacks:

```
model_run()
  for each layer:
    layer_run(layer)              // Software computes
    layer_callback(layer)          // New hook
```

### Strategy: Option A (Transparent Callback)

1. Software layer executes first (normal NNoM)
2. Callback attempts acceleration  
3. If successful, overwrites output with hardware result
4. Fallback: software result available as backup

**Advantages**:
- Zero NNoM core changes
- Graceful fallback

**Disadvantages**:
- Double-compute if successful (acceptable for Phase 1)

### Integration Flow

```c
// In kws_bare_main.c, before model_run():
#ifdef USE_CONV1D_ACCEL
conv1d_accel_stats_reset();
model_set_callback(model, conv1d_accel_layer_callback);
#endif

model_run(model);

#ifdef USE_CONV1D_ACCEL
printf("CONV1D_ACCEL: %d/%d layers accelerated\n",
       g_conv1d_accel_stats.conv1d_success,
       g_conv1d_accel_stats.conv1d_attempts);
#endif
```

---

## Files

### Created (New)
- `test/conv1d/include/conv1d_accel_nnom_bridge.h`
- `test/conv1d/c/conv1d_accel_nnom_bridge.c`
- `docs/conv1d/NNOM_INTEGRATION_PLAN.md` (this file)

### Referenced (Existing)
- `test/conv1d/include/conv1d_accel_regs.h` (accelerator register API)
- `test/common/kws_bare_main.c` (firmware entry point, line 495)
- `test/model/mel_compact_4blk_ch36/mel_compact_4blk_ch36_weights.h` (model weights)
- `third_party/nnom/src/core/nnom.c` (model_run, model_set_callback)

### No Modifications (Safety)
- NNoM core unchanged
- Model weights unchanged
- Main branch untouched

---

## Implementation Phases

### Phase 1: Design & Layer Detection ✅ DONE
- [x] Identify Conv1D layers (3 found)
- [x] Verify accelerator compatibility (all match)
- [x] Design callback architecture
- [x] Create bridge skeleton
- [x] Basic layer type/size checks

### Phase 2: Weight Handling (NEXT)
- [ ] Implement online transpose: [1,3,in_ch,out_ch] → [out_ch,3,in_ch]
- [ ] Test correctness vs golden model
- [ ] Measure transpose overhead

### Phase 3: Quantization Integration
- [ ] Map NNoM output_shift to APB QUANT register
- [ ] Handle per-axis vs per-tensor shifts
- [ ] Bit-exact output verification

### Phase 4: Full Acceleration
- [ ] Configure APB registers with layer parameters
- [ ] Start accelerator, poll done
- [ ] Verify output correctness
- [ ] Measure performance gain

### Phase 5: Integration Testing
- [ ] Link into kws_bare_main.c with USE_CONV1D_ACCEL
- [ ] Run end-to-end inference
- [ ] Verify output matches original
- [ ] Measure CYCLES_INFER improvement

### Phase 6: Profiling & Documentation
- [ ] Measure per-layer gains
- [ ] Document integration guide
- [ ] Plan future optimizations

---

## Key Decisions

| Decision | Rationale |
|----------|-----------|
| Option A (Callback) | Non-invasive, no NNoM fork needed |
| Online transpose | Simpler than toolchain modification |
| Per-layer stats | Enables per-layer profiling |
| Graceful fallback | Safety: software path always available |
| Phase-based implementation | Incremental validation at each step |

---

## Risk Mitigations

| Risk | Mitigation |
|------|-----------|
| Weight layout error | Compare transpose vs golden outputs |
| Quantization mismatch | Bit-exact verification test |
| Performance regression | Skip if overhead > gain |
| Compatibility | Callback-based (no NNoM core changes) |
| Disk space | Only adds ~100 lines of code per file |

---

## Success Criteria

✅ **Must Have**:
- Compiles with `-DUSE_CONV1D_ACCEL`
- No exceptions during inference
- Output matches original (cycle-exact)
- Bridge stats populated correctly

✅ **Nice to Have**:
- CYCLES_INFER improves ≥5%
- Per-layer measurement available
- Zero degradation without flag

---

## Next Steps

1. Review integration plan (this document)
2. Understand bridge header/implementation structure
3. Implement Phase 2 (weight transpose)
4. Test Phase 2 offline (unit test)
5. Implement Phase 3 (quantization mapping)
6. Integrate into kws_bare_main.c
7. Run end-to-end test with CYCLES measurement

---

## References

- NNoM model_run: `third_party/nnom/src/core/nnom.c:1070`
- Layer callback: `third_party/nnom/inc/nnom.h` (model_set_callback)
- KWS firmware: `test/common/kws_bare_main.c:495` (model_run call)
- Model weights: `test/model/mel_compact_4blk_ch36/mel_compact_4blk_ch36_weights.h`
- Accelerator regs: `test/conv1d/include/conv1d_accel_regs.h`
- Bridge API: `test/conv1d/include/conv1d_accel_nnom_bridge.h`

