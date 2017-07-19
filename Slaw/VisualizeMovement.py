# Functions to visualize the Slaw Instance

import binascii
import matplotlib.pyplot as plt
import OpenGLInterface as opengl
import os
import random as rand
import sys

# Make a matplotlib that shows the locations of the fractal points in the slaw instance
# Each cluster has a random color assigned to it
# Output the image to file with name filename
def fractalPointsImage(fractal_points_file, side_length):

  # Read in clusters  
  fractal_points = open(fractal_points_file, "r")

  for line in fractal_points:
     values = line.split(",")
     if "cluster" in values[0]:
        random_color = binascii.hexlify(os.urandom(3))
     else:
        [x, y] = values
        plt.plot(int(x), int(y), color = "#" + random_color, marker = 'o', linewidth=0)

  plt.axis([0, side_length, 0, side_length])
  plt.axis('off')

  plt.savefig("fractal_points.png")

# intitialize the opengl instance that will visualize the walker movement
def initializeVisualization(walker_count):

  # All walkers get a random color for the visualization
  colors = []

  for walker in range(walker_count):
     colors.append((rand.uniform(0, 1), rand.uniform(0, 1), rand.uniform(0, 1)))

  # Start window that will show walkers
  opengl.colors = colors
  opengl.precision = 10.0
  opengl.radius = 5.0
  opengl.point_range = 500
  opengl.startVisualization("Slaw Device Movement")

# Updates the opengl image to have the current locations
# locations is a list of location for each walker
def updateVisualization(locations):

   opengl.refreshProgram(locations)

if __name__ == "__main__":

   walker_count = int(sys.argv[1])

   print "Plotting fractal points ..."

   fractalPointsImage("fractal_points.txt", 500)

   print "Starting OpenGL simulation ...";

   initializeVisualization(walker_count);

   location_file = open(sys.argv[2], "r")

   walker = 0 # which walker's location we're reading
   second = 0
   locations = []

   for line in location_file:

      if (second % 60 == 0):
         hours = second/(60*60)
         minutes = second/(60)
         day = second/(86400)
         if hours > 0:
            minutes = minutes%(60*hours)
         if day > 0:
            hours = hours%(24*day)
         sys.stdout.write("Day {}: {}:{} \r".format(day, hours, minutes))
         sys.stdout.flush()

      [x, y] = line.split(" ")

      x = int(float(x));
      y = int(float(y));

      locations.append((x,y))

      if walker == (walker_count - 1):

         updateVisualization(locations)
         locations = []
         walker = 0
         second = second + 1
      else:

         walker = walker + 1


