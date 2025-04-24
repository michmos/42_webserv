#!/bin/bash

tempFile=$(mktemp)
trap 'rm -f "$tempFile"' EXIT

while true; do
    bash test_script.sh > "$tempFile"
    cat "$tempFile"
    if grep -q "Status is not ok" "$tempFile"; then
        break
    fi
done

