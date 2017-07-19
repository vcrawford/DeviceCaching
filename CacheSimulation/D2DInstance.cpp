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

   // cache controller for our greedy algorithm
   // pointer since we don't know which child class
   unique_ptr<CacheController> cache_cont;

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
   D2DInstance(const int& n, const int& m, const double& zipf,
      Graph& g, const int& cache_size, const double& epsilon, const int& radius, 
      vector<int>& thresholds, vector<double>& cache_hit_rates,
      Locations& locations, const bool& evolve,
      const double& evolve_portion, const string& alg, const int& seed):
      d2d_cont (devices, radius, current_locations),
      file_rank (m, evolve, evolve_portion, seed),
      req_cont (n, m, zipf, file_rank, 0, seed), bs (devices),
      time (0), locations (locations) {

      clog << "D2D instance created using " << alg << " caching.";

      if (alg == "greedy") {

         this->cache_cont = unique_ptr<CacheControllerGreedy> (new CacheControllerGreedy
            (g, n, cache_size, epsilon, thresholds, cache_hit_rates, file_rank, alg));
      }

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

      while (this->cache_cont->takeNextToCache(file, device_ids)) {

         this->bs.newCache(file, device_ids);
      }

   }

   void addDisplayTime(ostream& os) {

      int days = this->time/86400;

      int leftover = this->time%86400;

      int hours = leftover/(60*60);

      leftover = leftover%(60*60);

      int minutes = leftover/60;

      int seconds = leftover%60;

      os << "Day: " << days << ", " << hours << ":" << minutes << ":" << seconds;
   }

   // take the entire simulation to the next time step
   // if there doesn't exist any more contact data, returns false
   bool nextTimeStep() {

      this->time++;

      clog << endl << "***Beginning next time step (t = ";

      this->addDisplayTime(clog);

      clog << ")..." << endl;

      // update file popularity (if we choose to do that in this simulation)

      if (this->file_rank.nextTimeStep()) {

         // there has been a rank change

         if (this->cache_cont->nextTimeStep()) {

            // there is something new to cache

            this->cachePopular();
         }
      }

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

   // prints theoretical cache hit rate, actual cache hit rate, etc. for file
   // at certain rank
   // prints inside what algorithm was used to get it
   void printFileResults(ostream& os, const int& rank) {

      int file = this->file_rank.getPopularFile(rank);

      assert (this->num_requests.find(file) != this->num_requests.end());

      double cache_hits = 0;

      if (this->own_cache.find(file) != this->own_cache.end()) {

         // add in cache hits from a device's own cache
         cache_hits += this->own_cache.at(file);
      }

      if (this->d2d_cont.success.find(file) != this->d2d_cont.success.end()) {

         // add in d2d cache hits
         cache_hits += this->d2d_cont.success.at(file);
      }

      os << " <" << this->cache_cont->getAlgorithm() << ">" << endl;

      if (dynamic_cast<CacheControllerGreedy*> (this->cache_cont.get()) != NULL) {

         os << "  <p>" << this->cache_cont->getTheoreticalCacheHitRate(file)
            << "</p>" << endl;         
      }

      os << "  <hitrate>" << cache_hits/this->num_requests.at(file) << "</hitrate>" << endl;
      os << "  <n>" << this->devices.size() << "</n>" << endl;
      os << " </" << this->cache_cont->getAlgorithm() << ">" << endl;

   }


};

