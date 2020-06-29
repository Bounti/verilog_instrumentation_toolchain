`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company:
// Engineer:
//
// Create Date: 05/27/2019 02:40:47 PM
// Design Name:
// Module Name: tb_axi_lite_slave
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

module top(
    //clock and reset_n signals
    input wire clk_i,
    input wire rst_ni,

    //scan signals
    input  wire scan_input,
    output wire scan_output,
    input  wire scan_enable,
    input  wire scan_ck_en,

    //select target
    input wire [1:0] target_selector,

    //Write Address channel (AW)
    input reg [31:0] write_addr,  //Master write address
    input reg [2:0] write_prot,	  //type of write(leave at 0)
    input reg write_addr_valid,	  //master indicating address is valid
    output wire write_addr_ready, //slave ready to receive address

    //Write Data Channel (W)
    input reg [31:0] write_data,  //Master write data
    input reg [3:0] write_strb,	  //Master byte-wise write strobe
    input reg write_data_valid,	  //Master indicating write data is valid
    output wire write_data_ready, //slave ready to receive data

    //Write Response Channel (WR)
    input reg write_resp_ready,   //Master ready to receive write response
    output wire [1:0] write_resp, //slave write response
    output wire write_resp_valid, //slave response valid

    //Read Address channel (AR)
    input reg [31:0] read_addr,   //Master read address
    input reg [2:0] read_prot,	  //type of read(leave at 0)
    input reg read_addr_valid,	  //Master indicating address is valid
    output wire read_addr_ready,  //slave ready to receive address

    //Read Data Channel (R)
    input reg read_data_ready,	  //Master indicating ready to receive data
    output wire [31:0] read_data, //slave read data
    output wire [1:0] read_resp,  //slave read response
    output wire read_data_valid,  //slave indicating data in channel is valid

    output wire irq_ack,
    output wire irq_rq
  );

  reg [31:0] write_addr_t0;
  reg [2:0]  write_prot_t0;
  reg  write_addr_valid_t0;
  reg [31:0] write_data_t0;
  reg  [3:0] write_strb_t0;
  reg  write_data_valid_t0;
  reg  write_data_ready_t0;
  reg  write_resp_ready_t0;
  reg  [31:0] read_addr_t0;
  reg   [2:0] read_prot_t0;
  reg   read_addr_valid_t0;
  reg   read_data_ready_t0;	
  
  reg  [1:0] write_resp_t0; // output
  reg  write_resp_valid_t0; // output  
  reg   read_addr_ready_t0; // output 
  reg  write_addr_ready_t0; // output
  reg  [31:0] read_data_t0; // output
  reg   [1:0] read_resp_t0; // output 
  reg   read_data_valid_t0; // output

  reg [31:0] write_addr_t1;
  reg [2:0]  write_prot_t1;
  reg  write_addr_valid_t1;
  reg [31:0] write_data_t1;
  reg  [3:0] write_strb_t1;
  reg  write_data_valid_t1;
  reg  write_data_ready_t1;
  reg  write_resp_ready_t1;
  reg  [31:0] read_addr_t1;
  reg   [2:0] read_prot_t1;
  reg   read_addr_valid_t1;
  reg   read_data_ready_t1;	

  reg  [1:0] write_resp_t1; // output
  reg  write_resp_valid_t1; // output  
  reg   read_addr_ready_t1; // output 
  reg  write_addr_ready_t1; // output
  reg  [31:0] read_data_t1; // output
  reg   [1:0] read_resp_t1; // output 
  reg   read_data_valid_t1; // output

  reg [31:0] write_addr_t2;
  reg [2:0]  write_prot_t2;
  reg  write_addr_valid_t2;
  reg [31:0] write_data_t2;
  reg  [3:0] write_strb_t2;
  reg  write_data_valid_t2;
  reg  write_data_ready_t2;
  reg  write_resp_ready_t2;
  reg  [31:0] read_addr_t2;
  reg   [2:0] read_prot_t2;
  reg   read_addr_valid_t2;
  reg   read_data_ready_t2;	

  reg  [1:0] write_resp_t2; // output
  reg  write_resp_valid_t2; // output  
  reg   read_addr_ready_t2; // output 
  reg  write_addr_ready_t2; // output
  reg  [31:0] read_data_t2; // output
  reg   [1:0] read_resp_t2; // output 
  reg   read_data_valid_t2; // output

  reg  [1:0] write_resp_reg; // output
  reg  write_resp_valid_reg; // output  
  reg   read_addr_ready_reg; // output 
  reg  write_addr_ready_reg; // output
  reg  [31:0] read_data_reg; // output
  reg   [1:0] read_resp_reg; // output 
  reg   read_data_valid_reg; // output

  wire t0_interrupt;
  wire t1_interrupt;

  wire scan_output_t1;
  wire scan_input_t1;

  wire scan_output_t0;
  wire scan_input_t0;

  wire scan_output_pic;
  wire scan_input_pic;

  always @( target_selector )
  begin
    case ( target_selector )

      2'b00: begin
          write_addr_t0       = write_addr;
          write_prot_t0       = write_prot;
          write_addr_valid_t0 = write_addr_valid;
          write_data_t0       = write_data;
          write_strb_t0       = write_strb;
          write_data_valid_t0 = write_data_valid;
          write_data_ready_t0 = write_data_ready;
          write_resp_ready_t0 = write_resp_ready;
          read_addr_t0        = read_addr;
          read_prot_t0        = read_prot;
          read_addr_valid_t0  = read_addr_valid;
          read_data_ready_t0  = read_data_ready;	

          write_resp_reg       = write_resp_t0      ; // output
          write_resp_valid_reg = write_resp_valid_t0; // output  
          read_addr_ready_reg  = read_addr_ready_t0 ; // output 
          write_addr_ready_reg = write_addr_ready_t0; // output
          read_data_reg        = read_data_t0       ; // output
          read_resp_reg        = read_resp_t0       ; // output 
          read_data_valid_reg  = read_data_valid_t0 ; // output      
        end

      2'b01: begin
          write_addr_t1       = write_addr;
          write_prot_t1       = write_prot;
          write_addr_valid_t1 = write_addr_valid;
          write_data_t1       = write_data;
          write_strb_t1       = write_strb;
          write_data_valid_t1 = write_data_valid;
          write_data_ready_t1 = write_data_ready;
          write_resp_ready_t1 = write_resp_ready;
          read_addr_t1        = read_addr;
          read_prot_t1        = read_prot;
          read_addr_valid_t1  = read_addr_valid;
          read_data_ready_t1  = read_data_ready;	
          
          write_resp_reg       = write_resp_t1      ; // output
          write_resp_valid_reg = write_resp_valid_t1; // output  
          read_addr_ready_reg  = read_addr_ready_t1 ; // output 
          write_addr_ready_reg = write_addr_ready_t1; // output
          read_data_reg        = read_data_t1       ; // output
          read_resp_reg        = read_resp_t1       ; // output 
          read_data_valid_reg  = read_data_valid_t1 ; // output      
      end
     
      2'b10: begin
      /*
          write_addr_pic       = write_addr;
          write_prot_pic       = write_prot;
          write_addr_valid_pic = write_addr_valid;
          write_data_pic       = write_data;
          write_strb_pic       = write_strb;
          write_data_valid_pic = write_data_valid;
          write_data_ready_pic = write_data_ready;
          write_resp_ready_pic = write_resp_ready;
          read_addr_pic        = read_addr;
          read_prot_pic        = read_prot;
          read_addr_valid_pic  = read_addr_valid;
          read_data_ready_pic  = read_data_ready;	
          
          write_resp_reg       = write_resp_pic      ; // output
          write_resp_valid_reg = write_resp_valid_pic; // output  
          read_addr_ready_reg  = read_addr_ready_pic ; // output 
          write_addr_ready_reg = write_addr_ready_pic; // output
          read_data_reg        = read_data_pic       ; // output
          read_resp_reg        = read_resp_pic       ; // output 
          read_data_valid_reg  = read_data_valid_pic ; // output 
      */
      end
      2'b11: begin
      end

    endcase 
    end

    assign write_resp       = write_resp_reg      ; // output
    assign write_resp_valid = write_resp_valid_reg; // output  
    assign read_addr_ready  = read_addr_ready_reg ; // output 
    assign write_addr_ready = write_addr_ready_reg; // output
    assign read_data        = read_data_reg       ; // output
    assign read_resp        = read_resp_reg       ; // output 
    assign read_data_valid  = read_data_valid_reg ; // output      


    aes_ctr # (
        .C_S00_AXI_DATA_WIDTH(32),
        .C_S00_AXI_ADDR_WIDTH(32)
    ) aes_ctr_inst (

        .scan_input(scan_input),
        .scan_output(scan_output_t0),
        .scan_enable(scan_enable),
        .scan_ck_enable(scan_ck_en),

        .s00_axi_aclk(clk_i),
        .s00_axi_aresetn(rst_ni),

        .s00_axi_awaddr(write_addr_t1),
        .s00_axi_awprot(write_prot_t1),
        .s00_axi_awvalid(write_addr_valid_t1),
        .s00_axi_awready(write_addr_ready_t1),

        .s00_axi_wdata(write_data_t1),
        .s00_axi_wstrb(write_strb_t1),
        .s00_axi_wvalid(write_data_valid_t1),
        .s00_axi_wready(write_data_ready_t1),

        .s00_axi_bresp(write_resp_t1),
        .s00_axi_bvalid(write_resp_valid_t1),
        .s00_axi_bready(write_resp_ready_t1),

        .s00_axi_araddr(read_addr_t1),
        .s00_axi_arprot(read_prot_t1),
        .s00_axi_arvalid(read_addr_valid_t1),
        .s00_axi_arready(read_addr_ready_t1),

        .s00_axi_rdata(read_data_t1),
        .s00_axi_rresp(read_resp_t1),
        .s00_axi_rvalid(read_data_valid_t1),
        .s00_axi_rready(read_data_ready_t1),

        .interrupt(t1_interrupt)
    );

    //Instantiation of LED IP
    sha256 # (
        .C_S00_AXI_DATA_WIDTH(32),
        .C_S00_AXI_ADDR_WIDTH(32)
    ) sha256_inst (
        .s00_axi_aclk(clk_i),
        .s00_axi_aresetn(rst_ni),

        .scan_input(scan_output_t0),
        .scan_output(scan_output_t1),
        .scan_enable(scan_enable),
        .scan_ck_enable(scan_ck_en),

        .s00_axi_awaddr(write_addr_t0),
        .s00_axi_awprot(write_prot_t0),
        .s00_axi_awvalid(write_addr_valid_t0),
        .s00_axi_awready(write_addr_ready_t0),

        .s00_axi_wdata(write_data_t0),
        .s00_axi_wstrb(write_strb_t0),
        .s00_axi_wvalid(write_data_valid_t0),
        .s00_axi_wready(write_data_ready_t0),

        .s00_axi_bresp(write_resp_t0),
        .s00_axi_bvalid(write_resp_valid_t0),
        .s00_axi_bready(write_resp_ready_t0),

        .s00_axi_araddr(read_addr_t0),
        .s00_axi_arprot(read_prot_t0),
        .s00_axi_arvalid(read_addr_valid_t0),
        .s00_axi_arready(read_addr_ready_t0),

        .s00_axi_rdata(read_data_t0),
        .s00_axi_rresp(read_resp_t0),
        .s00_axi_rvalid(read_data_valid_t0),
        .s00_axi_rready(read_data_ready_t0),

        .interrupt(t0_interrupt)
      );


/*
	pic # ( 
		.C_S00_AXI_DATA_WIDTH(32),
		.C_S00_AXI_ADDR_WIDTH(32)
	) pic_inst (

    .scan_input(scan_output_t1),
    .scan_output(scan_output_pic),
    .scan_enable(scan_enable),
    .scan_ck_en(scan_ck_en),

    .interrupt0(t0_interrupt),
    .interrupt1(t1_interrupt),
    .interrupt2(0),
    .interrupt3(0),
    .interrupt4(0),
    .interrupt5(0),
    .interrupt6(0),
    .interrupt7(0),
    .irq_ack(irq_ack),
    .irq_rq(irq_rq),

   .s00_axi_aclk   (clk_i),
   .s00_axi_aresetn(rst_ni),
   .s00_axi_awaddr (write_addr_pic),
   .s00_axi_awprot (write_prot_pic),
   .s00_axi_awvalid(write_addr_valid_pic),
   .s00_axi_awready(write_addr_ready_pic),
   .s00_axi_wdata  (write_data_pic),
   .s00_axi_wstrb  (write_strb_pic),
   .s00_axi_wvalid (write_data_valid_pic),
   .s00_axi_wready (write_data_ready_pic),
   .s00_axi_bresp  (write_resp_pic),
   .s00_axi_bvalid (write_resp_valid_pic),
   .s00_axi_bready (write_resp_ready_pic),
   .s00_axi_araddr (read_addr_pic),
   .s00_axi_arprot (read_prot_pic),
   .s00_axi_arvalid(read_addr_valid_pic),
   .s00_axi_arready(read_addr_ready_pic),
   .s00_axi_rdata  (read_data_pic),
   .s00_axi_rresp  (read_resp_pic),
   .s00_axi_rvalid (read_data_valid_pic),
   .s00_axi_rready (read_data_ready_pic)
	);
*/

endmodule
