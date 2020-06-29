#!/usr/bin/env bash

echo "===================="
echo "Author:  Corteggiani Nassim"
echo "Date:    10/2019"
echo "Version: 0.1"
echo "Name:    HardSnap"
echo "===================="

mkdir build

vivado -nojournal -nolog -mode tcl -source ./vvsyn.tcl -tclargs $(pwd) $(pwd)/build/

FILE=$(pwd)/build/top.runs/impl_1/top_wrapper.sysdef
if test -f "$FILE"; then
    ./flash.sh $(pwd)/build
else
    echo "Synthesis failed..."
fi
