# Functions to visualize the Slaw Instance

import SlawInstance as si
import binascii
import matplotlib.pyplot as plt
import OpenGLInterface as opengl
import os
import random as rand

# Make a matplotlib that shows the locations of the fractal points in the slaw instance
# Each cluster has a random color assigned to it
# Output the image to file with name filename
def fractalPointsImage(slaw_instance, filename):

  for cluster in slaw_instance.fractal_clusters:
     x = [point[0] for point in cluster]
     y = [point[1] for point in cluster]
     random_color = binascii.hexlify(os.urandom(3))
     plt.plot(x, y, color = "#" + random_color, marker = 'o', linewidth=0)

  plt.axis([0, slaw_instance.area_side, 0, slaw_instance.area_side])
  plt.axis('off')

  plt.savefig(filename)

# intitialize the opengl instance that will visualize the walker movement
def initializeVisualization(slaw_instance):

  # All walkers get a random color for the visualization
  colors = []

  for walker in slaw_instance.walkers:
     colors.append((rand.uniform(0, 1), rand.uniform(0, 1), rand.uniform(0, 1)))

  # Start window that will show walkers
  opengl.colors = colors
  opengl.precision = 10.0
  opengl.radius = 5.0
  opengl.point_range = slaw_instance.area_side
  opengl.startVisualization("Slaw Device Movement")

# Updates the opengl image to have the current locations
def updateVisualization(slaw_instance):

   opengl.refreshProgram(slaw_instance.locations)

