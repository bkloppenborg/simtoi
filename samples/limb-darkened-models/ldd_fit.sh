#!/bin/bash

cd ../bin

# Execute SIMTOI as if executed from an open console
./simtoi -d ../samples/LDD-2_0mas-0_5alpha.oifits -m ../samples/LDD-2_0mas-0_5alpha.json -e levmar -c

# Execute SIMTOI as if it were executed remotely, specifying the machine and screen:
#./simtoi -d ../samples/LDD-2_0mas-0_5alpha.oifits -m ../samples/LDD-2_0mas-0_5alpha.json -e levmar -c -display :0
