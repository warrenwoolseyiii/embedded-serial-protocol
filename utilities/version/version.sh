#!/bin/bash

# The input JSON file
input_file="version.json"

### Bump the version ###
# Increment the rev field in the version.json file
jq ".VERSION_REV += 1" version.json > version.json.tmp
mv version.json.tmp version.json

### C ###

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

### Python ###

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

### Kotlin ###

# The output Kotlin file
kotlin_outfile="../../protocol-implementation/Kotlin/serial-protocol/src/main/kotlin/serial/protocol/Version.kt"

# Parse the JSON file to extract the constants
major=$(cat $input_file | jq -r '.VERSION_MAJOR')
minor=$(cat $input_file | jq -r '.VERSION_MINOR')
rev=$(cat $input_file | jq -r '.VERSION_REV')

# Write the constants to the Kotlin file
echo "object Version {" > $kotlin_outfile
echo "    const val VERSION_MAJOR = $major" >> $kotlin_outfile
echo "    const val VERSION_MINOR = $minor" >> $kotlin_outfile
echo "    const val VERSION_REV = $rev" >> $kotlin_outfile
echo "}" >> $kotlin_outfile

# Read the version fields from the JSON file
version_major=$(jq -r '.VERSION_MAJOR' version.json)
version_minor=$(jq -r '.VERSION_MINOR' version.json)
version_rev=$(jq -r '.VERSION_REV' version.json)

# Create the version string
version="$major.$minor.$rev"

# Insert the version string into the build.gradle.kts file
sed -i "s/version = \".*\"/version = \"$version\"/" ../../protocol-implementation/Kotlin/serial-protocol/build.gradle.kts

# Add and commit the version.json file
#./../../protocol-implementation/Kotlin/gradlew build
git add ../../protocol-implementation/Kotlin/serial-protocol/build.gradle.kts
git add version.json
git add $python_outfile
git add $c_out_file
git add $kotlin_outfile
git commit -m "Increment rev field in version.json"
