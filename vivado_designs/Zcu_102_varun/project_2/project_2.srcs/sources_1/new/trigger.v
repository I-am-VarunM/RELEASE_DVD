`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 07/17/2023 12:27:31 PM
// Design Name: 
// Module Name: trigger
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module trigger (
  input wire en,
  input wire clk,
  output reg [127:0] plaintext,
  output reg [127:0] key
);

  reg toggle;
  
  always @(posedge clk) begin
    if (en) begin
      if (toggle)
        begin
          key <= 128'haaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa;
          plaintext <= 128'h55555555555555555555555555555555;
        end
      else
        begin
          key <= 128'h55555555555555555555555555555555;
          plaintext <= 128'haaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa;
        end
      
      toggle <= ~toggle;
    end
    else begin
      key <= 128'b0;
      plaintext <= 128'b0;
      toggle<=1'b0;
    end
  end
  
endmodule


