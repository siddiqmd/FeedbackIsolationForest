#!/bin/bash

from=901
to=1200
declare -a names=("abalone" "magic.gamma" "landsat" "particle" "yeast" "shuttle" "spambase")

for dataset in "${names[@]}"
do
for (( bid=$from; bid<=$to; bid++ ))
do
    if [ -f /nfs/guille/bugid/adams/meta_analysis/benchmarks/${dataset}/${dataset}_benchmark_$(printf %04d $bid).csv ]
    then
    for g in 1
    do
        for u in 0
        do
            for r in 0
            do
                for l in 0 1 2
                do
                    for a in 1
                    do
                        for p in 0 1
                        do
                            for z in 0 1
                            do
                                if [ $p -ne 1 -o $z -ne 1 ]
                                then
                                cm=$(echo ../iforest.exe -i /nfs/guille/bugid/adams/meta_analysis/benchmarks/${dataset}/${dataset}_benchmark_$(printf %04d $bid).csv -o benchNomFeedOnly/${dataset}/out/${dataset}_${bid} -t 100 -s 256 -m 6,1-5 -x 10 -f 100 -l $l -u $u -g $g -r $r -w 2 -a $a -p $p -z $z)
                                cmd=$(echo qsub -cwd -N j${l}${p}${z}.${bid} -o benchNomFeedOnly/${dataset}/log/${dataset}_${bid}_${g}_${u}_${r}_${l}_${a}_${p}_${z}.log -j y -b y -l h_vmem=5G \"$cm\")
                                echo $cmd
                                eval $cmd
                                fi
                            done
                        done
                    done
                done
            done
        done
    done
    fi
done
done

