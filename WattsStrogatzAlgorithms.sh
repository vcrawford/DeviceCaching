# Generate random WS graphs, and find nodes to get desired cache hit rate
time=$(date +%s)
timeout="2h"
startn=40
endn=5000
incn=5
perc=0.1
beta=0.01
p=0.9
graph="WSFinal/wsgraph_"$time".txt"
results="WSFinal/results_"$time".xml"
n_vs_size="WSFinal/n_vs_size_"$time".png"
timedout=0

#Begin data file
echo "<Experiment id=\"WS_"$p"_4\">" >> $results

echo "<about>WSgraph,startn:$startn,endn:$endn,incn:$incn,k:$k,beta:$beta,p:$p,timeout:$timeout</about>" >> $results

for ((i=$startn;i<=$endn;i=i + $incn));
do
   #Random WS graph
   RandomGraph/WattsStrogatz $i $perc $beta $graph

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

