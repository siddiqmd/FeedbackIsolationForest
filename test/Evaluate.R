
dataname <- 'cardiotocography_1_1'
numFeedback <- 50

type <- c('seq_marg', 'seq_drop', 'rev_seq_marg', 'rev_seq_drop')

result <- matrix(0, nrow = 10, ncol = 5*numFeedback)

for(iter in 1:10){
  coln <- 0
  fName <- paste0('out/', dataname, '_iter', iter, '_0.csv')
  X <- read.csv(fName)
  score <- sort(X$score, decreasing = T, index.return = T)
  numAnom <- 0
  for(feed in 1:numFeedback){
    numAnom <- numAnom + (X$groundtruth[score$ix[feed]] == 'anomaly')
    coln <- coln + 1
    result[iter, coln] <- numAnom
  }
  for(t in 1:4){
    for(feed in 1:numFeedback){
      fName <- paste0('out/', dataname, '_', type[t], '_iter', iter, '_', numFeedback, '.csv')
      X <- read.csv(fName)
      score <- sort(X$score, decreasing = T, index.return = T)
      numAnom <- 0
      for(f in 1:feed){
        numAnom <- numAnom + (X$groundtruth[score$ix[f]] == 'anomaly')
      }
      coln <- coln + 1
      result[iter, coln] <- numAnom
    }
  }
}

write.csv(result, 'Result.csv', row.names = F, quote = F)
