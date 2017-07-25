
time=$(date +%s)
m=1000
location="locations_"$time".txt"
results="results_"$time".xml"
log="log_"$time".txt"
radius=20
zipf=0.56
epsilon=0.001
cache_size=5
evolve=1
graph="graph_"$time".txt"
#evolve_num=100
evolve_num=100
report_files=0
num_thresholds=3
#thresholds="9 49 99"
thresholds="9 19 99"
#thresholds="4 19 99"
#thresholds="4 9 99"
#thresholds="9 19 49"
#cache_hit_rates="0.9 0.6 0.3"
cache_hit_rates="0.9 0.6 0.3"
#cache_hit_rates="0.9 0.5 0.3"
#cache_hit_rates="0.9 0.6 0.3"
#cache_hit_rates="0.95 0.8 0.5"
n=200

echo "<Experiment id=\"D2DMF_1\">" >> $results

#for ((i=1;i<=10;i= i + 1));
for ((i=1;i<=1;i= i + 1));
do

   Slaw/RunSlaw $n 8 8 1 $location
   Slaw/ContactGraph $n $location $radius $graph 3
   CacheSimulation/CacheSimulationSlaw $n $m $cache_size $zipf $evolve $radius $epsilon $evolve_num 3 $location $graph $results $time $report_files $num_thresholds $thresholds $cache_hit_rates 2> $log

   #rm $log
   rm $location
   rm $graph
done

echo "</Experiment>" >> $results




