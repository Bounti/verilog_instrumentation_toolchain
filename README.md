# Project Description

# Running Examples

The commands below enable users to instrument two IPs (i.e., sha256 and AES CTR) in order to inject a scan-chain in the design.
Then the python script generates project files with synthesis scripts, simulation scripts, and drivers.


```
mkdir output && \          
./hardsnap.py --mode=instrument --input_dir=../fpga_ip/sha256/hdl --output_dir=./output/rtl/sha256 && \
./hardsnap.py --mode=instrument --input_dir=../fpga_ip/aes_ctr/hdl --output_dir=./output/rtl/aes_ctr && \
./hardsnap.py --mode=make --output_dir=./output
```

Synthesis and flashing the FPGA (Zedboard)
```
cd output/tcl
./build.sh
```
