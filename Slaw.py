# The main script that we use to run SLAW
# Run like "python Slaw.py 100 2"

import VisualizeMovement as vm
import FractalPoints as fp
import Walker as w
import ClusterFractalPoints as cfp
import sys
import random as rand

# Get fractal points that our walkers will move around
fractal_points = fp.getFractalPoints(10000, 10000, 1000, 4)
fractal_clusters = cfp.computeClusters(300, fractal_points)

# Initialize all walkers
number_walkers = int(sys.argv[1])
walkers = w.randomWalkers(number_walkers, fractal_clusters, fractal_points)

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
vm.radius = 10.0
vm.point_range = 10000
vm.startVisualization("Slaw Device Movement")

for day in range(num_days):

   # Locations of all the walkers
   locations = []

   # Each walker should determine the route they will take today
   for walker in walkers:
      walker.pickRoute(fractal_clusters)
      walker.setLocation(walker.home) # Move home any walkers that are not at home
      locations.append(walker.home)

   for second in range(86400):

      for i, walker in enumerate(walkers):
         locations[i] = walker.updateLocation()

      # Update visualization
      vm.refreshProgram(locations)

