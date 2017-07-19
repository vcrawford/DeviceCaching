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
#include <functional>
#include <cassert>
#include <memory>
#include "../Slaw/Locations.cpp"
#include "../Slaw/ContactGraphUtil.cpp"
#include "../Algorithms/Graph.cpp"
#include "../Algorithms/GammaUtil.cpp"
#include "../Algorithms/CacheGraph.cpp"
#include "../Algorithms/Greedy.cpp"
#include "CacheGraphMultiFile.cpp"
#include "FileRanking.cpp"
#include "Device.cpp"
#include "FileTransmission.cpp"
#include "D2DController.cpp"
#include "FileRequest.cpp"
#include "DeviceRequest.cpp"
#include "RequestController.cpp"
#include "BaseStation.cpp"
#include "CacheController.cpp"
#include "CacheControllerGreedy.cpp"
#include "D2DInstance.cpp"

using namespace std;

int main(int argc, char** argv) {

   // number of devices in this simulation
   int n = stoi(argv[1]);

   // number of potential cache files in this simulation
   int m = stoi(argv[2]);

   // how many files a device can cache
   int cache_size = stoi(argv[3]);

   // parameter for our Zipf distribution
   double zipf = stof(argv[4]);

   // whether we want the file distribution to evolve
   bool evolve = (stoi(argv[5]) == 1);

   // the radius that is considered in contact
   int radius = stoi(argv[6]);

   // epsilon for greedy
   double epsilon = stof(argv[7]);

   // what portion of the bottom 90% of files should be moving to the top 10%
   // of files per day
   double evolve_portion = stof(argv[8]);

   // day that we start on (to not overlap with contact graph days)
   int start_day = stoi(argv[9]);

   // the file that has the locations for all of our devices
   string locations_file = argv[10];

   string contact_graph_file = argv[11];

   string results_file = argv[12];

   string alg = argv[13];

   int seed = stoi(argv[14]);

   int report_files = stoi(argv[15]);

   // read in thresholds and their corresponding rates

   int num_thresholds = stoi(argv[16]);

   assert (argc >= 17 + 2*num_thresholds - 1);

   vector<int> thresholds;

   for (int i = 17; i < 17 + num_thresholds; i++) {

      thresholds.push_back(stoi(argv[i]));
   }

   vector<double> cache_hit_rates;

   for (int i = 17 + num_thresholds; i < 17 + 2*num_thresholds; i++) {

      cache_hit_rates.push_back(stof(argv[i]));
   }

   cout << endl << "=== Cache Simulation ===" << endl;

   // read in contact graph from file

   cout << "Reading contact graph from file " << contact_graph_file << "." << endl;

   Graph graph (contact_graph_file);   

   cout << "Beginning simulation with " << n << " devices. Starting on day "
        << start_day << endl;

   // start locations at days number of days
   Locations loc (locations_file, n, start_day);

   D2DInstance sim (n, m, zipf, graph, cache_size, epsilon, radius,
      thresholds, cache_hit_rates, loc, evolve, evolve_portion, alg, seed);

   int time = 0;

   while (sim.nextTimeStep()) {

      time++;
   };

   cout << "Simulation ran for " << time/86400 << " days." << endl;

   // write results to file

   ofstream output;
   output.open(results_file, ios_base::app);

   if (report_files > 0) {

      for (int i = 0; i < report_files; i++) {

          sim.printFileResults(output, i);     
      }
   }

   cout << "Results written to file " << results_file << endl;

   cout << "=== CACHE SIMULATION COMPLETE ===" << endl << endl;

}


