import os
import sys

def change_cos(cwd):
      while(not(cwd.endswith('composite'))):
            os.chdir("..")
            cwd = os.getcwd()

cwd = os.getcwd()
name = sys.argv[1]

print(cwd)
change_cos(cwd)
os.chdir("src/platform/i386/runscripts/")
script_name = name+".sh"
f = open(script_name, "w")
script_contents = """#!/bin/sh

cp tests."""
script_contents = script_contents + name + ".o ";
script_contents = script_contents + '''llboot.o
./cos_linker "llboot.o, :" ./gen_client_stub''';
f.write(script_contents)


cwd = os.getcwd()
change_cos(cwd)
os.chdir("src/components/implementation/tests")
cwd = os.getcwd()
test_path = cwd + "/" + name
os.mkdir(test_path)
os.chdir(test_path)

f = open("Makefile", "w")

make_contents = "COMPONENT = " + name + "t.o"
make_contents = make_contents + """
INTERFACES=
DEPENDENCIES=
IF_LIB=
ADDITIONAL_LIBS=-lcobj_format $(LIBSLRAW) -lsl_mod_fprr -lsl_thd_static_backend

include ../../Makefile.subsubdir
MANDITORY_LIB=simple_stklib.o
"""
f.write(make_contents)

file_name = name + ".c"
f = open(file_name, "w")
file_contents = """#include <cos_component.h>
#include <llprint.h>

void
cos_init(void)
{
	printc("Default Test Running!");
	while (1) ;
}
"""
f.write(file_contents)





      





      
