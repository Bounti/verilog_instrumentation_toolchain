`timescale 1ns / 1ps

//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 07/10/2019 03:18:41 PM
// Design Name: 
// Module Name: tb_overall_scan_system
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


import top_axi_vip_0_0_pkg::*;
import top_axi_vip_1_0_pkg::*;
import top_axi_vip_2_0_pkg::*;
import axi_vip_pkg::*;

module testbench();

localparam SCANIP_START = 32'H44A0_0000;

localparam REG_SNP1_ADDR  = 32'H44A0_0000;
localparam REG_SNP2_ADDR  = 32'H44A0_0004;
localparam REG_LENGTH     = 32'H44A0_0008;
localparam REG_START_ADDR = 32'H44A0_000C;
localparam REG_STATUS     = 32'H44A0_0010;
localparam SHA_START = 32'H44B0_0000;


reg aclk = 0;
reg aresetn = 0;

top_wrapper DUT
  (.aclk(aclk),
  .aresetn(aresetn));

always #1ns aclk = ~aclk;

// Declare agent
top_axi_vip_0_0_slv_mem_t slv_mem_agent;
top_axi_vip_1_0_mst_t master_agent;
top_axi_vip_2_0_mst_t master_agent_sha256;

xil_axi_prot_t  prot = 0;
xil_axi_resp_t  resp;

initial begin
    
    //Create an agent
    slv_mem_agent = new("slave vip agent",DUT.top_i.axi_vip_0.inst.IF);
    master_agent = new("master vip agent",DUT.top_i.axi_vip_1.inst.IF);
    master_agent_sha256 = new("master vip agent",DUT.top_i.axi_vip_2.inst.IF);

    slv_mem_agent.mem_model.set_memory_fill_policy(XIL_AXI_MEMORY_FILL_FIXED);

    // set tag for agents for easy debug
    slv_mem_agent.set_agent_tag("Slave VIP");
    master_agent.set_agent_tag("Master VIP");
    master_agent_sha256.set_agent_tag("Master VIP");

    // set print out verbosity level.
    slv_mem_agent.set_verbosity(400);
    master_agent.set_verbosity(400);
    master_agent_sha256.set_verbosity(400);

    //Start the agent
    slv_mem_agent.start_slave();
    master_agent.start_master();
    master_agent_sha256.start_master();

    aresetn = 0;
    #175ns
    aresetn = 1;

    slv_mem_agent.mem_model.set_default_memory_value(32'hAAAAAAAF);
    //backdoor_mem_write_from_file("/usr/bin/ls", 32'H0000_0000);

    init_sha;
    hash;
    wait(DUT.top_i.t0.inst.sha256_v1_0_S00_AXI_inst.core.digest_valid);

    // use the vip axi master to configure the scan ip
    #2ns
    master_agent.AXI4LITE_WRITE_BURST(REG_LENGTH, prot, 32'D1039, resp);
    #2ns
    master_agent.AXI4LITE_WRITE_BURST(REG_SNP1_ADDR, prot, 32'H0000_0000,resp);
    #2ns
    master_agent.AXI4LITE_WRITE_BURST(REG_SNP2_ADDR, prot, 32'H0000_1000, resp);
    #2ns
    master_agent.AXI4LITE_WRITE_BURST(REG_START_ADDR, prot, 32'D1, resp);
    #2ns
    master_agent.AXI4LITE_WRITE_BURST(REG_START_ADDR, prot, 32'D0, resp);

    wait(DUT.top_i.scanner_ip.inst.done);

    #2ns
    master_agent.AXI4LITE_WRITE_BURST(REG_SNP1_ADDR, prot, 32'H0000_1000,resp);
    #2ns
    master_agent.AXI4LITE_WRITE_BURST(REG_SNP2_ADDR, prot, 32'H0000_0000, resp);
    #2ns
    master_agent.AXI4LITE_WRITE_BURST(REG_START_ADDR, prot, 32'D1, resp);
    #2ns
    master_agent.AXI4LITE_WRITE_BURST(REG_START_ADDR, prot, 32'D0, resp);

    wait(DUT.top_i.scanner_ip.inst.done);

    /*
    top_axi_vip_0_0_passthrough.AXI4LITE_WRITE_BURST(SCANIP_START+32'D0, prot, 32'H00000000, resp);
    #2ns
    top_axi_vip_0_0_passthrough.AXI4LITE_WRITE_BURST(SCANIP_START+32'D4, prot, 32'H00001000, resp);
    #2ns
    top_axi_vip_0_0_passthrough.AXI4LITE_WRITE_BURST(SCANIP_START+32'D8, prot, 32'D128, resp);
    #2ns
    top_axi_vip_0_0_passthrough.AXI4LITE_WRITE_BURST(SCANIP_START+32'D12, prot, 32'D1, resp);
    #2ns
    top_axi_vip_0_0_passthrough.AXI4LITE_WRITE_BURST(SCANIP_START+32'D12, prot, 32'D0, resp);
    */

end

task backdoor_mem_write_from_file(input string fname, input bit[31:0] adr);
    integer fd;

    bit [32-1:0] write_data;
    integer 	  offset;

    fd = $fopen(fname, "rb");
    if (fd == 0) begin
        $display("Error can't open %s", fname);
    end else begin
        $display("open %s", fname);
    end

   offset = 0;
    while (!$feof(fd)) begin
        $fread(write_data, fd, offset, 4);
        slv_mem_agent.mem_model.backdoor_memory_write(adr+offset, write_data, 4'b1111);
        offset += 4;	
    end

    $fclose(fd);
endtask

task init_sha;
begin
    master_agent_sha256.AXI4LITE_WRITE_BURST(SHA_START+32'D20, prot, 32'h61626380, resp);
    #2ns
    master_agent_sha256.AXI4LITE_WRITE_BURST(SHA_START+32'D24, prot, 32'h00000000, resp);
    #2ns
    master_agent_sha256.AXI4LITE_WRITE_BURST(SHA_START+32'D28, prot, 32'h00000000, resp);
    #2ns
    master_agent_sha256.AXI4LITE_WRITE_BURST(SHA_START+32'D32, prot, 32'h00000000, resp);
    #2ns
    master_agent_sha256.AXI4LITE_WRITE_BURST(SHA_START+32'D36, prot, 32'h00000000, resp);
    #2ns
    master_agent_sha256.AXI4LITE_WRITE_BURST(SHA_START+32'D40, prot, 32'h00000000, resp);
    #2ns
    master_agent_sha256.AXI4LITE_WRITE_BURST(SHA_START+32'D44, prot, 32'h00000000, resp);
    #2ns
    master_agent_sha256.AXI4LITE_WRITE_BURST(SHA_START+32'D48, prot, 32'h00000000, resp);
    #2ns
    master_agent_sha256.AXI4LITE_WRITE_BURST(SHA_START+32'D52, prot, 32'h00000000, resp);
    #2ns
    master_agent_sha256.AXI4LITE_WRITE_BURST(SHA_START+32'D56, prot, 32'h00000000, resp);
    #2ns
    master_agent_sha256.AXI4LITE_WRITE_BURST(SHA_START+32'D60, prot, 32'h00000000, resp);
    #2ns
    master_agent_sha256.AXI4LITE_WRITE_BURST(SHA_START+32'D64, prot, 32'h00000000, resp);
    #2ns
    master_agent_sha256.AXI4LITE_WRITE_BURST(SHA_START+32'D68, prot, 32'h00000000, resp);
    #2ns
    master_agent_sha256.AXI4LITE_WRITE_BURST(SHA_START+32'D72, prot, 32'h00000000, resp);
    #2ns
    master_agent_sha256.AXI4LITE_WRITE_BURST(SHA_START+32'D76, prot, 32'h00000000, resp);
    #2ns
    master_agent_sha256.AXI4LITE_WRITE_BURST(SHA_START+32'D80, prot, 32'h00000018, resp);
end
endtask

task hash;
begin
    master_agent_sha256.AXI4LITE_WRITE_BURST(SHA_START+32'D4, prot, 32'h00000005, resp);
    #2ns
    master_agent_sha256.AXI4LITE_WRITE_BURST(SHA_START+32'D4, prot, 32'h00000004, resp);
end
endtask

endmodule
 
