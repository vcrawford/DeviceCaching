# Deals with keeping track of what walkers are in contact with other walkers
# Walkers are viewed as indices in 0,...,n
# Also writes contact data to file

import numpy as np
import math

# The distance (in meters) that walkers are considered in contact
contact_distance = 30

# Takes a WalkerContact object and prints to file as a contact graph
def printContactGraph(walker_contact, filename):

   contact_file = open(filename, 'w')

   max_contacts = walker_contact.contact[0][0]

   for i in walker_contact.walkers:
      for j in walker_contact.walkers:
         contact_value = walker_contact.contact[i][j]/max_contacts
         contact_file.write("{} ".format(contact_value))
      contact_file.write("\n")

   contact_file.close()

# Get the distance between two points
def get_distance(point_1, point_2):

   return math.sqrt((point_1[0] - point_2[0])**2 + (point_1[1] - point_2[1])**2)

class WalkerContact: 

   # locations is a list that takes the ith walker to its location
   def __init__(self, locations):

      # Keep track of locations so we only have to check contact for devices that
      # have moved
      self.locations = locations
      self.walkers = range(len(self.locations)) # save this to just make everything simpler

      # A matrix where the ith, jth entry is how long the ith and jth walkers have been
      # in contact in time_steps (usually seconds) over the entire simulation
      num_walkers = len(self.walkers)
      self.contact = np.zeros((num_walkers, num_walkers))

      # Find who is in contact
      self.updateContact(self.walkers)

   # An array of new locations for every walker
   def updateLocations(self, new_locations):

      self.locations = new_locations
      self.updateContact(self.walkers)

   # Update contact data for contact involving the walkers
   # Increments the contact between walkers by 1 if they are in contact
   def updateContact(self, walkers):

      # Go through each walker and update contacts that involve it
      for i in walkers:

         # The location of this walker
         location = self.locations[i]

         # Look at contact between i and every other walker (not just those passed in)
         for j in self.walkers:   

            other_location = self.locations[j] 

            distance = get_distance(location, other_location)

            if distance <= contact_distance:
               # These devices are in contact
               self.contact[i][j] = self.contact[i][j] + 1



