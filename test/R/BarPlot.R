
datasets <- c("ann_thyroid_1v3", "cardiotocography_1", "mammography", "yeast", "abalone")
updTypes <- c('online', 'stochastic', 'batch')
lossTypes <- c('linear', 'loglikelihood', 'logistic')
regs <- c('0', '0.0001', '0.001', '0.01', '0.1', '0.5')[1]
ngs <- c(1,10)
lrates <- c(0,1)
pwts <- c(0,1)[1]
inwgts <- c(0,1)[1]
numCols <- length(updTypes) * length(lossTypes) * length(regs) * length(ngs) *  length(lrates) * length(pwts) + 1

pdf(file = 'summary/AllBars_updTypes_lossTypes_ngs_lrates.pdf', width = 16, height = 9)
layout(matrix(c(1,1,2,2), 2, 2, byrow = TRUE), widths=c(1,1), heights=c(1,2))

for(dataset in datasets){
  resFinal <- rep(0, numCols)
  resArea <- rep(0, numCols)
  colPool <- rep('', numCols)
  cNames <- rep('', numCols)
  cIdx <- 0
  X <- read.csv(paste0('../datasets/anomaly/', dataset, '/fullsamples/', dataset, '_1.csv'))
  numAnomaly <- length(which(X[ ,1]=='anomaly'))
  N <- min(numAnomaly,100) # normalizer 1
  div <- c(1:N, rep(N,100-N))
  # N2 <- N*(N+1)/2 + (100-N)*N # normalizer 2

  # Baseline
  X <- read.csv(paste0('../out/', dataset, '_1_summary_feed_0_losstype_linear_updatetype_online_ngrad_1_reg_0_lrate_1_pwgt_0_inwgt_0.csv'))[,-1]
  cIdx <- cIdx + 1
  cNames[cIdx] <- 'Baseline'
  resFinal[cIdx] <- mean(X[ , 100])
  resArea[cIdx] <- mean(apply(X, 2, mean) / div)
  colPool[cIdx] <- 'black'

  for(lossType in lossTypes){
    for(updType in updTypes){
      for(reg in regs){
        for(ng in ngs){
          for(lrate in lrates){
            for(pwt in pwts){
              X <- read.csv(paste0('../out/', dataset, '_1_summary_feed_100_losstype_', lossType, '_updatetype_', updType,
                                   '_ngrad_', ng, '_reg_', reg, '_lrate_', lrate, '_pwgt_', pwt, '_inwgt_', pwt, '.csv'))[,-1]
              cIdx <- cIdx + 1
              cNames[cIdx] <- paste(paste0(lossType, ',', updType, ',ng=', ng),
                                    paste0('vlr=', (1-lrate)), sep = ',')
              resFinal[cIdx] <- mean(X[ , 100])
              # resArea[cIdx] <- mean(apply(X, 1, sum)/N2)
              resArea[cIdx] <- mean(apply(X, 2, mean) / div)
              colPool[cIdx] <- if(lossType=='linear') 'red4' else if(lossType=='loglikelihood') 'green4' else 'blue4'
            }
          }
        }
      }
    }
  }
  par(mar=c(1,5,2,0))
  barplot(resArea, ylim = c(0,1), axes=T, ylab='Avg Precision', col = colPool,
          main = paste0('dataset = ', dataset,', # of Anomaly = ', numAnomaly))
  grid(nx=NA, ny=NULL)
  par(mar=c(15,5,1,0))
  barplot(resFinal, ylim = c(0,N), names.arg = cNames, las = 3, ylab = '# of Anomaly', col = colPool)
  grid(nx=NA, ny=NULL)
  # write.table(rbind(cNames, resFinal, resArea), file = paste0('summary/Result_', dataset, '.csv'), sep = ',', row.names = F, quote = F, col.names = F)
}
dev.off()
