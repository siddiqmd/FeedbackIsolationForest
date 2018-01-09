
datasets <- c("ann_thyroid_1v3", "cardiotocography_1", "mammography", "yeast", "abalone")
updTypes <- c('online', 'stochastic', 'batch')
lossTypes <- c('linear', 'loglikelihood', 'logistic')
regs <- c('0', '0.0001', '0.001', '0.01', '0.1', '0.5')[1]
ngs <- c(1,10)[1]
lrates <- c(0,1)[2]
pwts <- c(0,1)[1]
numCols <- length(updTypes) * length(lossTypes) * length(regs) * length(ngs) *  length(lrates) * length(pwts) + 1

pdf(file = 'summary/AllBars_updTypes_lossTypes.pdf', width = 16, height = 9)
layout(matrix(c(1,1,2,2), 2, 2, byrow = TRUE), widths=c(1,1), heights=c(1,2))

for(dataset in datasets){
  resFinal <- rep(0, numCols)
  resArea <- rep(0, numCols)
  cNames <- rep('', numCols)
  cIdx <- 0
  X <- read.csv(paste0('../datasets/anomaly/', dataset, '/fullsamples/', dataset, '_1.csv'))
  numAnomaly <- length(which(X[ ,1]=='anomaly'))
  N <- min(numAnomaly,100) # normalizer 1
  N2 <- N*(N+1)/2 + (100-N)*N # normalizer 2

  # Baseline
  X <- read.csv(paste0('../out/', dataset, '_1_summary_feed_0_losstype_linear_updatetype_online_ngrad_1_reg_0_lrate_1_pwgt_0.csv'))[,-1]
  cIdx <- cIdx + 1
  cNames[cIdx] <- 'Baseline'
  resFinal[cIdx] <- mean(X[ , 100])
  resArea[cIdx] <- mean(apply(X, 1, sum)/N2)

  for(lossType in lossTypes){
    for(updType in updTypes){
      for(reg in regs){
        for(ng in ngs){
          for(lrate in lrates){
            for(pwt in pwts){
              X <- read.csv(paste0('../out/', dataset, '_1_summary_feed_100_losstype_', lossType, '_updatetype_', updType,
                                   '_ngrad_', ng, '_reg_', reg, '_lrate_', lrate, '_pwgt_', pwt, '.csv'))[,-1]
              cIdx <- cIdx + 1
              cNames[cIdx] <- paste(paste0('Loss=', lossType, ', ',updType),
                                    paste0('nGrad = ', ng, ', VarLRate = ', (1-lrate)),
                                    paste0('Pos.Wgt = ',pwt), sep = '\n')
              resFinal[cIdx] <- mean(X[ , 100])
              resArea[cIdx] <- mean(apply(X, 1, sum)/N2)
            }
          }
        }
      }
    }
  }
  par(mar=c(1,5,2,0))
  barplot(resArea, ylim = c(0,1), axes=T, ylab='Fractiion of Anomaly', main = paste0('dataset = ', dataset,', # of Anomaly = ', numAnomaly))
  grid(nx=NA, ny=NULL)
  par(mar=c(13,5,1,0))
  barplot(resFinal, ylim = c(0,min(numAnomaly,100)), names.arg = cNames, las = 3, ylab = '# of Anomaly')
  grid(nx=NA, ny=NULL)
  # write.table(rbind(cNames, resFinal, resArea), file = paste0('summary/Result_', dataset, '.csv'), sep = ',', row.names = F, quote = F, col.names = F)
}
dev.off()
