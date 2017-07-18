# Generate random BA graphs, and find nodes to get desired cache hit rate
time=$(date +%s)
timeout="2h"
startn=40
endn=5000
incn=5
p=0.9
perc=0.1
graph="BAFinal/bagraph_"$time".txt"
results="BAFinal/results_"$time".xml"
timedout=0

#Begin data file
echo "<Experiment id=\"BA_"$p"_4\">" >> $results

echo "<about>BAgraph,startn:$startn,endn:$endn,incn:$incn,perc:$perc,p:$p,timeout:$timeout</about>" >> $results

for ((i=$startn;i<=$endn;i=i + $incn));
do
   #Random BA graph
   RandomGraph/BarabasiAlbert $i $perc $graph

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
      incn=10
   fi
done

echo "</Experiment>" >> $results

