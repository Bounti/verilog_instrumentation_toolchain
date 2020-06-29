setws ./build/top.sdk/

createhw -name hw1 -hwspec ./build/top.sdk/top_wrapper.hdf

cd ./build/top.sdk

createbsp -name bsp1 -hwproject hw1 -proc ps7_cortexa9_0 -os standalone

setlib -bsp bsp1 -lib xilffs
updatemss -mss bsp1/system.mss
regenbsp -bsp bsp1

createapp -name app -hwproject hw1 -bsp bsp1 -proc ps7_cortexa9_0 -os standalone -lang C -app {Hello World}

createapp -name fsbl -hwproject hw1 -bsp bsp1 -proc ps7_cortexa9_0 -os standalone -lang C -app {Zynq FSBL}

set sources [glob -directory ../../../c/ *.c]
foreach fname $sources {
    file copy -force -- $fname ./app/src/
}

set sources [glob -directory ../../../c/ *.h]
foreach fname $sources {
    file copy -force -- $fname ./app/src/
}


sdk projects -build

exec bootgen -arch zynq -image ../../app.bif -w -o BOOT.bin

exec program_flash -f ./BOOT.bin -fsbl ./fsbl/Debug/fsbl.elf -flash_type qspi-x1-single -blank_check -verify -cable type xilinx_tcf url TCP:localhost:3121

