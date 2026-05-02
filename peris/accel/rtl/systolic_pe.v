module systolic_pe (
    input wire clk,                    // The heartbeat of the chip    
    input wire rst_n,                  // Active-low reset    
    input wire signed [7:0] x_in,      // 8-bit incoming audio data
    input wire signed [21:0] y_in,     // 21-bit incoming partial sum
    input wire signed [7:0] weight,    // 8-bit stationary learned weight
    
    output reg signed [7:0] x_out,     // The audio passed to the next PE
    output reg signed [21:0] y_out     // The new partial sum passed to next PE
);

    // This is the physical "Delay Register" (the extra trapdoor for x)
    reg signed [7:0] x_delay;

    // open the register trapdoors and move the electricity forward.
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            // Reset all registers to 0
            x_delay <= 8'b0;
            x_out   <= 8'b0;
            y_out   <= 22'b0;
        end else begin
            // The Staggered Speed: Delay 'x' by one extra cycle
            x_delay <= x_in;      // Catch incoming audio
            x_out   <= x_delay;   // Pass the PREVIOUS audio to the next PE
            
            // 2. The Math (Multiplier and Adder)
            // Electricity flows through the logic gates to multiply and add, 
            // instantly latching into the y_out register.
            y_out <= y_in + (x_in * weight);
        end
    end
endmodule