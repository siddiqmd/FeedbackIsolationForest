
datasets <- c("abalone", "landsat", "particle", "magic.gamma", "shuttle", "spambase", "yeast")
updTypes <- c('online', 'stochastic', 'batch')[1]
lossTypes <- c('linear', 'loglikelihood', 'logistic')
regs <- c('0', '0.0001', '0.001', '0.01', '0.1', '0.5')[1]
ngs <- c(0,1,10,50)[2]
lrates <- c('1', '0')
pwts <- c(0,1)[1]
inwgts <- c(0,1)
rtypes <- c(1,2)[2]
numCols <- 1 + length(updTypes) * length(lossTypes) * length(regs) * length(ngs) *  length(lrates) * length(pwts) * length(inwgts) * length(rtypes)

pdf(file = 'summary/ScaterPlot_Bench_lossTypes.pdf', width = 16, height = 9)
# layout(matrix(c(1,1,2,2), 2, 2, byrow = TRUE), widths=c(1,1), heights=c(1,2))

for(dataset in datasets){
  print(dataset)
for(numFeed in c(10, 20, 50, 100)){
  print(numFeed)

  X <- read.csv(paste0('../benchmarks_meta/meta_', dataset, '.csv'))
  numInst <- round(mean(X$size[which(X$benchnum > 900 & X$benchnum <= 1200)]), 2)
  numAnomaly <- round(mean(X$n.anom[which(X$benchnum > 900 & X$benchnum <= 1200)]), 2)
  resFinal <- matrix(0, nrow = 1*length(which(X$benchnum > 900 & X$benchnum <= 1200)),  ncol = numCols)
  resArea <- rep(0, numCols)
  colPool <- rep('', numCols)
  cNames <- rep('', numCols)
  total <- 0
  for(bid in 901:1200){
    if(bid %% 20 == 0) print(paste0('bid = ', bid))
    cIdx <- 0
    # X <- read.csv(paste0('../datasets/anomaly/', dataset, '/fullsamples/', dataset, '_1.csv'))
    N <- ceiling(min(numAnomaly,numFeed)) # normalizer 1
    div <- c(1:N, rep(N,numFeed-N))
    # N2 <- N*(N+1)/2 + (100-N)*N # normalizer 2
  
    # Baseline
    fName <- paste0('../benchmark/', dataset, '/out/', dataset, '_', bid, '_summary_feed_0_losstype_linear_updatetype_online_ngrad_1_reg_0_lrate_1_pwgt_0_inwgt_0_rtype_L2.csv')
    if(file.exists(fName) == F) next
    X <- read.csv(fName)[,-1]
    cIdx <- cIdx + 1
    cNames[cIdx] <- 'Baseline'
    total <- total + 1
    resFinal[(total-1)*1+1:1, cIdx] <- mean(X[ , numFeed])
    resArea[cIdx] <- resArea[cIdx] + mean(apply(X, 2, mean) / div)
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
                  # if(pwt == 1 && inwgt == 1) next
                  for(rtype in rtypes){
                    X <- read.csv(paste0('../benchmark/', dataset, '/out/', dataset, '_', bid, '_summary_feed_100_losstype_', lossType, '_updatetype_', 
                                         updType, '_ngrad_', ng, '_reg_', reg, '_lrate_', lrate, '_pwgt_', pwt, '_inwgt_', inwgt,'_rtype_L', rtype, '.csv'))[,-1]
                    cIdx <- cIdx + 1
                    
                    cNames[cIdx] <- paste0(#'reg=',reg,',',
                                           if(lossType == 'loglikelihood') 'llh' else substr(lossType,1,3),# ',', substr(updType,1,3),
                                           # ',ng=', ng,
                                           # ',vlr=', (1-lrate),
                                           ',lr=', lrate,
                                           # ',pwt=', pwt,
                                           ',mdsc=', inwgt
                                           # ',rt=',rtype
                                           )
                    resFinal[(total-1)*1+1:1, cIdx] <- mean(X[ , numFeed])
                    # resArea[cIdx] <- mean(apply(X, 1, sum)/N2)
                    resArea[cIdx] <- resArea[cIdx] + mean(apply(X, 2, mean) / div)
                    colPool[cIdx] <- paste0(if(lossType=='linear') 'red' else if(lossType=='loglikelihood') 'green' else 'blue', c(1,4)[((cIdx-1)%%2)+1])
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  MN <- min(resFinal[ ,c(1,2,6,10)])
  MX <- max(resFinal[ ,c(1,2,6,10)])
  plot(1:(total*1),  resFinal[1:(total*1) ,1],  lty = 1, col = 'black', 
       type = 'l', xlab = 'BenchID', ylab = '# Anomaly', ylim = c(MN, MX), 
       main = paste0('# Feedback = ', numFeed, ', dataset = ', dataset, ', lrate = 1, mirrorDescent = 0'))
  lines(1:(total*1), resFinal[1:(total*1) ,2],  lty = 2, col = 'green')
  lines(1:(total*1), resFinal[1:(total*1) ,6],  lty = 6, col = 'red')
  lines(1:(total*1), resFinal[1:(total*1) ,10], lty = 3, col = 'blue')
  legend(0, MX,
         legend=c("Baseline", "Linear", "Loglikelihood", "Logistic"),
         col=c("black", "green", "red", "blue"), lty=c(1,2,6,3))
  
  # break
  # print(paste0('Total = ', total))
  # par(mar=c(1,5,2,0))
  # barplot(resArea/total, ylim = c(0,1), axes=T, ylab='Avg Precision', col = colPool,
  #         main = paste0('Mirror Descent and variable learning rates, ', 'dataset = ', dataset, ', # of Inst = ', numInst, ', # of Anomaly = ', numAnomaly))
  # grid(nx=NA, ny=NULL)
  # par(mar=c(11,5,1,0))
  # loc <- barplot(resFinal/total, ylim = c(0,N), names.arg = cNames, las = 3, ylab = '# of Anomaly', col = colPool)
  # text(x = loc, y = resFinal/total+2, labels = as.character(round(resFinal/total,2)))
  # grid(nx=NA, ny=NULL)
  # write.table(rbind(cNames, resFinal, resArea), file = paste0('summary/Result_', dataset, '.csv'), sep = ',', row.names = F, quote = F, col.names = F)
}
}
dev.off()

