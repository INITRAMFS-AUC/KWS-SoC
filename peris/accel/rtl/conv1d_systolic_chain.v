module conv1d_systolic_chain (
    input wire clk,
    input wire rst_n,
    input wire first,
    input wire [7:0] kernel_size,      // Runtime configurable: kernel size
    input wire [31:0] num_input_samples, // Runtime configurable: number of input samples
    input wire valid_in,               // External: "I have input data"
    input wire ready_in,               // External: "I can accept output"
    input wire signed [7:0] audio_stream_in,
    
    // The 7 learned weights permanently pinned to the 7 PEs
    input wire signed [7:0] weight_0,
    input wire signed [7:0] weight_1,
    input wire signed [7:0] weight_2,
    input wire signed [7:0] weight_3,
    input wire signed [7:0] weight_4,
    input wire signed [7:0] weight_5,
    input wire signed [7:0] weight_6,
    
    output wire signed [21:0] final_y_out,
    output reg valid_out,              // Module: "I have valid output"
    output reg ready_out               // Module: "I can accept input"
);

    wire signed [21:0] y_wire [0:7];
    wire signed [7:0]  x_wire [0:6];

    // Internal control registers (latched when first is asserted)
    reg [7:0] kernel_size_reg;          // Latched kernel size
    reg [31:0] num_input_samples_reg;   // Latched number of input samples
    reg [31:0] num_valid_outputs_reg;   // Calculated: num_input_samples - kernel_size + 1
    reg [31:0] sample_count;            // Counts samples received via handshake

    // Counter-based delay: counts cycles after first is asserted (keeps counting)
    reg [31:0] cycle_count;      // Counts from 0 upward indefinitely
    
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            cycle_count <= 32'd0;
            valid_out <= 1'b0;
            ready_out <= 1'b0;
            kernel_size_reg <= 8'd0;
            num_input_samples_reg <= 32'd0;
            num_valid_outputs_reg <= 32'd0;
            sample_count <= 32'd0;
        end else begin            
            // Latch control registers when first is asserted
            if (first) begin
                kernel_size_reg <= kernel_size;
                num_input_samples_reg <= num_input_samples;
                num_valid_outputs_reg <= num_input_samples - kernel_size + 1;  // 1D conv formula
                cycle_count <= 32'd0;  // Reset counter when first pulses
                sample_count <= 32'd0; // Reset sample counter
                ready_out <= 1'b1;     // Ready to accept input after config
            end else begin
                // Accept input via handshake when valid_in is asserted
                if (valid_in && ready_out && sample_count < num_input_samples_reg) begin
                    sample_count <= sample_count + 1'b1;
                end
                
                // Keep counting cycles for valid_out timing
                cycle_count <= cycle_count + 1'b1;
                
                // De-assert ready_out after we've received all samples
                if (sample_count >= num_input_samples_reg) begin
                    ready_out <= 1'b0;
                end
            end
            
            // Assert valid_out after counting 2*kernel_size - 1 cycles
            if (cycle_count == 2 * kernel_size_reg - 1) begin
                valid_out <= 1'b1;
            end
            
            // De-assert valid_out after all valid outputs have been produced
            if (cycle_count == 2 * kernel_size_reg - 1 + num_valid_outputs_reg) begin
                valid_out <= 1'b0;
            end
        end
    end

    assign x_wire[0] = audio_stream_in;
    assign y_wire[0] = 22'b0;

    // Physically stamp out 7 PEs and solder their wires together left-to-right
    systolic_pe PE_0 (
        .clk(clk),
        .rst_n(rst_n),
        .x_in(x_wire[0]),
        .y_in(y_wire[0]),
        .weight(weight_0),
        .x_out(x_wire[1]),
        .y_out(y_wire[1])
    );
    
    systolic_pe PE_1 (
        .clk(clk),
        .rst_n(rst_n),
        .x_in(x_wire[1]),
        .y_in(y_wire[1]),
        .weight(weight_1),
        .x_out(x_wire[2]),
        .y_out(y_wire[2])
    );
    
    systolic_pe PE_2 (
        .clk(clk),
        .rst_n(rst_n),
        .x_in(x_wire[2]),
        .y_in(y_wire[2]),
        .weight(weight_2),
        .x_out(x_wire[3]),
        .y_out(y_wire[3])
    );
    
    systolic_pe PE_3 (
        .clk(clk),
        .rst_n(rst_n),
        .x_in(x_wire[3]),
        .y_in(y_wire[3]),
        .weight(weight_3),
        .x_out(x_wire[4]),
        .y_out(y_wire[4])
    );
    
    systolic_pe PE_4 (
        .clk(clk),
        .rst_n(rst_n),
        .x_in(x_wire[4]),
        .y_in(y_wire[4]),
        .weight(weight_4),
        .x_out(x_wire[5]),
        .y_out(y_wire[5])
    );
    
    systolic_pe PE_5 (
        .clk(clk),
        .rst_n(rst_n),
        .x_in(x_wire[5]),
        .y_in(y_wire[5]),
        .weight(weight_5),
        .x_out(x_wire[6]),
        .y_out(y_wire[6])
    );
    
    systolic_pe PE_6 (
        .clk(clk),
        .rst_n(rst_n),
        .x_in(x_wire[6]),
        .y_in(y_wire[6]),
        .weight(weight_6),
        .x_out(),
        .y_out(y_wire[7])
    );

    // Output multiplexer: select output based on kernel size
    assign final_y_out = (kernel_size_reg == 8'd1) ? y_wire[1] :
                         (kernel_size_reg == 8'd2) ? y_wire[2] :
                         (kernel_size_reg == 8'd3) ? y_wire[3] :
                         (kernel_size_reg == 8'd4) ? y_wire[4] :
                         (kernel_size_reg == 8'd5) ? y_wire[5] :
                         (kernel_size_reg == 8'd6) ? y_wire[6] :
                         y_wire[7];  // Default to 7

endmodule