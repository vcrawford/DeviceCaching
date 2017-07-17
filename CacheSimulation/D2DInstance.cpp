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

   // takes each file id to how many times it has been requested
   map<int, int> num_requests;

   // takes each file id to how many times it has been located in a device's own cache
   map<int, int> own_cache;

   public:

   // n is the number of devices
   // m is the number of files
   // zipf is the zipf parameter
   // dev_req is the probability at any second that there is a request for a file
   // per device at the busiest time of day
   // g is the contact graph we are working with
   // cache_size is how many files each device can store
   // epsilon is for when we run the greedy algorithm
   // thresholds is the min rank number for each threshold
   // cache_hit_rates is the rates at which files in each threshold should be cached
   // evolve is whether we want file popularity to evolve
   D2DInstance(const int& n, const int& m, const double& zipf, const double& dev_req,
      Graph& g, const int& cache_size, const double& epsilon, const int& radius, 
      const int& num_thresholds, const int& threshold_size, const double& top_rate,
      const double& rate_dec, Locations& locations, const bool& evolve):
      d2d_cont (devices, radius, current_locations), file_rank (m, evolve),
      req_cont (n, m, zipf, dev_req, file_rank, 0), bs (devices),
      cache_cont (g, n, cache_size, epsilon, num_thresholds, threshold_size,
      top_rate, rate_dec, file_rank), time (0), locations (locations) {

      this->makeDevices(n, cache_size);

      // add popular files to be multicasted
      this->cachePopular();

      this->time = 0;

   }

   // Add initial num devices
   void makeDevices(const int& num, const int& cache_size) {
 
      // add devices in with ids 0,..,n-1
      for (int i = 0; i < num; i++) {

         this->devices.push_back(Device(i, cache_size));
      }

   }

   // get any caching that must be done for popular files
   void cachePopular() {

      int file;

      vector<int> device_ids;

      while (this->cache_cont.takeNextToCache(file, device_ids)) {

         this->bs.newCache(file, device_ids);
      }

   }

   // take the entire simulation to the next time step
   // if there doesn't exist any more contact data, returns false
   bool nextTimeStep() {

      this->time++;

      clog << endl << "***Beginning next time step (t = " << this->time << ")..." << endl;

      // update file popularity (if we choose to do that in this simulation)

      this->file_rank.nextTimeStep();

      // get new locations

      if (!this->locations.nextStep(this->current_locations)) {

         return false;
      }

      // give any new requests to BS or D2D

      int device, file;

      if (this->req_cont.nextTimeStep(this->time, device, file)) {

         this->startTransmitNewRequest(device, file);
      }

      // to get any transmissions that have failed
      // for example, devices moved away from each other
      vector<D2DTransmission> failed;

      this->d2d_cont.nextTimeStep(failed);

      // give any failed D2D communications to the BS
      this->bs.takeFailedD2D(failed);

      // have the BS updated any transmissions it has
      this->bs.nextTimeStep();

      return true;

   }

   // updates the total number of cache hits from a device's own cache
   void countOwnCacheHit(const int& file) {

      if (this->own_cache.find(file) == this->own_cache.end()) {

         this->own_cache.insert( make_pair(file, 0) );
      }

      this->own_cache[file]++;
   }

   // updates the total number of requests that have occurred for each file
   void countRequest(const int& file) {

      if (this->num_requests.find(file) == this->num_requests.end()) {

         this->num_requests.insert( make_pair(file, 0) );
      }

      this->num_requests[file]++;
   }

   // Check for new requests and give them to either the d2d controller or bs
   void startTransmitNewRequest(const int& request_device, const int& request_file) {

      clog << "New request from device " << request_device << " for file "
         << request_file << "..." << endl;

      // check if this device is already downloading any file
      if (this->devices[request_device].is_downloading) {

         // don't want to do anything here, devices can't download multiple files at the
         // same time
         clog << "This device is currently already downloading a file" << endl;

         return;
      }

      this->countRequest(request_file);

      // first of all, see if this device has the file cached
      if (this->devices[request_device].hasFile(request_file)) {

         this->countOwnCacheHit(request_file);

         clog << "The requesting device has the file cached" << endl;

         return;
      }

      // get all ids of devices caching this file
      vector<int> cache_nodes;

      for (int i = 0; i < this->devices.size(); i++) {

         if (this->devices[i].hasFile(request_file)) {

            cache_nodes.push_back(i);
         }
      }

      clog << "There are " << cache_nodes.size() << " nodes that cache that file" << endl;

      // if there are any, attempt d2d file transfer
      if (cache_nodes.size() != 0) {

         for (int i = 0; i < cache_nodes.size(); i++) {

            if (this->d2d_cont.tryD2D(cache_nodes[i], request_device, request_file)) {

               return;
            }
         }
      }

      // d2d was not successful

      this->bs.newRequest(request_device, request_file); 

   }

   // compute how much of the cache space of devices is used
   // assumes all devices have the same size cache greater than 0,
   // and that there is at least one device
   double percentCacheSpaceUsed() {

      assert (this->devices.size() > 0);
      assert (this->devices[0].cache_size > 0);

      double used_space = 0;

      for (int i = 0; i < this->devices.size(); i++) {

         used_space += this->devices[i].numberCached();
      }

      return used_space/(this->devices[0].cache_size*this->devices.size());
   }

   friend ostream& operator<<(ostream&, D2DInstance&);

};

ostream& operator<<(ostream& os, D2DInstance& x) {

   //os << endl << x.file_rank;

   //os << x.cache_cont;

   //for (int i = 0; i < x.devices.size(); i++) {

   //   os << x.devices[i];
   //}

   //os << endl;

   os << "<Experimental cache hit rates>" << endl;

   double cache_hits_overall = 0;
   double num_requests_overall = 0;

   // iterate through each file that we got requests for
   for (auto it = x.num_requests.begin(); it != x.num_requests.end(); it++) {

      double cache_hits = 0;

      if (x.own_cache.find(it->first) != x.own_cache.end()) {

         // add in cache hits from a device's own cache
         cache_hits += x.own_cache.at(it->first);
      }

      if (x.d2d_cont.success.find(it->first) != x.d2d_cont.success.end()) {

         // add in d2d cache hits
         cache_hits += x.d2d_cont.success.at(it->first);
      }

      if (cache_hits > 0) {

         os << "File " << it->first << " ("
            << x.cache_cont.getTheoreticalCacheHitRate(it->first) << "): "
            << cache_hits/x.num_requests[it->first] << " cache hit rate" << endl;
      }
      
      cache_hits_overall += cache_hits;
      num_requests_overall += x.num_requests[it->first];

   }

   os << "Percent of cache space used: " << x.percentCacheSpaceUsed() << endl;

   if (num_requests_overall > 0 ) {

      os << "Overall cache hit rate: " << cache_hits_overall/num_requests_overall << endl;     
   }

   return os;
}
