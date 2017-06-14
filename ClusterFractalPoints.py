import FractalPoints as fractal
import binascii
import os
import matplotlib.pyplot as plt

# Finds all points in search_points that have a path to start_point
# Two nodes are connected if they are within r distance from each other
def searchForConnectedPoints(r, start_point, search_points):

   # Points we have found that are connected
   connected = set([start_point])

   # The points that we have queued to search the neighbors of
   # Should be a subset of connected
   search_neighbors = set([start_point])

   while search_neighbors:

      # Search the first element of search_neighbors
      point_a = search_neighbors.pop()

      # Look for neighbors
      for point_b in search_points:

         if point_b in connected:
            # Already have this looked at/to be looked at
            continue
         elif point_a == point_b:
            continue
         elif (point_a[0] - point_b[0])**2 + (point_a[1] - point_b[1])**2 <= r**2:
            # New point that is within r distance
            connected.add(point_b)
            search_neighbors.add(point_b)

   return connected 

# Take in fractal points in points and cluster them
# A cluster is a maximal connected component of points, where two points
# are connected if they are within r distance of each other
def computeClusters(r, points):

   # array of sets, each representing a cluster
   clusters = []

   # points that have not yet been put in a cluster
   unclustered = set(points)

   while unclustered:

      # Get cluster for this point
      point = unclustered.pop()
      connected = searchForConnectedPoints(r, point, unclustered)
      clusters.append(connected)
      unclustered = unclustered.difference(connected)

   return clusters

if __name__ == "__main__":
   points = fractal.getFractalPoints(10000, 10000, 1000, 4)
   clusters = computeClusters(200, points)
   plt.axis([0, 10000, 0, 10000])
   plt.axis('off')

   for cluster in clusters:
      x = [point[0] for point in cluster]
      y = [point[1] for point in cluster]
      random_color = binascii.hexlify(os.urandom(3))
      plt.plot(x, y, color = "#" + random_color, marker = 'o', linewidth=0)

   plt.savefig("fractal.png")
   #points = [(1, 2), (4, 2), (1, 1), (4, 4), (4, 3), (2, 2)]
   #print len(computeClusters(1, points))

