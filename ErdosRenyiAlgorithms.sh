# Generate random ER graphs, and find nodes to get desired cache hit rate
time=$(date +%s)
timeout="2h"
startn=20
endn=5000
incn=5
deg_percent=0.1
p=0.9
ergraph="ERFinal/ergraph_"$time".txt"
results="ERFinal/results_"$time".xml"
n_vs_size="ERFinal/n_vs_size_"$time".png"
timedout=0

#Begin data file
echo "<Experiment id=\"ER_"$p"_4\">" >> $results

echo "<about>ERgraph,startn:$startn,endn:$endn,incn:$incn,deg:$deg,p:$p,timeout:$timeout</about>" >> $results

for ((i=$startn;i<=$endn;i=i + $incn));
do
   #Random ER graph
   RandomGraph/ErdosRenyi $i $deg_percent $ergraph

   #Get cache nodes
   if [ $timedout -eq 0 ]
   then
      timeout $timeout Algorithms/RunMFP $p $results $ergraph 1
   else
      timeout $timeout Algorithms/RunMFP $p $results $ergraph 0
   fi

   if [ $? -eq 124 ]
   then
      timedout=1
      incn=10
   fi
done

echo "</Experiment>" >> $results

