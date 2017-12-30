#!/bin/bash

declare -a names=("abalone" "yeast" "ann_thyroid_1v3" "cardiotocography_1" "mammography")
for dataset in "${names[@]}"
do
    echo "=============================="
    echo $dataset
    echo "=============================="
    for w in 11
    do
        ../iforest.exe -i datasets/anomaly/$dataset/fullsamples/${dataset}_1.csv -o outNewReg0.5/${dataset}_1 -t 100 -s 256 -m 1 -w $w
    done
done

