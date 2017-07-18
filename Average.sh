name=ExperimentsData/Algorithms/SLAW/SLAW_0.9_avg.xml
config=Plots/config_average.xml
newid=SLAW_0.9_avg

data1=ExperimentsData/Algorithms/ER/*.xml
data2=ExperimentsData/Algorithms/BA/*.xml
data3=ExperimentsData/Algorithms/WS/*.xml
data4=ExperimentsData/Algorithms/SLAW/*.xml

python Plots/ComputeAverageResults.py n size $newid $config $(echo $data1) $(echo $data2) $(echo $data3) $(echo $data4) $name

