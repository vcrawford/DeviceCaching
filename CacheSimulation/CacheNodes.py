# Takes an xml file of experimental results, and outputs the cache nodes of the ith
# experiment (0,...) in the format that cache simulations need it
# Run like python CacheNodes.py 4 input.xml output.txt

import sys
import xml.etree.ElementTree as et

i = int(sys.argv[1])
inputfile = sys.argv[2]
outputfile = sys.argv[3]

tree = et.parse(inputfile)
root = tree.getroot()

cache_nodes = root[i].find('cache').text.split(',')[0:-1]

output = open(outputfile, 'w')

for node in cache_nodes:
   output.write(node + ",")

output.close()

