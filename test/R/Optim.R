
lossLinear <- function(x, y, lambda){
  (y * sum(x) + lambda * sum(abs(x - 1)))
}

lossLogistic <- function(x, y, lambda){
  (log(1 + exp(y * sum(x))) + lambda * sum(abs(x - 1)))
}
gradLogistic <- function(x, y, lambda){
  div <- abs(x - 1)
  div[which(div == 0)] <- 1
  (y * (1 / (1 + exp(-y * sum(x)))) + lambda * ((x - 1) / div))
}


lossLinear(c(2,1), 1, 1)
lossLogistic(x = c(1,1), y = 1, lambda = 1)

gradLogistic(x = c(2,1), y = 1, lambda = 2)

optim(par = c(1,1), fn = lossLinear,   gr = NULL, y = 1, lambda = 0)
optim(par = c(1,1), fn = lossLogistic, gr = NULL, y = -1, lambda = .01)
optim(par = c(1,1), fn = lossLogistic, gr = gradLogistic, method = 'BFGS', y = -1, lambda = .01)

# logistic loss is convex loss function
X <- seq(-10,10,.1)
Y <- rep(0, length(X))
cnt <-0
for(x in X){
  cnt <- cnt + 1
  Y[cnt] <- lossLogistic(x, -1, 0)
}
plot(x = X, y = Y, type = 'l')
