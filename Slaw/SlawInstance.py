# The main thing that we use to run SLAW
# Run like "python Slaw.py 100 2 1 1" for 100 walkers over 2 days moving at 1 m/s with visualization

import FractalPoints as fp
import Walker as w
import ClusterFractalPoints as cfp

class SlawInstance:

   area_side = 1000 # meters in each side of our area
   num_waypoints = 2000 # how many stopping points over our area
   levels = 6 # num of levels we use to distribute waypoints
   contact_distance = 20.0 # meters that waypoints are considered "near" for fractal algo
   speed = 1.0 # The speed that our walkers will go at

   # Make a SLAW instance with number_walkers number of walkers
   def __init__(self, number_walkers):

      # Get fractal points that our walkers will move around
      self.fractal_points = fp.getFractalPoints(self.area_side, self.area_side, self.num_waypoints, self.levels)
      self.fractal_clusters = cfp.computeClusters(self.contact_distance, self.fractal_points)

      self.number_walkers = number_walkers

      # Make walkers
      self.walkers = w.randomWalkers(self.number_walkers, self.fractal_clusters, self.fractal_points, self.speed)

      # Locations of walkers, hasn't been determined yet
      # Once set, the ith entry is the location of the ith walker
      self.locations = []

   # Pick a route among the fractal points for each walker
   # Meant to be called each day
   # Routes have regularity and some random variation
   # Sets the locations of the walkers to be the first stop (the home) on the route
   def generateRoutes(self):

      self.locations = []

      for walker in self.walkers:
         walker.pickRoute(self.fractal_clusters)
         walker.setLocation(walker.home)
         self.locations.append(walker.home)

   # Walkers start moving along their routes when nextSeconds is called, after this has been called
   def startRoutes(self):

      for walker in self.walkers:
         walker.startRoute()

   # Moves all walkers according to their location after one second
   # Assumes that the appropriate entries in locations has been called
   def nextSecond(self):

      for i, walker in enumerate(self.walkers):
         self.locations[i] = walker.updateLocation()


