#!/bin/bash

declare -a names=("abalone" "ann_thyroid_1v3" "cardiotocography_1" "yeast" "covtype" "kddcup" "mammography" "shuttle_1v23567")
for dataset in "${names[@]}"
do
    echo "=============================="
    echo $dataset
    echo "=============================="
    for w in 0 1 2 3 4 5 6 7 8 9
    do
        ../iforest.exe -i datasets/anomaly/$dataset/fullsamples/${dataset}_1.csv -o out/${dataset}_1 -t 100 -s 256 -m 1 -w $w
    done
done

