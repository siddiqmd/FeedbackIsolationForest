
pdf(file = paste0('pdf/LossPlots.pdf'), width = 16, height = 8)
for(dataset in c("ann_thyroid_1v3", "cardiotocography_1", "mammography", "abalone", "yeast")){
  for(ng in c(1,10)[1]){
    par(mfrow=c(1,2))
    for(lossType in c('linear', 'loglikelihood', 'logistic')){
      for(reg in c('0', '0.0001', '0.001', '0.01', '0.1', '0.5')[c(1,4,6)]){
        for(updType in c('online', 'stochastic', 'batch')){
          X <- read.csv(paste0('../out/', dataset, '_1_cost_feed_100_losstype_', lossType, '_updatetype_', updType, '_ngrad_', ng, '_reg_', reg, '.csv'))
          for(iter in 0){
            MAX <- max(apply(X[iter*4+(1:4),-1], 1, max))
            MIN <- min(apply(X[iter*4+(1:4),-1], 1, min))
            plot(x = 1:100, y = X[iter*4+1, -1], ylim = c(MIN,MAX), type = 'l', lty=1, col='black', xlab = '# Feedback', ylab = 'Loss', 
                 main = paste0(dataset, '-', lossType, '-', updType, '-', reg))#, '-', ng))
            lines(x = 1:100, y = X[iter*4+2, -1], col='red', lty=2)
            lines(x = 1:100, y = X[iter*4+3, -1], col='blue', lty=1)
            lines(x = 1:100, y = X[iter*4+4, -1], col='green', lty=2)
            legend(60, MAX,
                   legend=c("Current loss (Before update)", "Current loss (After update)", "Mean loss (Before update)", "Mean loss (after update)"),
                   col=c("black", "red", "blue", "green"), lty=c(1,2,1,2))
          }
          X <- read.csv(paste0('../out/', dataset, '_1_summary_feed_0_losstype_', lossType, '_updatetype_', updType, '_ngrad_', ng, '_reg_', reg, '.csv'))
          avg <- apply(X[1 ,-1], 2, mean)
          X <- read.csv(paste0('../datasets/anomaly/', dataset, '/fullsamples/', dataset, '_1.csv'))
          plot(x = 1:100, y = avg, ylim = c(0,100), type = 'l', lty=1, col='black',
               xlab = '# Feedback', ylab = '# Anomaly', main = paste0(dataset,', # Anomaly = ', length(which(X[ ,1]=='anomaly')), ', # Inst = ', nrow(X)))
          abline(a = 0, b = 1, lty = 3)
          X <- read.csv(paste0('../out/', dataset, '_1_summary_feed_100_losstype_', lossType, '_updatetype_', updType, '_ngrad_', ng, '_reg_', reg, '.csv'))
          avg <- apply(X[1 ,-1], 2, mean)
          lines(x = 1:100, y = avg, col='red', lty=6)
        }
      }
    }
  }
}
dev.off()
