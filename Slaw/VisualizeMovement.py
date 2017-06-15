from OpenGL.GL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *
import numpy as np
import math as m

# size of our (square) window
window_side = 1000

# values for drawing the circles
precision = 20.0
radius = 10.0
point_range = 1000
colors = []

# Get points for a circle in order to draw
# center is the location on the screen
# precision determines how round the circle will appear (since it's really a polygon)
def circlePoints(center):

   points = []
   for angle in np.linspace(0, 2*np.pi, precision):

      point = radius*np.array([m.cos(angle), m.sin(angle)])

      points.append(point + center)
 
   return points 

# Draw a circle at x,y with radius radius, and RGB color color
def drawCircle(x, y, color = (1.0, 1.0, 1.0)):

   glBegin(GL_POLYGON) #start drawing

   points = circlePoints([x, y])

   glColor3f(color[0], color[1], color[2])

   for point in points:
      glVertex2f(point[0], point[1])

   glEnd() #done drawing


# Call this each time you want update the visualization
# for new points
def refreshProgram(points):

   # Scale down points to fit in our window
   convert_rate = float(window_side)/point_range
   converted_points = [(convert_rate*x, convert_rate*y) for (x, y) in points]

   #clear window, reset appearance
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
   glLoadIdentity()
   refresh2D()


   for i, point in enumerate(converted_points):
      if len(colors) <= i:
         drawCircle(point[0], point[1], (1.0, 1.0, 1.0))
      else:
         drawCircle(point[0], point[1], colors[i])

   #Not sure why, just have to do this
   glutSwapBuffers() 

# Makes our window have 2d appearance
def refresh2D():

   glViewport(0, 0, window_side, window_side)
   glMatrixMode(GL_PROJECTION)
   glLoadIdentity()
   glOrtho(0.0, window_side, 0.0, window_side, 0.0, 1.0)
   glMatrixMode(GL_MODELVIEW)
   glLoadIdentity()

# Initialize the visualization
# The window will have name name
# Will repeatedly call func_call
def startVisualization(name):
   #Initialize display stuff
   glutInit()
   glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH)
   glutInitWindowSize(window_side, window_side) #set window size
   glutInitWindowPosition(0, 0) #set window location
   window = glutCreateWindow(name) #make window

   #glutIdleFunc(refreshProgram)

   #glutMainLoop() #start!



