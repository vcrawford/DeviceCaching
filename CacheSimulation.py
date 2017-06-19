# Runs an instance of caching on mobile devices
# Devices move around in a fixed area. They make requests for files, which have been
# pre-emptively cached on some devices using minimum file placement algorithms
# The algorithms are run in C++ program, not python, but python calls them

# Call like "python CacheSimulation.py 100" for 100 devices

import Slaw.SlawInstance as si
import Slaw.VisualizeMovement as vm
import sys
import Slaw.WalkerContact as wc

num_walkers = int(sys.argv[1])

# Initialize the slaw instance
slaw_instance = si.SlawInstance(num_walkers)

# Print fractal points image
vm.fractalPointsImage(slaw_instance, "fractal_points.png")

# Generate routes for the first day
slaw_instance.generateRoutes()
slaw_instance.startRoutes()

# Class to deal with contact data
contact_data = wc.WalkerContact(slaw_instance.locations)

# Initialize the opengl window
vm.initializeVisualization(slaw_instance)

# First, compute the contact graph

# This is the number of days that will be for generating the
# contact graph.
contact_days = 5

for day in range(contact_days):
   for second in range(86400):

      slaw_instance.nextSecond()
      vm.updateVisualization(slaw_instance)
      contact_data.updateLocations(slaw_instance.locations)

      # Show the time
      if (second % 60 == 0):
         hours = second/(60*60)
         minutes = second/(60)
         if hours > 0:
            minutes = minutes%(60*hours)
         sys.stdout.write("Day {}: {}:{} \r".format(day+1, hours, minutes))
         sys.stdout.flush()

# Print contact graph to file
wc.printContactGraph(contact_data, "contact_graph.txt")

 
# Start the simulation
