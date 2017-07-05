# Take a contact graph file and output dot file visualizing it
# Call like python VisualizeContact.py contact_graph.txt output_graph.dot ...
# output_graph.eps 0.01 cache_nodes.txt

import sys
import subprocess
import xml.etree.ElementTree as et

contact_data = sys.argv[1]
output_file = sys.argv[2]
output_file_im = sys.argv[3]
min_edge_weight = float(sys.argv[4])

cache_nodes = []
# If we want to plot cache nodes
# will plot the nodes for the first experiment
if len(sys.argv) > 5:
   cache_file = sys.argv[5]
   tree = et.parse(cache_file)
   root = tree.getroot()
   first_experiment = root[0]
   for data in first_experiment:
      if data.tag == "cache":
         cache_data = data.text.split(",")[0:-1]
         cache_nodes = [int(x) for x in cache_data]

output = open(output_file, 'w')
data_in = open(contact_data, 'r')

output.write("graph contact { \n")
output.write("overlap = false \n")
output.write("splines = true \n")
output.write("node [shape=circle, label=\"\", height=0.3, style=filled, fillcolor=deepskyblue2, color=deepskyblue3] \n")
output.write("edge [color=dimgray] \n")

node_i = 0
for line in data_in:

   values = line.split()

   node_j = 0
   for value in values:

      # Only need 1/2 of the contact since it is symmetric
      if node_i < node_j and float(value) > min_edge_weight:
         output.write("{} -- {} [penwidth={}] \n".format(node_i, node_j, value))
      node_j = node_j + 1

   node_i = node_i + 1

# Color cache nodes
for i in range(node_i):
   if i in cache_nodes:
      output.write("{} [fillcolor=pink]".format(i))

output.write("} \n")
output.close()
data_in.close()

subprocess.call(["dot", "-Tps", output_file, "-o", output_file_im])

