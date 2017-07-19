# get the theoretical cache hit rate vs. the experimental cache hit rate

n=200
m=1000
locations_file=Location/locations_200.txt
results_file=CacheSimulation/output.xml
alg="greedy"
seed=$(date +%s)
radius=20
zipf=0.56
start_day=3
epsilon=0.001
cache_size=10
evolve=0
contact_graph_file=Location/graph_200.txt
evolve_portion=0.03
report_files=3
num_thresholds=3
thresholds="0 1 2"
cache_hit_rates="0.9 0.6 0.3"
#num_thresholds=4
#thresholds="9 19 49 99"
#cache_hit_rates="0.95 0.9 0.8 0.6"

CacheSimulation/CacheSimulationSlaw $n $m $k $cache_size $zipf $evolve $radius $epsilon $evolve_portion $start_day $locations_file $contact_graph_file $results_file $alg $seed $report_files $num_thresholds $thresholds $cache_hit_rates


