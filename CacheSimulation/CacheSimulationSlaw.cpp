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

   // cache hit rate for most popular k files
   double p = stof(argv[4]);

   // probability that there is a request for a file during any second
   double f_p = stof(argv[5]);

   // the file that has the locations for all of our devices
   string locations_file = argv[6];

   // the file to output results to
   string results_file = argv[7];

   // the radius that is considered in contact
   int radius = stoi(argv[8]);

   // parameter for our Zipf distribution
   double zipf_param = stof(argv[9]);

   // how many days are used for the contact graph computation
   int days = stoi(argv[10]);

   // epsilon for greedy
   double epsilon = stof(argv[11]);

   int num_thresholds = stoi(argv[12]);

   int threshold_size = stoi(argv[13]);

   double first_rate = stof(argv[14]);

   double threshold_dec = stof(argv[15]);

   // how many files a device can cache
   int cache_size = stoi(argv[16]);

   cout << endl << "=== Cache Simulation ===" << endl;

   cout << "Beginning simulation with " << n << " devices, where the " << k
        << " most popular of them are cached at any time." << endl;

   // Make a ranking for the files
   
   FileRanking fr (m);

   // make devices

   vector<Device> devices;

   for (int i = 0; i < n; i++) {

      devices.push_back(Device (i, m));
   }

   // and a D2D controller

   D2DController d2d (devices);

   // make object that handles all probability stuff

   FileRequest freq (m, n, zipf_param, f_p, fr); 

   // and a BS
   BaseStation bs (devices);

   // Now compute contact graph from location data 

   cout << "Computing contact graph from the first " << days << " days" << endl;

   vector< vector<double> > contact_graph;

   contactGraphFromLocation(n, radius, locations_file, days, contact_graph); 

   // Now build the graph object we will use for our algorithms
   Graph g (contact_graph);   

   // And a multi file cache graph
   CacheGraphMultiFile cg (g, n, cache_size, epsilon);

   // Finally, a cache controller
   vector<int> thresholds;
   vector<double> cache_hit_rates;

   int threshold_lower_bound = n - threshold_size - 1;
   double cache_hit_rate = first_rate;

   for (int i = 0; i < num_thresholds; i++) {

     thresholds.push_back(threshold_lower_bound);

     cache_hit_rates.push_back(cache_hit_rate);

     threshold_lower_bound -= threshold_size;

     cache_hit_rate -= threshold_dec;
   }

   CacheController cc (thresholds, cache_hit_rates, fr, cg);

   cout << "=== CACHE SIMULATION COMPLETE ===" << endl << endl;

}


