
dataset <- c("All File Features",
             "All NetFlow Features",
             "All Process Features",
             "Beaconing Behavior",
             "Downloaded File Execution",
             "Exfil Staging File",
             "File Executed Stats",
             "File MMap Stats",
             "File Modify Event",
             "File Permission Event",
             "Netflow Read Stats",
             "Netflow Read Write Rate Lifetime",
             "NetFlow-related File Anomaly",
             "Netflow Write Stats",
             "Process Directory Scan",
             "Process Exec from Network",
             "Process File Events",
             "Process Memory Events",
             "Process Netflow Events",
             "Process Process Events")

types <- c("reg", "updMstk.reg", "runAvg", "updMstk.runAvg",  "PA.reg", "updMstk.PA.reg", "PA.runAvg", "updMstk.PA.runAvg")[c(1,3,5,7)]
idx <- c(1,2,4,6,8,10,11)
legendNames <- c("Base-IF-c++", "Perceptron.Both", "Perceptron.Mstk", "RunAvg.Both", "RunAvg.Mstk", 
  'PA.both', 'PA.Mstk', 'PA.runAvg.both', 'PA.runAvg.Mstk', 'Base-IF-py', 'AAD')[idx]
linetypes <- c(1,1,2,1,2,1,2,1,2,2,1)[idx]
clrs <- c("black", "red", "red", "blue", "blue", "cyan", "cyan", "magenta", "magenta", "black", "green")[idx]


baseDir <- '/scratch/ADAPT/E2GroundTruth/CSVs'

for(directory in list.dirs(path = baseDir, full.names = F, recursive = F)[c(2,6,8,12,15)]){
  print(directory)
  pdf(file = paste0(directory, '_both.pdf'), width = 8, height = 8)

  for(dataname in dataset){
    print(paste0('--', dataname))
    X <- read.csv(paste0(baseDir, '/', directory, '/CSVsWithGT/', dataname, '.csv'))
    nfeed <- 100
    if(nrow(X) < 100)
      nfeed <- 30

    cnt <- 1
    S <- list()
    S[[cnt]] <- read.csv(paste0(baseDir, '/', directory, '/result/', dataname, '_summary_feed0_type_reg.csv'))
    for(type in types){
      cnt <- cnt + 1
      S[[cnt]] <- read.csv(paste0(baseDir, '/', directory, '/result/', dataname, '_summary_feed', nfeed, '_type_', type, '.csv'))
    }

    dir <- paste0('/scratch/ADAPT/E2GroundTruth/CSVs/', directory, '/pyaad/temp/', dataname,
                  '/if_aad_trees100_samples256_i7_q1_bd', nfeed,
                  '_nscore4_leaf_tau0.03_xtau_s0.5_ca1_cx0.001_d100')
    fName <- paste0(dataname,
                    '-iforest_tau_instance-trees100_samples256_nscore4_leaf-top-active-unifprior-Ca1-1_1-fid1-runidx10-bd', nfeed,
                    '-tau0_030-topK0-pseudoanom_always_False-optim_scipy-baseline.csv')
    if(file.exists(paste0(dir, '/', fName)) && file.exists(paste0(dir, '/', fName))){
      cnt <- cnt + 1
      S[[cnt]] <- read.csv(paste0(dir, '/', fName), header = F)[, -1]
      
      fName <- paste0(dataname,
                      '-iforest_tau_instance-trees100_samples256_nscore4_leaf-top-active-unifprior-Ca1-1_1-fid1-runidx10-bd', nfeed,
                      '-tau0_030-topK0-pseudoanom_always_False-optim_scipy-num_seen.csv')
      cnt <- cnt + 1
      S[[cnt]] <- read.csv(paste0(dir, '/', fName), header = F)[ ,-1]
    }
    avg <- list()
    for(i in 1:length(S)){
      avg[[i]] <- apply(S[[i]][ ,-1], 2, mean)
    }

    ymax <- max(10,length(which(X[ ,1]=='anomaly')))
    plot(x = 1:nfeed, y = avg[[1]], xlim=c(0,nfeed), ylim = c(0,ymax), type = 'l', lty=linetypes[1], col=clrs[1],
         xlab = '# Feedback', ylab = '# Anomaly', main = paste0(dataname,', # Anomaly = ', length(which(X[ ,1]=='anomaly')), ', # Inst = ', nrow(X)))
    abline(a = 0, b = 1, lty = 3)
    for(i in 2:length(avg)){
      lines(x = 1:nfeed, y = avg[[i]], lty=linetypes[i], col=clrs[i])
    }

    legend(0, ymax, legend=legendNames[1:cnt], col=clrs, lty=linetypes, cex = .9, bty = "n")
  }

  dev.off()
}
