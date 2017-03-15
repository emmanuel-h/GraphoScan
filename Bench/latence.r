data <- read.table("latence.bench", header=T)
result <- (data$t3 - data$t2)
par(mfrow=c(1,2))
quart <- quantile(result)
latence <- result[result < quart[4]]
# modifier les boites à moustaches pour qu'elles soient à la même échelle / jolies
boxplot(result)
boxplot(latence)
