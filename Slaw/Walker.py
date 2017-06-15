
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
# start_point is an element of points
def getRoute(points, start_point):

   # The route in order of visit
   route = [start_point]

   # To be added to the route
   unrouted = set(points)
   unrouted.remove(start_point)

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

# Pick a random cluster, and then pick a random 10% of its points
# Clusters is a list of disjoint sets of points
# Points is the set of points in all clusters
def randomPointsRandomCluster(clusters):

   points = set([x for cluster in clusters for x in cluster])

   # Random point, which we will find the cluster of and that will be the random cluster
   # Do it this way so that the probability of picking a cluster is according to its size
   random_point = rand.sample(points, 1)[0]
   cluster = [x for x in clusters if random_point in x][0]
   cluster_count = len(cluster)
	  
   # Now pick 10% random points in this cluster
   # Note that there is always at least one waypoint
   cluster_random_points = set([random_point])
   cluster_random_points_count = 0.10*cluster_count

   while len(cluster_random_points) < cluster_random_points_count:
      random_point = rand.sample(cluster, 1)[0]

      if random_point not in cluster_random_points:
         cluster_random_points.add(random_point)

   return [cluster, cluster_random_points]

# A walker according to the SLAW movement model 
class Walker:

   def __init__(self, clusters, waypoints, home, speed):

      # The set of clusters this walker regularly visits
      # Set of indices, not actual cluster points
      self.clusters = clusters

      # A set of waypoints, or coordinates, that the walker regularly visits
      # Each belongs to one of the clusters above
      self.waypoints = waypoints

      # The home waypoint of this walker (is in waypoints)
      self.home = home

      # The current location of the walker
      self.location = home

      # The number of meters that the walker will travel every time 
      # update location is called (which should be called every "second")
      self.speed = speed

   # Pick a route for the walker to take, starting and ending with home
   # To make a route, the walker makes a sequence of its waypoints using the
   # "Least Action Trip Planning" algorithm (LATP)
   # Clusters has all clusters, all points are in some cluster
   def pickRoute(self, clusters):

      # Choose random new points to go to
      # Pick a random cluster and then 10% of its points=
      unvisitedClusters = [clusters[i] for i in range(len(clusters)) if i not in self.clusters]
      randomPoints = randomPointsRandomCluster(unvisitedClusters)[1]

      # So we want to visit the usual spots plus these random, new ones
      self.route = getRoute(self.waypoints.union(randomPoints), self.home)

      # Which route the walker moves to. Start at the beginning.
      self.route_step = 0

      # Walkers start out moving
      self.seconds_to_wait = 0

      # Don't start route until this is set to true
      self.route_started = False

      return

   # Sets the location of the walker
   # Does not change anything with the route
   def setLocation(self, location):
      self.location = location

   # Get the walker to start moving on its route
   def startRoute(self):
      self.route_started = True

   # Computes the new location for the next second, assuming the walker is heading at 1 m/s to the next
   # point on its route at any point in time
   # Also pauses at each waypoint according to some distribution
   def updateLocation(self):

      # Don't move if route not started yet
      if not self.route_started:
         return self.location

      # We have completed our route
      if self.route_step >= len(self.route):
         return self.location

      # We are waiting at a waypoint
      if self.seconds_to_wait > 0:
         self.seconds_to_wait = self.seconds_to_wait - 1
         return self.location

      # Check if we are at our next stop
      next_waypoint = self.route[self.route_step]
      route_vector = (next_waypoint[0] - self.location[0], next_waypoint[1] - self.location[1])
      route_magnitude = math.sqrt(route_vector[0]**2 + route_vector[1]**2)
      
      # We are close enough. Time to go to next place
      if route_magnitude < self.speed:
          # Wait time is pulled from a pareto distribution truncated between 30 and 10000 seconds
          self.seconds_to_wait = int(max(30, min(nprand.pareto(0.25), 10000)))
          self.route_step = self.route_step + 1
          return self.location

      # Move forward along vector speed distance
      move_distance = self.speed*(1.0/route_magnitude)
      self.location = (self.location[0] + move_distance*route_vector[0],
                       self.location[1] + move_distance*route_vector[1])

      return self.location

# Generate n random walkers
# Each walker has 5 random clusters from each of which it picks 10% random waypoints
# Randomly picks a home point from among these waypoints
def randomWalkers(n, clusters, waypoints, speed):

   walkers = []

   # Generate random waypoints and home points and build a walker
   for i in range(n):
      # Build the nth walker

      # Input for walker constructor      
      random_clusters_indices = set()
      random_waypoints = set()

      # Clusters and waypoints that we have not yet randomly sampled
      unpicked_clusters = list(clusters)

      # Pick the 5 random clusters and their waypoints
      for i in range(5):

         # Random cluster and its points
         [cluster, points] = randomPointsRandomCluster(unpicked_clusters)

         # The index of this cluster in clusters
         index = [i for i in range(len(clusters)) if cluster == clusters[i]][0]

         # Add these for this walker
         random_clusters_indices.add(index)
         random_waypoints = random_waypoints.union(points)

         # Don't want to pick from this cluster again
         unpicked_clusters.remove(cluster)
        
      # Just let home be the first random waypoint
      home = rand.sample(random_waypoints, 1)[0]

      # Make walker
      walkers.append(Walker(random_clusters_indices, random_waypoints, home, speed))

   return walkers


# For testing
if __name__ == '__main__':
   points = fractal.getFractalPoints(10000, 10000, 1000, 4)
   clusters = cluster.computeClusters(300, points)
   walkers = randomWalkers(100, clusters, points, 1.0)

   walkers[0].pickRoute(clusters)
   walkers[1].pickRoute(clusters)
   walkers[2].pickRoute(clusters)

   plt.axis([0, 10000, 0, 10000])
   plt.axis('off')

   for cluster in clusters:
      x = [point[0] for point in cluster]
      y = [point[1] for point in cluster]
      random_color = binascii.hexlify(os.urandom(3))
      plt.plot(x, y, color = "#" + random_color, marker = '.', alpha=0.5, linewidth=0)

   for i, point in enumerate(walkers[0].route):
      plt.plot(point[0], point[1], 'ro')
      plt.annotate(str(i), xy = point)

   for i, point in enumerate(walkers[1].route):
      plt.plot(point[0], point[1], 'bo')
      plt.annotate(str(i), xy = point)

   #for i, point in enumerate(walkers[2].route):
   #   plt.plot(point[0], point[1], 'go')
   #   plt.annotate(str(i), xy = point)

   plt.savefig("fractal.png")


