
args <- commandArgs(T)

# directory <- paste0('/scratch/ADAPT/E2GroundTruth/CSVs/cadets_bovia_2300K/pca/')
directory <- args[1]

fileNames <- list.files(directory, pattern = '.csv')

# pdf(paste0('PCA_', dataset, '.pdf'), width = 9, height = 9)

for(fName in fileNames){
  png(paste0(directory, '/', substr(fName, 1, nchar(fName)-4), '.png'), width = 800, height = 800)
  print(fName)
  X.2d <- read.csv(paste0(directory, '/', fName))
  
  aIdx <- which(X.2d[ ,1] == 'anomaly')
  plot(X.2d[ ,2], X.2d[ ,3], xlab = 'x', ylab = 'y',
       main = paste0(substr(fName,1,nchar(fName)-8),', # Anomaly = ', length(aIdx), ', # Inst = ', nrow(X.2d)))
  points(X.2d[aIdx,-1], col="red", pch=19, cex=1.2)
  
  dev.off()
}

# dev.off()
