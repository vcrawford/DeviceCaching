n=30
k=10
beta=0.2
graph=ws_graph.txt
dot=ws_graph.dot
img=ws_graph.eps
threshold=0

RandomGraph/WattsStrogatz $n $k $beta $graph

python ContactGraph/VisualizeContactGraph.py $graph $dot $img $threshold

