#
# Author  : Corteggiani Nassim
# Company : EURECOM
# DATA    : 2019

proc usage {} {
	puts "usage: vivado -mode batch -source <script> -tclargs <rootdir> <builddir>"
	puts "  <rootdir>:  absolute path of usb2jtag root directory"
	puts "  <builddir>: absolute path of build directory"
	exit -1
}

if { $argc == 2 } {
	set rootdir [lindex $argv 0]
	set builddir [lindex $argv 1]
} else {
	usage
}

file delete -force $builddir
file mkdir $builddir

cd $builddir

###################
# Create SHA256 
###################
create_project -part xc7z020clg484-1 -force sha256 sha256_ip
set sources [glob -directory ../../rtl/sha256/ *.v]
foreach f $sources {
        add_files $builddir$f
}
import_files -force -norecurse
ipx::package_project -root_dir sha256_ip -vendor www.eurecom.fr -library ip -force sha256
close_project

###################
# Create Fast IP Scanner 
###################
create_project -part xc7z020clg484-1 -force scanner scanner_ip
set sources [glob -directory ../../../../hardsnap_ip/scan_ip/hdl/ *.*v]
foreach f $sources {
        add_files $builddir$f
}
import_files -force -norecurse
ipx::package_project -root_dir scanner_ip -vendor www.eurecom.fr -library ip -force scanner
close_project

set top top
create_project -part xc7z020clg484-1 -force $top .
set_property board_part em.avnet.com:zed:part0:1.4 [current_project]
set_property ip_repo_paths { ./scanner_ip ./sha256_ip } [current_fileset]
update_ip_catalog
create_bd_design "$top"

create_bd_cell -type ip -vlnv www.eurecom.fr:ip:scanner:1.0 fast_ip_scanner_0
create_bd_cell -type ip -vlnv www.eurecom.fr:ip:sha256:1.0 sha256_0
create_bd_cell -type ip -vlnv xilinx.com:ip:processing_system7:5.5 processing_system7_0
create_bd_cell -type ip -vlnv xilinx.com:ip:fifo_generator:13.2 fifo_generator_t0
create_bd_cell -type ip -vlnv xilinx.com:ip:fifo_generator:13.2 fifo_generator_t1

apply_bd_automation -rule xilinx.com:bd_rule:processing_system7 -config {make_external "FIXED_IO, DDR" apply_board_preset "1" Master "Disable" Slave "Disable" }  [get_bd_cells processing_system7_0]

set_property -dict [list CONFIG.PCW_QSPI_GRP_SINGLE_SS_ENABLE {1} CONFIG.PCW_UART0_PERIPHERAL_ENABLE {1}] [get_bd_cells processing_system7_0]
set_property -dict [list CONFIG.PCW_QSPI_GRP_SINGLE_SS_ENABLE {1} CONFIG.PCW_UART1_PERIPHERAL_ENABLE {1}] [get_bd_cells processing_system7_0]
set_property -dict [list CONFIG.PCW_USE_S_AXI_HP0 {1} CONFIG.PCW_QSPI_GRP_SINGLE_SS_ENABLE {1}] [get_bd_cells processing_system7_0]
set_property -dict [list CONFIG.PCW_USE_S_AXI_HP0 {1}] [get_bd_cells processing_system7_0]

set_property -dict [list CONFIG.Input_Data_Width {32} CONFIG.Output_Data_Width {32} CONFIG.Reset_Pin {false} CONFIG.Reset_Type {Asynchronous_Reset} CONFIG.Use_Dout_Reset {false} CONFIG.Almost_Full_Flag {true}] [get_bd_cells fifo_generator_t0]
set_property -dict [list CONFIG.Input_Data_Width {32} CONFIG.Output_Data_Width {32} CONFIG.Reset_Pin {false} CONFIG.Reset_Type {Asynchronous_Reset} CONFIG.Use_Dout_Reset {false} CONFIG.Almost_Full_Flag {true}] [get_bd_cells fifo_generator_t1]

connect_bd_net [get_bd_pins sha256_0/scan_output] [get_bd_pins fast_ip_scanner_0/scan_output]
connect_bd_net [get_bd_pins fast_ip_scanner_0/scan_input] [get_bd_pins sha256_0/scan_input]

connect_bd_net [get_bd_pins fast_ip_scanner_0/scan_ck_enable] [get_bd_pins sha256_0/scan_ck_enable]
connect_bd_net [get_bd_pins fast_ip_scanner_0/scan_enable] [get_bd_pins sha256_0/scan_enable]

connect_bd_net [get_bd_pins fifo_generator_t0/almost_full] [get_bd_pins fast_ip_scanner_0/almost_full_t0]
connect_bd_net [get_bd_pins fifo_generator_t0/din] [get_bd_pins fast_ip_scanner_0/data_in_t0]
connect_bd_net [get_bd_pins fifo_generator_t0/wr_en] [get_bd_pins fast_ip_scanner_0/wr_en_t0]
connect_bd_net [get_bd_pins fifo_generator_t0/empty] [get_bd_pins fast_ip_scanner_0/empty_t0]
connect_bd_net [get_bd_pins fifo_generator_t0/rd_en] [get_bd_pins fast_ip_scanner_0/rd_en_t0]
connect_bd_net [get_bd_pins fifo_generator_t0/dout] [get_bd_pins fast_ip_scanner_0/data_out_t0]

connect_bd_net [get_bd_pins fifo_generator_t1/almost_full] [get_bd_pins fast_ip_scanner_0/almost_full_t1]
connect_bd_net [get_bd_pins fifo_generator_t1/din] [get_bd_pins fast_ip_scanner_0/data_in_t1]
connect_bd_net [get_bd_pins fifo_generator_t1/wr_en] [get_bd_pins fast_ip_scanner_0/wr_en_t1]
connect_bd_net [get_bd_pins fifo_generator_t1/empty] [get_bd_pins fast_ip_scanner_0/empty_t1]
connect_bd_net [get_bd_pins fifo_generator_t1/rd_en] [get_bd_pins fast_ip_scanner_0/rd_en_t1]
connect_bd_net [get_bd_pins fifo_generator_t1/dout] [get_bd_pins fast_ip_scanner_0/data_out_t1]

apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {Auto} Clk_slave {Auto} Clk_xbar {Auto} Master {/processing_system7_0/M_AXI_GP0} Slave {/fast_ip_scanner_0/s00_axi} intc_ip {New AXI Interconnect} master_apm {0}}  [get_bd_intf_pins fast_ip_scanner_0/s00_axi]
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {Auto} Clk_slave {Auto} Clk_xbar {Auto} Master {/processing_system7_0/M_AXI_GP0} Slave {/sha256_0/s00_axi} intc_ip {New AXI Interconnect} master_apm {0}}  [get_bd_intf_pins sha256_0/s00_axi]
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {Auto} Clk_slave {Auto} Clk_xbar {Auto} Master {/fast_ip_scanner_0/m00_axi} Slave {/processing_system7_0/S_AXI_HP0} intc_ip {Auto} master_apm {0}}  [get_bd_intf_pins processing_system7_0/S_AXI_HP0]

apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config {Clk "/processing_system7_0/FCLK_CLK0 (100 MHz)" }  [get_bd_pins fifo_generator_t0/clk]
apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config {Clk "/processing_system7_0/FCLK_CLK0 (100 MHz)" }  [get_bd_pins fifo_generator_t1/clk]

# Synthesis flow
validate_bd_design
set files [get_files *$top.bd]
generate_target all $files
add_files -norecurse -force [make_wrapper -files $files -top]
save_bd_design
set run [get_runs synth*]
set_property STEPS.SYNTH_DESIGN.ARGS.FLATTEN_HIERARCHY none $run
launch_runs $run
wait_on_run $run
open_run $run

# IOs
#array set ios {
#	"led[0]"   { "T22"  "LVCMOS33" }
#	"led[1]"   { "T21"  "LVCMOS33" }
#	"led[2]"   { "U22"  "LVCMOS33" }
#	"led[3]"   { "U21"  "LVCMOS33" }
#	"led[7]"   { "U14"  "LVCMOS33" }
#	"led[6]"   { "U19"  "LVCMOS33" }
#	"led[5]"   { "W22"  "LVCMOS33" }
#	"led[4]"   { "V22"  "LVCMOS33" }
#}

#foreach io [ array names ios ] {
#	set pin [ lindex $ios($io) 0 ]
#	set std [ lindex $ios($io) 1 ]
#	set_property package_pin $pin [get_ports $io]
#	set_property iostandard $std [get_ports [list $io]]
#}

# Timing constraints
set clock [get_clocks]
#set_false_path -from $clock -to [get_ports {led[*]}]

# Implementation
save_constraints
set run [get_runs impl*]
reset_run $run
set_property STEPS.WRITE_BITSTREAM.ARGS.BIN_FILE true $run
launch_runs -to_step write_bitstream $run
wait_on_run $run

# Messages
set rundir ${builddir}/$top.runs/$run
puts ""
puts "\[VIVADO\]: done"
puts "  bitstream in $rundir/${top}_wrapper.bit"
puts "  resource utilization report in $rundir/${top}_wrapper_utilization_placed.rpt"
puts "  timing report in $rundir/${top}_wrapper_timing_summary_routed.rpt"


file mkdir $builddir/top.sdk
file copy -force $rundir/top_wrapper.sysdef $builddir/top.sdk/top_wrapper.hdf

