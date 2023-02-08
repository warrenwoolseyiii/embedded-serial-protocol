#!/bin/bash

# The input JSON file
input_file="utilities/version/version.json"

# Parse the JSON file to extract the constants
major=$(cat $input_file | jq -r '.VERSION_MAJOR')
minor=$(cat $input_file | jq -r '.VERSION_MINOR')
rev=$(cat $input_file | jq -r '.VERSION_REV')

# Echo the version string
echo "$major.$minor.$rev"