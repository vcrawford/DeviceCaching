# Run slaw
./Slaw/RunSlaw 100 5 8

# Run contact graph computation
./Slaw/ContactGraph 100

# Run greedy algorithm
./Algorithms/Greedy 0.5

# image of contact graph
python ./ContactGraph/VisualizeContactGraph.py 0.1

# Visualize slaw data 
python ./Slaw/VisualizeMovement.py 100


