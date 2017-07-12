# get the theoretical cache hit rate vs. the experimental cache hit rate

n=130
m=1000
k=1
r_prob=0.1
locations_file=SlawLocation/1/location_1499459583.txt
results_file=output.xml
radius=20
zipf=1
contact_days=3
simulation_start_day=6
epsilon=0.001
num_thresholds=5
threshold_size=3
top_rate=0.9
rate_dec=0.2
cache_size=10

CacheSimulation/CacheSimulationSlaw $n $m $k $r_prob $locations_file $results_file $radius $zipf $contact_days $epsilon $num_thresholds $threshold_size $top_rate $rate_dec $cache_size


