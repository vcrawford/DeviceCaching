# The main script that we use to run SLAW
# Run like "python Slaw.py 100 2 1" for 100 walkers over 2 days moving at 1 m/s

import VisualizeMovement as vm
import FractalPoints as fp
import Walker as w
import ClusterFractalPoints as cfp
import sys
import random as rand
import math
import matplotlib.pyplot as plt
import os
import binascii
import WalkerContact as wc

# Variables
area_side = 1000 # meters in each side of our area
num_waypoints = 2000 # how many stopping points over our area
levels = 6 # num of levels we use to distribute waypoints
contact_distance = 20 # meters that waypoints are considered "near" for fractal algo
device_contact_distance = 30 # meters that walkers are considered "near"

# Get fractal points that our walkers will move around
fractal_points = fp.getFractalPoints(area_side, area_side, num_waypoints, levels)
fractal_clusters = cfp.computeClusters(contact_distance, fractal_points)

# Display fractal points in image
for cluster in fractal_clusters:
   x = [point[0] for point in cluster]
   y = [point[1] for point in cluster]
   random_color = binascii.hexlify(os.urandom(3))
   plt.plot(x, y, color = "#" + random_color, marker = 'o', linewidth=0)

plt.axis([0, area_side, 0, area_side])
plt.axis('off')

plt.savefig("fractal.png")


# Initialize all walkers
number_walkers = int(sys.argv[1])
speed = int(sys.argv[3])
walkers = w.randomWalkers(number_walkers, fractal_clusters, fractal_points, speed)

# SIMULATION
# Loop through days, and then seconds, updating the current location of the walkers

num_days = int(sys.argv[2])

colors = []
# Determine random colors for each walker
for walker in walkers:
   colors.append((rand.uniform(0, 1), rand.uniform(0, 1), rand.uniform(0, 1)))

# Start window that will show walkers
vm.colors = colors
vm.precision = 10.0
vm.radius = 5.0
vm.point_range = area_side
vm.startVisualization("Slaw Device Movement")

for day in range(num_days):

   # Locations of all the walkers
   # The ith walker has the location in the ith entry
   locations = []

   # Each walker should determine the route they will take today
   for walker in walkers:
      walker.pickRoute(fractal_clusters)
      walker.setLocation(walker.home) # Move home any walkers that are not at home
      locations.append(walker.home)

   # Initialize our contact data
   walker_contact = wc.WalkerContact(locations)

   for second in range(86400):

      # Every day at 8am, the new routes are started
      if second == 28800:
         for walker in walkers:
            walker.startRoute()

      # Print the current time to the command line
      if (second % 60 == 0):
         hours = second/(60*60)
         minutes = second/(60)
         if hours > 0:
            minutes = minutes%(60*hours)
         sys.stdout.write("Day {}: {}:{} \r".format(day+1, hours, minutes))
         sys.stdout.flush()

      for i, walker in enumerate(walkers):
         locations[i] = walker.updateLocation()

      # Update visualization
      vm.refreshProgram(locations)

      # Update contact data
      walker_contact.updateLocations(locations)

   # Print this day's contact data
   wc.printContactGraph(walker_contact, "slaw_contact.txt")


