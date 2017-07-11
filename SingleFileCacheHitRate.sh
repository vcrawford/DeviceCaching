# get the theoretical cache hit rate vs. the experimental cache hit rate

n=100
f=100
k=1
p=0.5
f_p=0.1
locations_file=ExperimentsData/Location/1/1/locations.txt
results_file=output.xml
radius=20
zipf_param=1
contact_days=5
simulation_start_day=6
epsilon=0.001
num_thresholds=1
threshold_size=1
first_rate=0.9
threshold_dec=0.2
cache_size=10

CacheSimulation/CacheSimulationSlaw $n $f $k $p $f_p $locations_file $results_file $radius $zipf_param $contact_days $epsilon $num_thresholds $threshold_size $first_rate $threshold_dec $cache_size


