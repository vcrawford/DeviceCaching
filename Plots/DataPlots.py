# Generate plots from an XML results file
# Run like "python DataPlots.py results1.xml results2.xml ... output.png 100"

import xml.etree.ElementTree as et
import sys
import matplotlib.pyplot as plt

# all the results files
files = sys.argv[1:-2]

max_n = int(sys.argv[-1])

# each exp, alg pair gets its own marker/color
exp_types = ['ER', 'WS']
alg_types = ['greedy0', 'exact']

colors = {'ER':'r', 'WS':'b'}
markers = {'greedy0':'--', 'exact':'^'}
handles = []

for f in files:

   tree = et.parse(f)
   root = tree.getroot()
   exp_type = root.attrib["type"]

   for alg in alg_types:

      n = []
      size = []

      runs = root.findall(alg)

      for run in runs:
 
         single_n = int(run.findall('n')[0].text)

         if single_n <= max_n:

            n.append(single_n)
            size.append(int(run.findall('size')[0].text))

      plt.plot(n, size, colors[exp_type] + markers[alg], label=exp_type + alg)
      handles.append(exp_type+alg)

plt.legend(handles, loc=2)
plt.savefig(sys.argv[-2])

