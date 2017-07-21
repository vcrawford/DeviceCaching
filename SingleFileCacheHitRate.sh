# get the theoretical cache hit rate vs. the experimental cache hit rate

time=$(date +%s)
m=1000
location="locations_"$time".txt"
results="results_"$time".xml"
log="log_"$time".txt"
radius=20
zipf=0.56
epsilon=0.001
cache_size=10
evolve=0
graph="graph_"$time".txt"
evolve_portion=0.03
report_files=3
num_thresholds=3
thresholds="0 1 2"
cache_hit_rates="0.9 0.6 0.3"
start_n=100
inc_n=10
end_n=500
#num_thresholds=4
#thresholds="9 19 49 99"
#cache_hit_rates="0.95 0.9 0.8 0.6"

for ((i=$start_n;i<=$end_n;i=i + $inc_n));
do

   Slaw/RunSlaw $i 15 8 1 $location
   Slaw/ContactGraph $i $location $radius $graph 3

   CacheSimulation/CacheSimulationSlaw $i $m $k $cache_size $zipf $evolve $radius $epsilon $evolve_portion 3 $location $graph $results $time $report_files $num_thresholds $thresholds $cache_hit_rates 2> $log

   rm $location
   rm $graph
done





