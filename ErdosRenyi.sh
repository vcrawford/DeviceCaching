# Generate random ER graphs, and find nodes to get desired cache hit rate
# Varying n

#Remove old data file
rm results.txt

#Begin data file
echo "<Experiment>" >> results.txt

# Do first experiment with visualization
#Random ER graph
RandomGraph/ErdosRenyi 20 5 ergraph.txt

#Get cache nodes
timeout 10m Algorithms/RunMFP 0.95 0.001 results.txt ergraph.txt

cp ergraph.txt ergraph_first.txt 

for i in {25..75..5}
do
   #Random ER graph
   RandomGraph/ErdosRenyi $i 5 ergraph.txt

   #Get cache nodes
   timeout 10m Algorithms/RunMFP 0.95 0.001 results.txt ergraph.txt 
done

echo "</Experiment>" >> results.txt

# Do plot of first experiment with pink greedy nodes
python ContactGraph/VisualizeContactGraph.py ergraph_first.txt ergraph.dot ergraph.eps 0 results.txt

