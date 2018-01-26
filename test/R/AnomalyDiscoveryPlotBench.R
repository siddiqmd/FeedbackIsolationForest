
datasets <- c("abalone", "landsat", "magic.gamma", "particle", "shuttle", "spambase", "yeast")
updTypes <- c('online', 'stochastic', 'batch')[1]
lossTypes <- c('linear', 'loglikelihood', 'logistic')
regs <- c('0', '0.0001', '0.001', '0.01', '0.1', '0.5')[1]
ngs <- c(0,1,10,50)[2]
lrates <- c('1', '0')[1]
pwts <- c(0,1)
inwgts <- c(0,1)
rtypes <- c(1,2)[2]
numCols <- 1 + length(updTypes) * length(lossTypes) * length(regs) * length(ngs) *  length(lrates) * length(pwts) * length(inwgts) * length(rtypes) * 3 / 4

pdf(file = 'summary/AnomalyDiscovery_Bench_lossTypes_pwgt_mrrDsc.pdf', width = 8, height = 8)
# layout(matrix(c(1,1,2,2), 2, 2, byrow = TRUE), widths=c(1,1), heights=c(1,2))

numFeed <- 100

for(dataset in datasets){
  print(dataset)

  X <- read.csv(paste0('../benchmarks_meta/meta_', dataset, '.csv'))
  numInst <- round(mean(X$size[which(X$benchnum > 900 & X$benchnum <= 1200)]), 2)
  numAnomaly <- round(mean(X$n.anom[which(X$benchnum > 900 & X$benchnum <= 1200)]), 2)
  resFinal <- matrix(0, nrow = numCols,  ncol = numFeed)
  colPool <- rep('', numCols)
  cNames <- rep('', numCols)
  total <- 0
  for(bid in 901:1200){
    if(bid %% 20 == 0) print(paste0('bid = ', bid))
    cIdx <- 0
    # X <- read.csv(paste0('../datasets/anomaly/', dataset, '/fullsamples/', dataset, '_1.csv'))
    # N <- ceiling(min(numAnomaly,numFeed)) # normalizer 1
    # div <- c(1:N, rep(N,numFeed-N))
    # N2 <- N*(N+1)/2 + (100-N)*N # normalizer 2
  
    # Baseline
    fName <- paste0('../bench/', dataset, '/out/', dataset, '_', bid, '_summary_feed_0_losstype_linear_updatetype_online_ngrad_1_reg_0_lrate_1_pwgt_0_inwgt_0_rtype_L2.csv')
    if(file.exists(fName) == F) next
    X <- read.csv(fName)[,-1]
    cIdx <- cIdx + 1
    total <- total + 1
    resFinal[cIdx, ] <- resFinal[cIdx, ] + apply(X, 2, mean)
    cNames[cIdx] <- 'Baseline'
    colPool[cIdx] <- 'black'

    
    # dir <- paste0('../AAD/', dataset)
    # fName <- paste0(dataset,
    #                 '-iforest_tau_instance-trees100_samples256_nscore4_leaf-top-active-unifprior-Ca1-1_1-fid1-runidx10-bd100-tau0_030-topK0-pseudoanom_always_False-optim_scipy-num_seen.csv')
    # X <- read.csv(paste0(dir, '/', fName), header = F)[,-c(1,2)]
    # cIdx <- cIdx + 1
    # cNames[cIdx] <- 'AAD'
    # resFinal[cIdx] <- mean(X[ , numFeed])
    # resArea[cIdx] <- mean(apply(X[ ,1:numFeed], 2, mean) / div)
    # colPool[cIdx] <- 'yellow'
    
    for(lossType in lossTypes){
      for(updType in updTypes){
        for(reg in regs){
          for(ng in ngs){
            for(lrate in lrates){
              for(pwt in pwts){
                for(inwgt in inwgts){
                  if(pwt == 1 && inwgt == 1) next
                  for(rtype in rtypes){
                    X <- read.csv(paste0('../bench/', dataset, '/out/', dataset, '_', bid, '_summary_feed_100_losstype_', lossType, '_updatetype_', 
                                         updType, '_ngrad_', ng, '_reg_', reg, '_lrate_', lrate, '_pwgt_', pwt, '_inwgt_', inwgt,'_rtype_L', rtype, '.csv'))[,-1]
                    cIdx <- cIdx + 1
                    resFinal[cIdx, ] <- resFinal[cIdx, ] + apply(X, 2, mean)
                    
                    cNames[cIdx] <- paste0(#'reg=',reg,',',
                                           if(lossType == 'loglikelihood') 'llh' else substr(lossType,1,3),# ',', substr(updType,1,3),
                                           # ',ng=', ng,
                                           # ',vlr=', (1-lrate),
                                           # ',lr=', lrate,
                                           ', pwt=', pwt,
                                           ', mdsc=', inwgt
                                           # ',rt=',rtype
                                           )
                    colPool[cIdx] <- paste0(if(lossType=='linear') 'red' else if(lossType=='loglikelihood') 'green' else 'blue', c('','','')[((cIdx-1)%%3)+1])
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  plot(x = 1:numFeed, y = resFinal[1, ]/total, ylim = c(0,min(numFeed,numAnomaly)), type = 'l', lty=1, col='black',
       xlab = '# Feedback', ylab = '# Anomaly',
       main = paste0(dataset, ', # bench = ', total, ', Avg # Anomaly = ', numAnomaly, ', Avg # Inst = ', numInst))
  for(cid in 2:numCols){
    lines(x = 1:numFeed, y = resFinal[cid, ]/total, col = colPool[cid], lty = c(1,2,6)[(cid-1)%%3+1])
  }
  legend(0, min(numFeed,numAnomaly),
         legend=cNames,
         col=colPool, lty=c(1,rep(c(1,2,6),3)))
}

dev.off()
