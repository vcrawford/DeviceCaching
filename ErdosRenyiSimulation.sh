n=100
p=0.9
deg=5
seconds=100000
reqprob=0.05

rm ergraph_sim_alg.xml

RandomGraph/ErdosRenyi $n $deg ergraph_sim.txt

echo "<Experiment>" >> ergraph_sim_alg.xml

Algorithms/RunMFP $p ergraph_sim_alg.xml ergraph_sim.txt 0

echo "</Experiment>" >> ergraph_sim_alg.xml

python CacheSimulation/CacheNodes.py 1 ergraph_sim_alg.xml ergraph_sim_nodes.txt

CacheSimulation/CacheSimulationGraph ergraph_sim.txt $seconds $reqprob ergraph_sim_nodes.txt


