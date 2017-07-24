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

   Statistics stat;

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
   // file_cache_count takes ranks to how many of that file should be cached, but it is
   // only used for certain caching algorithms
   D2DInstance(const int& n, const int& m, const double& zipf,
      Graph& g, const int& cache_size, const double& epsilon, const int& radius, 
      vector<int>& thresholds, vector<double>& cache_hit_rates,
      Locations& locations, const bool& evolve,
      const double& evolve_portion, const string& alg, const int& seed):
      stat (n, m),
      d2d_cont (devices, radius, current_locations, stat),
      file_rank (m, evolve, evolve_portion, seed),
      req_cont (n, m, zipf, file_rank, 0, seed), bs (devices),
      time (0), locations (locations) {

      clog << "D2D instance created using " << alg << " caching." << endl;

      if (alg == "greedy") {

         this->cache_cont = unique_ptr<CacheControllerGreedy> (new CacheControllerGreedy
            (g, n, cache_size, epsilon, thresholds, cache_hit_rates, file_rank, alg));
      }
      else {

         assert(false);
      }

      setup(n, cache_size);

   }

   D2DInstance(const int& n, const int& m, const double& zipf,
      Graph& g, const int& cache_size, const int& radius, 
      Locations& locations, const bool& evolve,
      const double& evolve_portion, const string& alg, const int& seed):
      stat (n, m),
      d2d_cont (devices, radius, current_locations, stat),
      file_rank (m, evolve, evolve_portion, seed),
      req_cont (n, m, zipf, file_rank, 0, seed), bs (devices),
      time (0), locations (locations) {

      clog << "D2D instance created using " << alg << " caching." << endl;

      if (alg == "maxhitrate") {

         this->cache_cont = unique_ptr<CacheControllerMaxHitRate> (new CacheControllerMaxHitRate
            (g, n, cache_size, file_rank, alg, this->req_cont.f_req));
      }
      else {

         assert(false);
      }

      setup(n, cache_size);

   }

   D2DInstance(const int& n, const int& m, const double& zipf,
      const int& cache_size, const int& radius,
      Locations& locations, const string& alg, const int& seed,
      const vector<int>& file_cache_count):
      stat (n, m),
      d2d_cont (devices, radius, current_locations, stat),
      file_rank (m, false, 0, seed),
      req_cont (n, m, zipf, file_rank, 0, seed), bs (devices),
      time (0), locations (locations) {

      clog << "D2D instance created using " << alg << " caching." << endl;

      if (alg == "random1") {

          this->cache_cont = unique_ptr<CacheControllerRandom1> (new CacheControllerRandom1
            (n, cache_size, file_rank, alg, file_cache_count));        
      }
      else {

         assert(false);
      }

      setup(n, cache_size);

   } 

   D2DInstance(const int& n, const int& m, const double& zipf,
      const int& cache_size, const int& radius, 
      Locations& locations, const bool& evolve,
      const double& evolve_portion, const string& alg, const int& seed):
      stat (n, m),
      d2d_cont (devices, radius, current_locations, stat),
      file_rank (m, evolve, evolve_portion, seed),
      req_cont (n, m, zipf, file_rank, 0, seed), bs (devices),
      time (0), locations (locations) {

      clog << "D2D instance created using " << alg << " caching." << endl;

      if (alg == "topfiles") {

         this->cache_cont = unique_ptr<CacheControllerTop> (new CacheControllerTop
            (n, cache_size, file_rank, alg));
      }
      else {

         assert(false);
      }

      setup(n, cache_size);

   }


   void setup(const int& n, const int& cache_size) {

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

   void uncache() {

      int file;

      while (this->cache_cont->takeNextToUncache(file)) {

         clog << "Uncaching file " << file << " from all devices..." << endl;

         // cancel transmission if it exists
         this->bs.cancelMulticastTransmission(file);

         // clear out from all devices
         for (int i = 0; i < this->devices.size(); i++) {

            if (this->devices[i].hasFilePart(file)) {

               clog << "Uncaching file " << file << " from device " << i << endl;

               this->devices[i].removeFile(file);  
            }
            
            assert(!this->devices[i].hasFile(file));
         }
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

            // there is something new to cache or uncache

            this->uncache();

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

      this->stat.nextTimeStep(time, this->bs.inTransmissionCount());

      return true;

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

      this->stat.request(request_file);

      // first of all, see if this device has the file cached
      if (this->devices[request_device].hasFile(request_file)) {

         this->stat.hit(request_file);

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

   // returns how many devices have file with ranking rank cached
   int numDevicesCache(const int& rank) {

      int count = 0;

      int file = this->file_rank.getPopularFile(rank);

      for (int i = 0; i < this->devices.size(); i++) {

         if (this->devices[i].hasFile(file)) {

            count++;
         }
      }

      return count;
   }

   // prints theoretical cache hit rate, actual cache hit rate, etc. for file
   // at certain rank
   // prints inside what algorithm was used to get it
   void printFileResults(ostream& os, const int& rank) {

      int file = this->file_rank.getPopularFile(rank);

      os << " <" << this->cache_cont->getAlgorithm() << ">" << endl;

      if (dynamic_cast<CacheControllerGreedy*> (this->cache_cont.get()) != NULL) {

         os << "  <p>" << this->cache_cont->getTheoreticalCacheHitRate(file)
            << "</p>" << endl;         
      }

      if (this->stat.hasrequest(file)) {

         os << "  <hitrate>" << this->stat.hitrate(file) << "</hitrate>" << endl;
      }

      os << "  <n>" << this->devices.size() << "</n>" << endl;
      os << "  <size>" << this->numDevicesCache(rank) << "</size>" << endl;
      os << " </" << this->cache_cont->getAlgorithm() << ">" << endl;

   }

   void printMultiFileResults(ostream& os) {

      os << " <" << this->cache_cont->getAlgorithm() << ">" << endl;

      if (dynamic_cast<CacheControllerMaxHitRate*> (this->cache_cont.get()) != NULL) {

         os << "  <theoreticalrate>" << this->cache_cont->getTheoreticalCacheHitRate()
            << "</theoreticalrate>" << endl;         
      }

      os << "  <hitrateall>" << this->stat.hitrateall() << "</hitrateall>" << endl;
      os << "  <n>" << this->devices.size() << "</n>" << endl;

      //for (int i = 0; i < this->stat.BS_in_transmission.size(); i++) {

      //   os << "  <BSintransmission hour=\"" << i << "\">"
      //      << this->stat.BS_in_transmission[i]
      //      << "</BSintransmission>" << endl;
      //}

      //for (int i = 0; i < this->stat.hour_hit_rate.size(); i++) {

      //   os << "  <hourhitrate hour=\"" << i << "\">"
      //      << this->stat.hour_hit_rate[i]
      //      << "</hourhitrate>" << endl;
      //}


      os << " </" << this->cache_cont->getAlgorithm() << ">" << endl;


   }

};

