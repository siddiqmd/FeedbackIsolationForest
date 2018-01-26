
datasets <- c("ann_thyroid_1v3", "cardiotocography_1", "mammography", "yeast", "abalone", "shuttle_1v23567", "covtype", "kddcup")[-7]
updTypes <- c('online', 'stochastic', 'batch')[1]
lossTypes <- c('linear', 'loglikelihood', 'logistic')
regs <- c('0', '0.0001', '0.001', '0.01', '0.1', '0.5')[1]
ngs <- c(0,1,10,50)[2]
lrates <- c('0.05', '0.1', '0.3', '0.5', '0.8', '1', '2', '4', '10', '0')
pwts <- c(0,1)
inwgts <- c(0,1)
rtypes <- c(1,2)[2]
numCols <- 2 + length(updTypes) * length(lossTypes) * length(regs) * length(ngs) *  length(lrates) * length(pwts) * length(inwgts) * length(rtypes) * 3/4

numFeed <- 100

pdf(file = 'summary/AllBars_lossTypes_lrates_pwgt_mrdsc.pdf', width = 16, height = 9)
layout(matrix(c(1,1,2,2), 2, 2, byrow = TRUE), widths=c(1,1), heights=c(1,2))

for(dataset in datasets){
  resFinal <- rep(0, numCols)
  resArea <- rep(0, numCols)
  colPool <- rep('', numCols)
  cNames <- rep('', numCols)
  cIdx <- 0
  X <- read.csv(paste0('../datasets/anomaly/', dataset, '/fullsamples/', dataset, '_1.csv'))
  numInst <- nrow(X)
  numAnomaly <- length(which(X[ ,1] == 'anomaly'))
  N <- min(numAnomaly,numFeed) # normalizer 1
  div <- c(1:N, rep(N,numFeed-N))
  # N2 <- N*(N+1)/2 + (100-N)*N # normalizer 2

  # Baseline
  X <- read.csv(paste0('../out/', dataset, '_1_summary_feed_0_losstype_linear_updatetype_online_ngrad_1_reg_0_lrate_1_pwgt_1_inwgt_0_rtype_L2.csv'))[,-1]
  cIdx <- cIdx + 1
  cNames[cIdx] <- 'Baseline'
  resFinal[cIdx] <- mean(X[ , numFeed])
  resArea[cIdx] <- mean(apply(X, 2, mean) / div)
  colPool[cIdx] <- 'black'

  dir <- paste0('../AAD/', dataset)
  fName <- paste0(dataset,
                  '-iforest_tau_instance-trees100_samples256_nscore4_leaf-top-active-unifprior-Ca1-1_1-fid1-runidx10-bd100-tau0_030-topK0-pseudoanom_always_False-optim_scipy-num_seen.csv')
  X <- read.csv(paste0(dir, '/', fName), header = F)[,-c(1,2)]
  cIdx <- cIdx + 1
  cNames[cIdx] <- 'AAD'
  resFinal[cIdx] <- mean(X[ , numFeed])
  resArea[cIdx] <- mean(apply(X[ ,1:numFeed], 2, mean) / div)
  colPool[cIdx] <- 'yellow'
  
  for(lossType in lossTypes){
    for(updType in updTypes){
      for(reg in regs){
        for(ng in ngs){
          for(lrate in lrates){
            for(pwt in pwts){
              for(inwgt in inwgts){
                if(pwt == 1 && inwgt == 1) next
                for(rtype in rtypes){
                  X <- read.csv(paste0('../out/', dataset, '_1_summary_feed_', numFeed, '_losstype_', lossType, '_updatetype_', updType,
                                       '_ngrad_', ng, '_reg_', reg, '_lrate_', lrate, '_pwgt_', pwt, '_inwgt_', inwgt,'_rtype_L', rtype, '.csv'))[,-1]
                  cIdx <- cIdx + 1
                  
                  cNames[cIdx] <- paste0(#'reg=',reg,',',
                                         if(lossType == 'loglikelihood') 'llh' else substr(lossType,1,3),# ',', substr(updType,1,3),
                                         # ',ng=', ng,
                                         # ',vlr=', (1-lrate),
                                         ',lr=', lrate,
                                         ',pwt=', pwt,
                                         ',mdsc=', inwgt
                                         # ',rt=',rtype
                                         )
                  resFinal[cIdx] <- mean(X[ , numFeed])
                  # resArea[cIdx] <- mean(apply(X, 1, sum)/N2)
                  resArea[cIdx] <- mean(apply(X, 2, mean) / div)
                  colPool[cIdx] <- paste0(c('red','green','blue')[((cIdx-3)%%3)+1], if(lossType=='linear') '' else if(lossType=='loglikelihood') '4' else '2')
                }
              }
            }
          }
        }
      }
    }
  }
  par(mar=c(1,5,2,0))
  barplot(resArea, ylim = c(0,1), axes=T, ylab='Avg Precision', col = colPool,
          main = paste0('PosWgt and Mirror Descent with learning rates, ', 'dataset = ', dataset, ', # of Inst = ', numInst, ', # of Anomaly = ', numAnomaly))
  grid(nx=NA, ny=NULL)
  par(mar=c(11,5,1,0))
  barplot(resFinal, ylim = c(0,N), names.arg = cNames, las = 3, ylab = '# of Anomaly', col = colPool)
  grid(nx=NA, ny=NULL)
  # write.table(rbind(cNames, resFinal, resArea), file = paste0('summary/Result_', dataset, '.csv'), sep = ',', row.names = F, quote = F, col.names = F)
}
dev.off()
