#!/usr/bin/python3
import sys
import os
import re

from antlr4 import *
from antlr.Verilog2001Lexer import Verilog2001Lexer
from antlr.Verilog2001Parser import Verilog2001Parser
from antlr.Verilog2001Listener import Verilog2001Listener
from antlr4 import *

import argparse
import ntpath
import networkx as nx
import numpy as np
import matplotlib.pyplot as plt
from shutil import copyfile
import shutil
import glob

import fileinput
from tqdm import tqdm

class Editable:

    def __init__(self):
        self.line = None
        self.column = None

    def insert(self, text):
        pass

class Register:

    def __init__(self, name, size, dimension):
        self.name = name
        self.size = size
        self.dimension = dimension

class Parameter:

    def __init__(self, name, value):
        self.name = name
        self.value = value

    def dump(self):
        return self.name+'='+self.value+'\n'

class Procedure:

    def __init__(self, start, stop):
        self.start = start
        self.stop = stop
        self.flip_flops = []
        self.synchronous = True

    def add_flip_flop(self, reg):
        self.flip_flops.append(reg)

    def is_synchronous(self):
        return self.synchronous

class Module(Editable):

    def __init__(self, name, module_file_path, start, named_port=False):
        self.name = name
        self.submodules = []
        self.parameters = []
        self.registers = []
        self.ports = []
        self.procedures = []
        self.verbose = False
        self.module_file_path = module_file_path
        self.port_list_pos = None
        self.start = start
        self.scan_chain_size = 0
        self.submodule_instances = []
        self.named_port = named_port
        self.port_decl_pos = None

        if self.verbose:
            print("[INFO] Adding new module {}".format(self.name))

    def add_submodule_instance(self, submodule, instance_name, pos, named_port):
        self.submodule_instances.append({'module':submodule, "pos": pos, "name": instance_name, "named_port": named_port})
        if self.verbose:
            print("[INFO] Adding instance {} for submodule {}".format(instance_name, submodule.name))

    def add_submodule(self, submodule):
        self.submodules.append({"module":submodule})
        if self.verbose:
            print("[INFO] Adding submodule {}".format(submodule.name))

    def add_parameter(self, name, value):
        self.parameters.append(Parameter(name, value))
        if self.verbose:
            print("[INFO] Adding paramater {} with value {}".format(name, value))

    def add_register(self, name, size, dimension=1):
        self.registers.append(Register(name, size, dimension))
        if self.verbose:
            print("[INFO] Adding register {} |{}| * |{}|".format(name, size, dimension))

    def add_procedure(self, start, end):
        self.procedures.append(Procedure(start, end))
        if self.verbose:
            print("[INFO] Procedure at line {} and column {}".format(start.line, start.column))

    def add_flip_flop(self, DFF_name):
        # remove [] in name
        DFF_name = re.sub("\\[\\d+\\]", "", DFF_name)

        if len(self.procedures) == 0:
            #print("Unable to add flip_flop {} since no synchronous procedure has been found so far.".format(DFF_name))
            return

        # DFF_name is already present
        for reg in self.procedures[-1].flip_flops:
            if reg.name == DFF_name:
                return

        for reg in self.registers:
            if reg.name == DFF_name:
                self.procedures[-1].add_flip_flop(reg)
                if self.verbose:
                    print("[INFO] Adding flip flop  {}".format(DFF_name))
                return
        raise Exception("Assignement of register {} before declaration in module {}".format(DFF_name, self.name))

    def hierarchy(self):
        ret = []
        for submodule in self.submodules:
           ret.append((self.name, submodule["module"].name))
        return ret

    def add_scan_chain(self):
        if self.verbose:
          print("[INFO]  Inserting Scan Chain Mux")

        modifications = []

        #if len(self.procedures) == 0:
        #    print("[WARNING]  Skip scan chain insertion because no procedure were recorded...")
        #    return modifications

        last_ff = None
        last_ff_name = "scan_output"
        first_ff = True

	# For each procedure in module
        for i in range(0, len(self.procedures)):

            procedure = self.procedures[i]

            # Only process synchronous procedures
            if not procedure.is_synchronous():
                if self.verbose:
                    print("[WARNING] skipping procedure at line {}".format(procedure.start.line))
                continue

            mux = ""
            start_pos = procedure.start

            # set mux
            mux =  "\n    if(scan_enable)\n" + "       begin\n"
            mux += "\n        if(scan_ck_enable)\n" + "       begin\n"

            sc_str = ""
            no_ff_in_scan_chain = 0
            for ff in procedure.flip_flops:
                if self.verbose:
                  print("[INFO]  adding flip-flop {} to scan chain...".format(ff.name))

		# is it the first iteration?
                #if no_ff_in_scan_chain == 0:
                if ff.size == 1:
                    sc_str += last_ff_name + " <= " + ff.name + ";\n"
                else:

                    ff_size = ff.size
                    if last_ff is not None and ff.name == last_ff.name:
                        ff_size = ff.size-1

                    self.scan_chain_size += (ff.dimension * ff.size)

                    for j in range(0, ff.dimension):
                        for k in reversed(range(0, ff_size)):
                            if ff.dimension > 1:
                                sc_str += last_ff_name + " <= " + ff.name + "[" + str(j) + "]" + "[" + str(k) + "];\n";
                                last_ff_name = ff.name + "[" + str(j) + "]" + "[" + str(k) + "]"
                            else:
                                sc_str += last_ff_name + " <= " + ff.name + "[" + str(k) + "];\n";
                                last_ff_name = ff.name + "[" + str(k) + "]"

                last_ff = ff
                last_ff_name = ""
                if last_ff == None:
                    last_ff_name = "scan_output"
                else:
                    if last_ff.size == 1:
                        last_ff_name = last_ff.name
                    else:
                        if last_ff.dimension == 1:
                            last_ff_name = last_ff.name+"[0]"
                        else:
                            last_ff_name = last_ff.name+"["+str(last_ff.dimension-1)+"]"+"[0]"

                # Last element
                if no_ff_in_scan_chain == (len(procedure.flip_flops) - 1):
                    if procedure.start.line == self.procedures[-1].start.line:
                        if len(self.submodules) > 0 and len(self.procedures) > 0:
                            sc_str += last_ff_name + " <= scan_output0;\n"
                        else:
                            sc_str += last_ff_name + " <= scan_input;\n"
                        break
                    else:
                        # last element in current procedure but still procedures to process
                        next_ff = self.procedures[i+1].flip_flops[0]
                        if next_ff.size == 1:
                            sc_str += last_ff_name + ' <='+next_ff.name+';\n'
                            last_ff_name = next_ff.name
                        else:
                            sc_str += last_ff_name + ' <='+next_ff.name+'['+str(next_ff.size-1)+'];\n'
                            last_ff_name = next_ff.name+"["+str(next_ff.size-1)+"]"
                        last_ff = next_ff
                        break

                no_ff_in_scan_chain += 1

            if first_ff == True:
                first_ff = False
                first_line = sc_str.split("\n")[0]
                
                assign_str = "assign scan_output = "+first_line.split(" ")[2]+";\n"
                modifications.append(Modification(self.start, assign_str))

                sc_str = "\n".join(sc_str.split("\n")[1:])

            mux += sc_str
            mux += "            end \n" + "        end \n" +"    else \n"
            modifications.append(Modification(start_pos, mux))

        # Add scan chain signals
        if not self.named_port:
            sc_str = "scan_input, scan_output, scan_ck_enable, scan_enable, "
        #elif self.scan_chain_size == 0:
        #    sc_str = "input wire scan_input, output wire scan_output, input wire scan_ck_enable, input wire scan_enable, "
        else:
            sc_str = "input wire scan_input, output wire scan_output, input wire scan_ck_enable, input wire scan_enable, "

        modifications.append(Modification(self.port_list_pos,sc_str))

        if not self.named_port:
            sc_str = "input wire scan_input; output reg scan_output; input wire scan_ck_enable; input wire scan_enable;"
            assert(self.port_decl_pos != None)
            modifications.append(Modification(self.port_decl_pos, sc_str))

        no_assign = 0

        for k in range(0, len(self.submodule_instances)):
            submodule = self.submodule_instances[k]

            if len(self.procedures) == 0:
                out = "output"
            else:
                out = "output"+str(k)

            if k == (len(self.submodule_instances)-1):
                entry = "input"
            else:
                entry = "output"+str(k+1)

            if submodule["named_port"] == True:
                sc_str = ".scan_input(scan_"+entry+"),\n.scan_output(scan_"+out+"),\n.scan_ck_enable(scan_ck_enable),\n.scan_enable(scan_enable),\n"
            else:
                sc_str = "scan_"+entry+",scan_"+out+",scan_ck_enable,scan_enable,"
            modifications.append(Modification(submodule["pos"], sc_str))
            no_assign = k+1

        for k in range(0, no_assign):
            sc_str = "wire scan_output"+str(k)+";\n"
            modifications.append(Modification(self.start, sc_str))
	
        # set loopback
        if self.scan_chain_size == 0 and len(self.submodules) == 0:
          sc_str = "assign scan_output = scan_input;\n"
          modifications.append(Modification(self.start, sc_str))

        return modifications

    def get_source_file_path(self):
        return self.module_file_path

class Modification:
    def __init__(self, position, payload):
        self.position = position
        self.payload = payload

    def __str__(self):
        return "Modification at line {} and column {}.\n".format(self.position.line, self.position.column)

class System:

    def __init__(self, input_dir, output_dir):
        self.modules = []
        self.last_module = None
        self.input_dir = input_dir
        self.output_dir = output_dir
        self.scan_chain_size = 0
        self.verbose = False

        if self.output_dir[-1] != '/':
            self.output_dir+= '/'

    def add_module(self, name, module_file_path, start, named_port):
        module = self.resolve_module(name)
        if not module:
            module = Module(name, module_file_path, start, named_port)
            self.modules.append(module)
        else:
            if module.module_file_path == "":
                module.module_file_path = module_file_path
        self.last_module = module

    def add_submodule_instance(self, submodule, instance_name, pos, named_port=False):
        if not self.last_module:
            raise Exception("unable to add submodule instance before module declaration")
        module = self.get_last_module()
        module.add_submodule_instance(submodule, instance_name, pos, named_port)

    def add_submodule(self, name):
        if not self.last_module:
            raise Exception("unable to add submodule before module declaration")
        submodule = self.resolve_module(name)
        if not submodule:
            submodule = Module(name, "", None)
        self.last_module.add_submodule(submodule)
        return submodule

    def resolve_module(self, name):
        for module in self.modules:
            if module.name == name:
                return module
        return None

    def get_last_module(self):
        return self.last_module

    def get_parameters_expression(self, module):
        expr = ''
        for parameter in module.parameters:
            expr += parameter.dump()
        return expr

    def add_parameter(self, name, value):
        self.last_module.add_parameter(name, value)

    def add_register(self, name, size, dimension=1):
        self.last_module.add_register(name, size, dimension)

    def add_procedure(self, start, end):
        self.last_module.add_procedure(start, end)

    def add_flip_flop(self, DFF_name):
        self.last_module.add_flip_flop(DFF_name)

    def list_modules(self):
        ret = []
        for module in self.modules:
            ret.append(module.name)
        return ret

    def list_modules_hierarchy(self):
        ret = []
        for module in self.modules:
            ret.extend(module.hierarchy())
        return ret

    def add_scan_chain(self):

        if self.verbose:
            print("\n\n\n\n\n[INFO] Adding scan chain...")

        for module in self.modules:
            #if self.verbose:
            print("[INFO] Processing module {}".format(module.name))

            filename = ntpath.basename(module.get_source_file_path())

            module_file = open(module.get_source_file_path(), "r")
            mem_file = module_file.readlines()
            module_file.close()

            modifications = module.add_scan_chain()

            self.scan_chain_size += module.scan_chain_size

            #global_offset = 0

            def getModKey(mod):
                return mod.position

            modifications.sort(key=getModKey,reverse=True)

            for modification in modifications:
                #print("at line {} {}".format(modification.position.line, modification.position.column))
                if self.verbose:
                    print(modification)
                    print("[INFO]  Inserting at position l:{} c:{} file:{}\n==========\n{}\n=========\n".format(modification.position.line,modification.position.column,module.get_source_file_path(),modification.payload))
                    print("at line {} size {} pos {}".format(modification.position.line, len(mem_file[modification.position.line]), modification.position.column))
                if modification.position.column == 0xFFFFFFFF or len(mem_file[modification.position.line])-1 < modification.position.column or mem_file[modification.position.line][modification.position.column] == ';':
                    if self.verbose:
                        print(mem_file[modification.position.line])
                    mem_file.insert(modification.position.line, modification.payload)
                else: 
                    if self.verbose:
                        print(mem_file[modification.position.line])
                    mem_file[modification.position.line] = mem_file[modification.position.line][0:modification.position.column] + modification.payload + mem_file[modification.position.line][modification.position.column:]
                #global_offset += len(modification.payload)
            #print(mem_file)

            copy_module_file = open(self.output_dir+filename, "w+")
            copy_module_file.write("".join(mem_file))
            copy_module_file.close()

        if self.verbose:
            print("[STATS] scan chain size = {} bits".format(self.scan_chain_size))

    def draw_mdg(self):
        edges = system.list_modules_hierarchy()

        options = {
            'node_color': 'blue',
            'node_size': 100,
            'width': 3,
            'arrowstyle': '-|>',
            'arrowsize': 12,
        }

        g = nx.DiGraph(directed=True)

        pos = nx.layout.random_layout(g)

        g.add_edges_from(edges)

        circPos=nx.circular_layout(g)

        nx.draw(g,pos=circPos, with_labels=True, options=options)

        plt.show()

    def set_current_module_ports_list_position(self, position):

        self.modules[-1].port_list_pos = position

    def set_current_module_port_declaration(self, position):
        
        self.modules[-1].port_decl_pos = position

def create_or_clean_directory(dir):
	"""
	This method attempts to create the specified directory. However, if it
	already exists then it will be cleaned to ensure there are no stale files.
	"""
	if not os.path.exists(dir):
		print("The path \"" + dir + "\" does not exist")
		print("creating directory \"" + dir + "\"")
		os.makedirs(dir)
	else: #Directory exists, but we want to clean it before use
		print(dir + " already exists. Cleaning before use...")
		shutil.rmtree(dir)
		os.makedirs(dir)

def main(argv):

    parser = argparse.ArgumentParser()
  
    parser.add_argument('--mode', "-m", dest='mode', choices=["instrument","make"],
            action='store', required=True, help='.')
    parser.add_argument('--input_dir', '-i', action='store', required=False,
                        help='Inform the input directory.')
    parser.add_argument('--output_dir', '-o', action='store', required=True,
                        help='Inform the output directory.')

    args = parser.parse_args()

    if args.mode == "instrument":

        if args.input_dir is None:
            parser.error("--instrument requires input_dir.")

        files_to_analyze = []
        for root, dirs, files in os.walk(args.input_dir):
            for f in files:
                if f.endswith(".v"):
                    files_to_analyze.append(os.path.join(root, f))

        system = System(args.input_dir, args.output_dir)

        create_or_clean_directory(args.output_dir) 

        for i in tqdm(range(len(files_to_analyze))):
        #for file in files_to_analyze:
            input = FileStream(files_to_analyze[i])
            #input = FileStream(file)

            print("\n\n\n\n\n[INFO] Analyzing {}".format(files_to_analyze[i]))

            lexer = Verilog2001Lexer(input)

            stream = CommonTokenStream(lexer)

            parser = Verilog2001Parser(stream)

            listener = Verilog2001Listener(system, files_to_analyze[i])

            tree = parser.source_text()

            walker = ParseTreeWalker()

            walker.walk(listener, tree)

        system.add_scan_chain()

    if args.mode == "make":
        print("make dir")

        create_or_clean_directory(args.output_dir+"/sim")
        create_or_clean_directory(args.output_dir+"/tb")
        create_or_clean_directory(args.output_dir+"/sw")
        create_or_clean_directory(args.output_dir+"/c")
        create_or_clean_directory(args.output_dir+"/tcl")

        dirs = ["sim", "sw", "tb", "rtl", "tcl", "c"]

        for c_dir in dirs:
            for file in glob.glob(r'./template/'+c_dir+'/*'):
                shutil.copy(file, args.output_dir+c_dir)

    #system.draw_mdg()

if __name__ == '__main__':
    main(sys.argv)
