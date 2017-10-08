
dataset <- c("abalone", "ann_thyroid_1v3", "cardiotocography_1", "yeast", "covtype", "kddcup", "mammography", "shuttle_1v23567")
types <- c("reg", "updMstk.reg", "exp", "updMstk.exp", "runAvg", "updMstk.runAvg", "PA.reg", "updMstk.PA.reg", "PA.runAvg", "updMstk.PA.runAvg")
methodNames <- c("Base-IF-c++", "Base-IF-py", "AAD-IF", "Perceptron.Both", "Perceptron.Mstk", "Perceptron.exp.Both", "Perceptron.exp.Mstk",
                 "RunAvg.Both", "RunAvg.Mstk", 'PA.both', 'PA.Mstk', 'PA.runAvg.both', 'PA.runAvg.Mstk')

resultFinal <- matrix(0, nrow = 13, ncol = 8)
colnames(resultFinal) <- dataset
rownames(resultFinal) <- methodNames
resultArea <- matrix(0, nrow = 13, ncol = 8)
colnames(resultArea) <- dataset
rownames(resultArea) <- methodNames

X <- list()
for(dataname in dataset){
  X[[1]] <- read.csv(paste0('out/', dataname, '_1_summary_feed0_type_reg.csv'))[ ,-1]
  
  Xraw <- read.csv(paste0('datasets/anomaly/', dataname, '/fullsamples/', dataname, '_1.csv'))
  numAnomaly <- length(which(Xraw[ ,1]=='anomaly'))
  dir <- paste0('/scratch/Codes/IFPerceptron/pyaad/temp/', dataname,
                '/if_aad_trees100_samples256_i7_q1_bd100_nscore4_leaf_tau0.03_xtau_s0.5_ca1_cx0.001_d100')
  fName <- paste0(dataname,
                  '-iforest_tau_instance-trees100_samples256_nscore4_leaf-top-active-unifprior-Ca1-1_1-fid1-runidx10-bd100-tau0_030-topK0-pseudoanom_always_False-optim_scipy-baseline.csv')
  X[[2]] <- read.csv(paste0(dir, '/', fName), header = F)[ ,-c(1:2)]
  fName <- paste0(dataname,
                  '-iforest_tau_instance-trees100_samples256_nscore4_leaf-top-active-unifprior-Ca1-1_1-fid1-runidx10-bd100-tau0_030-topK0-pseudoanom_always_False-optim_scipy-num_seen.csv')
  X[[3]] <- read.csv(paste0(dir, '/', fName), header = F)[ ,-c(1:2)]

  for(i in 1:10){
    X[[i+3]] <- read.csv(paste0('out/', dataname, '_1_summary_feed100_type_', types[i], '.csv'))[ ,-1]
  }
  N <- min(numAnomaly,100) # normalizer 1
  N2 <- N*(N+1)/2 + (100-N)*N # normalizer 2
  for(i in 1:13){
    resultArea[i,which(dataname==dataset)] <- mean(X[[i]][ ,100]/N)
    resultFinal[i,which(dataname==dataset)] <- mean(apply(X[[i]], 1, sum)/N2)
  }
}
write.csv(resultArea,  "ResultArea.csv", quote = F)
write.csv(resultFinal, "ResultFinal.csv", quote = F)
