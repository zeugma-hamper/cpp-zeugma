#!/bin/bash

# $1 shader type
# $2 input filename
# $3 varying file
# $4 output filename

bld/shaderc -i bgfx/src -f $2 -o $4 --type $1 --varyingdef $3 --profile 460
