
library(tsne, lib.loc = '~/R.lib')
# require(tsne)

numTop <- 50

Xorg <- read.csv('data/benchid_abalone960_regression_hard_0.408320542605121_level-1_1.29685177578128_0.01_0.00997375328083989_s_1.2887236673124.csv')

for(expl_type in c('SeqMarg','SeqDrop')){
  expl_in_file <- paste0('out/abalone960.', expl_type, '.csv')
  clust_out_dend_file <- paste0('clust_dedogram_', expl_type, '.pdf')
  expl_clust_plot_file <- paste0('expl_plot_clust_', expl_type, '.pdf')
  
  # aidx <- which(Xorg$ground.truth == 'anomaly')
  # plot(X)
  # points(X[aidx,], col='red', pch=19)
  
  explOrg <- read.csv(expl_in_file)
  
  sScore <- sort(explOrg$anomalyscore, decreasing = T, index.return = T)
  topIdx <- sScore$ix[1:numTop]
  
  X <- tsne(Xorg[topIdx,-(1:2)])
  
  expl <- as.matrix(explOrg[,-(1:2)])
  sp_cor = matrix(0, nrow = numTop, ncol = numTop)
  for(i in 1:(numTop-1)){
    for(j in (i+1):numTop){
      scor <- cor.test(expl[topIdx[i], ], expl[topIdx[j], ],  method = 'spearman')
      sp_cor[j,i] <- scor$estimate
    }
  }
  
  distance <- as.dist( 1 - sp_cor )
  
  clust <- hclust(distance)
  
  pdf(clust_out_dend_file, width = 9, height = 9)
  plot(clust)
  dev.off()
  
  clustercut <- cutree(clust, 3)
  
  pdf(expl_clust_plot_file, width = 9, height = 9)
  plot(X,
       col = c('red','green','blue','black','yellow')[clustercut],
       pch = c(15,17,19,8,11)[clustercut],
       xlab = 'X', ylab = 'Y')
  text(x = X[ ,1], y = X[ ,2]-25, labels = 1:numTop, cex = 0.4)
  dev.off()
}
