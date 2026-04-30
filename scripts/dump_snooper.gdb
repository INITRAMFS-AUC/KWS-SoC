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
  printf "memop legend: 04=LBU 05=SW 0a=SH 0b=SB 10=NONE\n"
  printf "%-3s %-10s %-6s %-10s %-2s %-10s %-10s %-3s %-3s %-10s %-10s %-3s %-2s %-2s\n", \
         "i", "cycle", "bridge", "dport_addr", "wr", "hwdata", "m_wdata", \
         "rs2", "rd", "xm_res", "mw_res", "memop", "aphq", "stl"

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
    set $dport_a  = *(unsigned int *)($e + 0x1C)

    set $bridge_haddr = $addrctrl & 0xffff
    set $bridge_wr    = ($addrctrl >> 16) & 1
    set $bridge_trans = ($addrctrl >> 17) & 3
    set $dport_wr     = ($addrctrl >> 19) & 1
    set $dport_trans  = ($addrctrl >> 20) & 3
    set $aph_req      = ($addrctrl >> 22) & 1
    set $xm_memop     = ($addrctrl >> 23) & 0x1f
    set $m_bus_stall  = ($addrctrl >> 28) & 1
    set $rs2 = $idx & 0x1f
    set $rd  = ($idx >> 8) & 0x1f

    printf "%2d  %-10u 0x%04x 0x%08x  %d  0x%08x 0x%08x  %2u  %2u  0x%08x 0x%08x  %02x   %d  %d", \
           $i, $cyc, $bridge_haddr, $dport_a, $bridge_wr, $hwdata, $mwdata, \
           $rs2, $rd, $xmres, $mwres, $xm_memop, $aph_req, $m_bus_stall
    set $note_count = 0
    if ($bridge_haddr != ($dport_a & 0xffff))
      printf "   <- ROUTING: bridge=%04x vs dport=%08x", $bridge_haddr, $dport_a
      set $note_count = $note_count + 1
    end
    if ($bridge_wr == 1) && (($hwdata & 0xff) != ($mwdata & 0xff))
      if $note_count == 0
        printf "   <-"
      else
        printf " ;"
      end
      printf " HWDATA: bridge=%02x vs m_wdata=%02x", \
             ($hwdata & 0xff), ($mwdata & 0xff)
      set $note_count = $note_count + 1
    end
    if ($bridge_wr == 1) && ($xm_memop != 5) && ($xm_memop != 10) && ($xm_memop != 11)
      if $note_count == 0
        printf "   <-"
      else
        printf " ;"
      end
      printf " EARLY-APHASE: write fired but xm_memop=0x%02x is not a store", $xm_memop
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
