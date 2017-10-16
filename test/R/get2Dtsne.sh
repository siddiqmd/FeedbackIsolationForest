#!/bin/bash

echo PCA
Rscript Convert2D.R pca /scratch/ADAPT/E2GroundTruth/CSVs/cadets_bovia_780K
Rscript plot2D.R /scratch/ADAPT/E2GroundTruth/CSVs/cadets_bovia_780K/pca

echo TSNE
Rscript Convert2D.R tsne /scratch/ADAPT/E2GroundTruth/CSVs/cadets_bovia_780K
Rscript plot2D.R /scratch/ADAPT/E2GroundTruth/CSVs/cadets_bovia_780K/tsne

