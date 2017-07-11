
dataname <- 'cardiotocography_1_1'
numFeedback <- 50

type <- c('seq_marg', 'seq_drop', 'rev_seq_marg', 'rev_seq_drop')

result <- matrix(0, nrow = 5*numFeedback, ncol = 10)

for(iter in 1:10){
  cnt <- 0
  fName <- paste0('out/', dataname, '_iter', iter, '_0.csv')
  X <- read.csv(fName)
  score <- sort(X$score, decreasing = T, index.return = T)
  numAnom <- 0
  for(feed in 1:numFeedback){
    numAnom <- numAnom + (X$groundtruth[score$ix[feed]] == 'anomaly')
    cnt <- cnt + 1
    result[cnt, iter] <- numAnom
  }
  for(t in 1:4){
    for(feed in 1:numFeedback){
      fName <- paste0('out/', dataname, '_', type[t], '_iter', iter, '_', feed, '.csv')
      X <- read.csv(fName)
      score <- sort(X$score, decreasing = T, index.return = T)
      cnt <- cnt + 1
      result[cnt,iter] <- length(which(X$groundtruth[score$ix[1:feed]] == 'anomaly'))
    }
  }
}

write.csv(result, 'Result.csv', row.names = F, quote = F)
