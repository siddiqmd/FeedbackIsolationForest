#!/bin/csh

../iforest.exe -i datasets/anomaly/kddcup/fullsamples/kddcup_1.csv -o out/kddcup_1 -t 100 -s 256 -m 1
../iforest.exe -i datasets/anomaly/mammography/fullsamples/mammography_1.csv -o out/mammography_1 -t 100 -s 256 -m 1
../iforest.exe -i datasets/anomaly/shuttle_1v23567/fullsamples/shuttle_1v23567_1.csv -o out/shuttle_1v23567_1 -t 100 -s 256 -m 1

../iforest.exe -i datasets/anomaly/kddcup/fullsamples/kddcup_1.csv -o out/kddcup_1 -t 100 -s 256 -m 1 -w 1
../iforest.exe -i datasets/anomaly/mammography/fullsamples/mammography_1.csv -o out/mammography_1 -t 100 -s 256 -m 1 -w 1
../iforest.exe -i datasets/anomaly/shuttle_1v23567/fullsamples/shuttle_1v23567_1.csv -o out/shuttle_1v23567_1 -t 100 -s 256 -m 1 -w 1

