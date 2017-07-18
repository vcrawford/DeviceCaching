using namespace std;

// Takes in location data and computes contact graph from some number of the first days
// stores data in contact_graph
void contactGraphFromLocation(const int& n, const int& radius, const string& input,
                              const int& days, vector<vector <double>>& contact_graph) {

   // this will give us the locations
   Locations loc (input, n);

   // time in simulation
   int time = 0;

   // point we are looking at the location of
   int node = 0;

   // a location
   int x, y;

   // locations of each device in a second
   vector< pair<int, int> > locations;

   contact_graph = vector< vector<double>> (n, vector<double> (n, 0));

   // now compute contact events

   for (int i = 0; i < 60*60*24*days; i++) {

      if (loc.nextStep(locations)) {

          for (int i = 0; i < n; i++) {

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

        
      }
      else {

         throw invalid_argument("There is not enough location data for the number of days "
                                 "for computing the contact graph");
      }
   }

   // normalize contact graph by total time

   for (int i = 0; i < contact_graph.size(); i++) {

      for (int j = 0; j < contact_graph[i].size(); j++) {

         contact_graph[i][j] = contact_graph[i][j]/(60*60*24*days);
      }
   }

}



