# Plots all results files in the current directory
# Assumes there are no other things that end with ".xml"

#python Plots/DataPlots.py $(echo *.xml) results.png 200

python Plots/DataPlots.py Plots/config.xml ExperimentsData/Algorithms/1/results_1499294592.xml ExperimentsData/Algorithms/1/results_1499294534.xml ExperimentsData/Algorithms/1/results_1499294567.xml results.png

#python Plots/DataPlots.py Plots/config.xml ExperimentsData/Algorithms/1/results_1499294592.xml results.png
