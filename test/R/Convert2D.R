
library(tsne)

set.seed(0)

args <- commandArgs(T)
# dataset <- 'E2_Clearscope_Bovia_lobiwapp/clearscope_1800K'

# dataset <- '/scratch/ADAPT/E2GroundTruth/CSVs/cadets_bovia_2300K'
type <- args[1]
dataset <- args[2]
directory <- paste0(dataset, '/CSVsWithGT/')
outDir <- paste0(dataset, '/', type)
if(dir.exists(outDir) == F){
  dir.create(outDir)
}

fileNames <- list.files(path = directory)
for(fName in fileNames){
  X <- read.csv(paste0(directory, fName))
  remCol <- c(1,2)
  v <- apply(X[ ,-c(1,2)], 2, var)
  if(length(which(v < 1e-9)) > 0){
    remCol <- as.vector(c(remCol, which(v < 1e-15)+2))
  }
  if(ncol(X) > length(remCol) + 1){
    print(fName)
    if(type == 'pca'){
      pca <- prcomp(X[ ,-remCol], center = T, scale. = T)
      X.2d <- pca$x[, 1:2]
    }else{
      if(nrow(X) > 20000){
        aidx <- which(X[ ,1] == 'anomaly')
        nidx <- sample(1:nrow(X), size = 20000)
        X.2d <- tsne(X[c(aidx,nidx),-remCol])
      }else{
        X.2d <- tsne(X[ ,-remCol])
      }
    }
    
    write.csv(cbind(groundtruth=as.character(X[ ,1]), x=X.2d[ ,1], y=X.2d[ ,2]),
              paste0(outDir, '/', substr(fName,1,nchar(fName)-4), '_', type, '.csv'), quote = F, row.names = F)
  }
}
