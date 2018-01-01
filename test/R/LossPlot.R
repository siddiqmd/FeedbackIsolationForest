
for(dataset in c("abalone", "yeast", "ann_thyroid_1v3", "cardiotocography_1", "mammography")[1]){
  for(type in c('linear', 'loglikelihood')){
    pdf(file = paste0('Loss_', dataset, '_', type, '.pdf'), width = 8, height = 8)
    
    for(reg in c('0', '0.0001', '0.001', '0.01', '0.1')){
      for(ng in c(1,10)){
        X <- read.csv(paste0('../out/', dataset, '_1_cost_feed_100_losstype_', type, '_updatetype_', 'online', '_ngrad_', ng, '_reg_', reg, '.csv'))
        for(iter in 0){
          MAX <- max(apply(X[iter*4+(1:4),-1], 1, max))
          MIN <- min(apply(X[iter*4+(1:2),-1], 1, min))
          plot(x = 1:100, y = X[iter*4+1, -1], ylim = c(MIN,MAX), type = 'l', lty=1, col='black', xlab = '# Feedback', ylab = 'Loss', 
               main = paste0('LossType = ', type, ', UpdType = ', 'online', ', ng = ', ng, ', Reg = ', reg))
          lines(x = 1:100, y = X[iter*4+2, -1], col='red', lty=2)
          lines(x = 1:100, y = X[iter*4+3, -1], col='blue', lty=1)
          lines(x = 1:100, y = X[iter*4+4, -1], col='green', lty=2)
          legend(60, MAX,
                 legend=c("Current loss (Before update)", "Current loss (After update)", "Mean loss (Before update)", "Mean loss (after update)"),
                 col=c("black", "red", "blue", "green"), lty=c(1,2,1,2))
        }
      }
    }
    dev.off()
  }
}
