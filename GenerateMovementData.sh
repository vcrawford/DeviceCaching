# Run slaw
./Slaw/RunSlaw 100 2 8

# Run contact graph computation
./Slaw/ContactGraph 100

# image of contact graph
python ./ContactGraph/VisualizeContactGraph.py 0.1

# Visualize slaw data 
python ./Slaw/VisualizeMovement.py 100


