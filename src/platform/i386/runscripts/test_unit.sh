#!/bin/sh

cp tests.test_unit.o llboot.o
./cos_linker "llboot.o, :" ./gen_client_stub