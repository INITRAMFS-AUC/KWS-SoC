  module our #(
     parameter WIDTH /*verilator public_flat_rd*/ = 32
  ) ();
     reg [WIDTH-1:0] readme   /*verilator public_flat_rd*/;
     reg [WIDTH-1:0] writeme  /*verilator public_flat_rw*/;
     initial $finish;
  endmodule
