
require(tsne)

Xorg <- read.csv('data/adult.data')

X <- tsne(Xorg[,-7])
