# Generate random BA graphs, and find nodes to get desired cache hit rate
time=$(date +%s)
timeout="5"
startn=20
endn=5000
incn=10
p=0.7
d=10
graph="bagraph_"$time".txt"
results="results_"$time".xml"
timedout=0

#Begin data file
echo "<Experiment id=\"BA_"$p"\">" >> $results

echo "<about>BAgraph,startn:$startn,endn:$endn,incn:$incn,d:$d,p:$p,timeout:$timeout</about>" >> $results

for ((i=$startn;i<=$endn;i=i + $incn));
do
   #Random BA graph
   RandomGraph/BarabasiAlbert $i $d $graph

   #Get cache nodes
   if [ $timedout -eq 0 ]
   then
      timeout $timeout Algorithms/RunMFP $p $results $graph 1
   else
      timeout $timeout Algorithms/RunMFP $p $results $graph 0
   fi

   if [ $? -eq 124 ]
   then
      timedout=1
   fi
done

echo "</Experiment>" >> $results

