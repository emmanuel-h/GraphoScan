data <- read.table("latence.bench", h=T)
dataSeq <- data[which(data$category=="seq"),]
dataPar = data[which(data$category=="par"),]
data3 <- cbind(dataSeq[,3], dataPar[,3])
colnames(data3) <- c("Séquentiel", "Parallèle")
rownames(data3) <- c(1:10)
barplot(t(data3), beside=T, xlab="Observations", ylab="Latence (ms)", cex.names=0.8, las=2, ylim=c(0,12), col=c("darkblue", "red"))
