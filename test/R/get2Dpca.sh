#!/bin/bash

for f in /scratch/ADAPT/E2GroundTruth/CSVs/*;do
    Rscript Convert2D.R pca $f
    Rscript plot2D.R $f/pca
done

