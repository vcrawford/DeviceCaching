// Takes in location data and compute contact graph from it

#include <fstream>
#include <cstdlib>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <list>
#include <cmath>
#include <map>

using namespace std;

int main(int agrc, char* argv[]) {

   ifstream location_data;
   location_data.open("locations.txt");   
   int node_count = atoi(argv[1]);
   int time = 1; // The time in simulation we are (in seconds from the beginning)
   int node = 0; // What point we are looking at the location x,y of
   int x, y; // the location we read in
   int radius = 30; // the radius that two devices are considered in contact
   vector< pair<int, int> > locations; // The locations of each device
   vector< vector <float> > contact_graph; // ith, jth entry is the time the ith device has been in contact with the
                                           // jth device over the entire simulation

   cout << "Generating contact data for " << node_count << " devices ..." << endl;

   // From the first second data, get locations, add blank maps to in_contact, and fill in contact_graph
   // with 0s
   string line;
   stringstream line_strm;
   while (getline(location_data, line)) {

      // read in point
      line_strm.clear();
      line_strm.str(line);
      line_strm >> x >> y;

      // Add first location
      locations.push_back(make_pair(x, y));

      // Add row in contact_graph
      contact_graph.push_back(vector<float> (node_count, 0.0));
 
      if (node == (node_count - 1)) {
         // we have got to the end of the first second
         break;
      }

      node++;
   }

   node = 0;
   int time_steps = 0;
   // Compute contact graph
   // doesn't include the first second, but that's okay
   while (getline(location_data, line)) {

      // read in point
      line_strm.clear();
      line_strm.str(line);
      line_strm >> x >> y;

      locations[node] = make_pair(x, y);

      if (node == (node_count - 1)) {
         // we got to the end of a time step, update contact data
         time_steps++;

         for (int i = 0; i < node_count; i++) {
            //cout << "Check contact for " << i << endl;
            // only check lower id nodes, since the graph is symmetric
            for (int j = 0; j < i; j++) {

               float distance = sqrt(pow(locations[i].first - locations[j].first, 2.0) +
                                     pow(locations[i].second - locations[j].second, 2.0));

               if (distance < radius) {
                 contact_graph[i][j]++;
                 contact_graph[j][i]++;
               }
            }
         }

         node = -1; // will be incremented to 0
      }       

      node++;
   }

   location_data.close();

   // write contact graph to file
   ofstream contact_data_file;
   contact_data_file.open("contact_graph.txt");

   for (int i = 0; i < contact_graph.size(); i++) {

      for (int j = 0; j < contact_graph[i].size(); j++) {

         contact_data_file << contact_graph[i][j]/time_steps << "  ";
      }

      contact_data_file << endl;
   }

   contact_data_file.close();

   cout << "Contact graph written to file." << endl;
}



