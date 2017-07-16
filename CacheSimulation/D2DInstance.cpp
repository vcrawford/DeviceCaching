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

   // takes each file id to how many times it has been transferred D2D
   map<int, int> cache_hit;

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

      cout << "D2D instance starting ..." << endl;

      this->makeDevices(n);

      // add popular files to be multicasted
      this->cachePopular();

      this->time = 0;

   }

   // Add initial num devices
   void makeDevices(const int& num) {
 
      // add devices in with ids 0,..,n-1
      for (int i = 0; i < num; i++) {

         this->devices.push_back(Device(i));
      }

      cout << num << " devices in the D2D instance" << endl;

   }

   // get any caching that must be done for popular files
   void cachePopular() {

      int file;

      vector<int> device_ids;

      while (this->cache_cont.takeNextToCache(file, device_ids)) {

         this->bs.newCache(file, device_ids);

         cout << "File " << file << " has been given to the BS to be cached on "
            << device_ids.size() << " devices." << endl;
      }

   }

   // take the entire simulation to the next time step
   void nextTimeStep() {

      cout << "***Beginning next time step ..." << endl;

      this->time++;

      // get new locations
      this->locations.nextStep(this->current_locations);

      // give any new requests to BS or D2D
      this->startTransmitNewRequest();

      // to get any transmissions that have failed
      // for example, devices moved away from each other
      vector<D2DTransmission> failed;

      this->d2d_cont.nextTimeStep(failed);

      // give any failed D2D communications to the BS
      this->bs.takeFailedD2D(failed);

      // have the BS updated any transmissions it has
      this->bs.nextTimeStep();

   }

   // updates the total number of cache hits for this file
   void countCacheHit(const int& file) {

      //if (this->cache_hit.find(file) == this->cache_hit.end()) {

      //   this->cache_hit.insert( make_pair(file, 0) );
      //}

      //this->cache_hit[file]++;
   }

   // updates the total number of requests that have occurred for each file
   void countRequest(const int& file) {

      //if (this->num_requests.find(file) == this->num_requests.end()) {

      //   this->num_requests.insert( make_pair(file, 0) );
      //}

      //this->num_requests[file]++;
   }

   // Check for new requests and give them to either the d2d controller or bs
   void startTransmitNewRequest() {

      int request_device, request_file;

      if (this->req_cont.getRequest(request_device, request_file)) {

         cout << "Random request: file " << request_file << " from device "
            << request_device << endl;

         // check if this device is already downloading the file
         if (this->devices[request_device].isDownloading(request_file)) {

            // don't want to do anything here, not possible
            cout << "This device is actually already downloading that file" << endl;

            return;
         }

         this->countRequest(request_file);

         // first of all, see if this device has the file cached
         if (this->devices[request_device].hasFile(request_file)) {
 
            this->countCacheHit(request_file);

            cout << "The requesting device has the file cached" << endl;

            return;
         }

         // get all ids of devices caching this file
         vector<int> cache_nodes;

         for (int i = 0; i < this->devices.size(); i++) {

            if (this->devices[i].hasFile(request_file)) {

               cache_nodes.push_back(i);
            }
         }

         cout << "There are " << cache_nodes.size() << " nodes that cache this file" << endl;

         // if there are any, attempt d2d file transfer
         if (cache_nodes.size() != 0) {

            for (int i = 0; i < cache_nodes.size(); i++) {

               if (this->d2d_cont.tryD2D(cache_nodes[i], request_device, request_file)) {

                  cout << "The requesting device may receive this from D2D from device "
                     << cache_nodes[i] << endl;

                  // TODO: change this to completed D2D
                  this->countCacheHit(request_file);

                  return;
               }
            }
         }

         // d2d was not successful

         cout << "The requesting device may receive this file from the BS" << endl;
         this->bs.newRequest(request_device, request_file);
      }

   }

   friend ostream& operator<<(ostream&, D2DInstance&);

};

ostream& operator<<(ostream& os, D2DInstance& x) {

   //os << endl << x.file_rank;

   os << x.cache_cont;

   //for (int i = 0; i < x.devices.size(); i++) {

   //   os << x.devices[i];
   //}

   //os << endl;

   os << "<Experimetal cache hit rates>" << endl;

   for (auto it = x.cache_hit.begin(); it != x.cache_hit.end(); it++) {

      os << "File " << it->first << " has " << it->second << " cache hits" << endl;
   }

   for (auto it = x.num_requests.begin(); it != x.num_requests.end(); it++) {

      os << "File " << it->first << " has " << it->second << " requests" << endl;
   }

   //for (auto it = x.cache_hit.begin(); it != x.cache_hit.end(); it++) {

   //   os << " File " << it->first << ": " << double(it->second)/x.num_requests[it->first]
   //      << endl;
   //}

   return os;
}
