
result <- read.csv(text = 'bid,mf0,mf1,mf2,mf3,mf4,mf5,df0,df1,df2,df3,df4,df5')
i <- 0
for(bid in c(1:100,401:500)){
  fName <- paste0('auc/AUC.abalone', bid, '.seq_marg.csv')
  X <- read.csv(fName, header = F)

  result[(1:10)+i, 1] <- bid
  result[(1:10)+i, 2:7] <- X
  
  fName <- paste0('auc/AUC.abalone', bid, '.seq_drop.csv')
  X <- read.csv(fName, header = F)
  
  result[(1:10)+i, 8:13] <- X
  
  i <- i + 10
}

write.csv(result, 'AUC.easy.medium.csv', row.names = F, quote = F)
