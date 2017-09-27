
dataset <- c("All_File_Features",
             "All_NetFlow_Features",
             "All_Process_Features",
             "Beaconing_Behavior",
             "Downloaded_File_Execution",
             "Exfil_Staging_File",
             "File_Executed_Stats",
             "File_MMap_Stats",
             "File_Modify_Event",
             "File_Permission_Event",
             "Netflow_Read_Stats",
             "Netflow_Read_Write_Rate_Lifetime",
             "NetFlow-related_File_Anomaly",
             "Netflow_Write_Stats",
             "Process_Directory_Scan",
             "Process_Exec_from_Network",
             "Process_File_Events",
             "Process_Memory_Events",
             "Process_Netflow_Events",
             "Process_Process_Events")

pdf(file = 'E2_Bovia_Webshell_Plots.pdf', width = 8, height = 8)

for(dataname in dataset){
  X <- read.csv(paste0('E2_Cadets_Bovia_Webshell/', dataname, '.csv'))
  nfeed <- 100
  if(nrow(X) < 100)
    nfeed <- 30
  
  orgX <- read.csv(paste0('out/E2_Cadets_Bovia_Webshell/', dataname, '_summary_feed0_type_reg.csv'))
  regX <- read.csv(paste0('out/E2_Cadets_Bovia_Webshell/', dataname, '_summary_feed', nfeed, '_type_reg.csv'))
  expX <- read.csv(paste0('out/E2_Cadets_Bovia_Webshell/', dataname, '_summary_feed', nfeed, '_type_exp.csv'))
  
  dir <- paste0('/scratch/Codes/IFPerceptron/pyaad/temp/', dataname,
                '/if_aad_trees100_samples256_i7_q1_bd', nfeed,
                '_nscore4_leaf_tau0.03_xtau_s0.5_ca1_cx0.001_d100')
  fName <- paste0(dataname,
                  '-iforest_tau_instance-trees100_samples256_nscore4_leaf-top-active-unifprior-Ca1-1_1-fid1-runidx10-bd', nfeed,
                  '-tau0_030-topK0-pseudoanom_always_False-optim_scipy-baseline.csv')
  baseIF <- read.csv(paste0(dir, '/', fName), header = F)
  fName <- paste0(dataname,
                  '-iforest_tau_instance-trees100_samples256_nscore4_leaf-top-active-unifprior-Ca1-1_1-fid1-runidx10-bd', nfeed,
                  '-tau0_030-topK0-pseudoanom_always_False-optim_scipy-num_seen.csv')
  feedIF <- read.csv(paste0(dir, '/', fName), header = F)
  
  avgOrg <- apply(orgX[ ,-1], 2, mean)
  avgReg <- apply(regX[ ,-1], 2, mean)
  avgExp <- apply(expX[ ,-1], 2, mean)
  avgBIF <- apply(baseIF[,-(1:2)], 2, mean)
  avgFIF <- apply(feedIF[,-(1:2)], 2, mean)
  
  plot(x = 1:nfeed, y = avgOrg, ylim = c(0,length(which(X[ ,1]=='anomaly'))), type = 'l', col='blue',
       xlab = '# Feedback', ylab = '# Anomaly', main = paste0(dataname,', # Anomaly = ', length(which(X[ ,1]=='anomaly')), ', # Inst = ', nrow(X)))
  abline(a = 0, b = 1, lty = 3)
  lines(x = 1:nfeed, y = avgBIF, col='blue', lty=2)
  lines(x = 1:nfeed, y = avgReg, col='red')
  lines(x = 1:nfeed, y = avgExp, col='red', lty=2)
  lines(x = 1:nfeed, y = avgFIF, col='green')
  
  legend(0, length(which(X[ ,1]=='anomaly')), legend=c("Base-IF-c++", "Base-IF-py", "Perceptron", "Perceptron-Exp", 'AAD-IF'),
         col=c("blue", "blue", "red", "red", "green"), lty=c(1,2,1,2,1))
}

dev.off()
