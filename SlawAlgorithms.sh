# Generate random SLAW contact graphs, and find nodes to get desired cache hit rate
time=$(date +%s)
timeout="2h"
startn=20
endn=500
incn=5
p=0.9
graph="SLAWFinal/slawgraph_"$time".txt"
location="Location/location_"$time
results="SLAWFinal/results_"$time".xml"
timedout=0
num_days=10
start_time=8
radius=20
speed=1

#Begin data file
echo "<Experiment id=\"SLAW_"$p"_4\">" >> $results

echo "<about>SLAWgraph,startn:$startn,endn:$endn,incn:$incn,p:$p,timeout:$timeout,radius:$radius,num_days:$num_days,start_time:$start_time,speed:$speed</about>" >> $results

for ((i=$startn;i<=$endn;i=i + $incn));
do
   #Random SLAW contact graph
   ./Slaw/RunSlaw $i $num_days $start_time $speed $location"_"$i".txt"
   ./Slaw/ContactGraph $i $location"_"$i".txt" $radius $graph $num_days

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

