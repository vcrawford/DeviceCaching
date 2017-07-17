# get the theoretical cache hit rate vs. the experimental cache hit rate

n=130
m=1000
k=1
dev_req=0.001
locations_file=SlawLocation/1/location_1499459583.txt
results_file=output.xml
radius=20
zipf=0.56
contact_days=3
epsilon=0.001
num_thresholds=14
threshold_size=10
top_rate=0.8
rate_dec=0.05
cache_size=10
evolve=1

CacheSimulation/CacheSimulationSlaw $n $m $k $dev_req $locations_file $results_file $radius $zipf $contact_days $epsilon $num_thresholds $threshold_size $top_rate $rate_dec $cache_size $evolve


