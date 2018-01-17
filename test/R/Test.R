
X <- read.csv('data/adult.data')

X <- read.csv('data/ann_thyroid_1v3_1.csv')

x <- -700:700
loss <- log(1+exp(x))
plot(x,log(loss),type = 'l')

l <- 2
x <- seq(-2,2,0.01)
y <- -x + l * abs(x - 1)
print(y)
plot(x, y, type = 'l')
