#!/bin/bash

# The input JSON file
input_file="version.json"

# The output header file
c_out_file="../../protocol-implementation/C/src/version.h"

# Parse the JSON file to extract the constants
major=$(cat $input_file | jq -r '.VERSION_MAJOR')
minor=$(cat $input_file | jq -r '.VERSION_MINOR')
rev=$(cat $input_file | jq -r '.VERSION_REV')

# Write the constants to the header file
echo "#ifndef VERSION_H_" > $c_out_file
echo "#define VERSION_H_" >> $c_out_file
echo "" >> $c_out_file
echo "#define VERSION_MAJOR $major" >> $c_out_file
echo "#define VERSION_MINOR $minor" >> $c_out_file
echo "#define VERSION_REV $rev" >> $c_out_file
echo "" >> $c_out_file
echo "#endif /* VERSION_H_ */" >> $c_out_file

# The output Python file
python_outfile="../../protocol-implementation/Python/version.py"

# Parse the JSON file to extract the constants
major=$(cat $input_file | jq -r '.VERSION_MAJOR')
minor=$(cat $input_file | jq -r '.VERSION_MINOR')
rev=$(cat $input_file | jq -r '.VERSION_REV')

# Write the constants to the Python file
echo "VERSION_MAJOR = $major" > $python_outfile
echo "VERSION_MINOR = $minor" >> $python_outfile
echo "VERSION_REV = $rev" >> $python_outfile

