
# updTypes <- c('online', 'stochastic', 'batch')[1]
# lossTypes <- c('linear', 'loglikelihood', 'logistic')
# regs <- c('0', '0.0001', '0.001', '0.01', '0.1', '0.5')[c(1,4,6)[1]]
# lrates <- c(0,1)
# pwts <- c(0,1)
# lnames <- as.vector(t(outer(
#             as.vector(t(outer(
#               as.vector(t(outer(
#                 lossTypes, regs, paste, sep='-'))), 
#                   lrates, paste, sep='-'))),
#                     pwts, paste, sep='-')))
# cols <- rep(c('red','green','blue','black'), 3)
# ltypes <- c(rep(1,4), rep(2,4), rep(6,4))

updTypes <- c('online', 'stochastic', 'batch')
lossTypes <- c('linear', 'loglikelihood', 'logistic')
regs <- c('0', '0.0001', '0.001', '0.01', '0.1', '0.5')[c(1,4,6)[1]]
lrates <- c(0,1)[2]
pwts <- c(0,1)[1]
lnames <- #as.vector(t(outer(
          #  as.vector(t(outer(
              as.vector(t(outer(
                lossTypes, updTypes, paste, sep='-')))#,
                  # lrates, paste, sep='-'))),
                  #   pwts, paste, sep='-')))
cols <- c(rep('red',3),rep('green',3),rep('blue',3))
ltypes <- rep(c(1,5,6),3)

pdf(file = paste0('pdf/Result_updateTypes.pdf'), width = 8, height = 8)
for(dataset in c("ann_thyroid_1v3", "cardiotocography_1", "mammography", "yeast", "abalone")){
  X <- read.csv(paste0('../out/', dataset, '_1_summary_feed_0_losstype_linear_updatetype_online_ngrad_1_reg_0_lrate_0_pwgt_0_inwgt_0.csv'))
  avg <- apply(X[ ,-1], 2, mean)
  X <- read.csv(paste0('../datasets/anomaly/', dataset, '/fullsamples/', dataset, '_1.csv'))
  plot(x = 1:100, y = avg, ylim = c(0,100), type = 'l', lty=1, col='black',
       xlab = '# Feedback', ylab = '# Anomaly', 
       main = paste0(dataset, ', # Anomaly = ', length(which(X[ ,1]=='anomaly')), ', # Inst = ', nrow(X)))
  abline(a = 0, b = 1, lty = 3)
  cnt <- 0
  for(lossType in lossTypes){
    for(updType in updTypes){
      for(reg in regs){
        for(ng in 1){
          for(lrate in lrates){
            for(pwt in pwts){
              X <- read.csv(paste0('../out/', dataset, '_1_summary_feed_100_losstype_', lossType, '_updatetype_', updType,
                                   '_ngrad_', ng, '_reg_', reg, '_lrate_', lrate, '_pwgt_', pwt, '_inwgt_0.csv'))
              avg <- apply(X[ ,-1], 2, mean)
              cnt <- cnt + 1
              lines(x = 1:100, y = avg, col=cols[cnt], lty=ltypes[cnt])
            }
          }
        }
      }
    }
    legend(0, 100, legend=lnames, col=cols, lty = ltypes, bty = "n")
  }
}
dev.off()
