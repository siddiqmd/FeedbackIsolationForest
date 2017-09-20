#!/bin/csh

../iforest.exe -i datasets/anomaly/abalone/fullsamples/abalone_1.csv -o out/abalone_1 -t 100 -s 256 -m 1
../iforest.exe -i datasets/anomaly/ann_thyroid_1v3/fullsamples/ann_thyroid_1v3_1.csv -o out/ann_thyroid_1v3_1 -t 100 -s 256 -m 1
../iforest.exe -i datasets/anomaly/cardiotocography_1/fullsamples/cardiotocography_1_1.csv -o out/cardiotocography_1_1 -t 100 -s 256 -m 1
../iforest.exe -i datasets/anomaly/yeast/fullsamples/yeast_1.csv -o out/yeast_1 -t 100 -s 256 -m 1
../iforest.exe -i datasets/anomaly/covtype/fullsamples/covtype_1.csv -o out/covtype_1 -t 100 -s 256 -m 1

../iforest.exe -i datasets/anomaly/abalone/fullsamples/abalone_1.csv -o out/abalone_1 -t 100 -s 256 -m 1 -w 1
../iforest.exe -i datasets/anomaly/ann_thyroid_1v3/fullsamples/ann_thyroid_1v3_1.csv -o out/ann_thyroid_1v3_1 -t 100 -s 256 -m 1 -w 1
../iforest.exe -i datasets/anomaly/cardiotocography_1/fullsamples/cardiotocography_1_1.csv -o out/cardiotocography_1_1 -t 100 -s 256 -m 1 -w 1
../iforest.exe -i datasets/anomaly/yeast/fullsamples/yeast_1.csv -o out/yeast_1 -t 100 -s 256 -m 1 -w 1
../iforest.exe -i datasets/anomaly/covtype/fullsamples/covtype_1.csv -o out/covtype_1 -t 100 -s 256 -m 1 -w 1
