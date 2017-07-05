// Simulates a bunch of devices moving and requesting a single file
// This is meant to be used with SLAW location data

#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <random>

using namespace std;

int main(int argc, char** argv) {

   // number of devices in this simulation
   int n = atoi(argv[1]);

   // random number generator for file request
   mt19937 generator(random_device{}());
   uniform_real_distribution<float> unif_0_1 (0, 1); // computing if there is a request at all
   uniform_int_distribution<int> unif_n (0, n - 1); // which device requested the file

   // devices that cache the file for greedy algorithm
   ifstream cache_greedy;
   cache_greedy.open("cache_greedy.txt");

   string cache_greedy_str;
   getline(cache_greedy, cache_greedy_str);

   // search through the string and get all devices
   vector<int> greedy_devices;
   int search_start = 0;
   int pos;

   while ((pos = cache_greedy_str.find(",", search_start)) != string::npos) {

      greedy_devices.push_back(stoi(cache_greedy_str.substr(search_start, pos - search_start)));
      search_start = pos + 1;
   }

   cache_greedy.close();

   // Simulate caching requests, where at each time step devices probabilistically request
   // the file
   ifstream location_data;
   location_data.open("locations.txt");

   vector< pair<int, int> > locations; // the locations of the n devices
   int node = 0; // which node we are looking at the location of in the loop
   stringstream line_strm;
   string line;
   float request_probability = 0.015; // the probability that in a second a file will be requested
                                      // about 1 request per minute
   bool is_request;
   int requesting_device;
   int time = 0; // how many seconds in our simulation
   int x, y; // the location ints

   int num_requests = 0; // total number of file requests
   int cache_hit = 0; // the number of times a request has had a cache hit
   float radius = 30; // the distance that two devices are in contact

   vector<int> device_requests (n, 0);

   while (getline(location_data, line)) {

      // read in point
      line_strm.clear();
      line_strm.str(line);
      line_strm >> x >> y;

      if (locations.size() < n) {
         // hasn't been filled in yet
         locations.push_back(make_pair(x, y));
      }
      else {
         locations[node] = make_pair(x, y);
      }

      if (node == (n - 1)) {
         // we got to the end of a time step, update contact data
         // time to do request simulation
         time++;

         // Is there a request?
         
         is_request = ((unif_0_1(generator)) < request_probability);
         
         if (is_request) {

            num_requests++;

            // which device?
            requesting_device = unif_n(generator);
            device_requests[requesting_device]++;

            // check if it's in contact with a cache device
            for (int j = 0; j < greedy_devices.size(); j++) {
               pair<int, int> cache_device_location = locations[greedy_devices[j]];
               float distance = sqrt(pow(locations[greedy_devices[j]].first -
                                         locations[requesting_device].first, 2.0) +
                                     pow(locations[greedy_devices[j]].second -
                                         locations[requesting_device].second, 2.0));

               if (distance < radius) {
                  // cache hit
                  cache_hit ++;
                  break;
               }
            }
         }

         node = -1; // will be incremented to 0

      }


      node++;
   }

   cout << cache_hit/((float)num_requests) << endl;
}


