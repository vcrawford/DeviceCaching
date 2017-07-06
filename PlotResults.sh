# Plots all results files in the current directory
# Assumes there are no other things that end with ".xml"

python Plots/DataPlots.py $(echo *.xml) results.png 200

#python Plots/DataPlots.py ExperimentsData/Algorithms/1/results_1499294592.xml results.png 200

