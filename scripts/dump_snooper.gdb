# GDB helper for the bus_snooper at 0x4000_C000.
#
# Use after the snooper_test firmware ebreaks:
#   (gdb) source scripts/dump_snooper.gdb
#   (gdb) dump_snooper
#
# Decodes all 16 ring entries with side-by-side bridge / CPU-tap comparison
# so a corrupted bridge_hwdata vs. m_wdata divergence is obvious.

define dump_snooper
  set $base  = 0x4000C000
  set $stat  = *(unsigned int *)($base + 0x204)
  set $count = $stat & 0x1f
  set $head  = ($stat >> 8) & 0xf
  set $cycle = *(unsigned int *)($base + 0x208)
  printf "snooper: count=%u head=%u live_cycle=%u\n", $count, $head, $cycle
  printf "%-3s %-10s %-6s %-2s %-10s %-10s %-3s %-3s %-10s %-10s\n", \
         "i", "cycle", "addr", "wr", "hwdata", "m_wdata", "rs2", "rd", "xm_res", "mw_res"

  set $i = 0
  while $i < 16
    set $e        = $base + ($i * 32)
    set $cyc      = *(unsigned int *)($e + 0x00)
    set $addrctrl = *(unsigned int *)($e + 0x04)
    set $hwdata   = *(unsigned int *)($e + 0x08)
    set $mwdata   = *(unsigned int *)($e + 0x0C)
    set $idx      = *(unsigned int *)($e + 0x10)
    set $xmres    = *(unsigned int *)($e + 0x14)
    set $mwres    = *(unsigned int *)($e + 0x18)

    set $haddr  = $addrctrl & 0xffff
    set $hwrite = ($addrctrl >> 16) & 1
    set $rs2    = $idx & 0x1f
    set $rd     = ($idx >> 8) & 0x1f

    printf "%2d  %-10u 0x%04x %d  0x%08x 0x%08x  %2u  %2u  0x%08x 0x%08x", \
           $i, $cyc, $haddr, $hwrite, $hwdata, $mwdata, $rs2, $rd, $xmres, $mwres
    if ($hwrite == 1) && (($hwdata & 0xff) != ($mwdata & 0xff))
      printf "   <- HWDATA != M_WDATA (low byte: 0x%02x vs 0x%02x)", \
             ($hwdata & 0xff), ($mwdata & 0xff)
    end
    printf "\n"
    set $i = $i + 1
  end
end
document dump_snooper
Decode and pretty-print the bus_snooper ring buffer (16 entries) at
0x4000_C000. Flags entries where bridge_hwdata's low byte diverges from
the CPU's m_wdata low byte — that's the d-port → APB-bridge corruption
the snooper was built to catch.
end
