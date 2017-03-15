#! /bin/bash

make
rm result.bench
touch result.bench

for i in `seq 1 4`;
do
    ./../acquisition
done

echo "Moyenne : " >> result.bench
awk '{ total += $1; count++ } END { print total/(count-1) }' result.bench >> result.bench
cat result.bench
