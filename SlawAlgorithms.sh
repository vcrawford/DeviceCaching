# Generate random SLAW contact graphs, and find nodes to get desired cache hit rate
time=$(date +%s)
timeout="10m"
startn=20
endn=1000
incn=5
p=0.5
graph="slawgraph_"$time".txt"
location="location_"$time".txt"
results="results_"$time".xml"
timedout=0
num_days=5
start_time=8
radius=20
speed=1

#Begin data file
echo "<Experiment id=\"SLAW_"$p"\">" >> $results

echo "<about>SLAWgraph,startn:$startn,endn:$endn,incn:$incn,p:$p,timeout:$timeout</about>" >> $results

for ((i=$startn;i<=$endn;i=i + $incn));
do
   #Random SLAW contact graph
   ./Slaw/RunSlaw $i $num_days $start_time $speed $location
   ./Slaw/ContactGraph $i $location $radius $graph

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

