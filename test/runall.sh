#!/bin/bash

declare -a names=("abalone" "yeast" "ann_thyroid_1v3" "cardiotocography_1" "mammography")
for dataset in "${names[@]}"
do
    echo "=============================="
    echo $dataset
    echo "=============================="
    for l in 0 1
    do
        for r in 0 0.0001 0.001 0.01 0.1
        do
            for g in 1 10
            do
                ../iforest.exe -i datasets/anomaly/$dataset/fullsamples/${dataset}_1.csv -o out/${dataset}_1 -t 100 -s 256 -m 1 -x 1 -f 100 -l $l -u 0 -g $g -r $r
            done
        done
    done
done
