#!/usr/bin/env bash

sudo -S apt install antlr4

antlr4 -Dlanguage=Python3 ./antlr/Verilog2001.g4

sudo -S pip3 install antlr4-python3-runtime networkx numpy matplotlib tqdm

cp ./antlr/listener.py ./antlr/Verilog2001Listener.py
