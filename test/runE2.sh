#!/bin/bash

for dir in /scratch/ADAPT/E2GroundTruth/CSVs/*; do
    if [ ! -d "$dir/result" ]; then
        mkdir "$dir/result"
    fi
    for filename in $dir/CSVsWithGT/*.csv; do
        echo $filename
        base=$(basename "$filename" .csv)
        lines=$(cat "$filename" | wc -l)
        echo $lines
        feed=100
        if [ $lines -le 100 ]; then
            feed=30
        fi
        for w in 0 1 2 3 4 5 6 7 8 9; do
            cmd=$(echo ../iforest.exe -i \"$filename\" -o \"$dir/result/$base\" -t 100 -s 256 -m 1,2 -c $feed -w $w)
            echo ""
            echo $cmd
            eval $cmd
        done
    done
done

