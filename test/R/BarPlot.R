
datasets <- c("ann_thyroid_1v3", "cardiotocography_1", "mammography", "yeast", "abalone")
updTypes <- c('online', 'stochastic', 'batch')[1]
lossTypes <- c('linear', 'loglikelihood', 'logistic')
regs <- c('0', '0.0001', '0.001', '0.01', '0.1', '0.5')
lrates <- c(0,1)
pwts <- c(0,1)

for(dataset in datasets){
  resFinal <- rep(0, 73)
  resArea <- rep(0, 73)
  cNames <- rep('', length(resArea))
  cIdx <- 0
  X <- read.csv(paste0('../datasets/anomaly/', dataset, '/fullsamples/', dataset, '_1.csv'))
  numAnomaly <- length(which(X[ ,1]=='anomaly'))
  N <- min(numAnomaly,100) # normalizer 1
  N2 <- N*(N+1)/2 + (100-N)*N # normalizer 2

  # Baseline
  X <- read.csv(paste0('../out/', dataset, '_1_summary_feed_0_losstype_linear_updatetype_online_ngrad_1_reg_0_lrate_0_pwgt_0.csv'))[,-1]
  cIdx <- cIdx + 1
  cNames[cIdx] <- 'Baseline'
  resFinal[cIdx] <- mean(X[ , 100])
  resArea[cIdx] <- mean(apply(X, 1, sum)/N2)

  for(updType in updTypes){
    for(lossType in lossTypes){
      for(reg in regs){
        for(ng in 1){
          for(lrate in lrates){
            for(pwt in pwts){
              X <- read.csv(paste0('../out/', dataset, '_1_summary_feed_100_losstype_', lossType, '_updatetype_', updType,
                                   '_ngrad_', ng, '_reg_', reg, '_lrate_', lrate, '_pwgt_', pwt, '.csv'))[,-1]
              cIdx <- cIdx + 1
              cNames[cIdx] <- paste(lossType, reg, lrate, pwt, sep = '-')
              resFinal[cIdx] <- mean(X[ , 100])
              resArea[cIdx] <- mean(apply(X, 1, sum)/N2)
            }
          }
        }
      }
    }
  }
  write.table(rbind(cNames, resFinal, resArea), file = paste0('Result_', dataset, '.csv'), sep = ',', row.names = F, quote = F, col.names = F)
}
