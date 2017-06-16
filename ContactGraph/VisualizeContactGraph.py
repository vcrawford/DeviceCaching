# Take a contact graph file and output dot file visualizing it
# Call like python VisualizeContact.py contact.txt contact.dot 0.01

import sys
import subprocess

contact_data = sys.argv[1]
output_file = sys.argv[2]
min_edge_weight = float(sys.argv[3])

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

output.write("} \n")
output.close()
data_in.close()

subprocess.call(["dot", "-Tps", output_file, "-o", "contact.eps"])

