# Simulate the movement of n people according to the SLAW mobility model

import matplotlib.pyplot as plt
import numpy.random as rand
import math

# Place (add coordinates to the set points) count points between x_min, y_min and x_max, y_max
# level is the current level we are splitting the region into
# levels is the maximum level that we want to go to before placing the points
# Returns the number of points that were placed successfully
def addFractalPoints(x_min, x_max, y_min, y_max, points, levels, level, count, total_count):

   if count == 0 or x_min >= x_max or y_min >= y_max:
      # impossible space to place them on, or no points
      return 0

   # Find midpoint of square
   mid = ((x_min + x_max)/2.0, (y_min + y_max)/2.0)
   # we use normal distribution around the center for splitting, placing points
   # beta is a constant for the variance
   beta = min(x_max - x_min, y_max - y_min)

   if level > levels:
      # No more splitting, time to place the points
      for i in range(count):
         point = rand.normal(mid, 0.5*beta)
         points.add((point[0], point[1]))
         #points.append((rand.uniform(x_min, x_max), rand.uniform(y_min, y_max)))

      return count

   # We split again
   # Randomly move that midpoint a bit to introduce some variation in the
   # square sizes
   mid_samp = rand.normal(mid, 0.1*beta)

   # Randomly determine how the total number of points will be distributed
   # amongst the squares
   # alpha is the deviation
   alpha = 0.1*total_count/2**(0.8*level) #20.0/2**(0.8*level)
   # sample 4 values, one for each square
   count_samp = rand.normal(count/4.0, alpha, 4)

   # take out not allowed values
   count_samp = [min(max(0, x), count) for x in count_samp]

   # normalize the count for each square, turn into integer
   count_sum = max(sum(count_samp), 1)
   count_samp_norm = [int(max(0, round(count*x/count_sum))) for x in count_samp]

   # Place points on the smaller squares
   p1 = addFractalPoints(x_min, mid_samp[0], y_min, mid_samp[1], points, levels, level + 1, count_samp_norm[0], total_count)
   p2 = addFractalPoints(x_min, mid_samp[0], mid_samp[1], y_max, points, levels, level + 1, count_samp_norm[1], total_count)
   p3 = addFractalPoints(mid_samp[0], x_max, y_min, mid_samp[1], points, levels, level + 1, count_samp_norm[2], total_count)
   p4 = addFractalPoints(mid_samp[0], x_max, mid_samp[1], y_max, points, levels, level + 1, count_samp_norm[3], total_count)

   return p1 + p2 + p3 + p4

# Compute the location of k fractal points (locations to move to) over an area of size
# levels is the number splits that should be performed
def getFractalPoints(width, height, k, levels):

   # x and y coordinates, each a tuple
   points = set()

   count = addFractalPoints(0, width, 0, height, points, levels, 1, k, k)

   print "{} points added".format(count)

   return points

# For testing
if __name__ == "__main__":
   points = getFractalPoints(10000, 10000, 1000, 4)
   plt.axis([0, 10000, 0, 10000])
   x = [p[0] for p in points]
   y = [p[1] for p in points]
   plt.plot(x, y, 'ro')
   plt.axis('off')
   plt.savefig("fractal.png")
   #plt.show()


