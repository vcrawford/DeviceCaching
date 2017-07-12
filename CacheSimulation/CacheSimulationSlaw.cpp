// Simulates a bunch of devices moving and requesting a single file
// This is meant to be used with SLAW location data

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <random>
#include <map>
#include <queue>
#include <cmath>
#include <list>
#include "../ContactGraph/ContactGraph.cpp"
#include "../Algorithms/Graph.cpp"
#include "../Algorithms/GammaUtil.cpp"
#include "../Algorithms/CacheGraph.cpp"
#include "../Algorithms/Greedy.cpp"
#include "CacheGraphMultiFile.cpp"
#include "D2D.cpp"

using namespace std;

int main(int argc, char** argv) {

   // number of devices in this simulation
   int n = stoi(argv[1]);

   // number of potential cache files in this simulation
   int m = stoi(argv[2]);

   // number of files that should be cached at any time (assumed to be the most popular k)
   int k = stoi(argv[3]);

   // probability that there is a request for a file during any second
   double r_prob = stof(argv[4]);

   // the file that has the locations for all of our devices
   string locations_file = argv[5];

   // the file to output results to
   string results_file = argv[6];

   // the radius that is considered in contact
   int radius = stoi(argv[7]);

   // parameter for our Zipf distribution
   double zipf = stof(argv[8]);

   // how many days are used for the contact graph computation
   int days = stoi(argv[9]);

   // epsilon for greedy
   double epsilon = stof(argv[10]);

   int num_thresholds = stoi(argv[11]);

   int threshold_size = stoi(argv[12]);

   double top_rate = stof(argv[13]);

   double rate_dec = stof(argv[14]);

   // how many files a device can cache
   int cache_size = stoi(argv[15]);

   cout << endl << "=== Cache Simulation ===" << endl;

   // get a contact graph from file

   cout << "Computing contact graph from file " << locations_file << endl;

   vector< vector<double> > contact_graph;

   contactGraphFromLocation(n, radius, locations_file, days, contact_graph);

   Graph graph (contact_graph);   

   cout << "Beginning simulation with " << n << " devices, where the " << k
        << " most popular of them are cached at any time." << endl;

   D2DInstance sim (n, m, zipf, r_prob, graph, cache_size, epsilon, radius,
      num_thresholds, threshold_size, top_rate, rate_dec);

   cout << sim;

   cout << "=== CACHE SIMULATION COMPLETE ===" << endl << endl;

}


