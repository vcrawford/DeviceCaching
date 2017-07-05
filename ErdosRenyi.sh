# Generate random ER graphs, and find nodes to get desired cache hit rate
time=$(date +%s)
timeout="5h"
startn=20
endn=500
incn=10
deg=10
p=0.5
eps=0.001
ergraph="ergraph_"$time".txt"
results="results_"$time".xml"
n_vs_size="n_vs_size_"$time".png"
timedout=0

#Begin data file
echo "<Experiment>" >> $results

echo "<about>ERgraph,startn:$startn,endn:$endn,incn:$incn,deg:$deg,p:$p,eps:$eps,timeout:$timeout</about>" >> $results

for ((i=$startn;i<=$endn;i=i+5));
do
   #Random ER graph
   RandomGraph/ErdosRenyi $i $deg $ergraph

   #Get cache nodes
   if [ $timedout -eq 0 ]
   then
      timeout $timeout Algorithms/RunMFP $p $eps $results $ergraph 1
   else
      timeout $timeout Algorithms/RunMFP $p $eps $results $ergraph 0
   fi

   if [ $? -eq 124 ]
   then
      timedout=1
   fi
done

echo "</Experiment>" >> $results

python Plots/DataPlots.py $results $n_vs_size

