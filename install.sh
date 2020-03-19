#!/usr/bin/env bash

sudo -S apt install antlr4

antlr4 -Dlanguage=Python3 ./Verilog2001.g4
