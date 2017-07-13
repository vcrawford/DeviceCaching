// Classes and functions related to D2D

using namespace std;

// Has all of the classes above, controls everything
class D2DInstance {

   vector<Device> devices;

   D2DController d2d_cont;

   FileRanking file_rank;

   RequestController req_cont;

   BaseStation bs;

   CacheController cache_cont;

   Locations& locations;

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
      d2d_cont (devices), file_rank (m),
      req_cont (m, n, zipf, r_prob, file_rank), bs (devices),
      cache_cont (g, n, cache_size, epsilon, num_thresholds, threshold_size,
      top_rate, rate_dec, file_rank), time (0), locations (locations) {

      // add devices in with ids 0,..,n-1
      for (int i = 0; i < n; i++) {

         this->devices.push_back(Device());
      }

      map<int, vector<int>> to_cache;

      this->cache_cont.initialCache(to_cache);

      this->time = 0;

   }

   // locations takes the ith node to its x,y coordinates
   void nextTimeStep(vector< pair<int, int> > locations) {

      this->time++;

   }

   friend ostream& operator<<(ostream&, D2DInstance&);

};

ostream& operator<<(ostream& os, D2DInstance& x) {

   os << endl << x.file_rank;

   for (int i = 0; i < x.devices.size(); i++) {

      os << "Device " << i << ": " << endl;
      os << x.devices[i];
   }

   os << x.d2d_cont;

   os << x.req_cont;

   os << x.bs;

   os << x.cache_cont;

   os << endl;

   return os;
}


