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
#include <queue>
#include "../Slaw/Locations.cpp"
#include "../Slaw/ContactGraphUtil.cpp"
#include "../Algorithms/Graph.cpp"
#include "../Algorithms/GammaUtil.cpp"
#include "../Algorithms/CacheGraph.cpp"
#include "../Algorithms/Greedy.cpp"
#include "../Algorithms/GreedyMultiFile.cpp"
#include "Statistics.cpp"
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
#include "CacheControllerNone.cpp"
#include "CacheControllerMaxHitRate.cpp"
#include "CacheControllerGreedy.cpp"
#include "CacheControllerRandom1.cpp"
#include "CacheControllerTop.cpp"
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

   // what number of files should be moving from the bottom 
   int evolve_num = stoi(argv[8]);

   // day that we start on (to not overlap with contact graph days)
   int start_day = stoi(argv[9]);

   // the file that has the locations for all of our devices
   string locations_file = argv[10];

   string contact_graph_file = argv[11];

   string results_file = argv[12];

   int seed = stoi(argv[13]);

   int report_files = stoi(argv[14]);

   // read in thresholds and their corresponding rates

   int num_thresholds = stoi(argv[15]);

   assert (argc >= 16 + 2*num_thresholds - 1);

   vector<int> thresholds;

   for (int i = 16; i < 16 + num_thresholds; i++) {

      thresholds.push_back(stoi(argv[i]));
   }

   vector<double> cache_hit_rates;

   for (int i = 16 + num_thresholds; i < 16 + 2*num_thresholds; i++) {

      cache_hit_rates.push_back(stof(argv[i]));
   }

   cout << endl << "=== Cache Simulation ===" << endl;

   if (report_files > 0) {

      cout << "Running single file caching experiments ..." << endl;

      // just reporting on the top files

      // always do greedy first, so we can get the numbers for how many to place
      // on the remaining algorithms
      string algs [2] = {"greedy", "random1"};
      //string algs[1] = {"greedy"};

      // to be used in non-greedy algorithms to decide how many of each file should
      // be placed
      vector<int> file_cache_count;

      for (int i = 0; i < 2; i++) {

         cout << endl << "Beginning simulation for algorithm " << algs[i] << endl;
 
         // read in contact graph from file

         clog << "Reading contact graph from file " << contact_graph_file << "." << endl;

         Graph graph (contact_graph_file);   

         clog << "Beginning simulation with " << n << " devices. Starting on day "
            << start_day << endl;

         // start locations at days number of days
         Locations loc (locations_file, n, start_day);

         int time = 0;

         unique_ptr<D2DInstance> sim;

         if (algs[i] == "greedy") {

            sim.reset(new D2DInstance (n, m, zipf, graph, cache_size, epsilon, radius,
               thresholds, cache_hit_rates, loc, evolve, evolve_num, algs[i], seed));
           
         }
         else if (algs[i] == "random1") {

            sim.reset(new D2DInstance (n, m, zipf, cache_size, radius,
               loc, algs[i], seed, file_cache_count));

         }

         while (sim->nextTimeStep()) {

            time++;
         };

         if (algs[i] == "greedy") {

            for (int i = 0; i < report_files; i++) {

               file_cache_count.push_back(sim->numDevicesCache(i));
            }
         }

         clog << "Simulation ran for " << time/86400 << " days." << endl;

         // write results to file

         ofstream output;
         output.open(results_file, ios_base::app);

         for (int i = 0; i < report_files; i++) {

            sim->printFileResults(output, i);     
         }

         clog << "Results appended to file " << results_file << endl;         
      }

   }
   else {

      cout << "Running multi file caching experiments ..." << endl;

      string algs [5] = {"greedy", "maxhitrate", "topfiles", "request", "none"};

      for (int i = 0; i < 5; i++) {

         cout << endl << "Beginning simulation for algorithm " << algs[i] << endl;
 
         // read in contact graph from file

         clog << "Reading contact graph from file " << contact_graph_file << "." << endl;

         Graph graph (contact_graph_file);   

         clog << "Beginning simulation with " << n << " devices. Starting on day "
            << start_day << endl;

         // start locations at days number of days
         Locations loc (locations_file, n, start_day);

         unique_ptr<D2DInstance> sim;

         if (algs[i] == "greedy") {

            sim.reset(new D2DInstance (n, m, zipf, graph, cache_size, epsilon, radius,
               thresholds, cache_hit_rates, loc, evolve, evolve_num, algs[i], seed));
           
         }
         else if (algs[i] == "maxhitrate") {

            sim.reset(new D2DInstance (n, m, zipf, graph, cache_size, radius,
               loc, evolve, evolve_num, algs[i], seed));
         }
         else if ((algs[i] == "topfiles") || (algs[i] == "request") ||
            (algs[i] == "none")) {

            sim.reset(new D2DInstance (n, m, zipf, cache_size, radius,
               loc, evolve, evolve_num, algs[i], seed));
         }

         int time = 0;

         while (sim->nextTimeStep()) {

            time++;
         };

         clog << "Simulation ran for " << time/86400 << " days." << endl;

         // write results to file

         ofstream output;
         output.open(results_file, ios_base::app);

         sim->printMultiFileResults(output);

         clog << "Results appended to file " << results_file << endl << endl;

      }
   }



   cout << "=== CACHE SIMULATION COMPLETE ===" << endl << endl;

}


