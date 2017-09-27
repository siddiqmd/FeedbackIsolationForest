#!/bin/bash

for filename in E2_Cadets_Bovia_Webshell/*.csv; do
    base=$(basename "$filename" .csv)
    lines=$(cat $filename | wc -l)
    if [ $lines -le 100 ]; then
        ../iforest.exe -i $filename -o out/E2_Cadets_Bovia_Webshell/$base -t 100 -s 256 -m 1 -c 30
        ../iforest.exe -i $filename -o out/E2_Cadets_Bovia_Webshell/$base -t 100 -s 256 -m 1 -c 30 -w 1
    fi
done

