# NNoM Conv1D Accelerator Integration Plan

**Status**: Phase 3 - Quantization Mapping Helpers Added
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

## Phase 3 Quantization Mapping

### Exact NNoM Symbols

The target Conv2D layers use per-axis quantization tables from
`test/model/mel_compact_4blk_ch36/mel_compact_4blk_ch36_weights.h`:

| Symbol | Values |
|--------|--------|
| `conv2d_1_output_shift` / `CONV2D_1_OUTPUT_RSHIFT` | `{9, 10, 11, 10, 9, 9, 9, 10, 9, 9, 9, 9, 9, 10, 9, 10, 9, 9, 10, 9, 10, 10, 9, 10, 9, 10, 9, 9, 9, 10, 10, 9, 10, 9, 10, 9}` |
| `conv2d_1_bias_shift` / `CONV2D_1_BIAS_LSHIFT` | `{5, 5, 6, 4, 5, 4, 4, 5, 5, 4, 4, 5, 3, 2, 4, 5, 4, 2, 4, 4, 3, 3, 4, 4, 5, 6, 5, 4, 3, 2, 5, 4, 6, 5, 6, 5}` |
| `conv2d_2_output_shift` / `CONV2D_2_OUTPUT_RSHIFT` | `{8, 8, 8, 7, 9, 7, 7, 7, 8, 8, 9, 9, 7, 10, 9, 8, 9, 7, 8, 8, 8, 8, 8, 8, 8, 9, 9, 7, 8, 9, 8, 8, 9, 8, 8, 7}` |
| `conv2d_2_bias_shift` / `CONV2D_2_BIAS_LSHIFT` | `{4, 5, 2, 4, 0, 4, 3, 2, 4, 3, 5, 0, 3, 5, 3, 3, 4, 0, 5, 4, 3, 5, 4, 6, 4, 4, 2, 3, 5, 5, 6, 3, 5, 5, 4, 4}` |
| `conv2d_3_output_shift` / `CONV2D_3_OUTPUT_RSHIFT` | `{6, 6, 7, 6, 6, 6, 6, 7, 6, 7, 7, 6, 7, 6, 6, 7, 6, 6, 6, 6, 7, 7, 6, 7, 6, 6, 6, 6, 7, 7, 6, 6, 6, 6, 7, 6}` |
| `conv2d_3_bias_shift` / `CONV2D_3_BIAS_LSHIFT` | `{2, 2, 4, 3, 2, 2, 2, 5, 3, 4, 4, 0, 5, 2, 2, 3, 1, 2, 2, 3, 1, 4, 3, 3, 3, 2, 4, 3, 1, 4, 1, 1, 3, 0, 1, 4}` |

### NNoM Software Formula

For the HWC q7 Conv2D path, NNoM calls
`local_convolve_HWC_q7_nonsquare()` from
`third_party/nnom/src/backends/nnom_local.c`.

The effective formula for each output channel is:

```c
acc = ((int32_t)bias[oc] << bias_shift[oc]) + NNOM_ROUND(output_shift[oc]);
acc += sum(input * weight);
out = saturate_int8(acc >> output_shift[oc]);
```

With the default build, `NNOM_ROUND(s)` is `1 << (s - 1)`.
If `NNOM_TRUNCATE` is defined, the rounding term is zero. The right shift is
an arithmetic signed shift on the host/compiler target. ReLU is not fused into
these Conv2D layers in the generated model: `act_relu()` layers follow
`conv2d_1`, `conv2d_2`, and `conv2d_3` separately.

### Accelerator Mapping Decision

- Bias must be pre-shifted before writing accelerator bias memory:
  `accel_bias[oc] = nnom_bias[oc] << bias_shift[oc]`.
- To match default NNoM rounding with the current RTL, add the rounding term to
  the prepared bias: `+ (1 << (output_shift[oc] - 1))`.
- Per-output-channel `output_shift[oc]` maps to the APB-loaded shift buffer:
  write `CONV1D_QUANT_INDEX = oc`, then `CONV1D_QUANT_SHIFT_DATA = output_shift[oc]`.
- The scalar `CONV1D_QUANT[4:0]` path is preserved. A scalar QUANT write seeds
  every per-channel shift slot with the same value, so old firmware still uses
  scalar behavior unless it loads per-channel entries.
- `relu_en` should remain `0` for bit-exact replacement of the Conv2D output,
  because NNoM applies ReLU in the following layer.
- Negative output shifts are not present in the target Conv2D_1/2/3 tables and
  are not supported by the accelerator QUANT field.

### APB Quantization Registers

| Offset | Name | Purpose |
|--------|------|---------|
| `0x28` | `CONV1D_QUANT` | Scalar fallback: `[4:0]=out_shift`, `[5]=relu_en`; scalar writes also seed all per-OC shift slots |
| `0x2C` | `CONV1D_QUANT_INDEX` | Select output-channel index for the next per-channel shift write |
| `0x30` | `CONV1D_QUANT_SHIFT_DATA` | Write `[4:0]` shift into the selected output-channel slot |

### Remaining Limitation

Per-output-channel shifts are now representable in RTL, APB, and firmware
helpers. Live acceleration is still disabled because Phase 4 must configure
the APB registers for a real NNoM layer and connect the accelerator memory path
before replacing software outputs.

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

### Phase 2: Weight Handling ✅ DONE
- [x] Implement online transpose: [1,3,in_ch,out_ch] → [out_ch,3,in_ch]
- [x] Test correctness with standalone weight packing unit test
- [x] Keep live inference unchanged

### Phase 3: Quantization Integration ✅ DONE
- [x] Map NNoM output_shift to APB QUANT register format
- [x] Map NNoM bias_shift to accelerator bias memory preparation
- [x] Add reference quantization helper for unit testing
- [x] Add per-output-channel output shift support in RTL/APB/firmware helpers

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
