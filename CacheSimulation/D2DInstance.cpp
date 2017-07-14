// Classes and functions related to D2D

using namespace std;

// Has all of the classes above, controls everything
class D2DInstance {

   // all devices, in order of id
   vector<Device> devices;

   D2DController d2d_cont;

   FileRanking file_rank;

   RequestController req_cont;

   BaseStation bs;

   CacheController cache_cont;

   // generates locations
   Locations& locations;

   // the actual locations at the current time
   vector< pair<int, int> > current_locations;

   int time;

   public:

   // n is the number of devices
   // m is the number of files
   // zipf is the zipf parameter
   // r_prob is the probability at any second that there is a request for a file
   // g is the contact graph we are working with
   // cache_size is how many files each device can store
   // epsilon is for when we run the greedy algorithm
   // thresholds is the min rank number for each threshold
   // cache_hit_rates is the rates at which files in each threshold should be cached
   D2DInstance(const int& n, const int& m, const double& zipf, const double& r_prob,
      Graph& g, const int& cache_size, const double& epsilon, const int& radius, 
      const int& num_thresholds, const int& threshold_size, const double& top_rate,
      const double& rate_dec, Locations& locations):
      d2d_cont (devices, radius, current_locations), file_rank (m),
      req_cont (n, m, zipf, r_prob, file_rank), bs (devices),
      cache_cont (g, n, cache_size, epsilon, num_thresholds, threshold_size,
      top_rate, rate_dec, file_rank), time (0), locations (locations) {

      this->makeDevices(n);

      // cache popular files

      this->cachePopular();


      this->time = 0;

   }

   // Add initial num devices
   void makeDevices(const int& num) {
 
      // add devices in with ids 0,..,n-1
      for (int i = 0; i < num; i++) {

         this->devices.push_back(Device(i));
      }

   }

   // get any caching that must be done for popular files
   void cachePopular() {

      int file;

      vector<int> device_ids;

      while (this->cache_cont.takeNextToCache(file, device_ids)) {

         this->bs.newMulticast(file, device_ids);
      }

   }

   // take the entire simulation to the next time step
   void nextTimeStep() {

      this->time++;

      // get new locations
      this->locations.nextStep(this->current_locations);

      // have the BS updated any transmissions it has
      this->bs.nextTimeStep();

   }

   // Check for new requests and give them to either the d2d controller or bs
   bool getNewRequests() {

      int request_device, request_file;

      if (this->req_cont.getRequest(request_device, request_file)) {

         // set to true if we find a way to transfer the file other than by the BS
         bool transferred = false;

         // get all ids of devices caching this file
         vector<int> cache_nodes;

         for (int i = 0; i < this->devices.size(); i++) {

            if (this->devices[i].hasFile(request_file)) {

               cache_nodes.push_back(i);
            }
         }

         // if there are any, attempt d2d file transfer
         if (cache_nodes.size() != 0) {

            for (int i = 0; i < cache_nodes.size(); i++) {

               if (request_device == cache_nodes[i]) {

                  transferred = true;
                  break;
               }

               if (this->d2d_cont.tryD2D(cache_nodes[i], request_device, request_file)) {

                  // we can transfer this via D2D
                  transferred = true;
                  break;
               }
            }
         }

         // if d2d was not successful, just give to BS
         if (!transferred) {

            this->bs.newRequest(request_device, request_file);
         }
      }

   }

   friend ostream& operator<<(ostream&, D2DInstance&);

};

ostream& operator<<(ostream& os, D2DInstance& x) {

   //os << endl << x.file_rank;

   os << x.cache_cont;

   for (int i = 0; i < x.devices.size(); i++) {

      os << x.devices[i];
   }

   os << endl;

   return os;
}


