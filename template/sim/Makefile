######################################################################
#
# DESCRIPTION: Verilator Example: Small Makefile
#
# This calls the object directory makefile.  That allows the objects to
# be placed in the "current directory" which simplifies the Makefile.
#
# Copyright 2003-2018 by Wilson Snyder. This program is free software; you can
# redistribute it and/or modify it under the terms of either the GNU
# Lesser General Public License Version 3 or the Perl Artistic License
# Version 2.0.
#
######################################################################

#####################################################################
# Set up variables

# If $VERILATOR_ROOT isn't in the environment, we assume it is part of a
# package inatall, and verilator is in your path. Otherwise find the
# binary relative to $VERILATOR_ROOT (such as when inside the git sources).
#ifeq ($(VERILATOR_ROOT),)
ifndef ($(VERILATOR_ROOT))
VERILATOR = verilator
VERILATOR_COVERAGE = verilator_coverage
else
export VERILATOR_ROOT
VERILATOR = $(VERILATOR_ROOT)/bin/verilator
VERILATOR_COVERAGE = $(VERILATOR_ROOT)/bin/verilator_coverage
endif

# Generate SystemC in executable form
VERILATOR_FLAGS += --cc --exe
# Tune maximum loop iterations
VERILATOR_FLAGS += --unroll-count 256
# Generate makefile dependencies (not shown as complicates the Makefile)
# Optimize
VERILATOR_FLAGS += -O3 -x-assign 0
# Warn abount lint issues; may not want this on less solid designs
VERILATOR_FLAGS += -Werror-PINMISSING                                  \
-Werror-IMPLICIT                                                       \
-Wno-fatal                                                             \
-Wno-PINCONNECTEMPTY                                                   \
-Wno-ASSIGNDLY                                                         \
-Wno-DECLFILENAME                                                      \
-Wno-UNOPTFLAT                                                         \
-Wno-UNUSED                                                            \
-Wno-style                                                             \
-Wno-lint                                                              \

# Make waveforms
ifeq ($(TRACE),1)
VERILATOR_FLAGS += --trace --trace-structs
endif
# Check SystemVerilog assertions
# VERILATOR_FLAGS += --assert
# Generate coverage analysis
# VERILATOR_FLAGS += --coverage
# Run Verilator in debug mode
#VERILATOR_FLAGS += --debug
# Add this trace to get a backtrace in gdb
# VERILATOR_FLAGS += --gdbbt

TOP_RTL      = top
TOP_RTL_FILE = $(patsubst %,%.sv,$(TOP_RTL))
RTL_PATH     = ../rtl/
RTL_FILES    =  $(shell find $(RTL_PATH) -name '*.v')
RTL_FILES    += $(shell find $(RTL_PATH) -name '*.sv')
SC_TB_FILE   = ../tb/main.cpp ../tb/net.cpp ../tb/sim.cpp
OBJ_DIR      = obj_dir
EXE          = simu

ifeq ($(DEBUG),1)
CFLAGS       = -CFLAGS -g3 -CFLAGS -O0 -CFLAGS -std=c++11
else
CFLAGS       = -CFLAGS -O3 -CFLAGS -std=c++11
endif

CFLAGS       += -CFLAGS -I../../tb/
CFLAGS       += -CFLAGS -I../../../system/include

LDFLAGS     = -LDFLAGS "-lpthread -lrt -L../../../system/bin/ -lsimulator_system"

######################################################################

PHONY: sanity gen exe all

all:
	make gen
	make exe

gen:
	@echo
	@echo "-- VERILATE ----------------"
	verilator $(VERILATOR_FLAGS)  \
		-y $(RTL_PATH) $(RTL_FILES) \
		$(CFLAGS) $(LDFLAGS)        \
		--top-module $(TOP_RTL)     \
		--Mdir $(OBJ_DIR)           \
		--exe $(SC_TB_FILE)         \
		-o ../$(EXE)

exe:
	@echo "-- EXECUTABLE ----------------"
	make -C $(OBJ_DIR) -f $(patsubst %,V%.mk, $(TOP_RTL))

run:
	@echo
	@echo "-- RUN ---------------------"
	@mkdir -p logs
ifeq ($(TRACE),1)
	./simu +trace
else
	./simu
endif

covarage:
	@echo
	@echo "-- COVERAGE ----------------"
	$(VERILATOR_COVERAGE) --annotate logs/annotated logs/coverage.dat

wave:
	@echo
	@echo "-- DONE --------------------"
	@echo "To see waveforms, open vlt_dump.vcd in a waveform viewer"
	@echo
	gtkwave ./logs/simx.vcd

clean:
	rm -f *.vcd
	rm -fr $(OBJ_DIR)
	rm -f $(EXE)
	rm -rf obj_dir logs *.log *.dmp *.vpd coverage.dat

######################################################################
# Other targets
show-config:
	$(VERILATOR) -V

######################################################################
sanity:
# Check for sanity to avoid later confusion
ifneq ($(words $(CURDIR)),1)
 $(error Unsupported: GNU Make cannot build in directories containing spaces, build elsewhere: '$(CURDIR)')
endif

#ifndef VERILATOR_ROOT
#	$(error VERILATOR_ROOT is undefined)
#endif

nosc:
	@echo
	@echo "%Skip: SYSTEMC_INCLUDE not in environment"
	@echo "(If you have SystemC see the README, and rebuild Verilator)"
	@echo
