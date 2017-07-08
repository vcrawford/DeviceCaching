# Computes a results xml file that is the average of a few files
# Run like "python ComputeAverageResults.py match_val avg_val new_exp_id 
#            results1.xml ... outputresults.xml"
# match_val is how we match two results (on n, for example) and avg_val is what value
# we average (size, for example)

import xml.etree.ElementTree as et
import sys
import matplotlib.pyplot as plt

match_val = sys.argv[1]
avg_val = sys.argv[2]
new_exp_id = sys.argv[3]

# files to average
files = sys.argv[4:-1]
num_files = len(files)

#output file
output = sys.argv[-1]

#acceptable run types
run_types = ["greedy0", "exact"]

# takes each algorithm to match_val to avg_val
# For example, greedy => 100 => 25
results = {}

# how many have been added for each point
# takes algorithm to x to an integer
count = {}

for f in files:

   tree = et.parse(f)
   root = tree.getroot()

   for run in root:

      if run.tag in run_types:

         if run.tag not in results:

            results[run.tag] = {}
            count[run.tag] = {}

         x = float(run.findall(match_val)[0].text)
         
         if x not in results[run.tag]:

            results[run.tag][x] = 0
            count[run.tag][x] = 0

         y = float(run.findall(avg_val)[0].text)

         results[run.tag][x] = y + results[run.tag][x]

         count[run.tag][x] = count[run.tag][x] + 1

# divide by how many we added
for alg, points in results.iteritems():

   for x in points:

      results[alg][x] = results[alg][x]/count[alg][x]

# Write new results file
out = open(output, 'w')
out.write("<Experiment id=\"{}\">\n".format(new_exp_id))

for alg_type, points in results.iteritems():

   for x, y in sorted(points.iteritems()):
      out.write(" <" + alg_type + ">\n")
      out.write("  <" + match_val + ">{}</".format(x) + match_val + ">\n")
      out.write("  <" + avg_val + ">{}</".format(y) + avg_val + ">\n")
      out.write(" </" + alg_type + ">\n")

out.write("</Experiment>")

