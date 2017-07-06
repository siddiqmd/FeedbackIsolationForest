#!/bin/csh

# use current working directory for input and output
# default is to use the users home directory
#$ -cwd

# name this job
#$ -N ifexpl

# send stdout and stderror to this file
#$ -o debug/dbg_$TASK_ID.txt
#$ -j y

#$ -l mem_free=5G

# select queue - if needed
# -q eecs,eecs2,em64t,share

# see where the job is being run
hostname

# print date and time
date

../osu_iforest/iforest.exe -i /nfs/guille/bugid/adams/emmotta/bake_off/data/benchmarks/abalone/benchid_abalone{$SGE_TASK_ID}_* -o out/abalone$SGE_TASK_ID -t 250 -s 1000 -m 1-2

matlab -nojvm -nodisplay -nodesktop -nosplash -r "cd $PWD; getAUC('abalone$SGE_TASK_ID',5); exit;"

# print date and time again
date
