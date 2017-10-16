#!/bin/bash

declare -a names=("abalone" "ann_thyroid_1v3" "cardiotocography_1" "yeast" "covtype" "kddcup" "mammography" "shuttle_1v23567")
for dataset in "${names[@]}"
do
    echo $dataset
    for w in 6 7 8 9
    do
        ../iforest.exe -i datasets/anomaly/$dataset/fullsamples/${dataset}_1.csv -o out/${dataset}_1 -t 100 -s 256 -m 1 -w $w
    done
done

