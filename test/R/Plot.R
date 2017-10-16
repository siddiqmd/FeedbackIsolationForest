
dataset <- c("abalone_1", "ann_thyroid_1v3_1", "cardiotocography_1_1", "yeast_1", "covtype_1", "kddcup_1", "mammography_1", "shuttle_1v23567_1")

pdf(file = 'AllPlots.pdf', width = 8, height = 8)

for(dataname in dataset){
  orgX <- read.csv(paste0('out/', dataname, '_summary_feed0_type_reg.csv'))
  regX <- read.csv(paste0('out/', dataname, '_summary_feed100_type_reg.csv'))
  expX <- read.csv(paste0('out/', dataname, '_summary_feed100_type_exp.csv'))
  runAvgX <- read.csv(paste0('out/', dataname, '_summary_feed100_type_runAvg.csv'))
  runAvgMstkX <- read.csv(paste0('out/', dataname, '_summary_feed100_type_updMstk.runAvg.csv'))
  regMstkX <- read.csv(paste0('out/', dataname, '_summary_feed100_type_updMstk.reg.csv'))
  PAregX <- read.csv(paste0('out/', dataname, '_summary_feed100_type_PA.reg.csv'))
  PAregMstkX <- read.csv(paste0('out/', dataname, '_summary_feed100_type_updMstk.PA.reg.csv'))
  PArunAvgX <- read.csv(paste0('out/', dataname, '_summary_feed100_type_PA.runAvg.csv'))
  PArunAvgMstkX <- read.csv(paste0('out/', dataname, '_summary_feed100_type_updMstk.PA.runAvg.csv'))
  
  datasetPrefix <- substr(dataname,1,nchar(dataname)-2)
  X <- read.csv(paste0('datasets/anomaly/', datasetPrefix, '/fullsamples/', dataname, '.csv'))
  dir <- paste0('/scratch/Codes/IFPerceptron/pyaad/temp/', datasetPrefix,
                '/if_aad_trees100_samples256_i7_q1_bd100_nscore4_leaf_tau0.03_xtau_s0.5_ca1_cx0.001_d100')
  fName <- paste0(datasetPrefix,
                  '-iforest_tau_instance-trees100_samples256_nscore4_leaf-top-active-unifprior-Ca1-1_1-fid1-runidx10-bd100-tau0_030-topK0-pseudoanom_always_False-optim_scipy-baseline.csv')
  baseIF <- read.csv(paste0(dir, '/', fName), header = F)
  fName <- paste0(datasetPrefix,
                  '-iforest_tau_instance-trees100_samples256_nscore4_leaf-top-active-unifprior-Ca1-1_1-fid1-runidx10-bd100-tau0_030-topK0-pseudoanom_always_False-optim_scipy-num_seen.csv')
  feedIF <- read.csv(paste0(dir, '/', fName), header = F)
  
  avgOrg <- apply(orgX[ ,-1], 2, mean)
  avgReg <- apply(regX[ ,-1], 2, mean)
  avgExp <- apply(expX[ ,-1], 2, mean)
  avgrunAvg <- apply(runAvgX[ ,-1], 2, mean)
  avgrunAvgMstk <- apply(runAvgMstkX[ ,-1], 2, mean)
  avgRegMstk <- apply(regMstkX[ ,-1], 2, mean)
  avgRegPA <- apply(PAregX[ ,-1], 2, mean)
  avgRegMstkPA <- apply(PAregMstkX[ ,-1], 2, mean)
  avgrunAvgPA <- apply(PArunAvgX[ ,-1], 2, mean)
  avgrunAvgMstkPA <- apply(PArunAvgMstkX[ ,-1], 2, mean)
  avgBIF <- apply(baseIF[,-(1:2)], 2, mean)
  avgFIF <- apply(feedIF[,-(1:2)], 2, mean)
  
  plot(x = 1:100, y = avgOrg, ylim = c(0,100), type = 'l', lty=2, col='black',
       xlab = '# Feedback', ylab = '# Anomaly', main = paste0(dataname,', # Anomaly = ', length(which(X[ ,1]=='anomaly')), ', # Inst = ', nrow(X)))
  abline(a = 0, b = 1, lty = 3)
  # lines(x = 1:100, y = avgBIF, col='blue', lty=2)
  lines(x = 1:100, y = avgReg, col='red')
  lines(x = 1:100, y = avgRegMstk, col='red', lty=2)
  # lines(x = 1:100, y = avgExp, col='red', lty=2)
  lines(x = 1:100, y = avgFIF, col='green')
  lines(x = 1:100, y = avgrunAvg, col='blue')
  lines(x = 1:100, y = avgrunAvgMstk, col='blue', lty=2)
  lines(x = 1:100, y = avgRegPA, col='pink')
  lines(x = 1:100, y = avgRegMstkPA, col='pink', lty=2)
  lines(x = 1:100, y = avgrunAvgPA, col='purple')
  lines(x = 1:100, y = avgrunAvgMstkPA, col='purple', lty=2)
  
  # legend(0, 100, legend=c("Base-IF-c++", "Base-IF-py", "Perceptron", "Perceptron-Exp", 'AAD-IF')[-c(2,4)],
  #        col=c("blue", "blue", "red", "red", "green")[-c(2,4)], lty=c(2,2,1,2,1)[-c(2,4)])
  legend(0, 100, legend=c("Base-IF-c++", "Perceptron.Both", "Perceptron.Mstk", "RunAvg.Both", "RunAvg.Mstk", 'AAD-IF',
                          'PA.both', 'PA.Mstk', 'PA.runAvg.both', 'PA.runAvg.Mstk'),
         col=c("black", "red", "red", "blue", "blue", "green", "pink", "pink", "purple", "purple"), lty=c(2,1,2,1,2,1,1,2,1,2))
}

dev.off()
