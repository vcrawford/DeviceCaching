x=n
y=time

# ER 0.3
output1="ExperimentsData/Algorithms/ER/er_0.3_avg_"$y".xml"
id1="ER_0.3_avg_"$y

to_avg1=ExperimentsData/Algorithms/ER/results_1499445409.xml
to_avg2=ExperimentsData/Algorithms/ER/results_1499445504.xml
to_avg3=ExperimentsData/Algorithms/ER/results_1499445521.xml

# n vs y of cache
python Plots/ComputeAverageResults.py $x $y $id1 $to_avg1 $to_avg2 $to_avg3 $output1

# ER 0.5
output1="ExperimentsData/Algorithms/ER/er_0.5_avg_"$y".xml"
id1="ER_0.5_avg_"$y

to_avg1=ExperimentsData/Algorithms/ER/results_1499445538.xml
to_avg2=ExperimentsData/Algorithms/ER/results_1499445551.xml
to_avg3=ExperimentsData/Algorithms/ER/results_1499445573.xml

# n vs y of cache
python Plots/ComputeAverageResults.py $x $y $id1 $to_avg1 $to_avg2 $to_avg3 $output1

# ER 0.7
output1="ExperimentsData/Algorithms/ER/er_0.7_avg_"$y".xml"
id1="ER_0.7_avg_"$y

to_avg1=ExperimentsData/Algorithms/ER/results_1499445587.xml
to_avg2=ExperimentsData/Algorithms/ER/results_1499445598.xml
to_avg3=ExperimentsData/Algorithms/ER/results_1499445609.xml

# n vs y of cache
python Plots/ComputeAverageResults.py $x $y $id1 $to_avg1 $to_avg2 $to_avg3 $output1

# ER 0.9
output1="ExperimentsData/Algorithms/ER/er_0.9_avg_"$y".xml"
id1="ER_0.9_avg_"$y

to_avg1=ExperimentsData/Algorithms/ER/results_1499445624.xml
to_avg2=ExperimentsData/Algorithms/ER/results_1499445635.xml
to_avg3=ExperimentsData/Algorithms/ER/results_1499445651.xml

# n vs y of cache
python Plots/ComputeAverageResults.py $x $y $id1 $to_avg1 $to_avg2 $to_avg3 $output1

