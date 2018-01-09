#!/bin/bash

declare -a names=("abalone" "yeast" "ann_thyroid_1v3" "cardiotocography_1" "mammography")
for dataset in "${names[@]}"
do
    echo "=============================="
    echo $dataset
    echo "=============================="
    for u in 0 1 2
    do
        for r in 0
        do
            for g in 1
            do
                for l in 0 1 2
                do
                    for a in 1
                    do
                        for p in 0
                        do
                            for z in 0
                            do
                                cm=$(echo ../iforest.exe -i datasets/anomaly/$dataset/fullsamples/${dataset}_1.csv -o out/${dataset}_1 -t 100 -s 256 -m 1 -x 10 -f 100 -l $l -u $u -g $g -r $r -a $a -p $p -z $z)
                                cmd=$(echo qsub -cwd -N j${l}_${u}_${dataset} -o log/${dataset}_${u}_${r}_${g}_${l}_${a}_${p}_${z}.log -j y -b y -l h_vmem=5G \"$cm\")
                                echo $cmd
                                eval $cmd
                            done
                        done
                    done
                done
            done
        done
    done
done
