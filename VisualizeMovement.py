from OpenGL.GL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *
import numpy as np
import math as m

#size of our window
win_width, win_height = 1000, 600

# Get points for a circle in order to draw
# center is the location on the screen
# precision determines how round the circle will appear (since it's really a polygon)
def circlePoints(center , radius = 1, precision = 20):

   points = []
   for angle in np.linspace(0, 2*np.pi, precision):

      point = radius*np.array([m.cos(angle), m.sin(angle)])

      points.append(point + center)
  
   return points 

# Draw a circle at x,y with radius radius, and RGB color color
def drawCircle(x, y, radius = 1, precision = 20, color = (0.0, 0.0, 0.0)):

   glBegin(GL_POLYGON) #start drawing

   points = circlePoints([x, y], radius, precision)

   glColor3f(color[0], color[1], color[2])

   for point in points:
      glVertex2f(point[0], point[1])

   glEnd() #done drawing


# points is a list of tuples, each tuple an x,y coordinate we want to draw
# Draw each of these points as a circle with radius radius
# colors is a list of 3-ples that gives the color for each circle
# Call this each time you want to update the visualization
def refreshProgram(points, radius, precision, colors):

   #clear window, reset appearance
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
   glLoadIdentity()
   refresh2D()

   for i, point in enumerate(points):
      drawCircle(point[0], point[1], radius, precision, colors[i])

   #Not sure why, just have to do this
   glutSwapBuffers() 

# Makes our window have 2d appearance
def refresh2D():
   glViewport(0, 0, win_width, win_height)
   glMatrixMode(GL_PROJECTION)
   glLoadIdentity()
   glOrtho(0.0, win_width, 0.0, win_height, 0.0, 1.0)
   glMatrixMode(GL_MODELVIEW)
   glLoadIdentity()

# Initialize the visualization
# The window will have name name
def startVisualization(name):
   #Initialize display stuff
   glutInit()
   glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH)
   glutInitWindowSize(win_width, win_height) #set window size
   glutInitWindowPosition(0, 0) #set window location
   window = glutCreateWindow(name) #make window

   refreshProgram([(0,0)], 100, 30, [(0.2, 0.4, 0.9)])
   glutMainLoop() #start!

# TESTING
if __name__ == "__main__":

   startVisualization("Device Movement")

