#!/bin/bash

declare -a names=("abalone" "yeast" "ann_thyroid_1v3" "cardiotocography_1" "mammography")
for dataset in "${names[@]}"
do
    echo "=============================="
    echo $dataset
    echo "=============================="
    for u in 0 1 2
    do
        for r in 0 0.1 0.5
        do
            for g in 1 10
            do
                cm=$(echo ../iforest.exe -i datasets/anomaly/$dataset/fullsamples/${dataset}_1.csv -o out/${dataset}_1 -t 100 -s 256 -m 1 -x 10 -f 100 -l 0 -u $u -g $g -r $r)
                cmd=$(echo qsub -cwd -N lin_${u}_${r}_${g}_${dataset} -o log/${dataset}_linear_${u}_${r}_${g}.log -j y -b y -l h_vmem=5G \"$cm\")
                echo $cmd
                eval $cmd
            done
        done
    done
    for u in 0 1 2
    do
        for r in 0 0.0001 0.001
        do
            for g in 1 10
            do
                cm=$(echo ../iforest.exe -i datasets/anomaly/$dataset/fullsamples/${dataset}_1.csv -o out/${dataset}_1 -t 100 -s 256 -m 1 -x 10 -f 100 -l 1 -u $u -g $g -r $r)
                cmd=$(echo qsub -cwd -N llh_${u}_${r}_${g}_${dataset} -o log/${dataset}_loglikelihood_${u}_${r}_${g}.log -j y -b y -l h_vmem=5G \"$cm\")
                echo $cmd
                eval $cmd
            done
        done
    done
done
