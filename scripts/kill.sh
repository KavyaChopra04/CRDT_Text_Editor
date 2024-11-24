#!/bin/bash

# Define process names to search for
TARGETS=("alice" "bob")

# Iterate over target names
for TARGET in "${TARGETS[@]}"; do
    # Find processes with the target name
    PIDS=$(pgrep -i "$TARGET")
    
    if [ -n "$PIDS" ]; then
        for PID in $PIDS; do
            # Kill the process
            echo "Killing process with PID: $PID (name contains: $TARGET)"
            kill -9 "$PID"
        done
    else
        echo "No processes found for target: $TARGET"
    fi
done
