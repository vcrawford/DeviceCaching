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
#include "../ContactGraph/ContactGraph.cpp"

using namespace std;

int main(int agrc, char* argv[]) {

   // input parameters

   int node_count = atoi(argv[1]);
   string location_data_file = argv[2];
   int radius = atoi(argv[3]);
   string contact_graph_file = argv[4];
   int days = stoi(argv[5]);

   // generate contact graph

   vector< vector <double> > contact_graph;

   cout << "Generating contact data for " << node_count << " devices ..." << endl;

   contactGraphFromLocation(node_count, radius, location_data_file, days, contact_graph);

   // write contact graph to file
   ofstream contact_data_file;
   contact_data_file.open(contact_graph_file);

   for (int i = 0; i < contact_graph.size(); i++) {

      for (int j = 0; j < contact_graph[i].size(); j++) {

         contact_data_file << contact_graph[i][j] << "  ";
      }

      contact_data_file << endl;
   }

   contact_data_file.close();

   cout << "Contact graph written to file." << endl;
}



