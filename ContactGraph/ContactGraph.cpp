using namespace std;

// Takes in location data and computes contact graph from some number of the first days
// stores data in contact_graph
void contactGraphFromLocation(const int& n, const int& radius, const string& input,
                              const int& days, vector<vector <double>>& contact_graph) {

   ifstream location_data;
   location_data.open(input);

   // time in simulation
   int time = 0;

   // point we are looking at the location of
   int node = 0;

   // a location
   int x, y;

   // locations of each device in a second
   vector< pair<int, int> > locations (n, pair<int, int> (0, 0));

   contact_graph = vector< vector<double>> (n, vector<double> (n, 0));

   // now read in the location data
  
   string line;
   stringstream line_strm;

   while (getline(location_data, line)) {

      // read in point
      line_strm.clear();
      line_strm.str(line);
      line_strm >> x >> y;

      locations[node] = make_pair(x, y);

      if (node == (n - 1)) {

         // we got to the end of a time step, update contact data
         time++;

         // break out if this was enough days
         if (time > 60*60*24*days) break;

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

         node = -1; // will be incremented to 0

      } 

      node++;
   }

   // normalize contact graph by total time

   for (int i = 0; i < contact_graph.size(); i++) {

      for (int j = 0; j < contact_graph[i].size(); j++) {

         contact_graph[i][j] = contact_graph[i][j]/time;
      }
   }

}



