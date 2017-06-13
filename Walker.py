# A walker according to the SLAW movement model 

import ClusterFractalPoints as cluster
import FractalPoints as fractal
import random as rand
import numpy.random as nprand
import matplotlib.pyplot as plt
import binascii
import os

class Walker:

   def __init__(self, waypoints, home):

      # A set of waypoints, or coordinates, that the walker regularly visits
      self.waypoints = waypoints

      # The home waypoint of this walker (is in waypoints)
      self.home = home


# Generate n random walkers
# Each walker has 5 random clusters from each of which it picks 10% random waypoints
# Randomly picks a home point from among these waypoints
def randomWalkers(n, clusters, waypoints):

   walkers = []

   # Generate random waypoints and home points and build a walker
   for i in range(n):
      # Build the nth walker
      
      # Get 10% of waypoints from 5 random clusters
      random_clusters = []
      random_waypoints = []

      while len(random_clusters) < 5:

         # Random waypoint, which we will find the cluster of and that will be the random cluster
         # (Unless we've already chosen that cluster)
         a = int(rand.uniform(0, len(waypoints) - 1))
         random_waypoint = waypoints[a]
         cluster_list = [x for x in clusters if random_waypoint in x and x not in random_clusters]

         if len(cluster_list) == 0:
            # This waypoint corresponds to a cluster we've already chosen
            continue

         # New cluster to add
         cluster = cluster_list[0]
         cluster_count = len(cluster)
         random_clusters.append(cluster)
                  
         # Now pick 10% random waypoints in this cluster
         # Note that there is always at least one waypoint
         cluster_random_waypoints = [random_waypoint]
         cluster_random_waypoints_count = 0.10*cluster_count

         while len(cluster_random_waypoints) < cluster_random_waypoints_count:
            a = int(rand.uniform(0, len(cluster) - 1))
            random_waypoint = cluster[a]

            if random_waypoint not in cluster_random_waypoints:
               cluster_random_waypoints.append(random_waypoint)

         # Add this cluster's random waypoints
         random_waypoints.extend(cluster_random_waypoints)

      # Just let home be the first random waypoint
      home = random_waypoints[0]

      # Make walker
      walkers.append(Walker(random_waypoints, home))

   return walkers


# For testing
if __name__ == '__main__':
   points = fractal.getFractalPoints(10000, 10000, 1000, 4)
   clusters = cluster.computeClusters(300, points)
   walkers = randomWalkers(5, clusters, points)

   plt.axis([0, 10000, 0, 10000])
   plt.axis('off')

   for cluster in clusters:
      x = [point[0] for point in cluster]
      y = [point[1] for point in cluster]
      random_color = binascii.hexlify(os.urandom(3))
      plt.plot(x, y, color = "#" + random_color, marker = '.', alpha=0.5, linewidth=0)

   for walker in walkers:
      x = [point[0] for point in walker.waypoints]
      y = [point[1] for point in walker.waypoints]
      random_color = binascii.hexlify(os.urandom(3))
      plt.plot(x, y, color = "#" + random_color, marker = 'o', linewidth=0)


   plt.savefig("fractal.png")


