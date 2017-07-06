# Generate random WS graphs, and find nodes to get desired cache hit rate
time=$(date +%s)
timeout="5h"
startn=20
endn=10000
incn=5
k=10
beta=0.2
p=0.5
graph="wsgraph_"$time".txt"
results="results_"$time".xml"
n_vs_size="n_vs_size_"$time".png"
timedout=0

#Begin data file
echo "<Experiment id=\"WS\">" >> $results

echo "<about>WSgraph,startn:$startn,endn:$endn,incn:$incn,k:$k,beta:$beta,p:$p,timeout:$timeout</about>" >> $results

for ((i=$startn;i<=$endn;i=i + $incn));
do
   #Random WS graph
   RandomGraph/WattsStrogatz $i $k $beta $graph

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

