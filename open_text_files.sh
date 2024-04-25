#!/bin/bash

# Directory containing the text files
DIR="/home/it/Downloads/event-test"

# Change to the directory
cd "$DIR"

# Loop through each text file and open it with build_agent_guide
for file in *.txt; do
    if [ -f "$file" ]; then
        build_agent_guide "$file"
    fi
done
