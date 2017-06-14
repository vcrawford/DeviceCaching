
import ClusterFractalPoints as cluster
import FractalPoints as fractal
import random as rand
import numpy.random as nprand
import matplotlib.pyplot as plt
import binascii
import os
import math

#Calculate the distance of each point to location
def getDistances(points, location):
   
   distances = {}

   for point in points:
      distance = math.sqrt((point[0] - location[0])**2 + (point[1] - location[1])**2)
      distances[point] = distance

   return distances
     

# Pick a route for the walker to take, beginning with start_point, according to
# the "Least Action Trip Planning" algorithm (LATP)
# Assumes that points does not include start_point
def getRoute(points, start_point):

   # The route in order of visit
   route = [start_point]

   # To be added to the route
   unrouted = points

   # Add points to route until there are no more
   while unrouted:

      # Distances of all points to the point we just added
      distances = getDistances(unrouted, route[-1])
      distances_values = distances.values()
      distances_inv_sum = sum([1.0/x for x in distances_values])

      # Calculate probabilities for each point according to distances
      probabilities = {x: (1.0/distances[x])/distances_inv_sum for x in distances.keys()}

      random_number = rand.uniform(0, 1)
      probability_sum = 0

      for point, probability in probabilities.iteritems():

         probability_sum = probability_sum + probability

         if random_number <= probability_sum:
            route.append(point)
            unrouted.remove(point)
            break

   # End with the beginning
   route.append(start_point)

   return route 

# A walker according to the SLAW movement model 
class Walker:

   def __init__(self, waypoints, home):

      # A set of waypoints, or coordinates, that the walker regularly visits
      self.waypoints = waypoints

      # The home waypoint of this walker (is in waypoints)
      self.home = home

      # The current location of the walker
      self.location = home

   # Pick a route for the walker to take, starting and ending with home
   # To make a route, the walker makes a sequence of its waypoints using the
   # "Least Action Trip Planning" algorithm (LATP)
   # Additional waypoints are some points to visit in addition to the ones randomly picked
   def pickRoute(self, additionalWaypoints):

      points = self.waypoints + additionalWaypoints
      points.remove(self.home)

      self.route = getRoute(points, self.home)
        

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
   walkers = randomWalkers(100, clusters, points)

   extra_waypoints = []
   points_indices = range(len(points))

   for i in range(20):
      random_point = points[nprand.choice(points_indices)]
      if random_point not in extra_waypoints:
         extra_waypoints.append(random_point)

   #walkers[0].pickRoute(extra_waypoints)
   walkers[0].pickRoute([])

   plt.axis([0, 10000, 0, 10000])
   plt.axis('off')

   for cluster in clusters:
      x = [point[0] for point in cluster]
      y = [point[1] for point in cluster]
      random_color = binascii.hexlify(os.urandom(3))
      plt.plot(x, y, color = "#" + random_color, marker = '.', alpha=0.5, linewidth=0)

   #for walker in walkers:
   #   x = [point[0] for point in walker.waypoints]
   #   y = [point[1] for point in walker.waypoints]
   #   random_color = binascii.hexlify(os.urandom(3))
   #   plt.plot(x, y, color = "#" + random_color, marker = 'o', linewidth=0)


   for i, point in enumerate(walkers[0].route):
      plt.plot(point[0], point[1], 'ro')
      plt.annotate(str(i), xy = point)

   plt.savefig("fractal.png")


