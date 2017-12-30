#!/bin/bash

declare -a names=("abalone" "ann_thyroid_1v3" "cardiotocography_1" "yeast" "kddcup" "mammography" "shuttle_1v23567" "covtype")
for dataset in "${names[@]}"
do
    echo "=============================="
    echo $dataset
    echo "=============================="
    for w in 10 11
    do
        ../iforest.exe -i datasets/anomaly/$dataset/fullsamples/${dataset}_1.csv -o outNew/${dataset}_1 -t 100 -s 256 -m 1 -w $w
    done
done

