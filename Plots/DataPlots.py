# Generate plots from an XML results file
# Run like "python DataPlots.py config.xml results1.xml results2.xml ... output.png"

import xml.etree.ElementTree as et
import sys
import matplotlib.pyplot as plt

#config file
config = sys.argv[1]

# all the results files
files = sys.argv[2:-1]

#output file
output = sys.argv[-1]

# read in config file

exps = []
alg_types = []
colors = {}
exp_names = {}
alg_names = {}
markers = {}

tree = et.parse(config)
root = tree.getroot()
exps_xml = root.findall("experiments")[0]

for exp_xml in exps_xml:

   exps.append(exp_xml.text)

   colors[exp_xml.text] = exp_xml.attrib["color"]

   exp_names[exp_xml.text] = exp_xml.attrib["name"]

alg_types_xml = root.findall("algorithm_types")[0]

for alg_type_xml in alg_types_xml:

   alg_types.append(alg_type_xml.text)

   markers[alg_type_xml.text] = alg_type_xml.attrib["marker"]

   alg_names[alg_type_xml.text] = alg_type_xml.attrib["name"]


# What we will measure on the x and y axis
x_axis = root.findall("x_axis")[0].text
y_axis = root.findall("y_axis")[0].text

# What value we will cap the x axis at
x_max = float(root.findall("x_max")[0].text)

# Now plot the data

handles = []

for f in files:

   tree = et.parse(f)
   root = tree.getroot()
   exp = root.attrib["id"]

   for alg in alg_types:

      x = []
      y = []

      runs = root.findall(alg)

      for run in runs:
 
         single_x = float(run.findall(x_axis)[0].text)

         if single_x <= x_max:

            x.append(single_x)
            y.append(float(run.findall(y_axis)[0].text))

      plt.plot(x, y, colors[exp] + markers[alg])
 
      handles.append(exp_names[exp] + " " + alg_names[alg])

plt.legend(handles, loc=2)
plt.savefig(sys.argv[-1])

