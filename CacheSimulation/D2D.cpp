// Classes and functions related to D2D

using namespace std;

// A ranking of files in order of popularity
// Each file is just reprensented by an integer
class FileRanking {

   // the files (identified by an int) in order of popularity
   // i.e. the 0th spot is the most popular file
   vector<int> ranking;

   public:

   // m is the number of files
   // ranking is initialized to be in order of id
   FileRanking(const int& m) {

      for (int i = 0; i < m; i++) {

         this->ranking.push_back(i);
      }

   }

   // get the id of the i-st most popular file
   int getPopularFile(const int& i) {

      return this->ranking[i];
   }

   // get the number of files
   int getNumFiles() {

      return this->ranking.size();
   }

   friend ostream& operator<<(ostream&, const FileRanking&);

};

ostream& operator<<(ostream& os, const FileRanking& x) {

   os << "File ranking: ";

   for (int i = 0; i < x.ranking.size(); i++) {

      os << i << ", ";
   }

   os << endl;

   return os;

}


// A single mobile device
class Device {

   public:

   // every device is identified by an id
   int id;

   // the current location (x,y) of this device
   int x;
   int y;

   // a map of file id to how much of that file this device has
   // (a value between 0 and 1)
   map<int, double> stored_files;

   // m is the number of files
   // initializes with 0 of every file
   Device(const int& id, const int& m) : id(id), x(0), y(0) {}

};

ostream& operator<<(ostream& os, const Device& x) {

   os << "Device " << x.id << ": " << endl;

   os << " Location: " << x.x << ", " << x.y << endl;

   os << " Stored files: ";

   for (auto it = x.stored_files.begin(); it != x.stored_files.end(); it++) {

      os << "(" << it->first << ", " << it->second << ") ";
   }

   os << endl;

}

// controls device to device transfer of files
class D2DController {

   // The D2D transmission rate in part of file per second
   // (Assuming file size is 100MB and transmission rate is 3MB per second)
   static constexpr double TRANS_RATE = 0.03;

   // all devices, in order of id
   vector<Device>& devices;

   // device id to device id, takes transmitter to receiver for all
   // current communications
   map<int, int> in_transmission;

   public:

   D2DController(vector<Device>& devices): devices(devices) { }

   friend ostream& operator<<(ostream&, const D2DController&);

};

ostream& operator<<(ostream& os, const D2DController& x) {

   os << "D2D in transmission: ";

   for (auto it = x.in_transmission.begin(); it != x.in_transmission.end(); it++) {

      os << "(" << it->first << ", " << it->second << ") ";
   }

   os << endl;

   return os;

}

// Returns requests for a file according to a Zipf distribution
class FileRequest {

   // popularity ranking of the files, which affects what devices
   // are requested
   FileRanking& file_ranking;

   // the zipf parameter
   // file requests follow a zipf distribution over the above ranking
   double zipf;

   // the denominator of the zipf probability function
   double denom;

   // random generator
   default_random_engine gen;

   // for picking random rank (and therefore random file)
   uniform_real_distribution<double> rand_0_1;

   public:

   // m is the number of files
   FileRequest(const int& m, const double& zipf, FileRanking& fr):
               gen (time(NULL)), rand_0_1 (0, 1), file_ranking (fr), zipf (zipf) {

      // calculate the denominator

      this->denom = 0;

      for (int i = 1; i <= m; i++) {

         this->denom += 1/pow(i, zipf);
      }

   }

   // gets a random request for a file
   // returns the id of that file
   int getRequest() {

      double rand_num = rand_0_1 (gen);

      // the popularity ranking we are on
      int pop_spot = 0;

      double prob_sum = (1/pow(1, this->zipf))/this->denom;

      while (rand_num > prob_sum) {

         pop_spot++;

         prob_sum += (1/pow(pop_spot + 1, this->zipf))/this->denom;
      }

      return this->file_ranking.getPopularFile(pop_spot);

   }

   friend ostream& operator<<(ostream&, const FileRequest&);

};

ostream& operator<<(ostream& os, FileRequest& x) {

   os << "File request samples: ";

   for (int i = 0; i < 20; i++) {

      os << x.getRequest() << " ";
   }

   os << endl;

}

// Returns requests from a device
class DeviceRequest {

   // random generator
   default_random_engine gen;

   // for which device
   uniform_int_distribution<int> rand_0_n;

   public:

   // n is the number of devices
   DeviceRequest(const int& n): gen (time(NULL)), rand_0_n (0, n) { }

   // get a request from a random device
   // returns the device's id
   int getRequest() {

      return rand_0_n (gen);
   }

};

// Controls requests for files from devices
class RequestController {

   default_random_engine gen;

   uniform_real_distribution<double> rand_0_1;

   FileRequest f_req;

   DeviceRequest d_req;

   // probability in a second there will be a request for a
   // file
   double r_prob;

   public:

   RequestController(const int& n, const int& m, const double& zipf, const double& r_prob,
      FileRanking& ranking):
      f_req(m, zipf, ranking), d_req (n), r_prob (r_prob), gen (time(NULL)), rand_0_1 (0, 1) { }

   // gets a request from a random uniformly chosen device
   // and a file from the zipf distribution
   // with probability r_prob
   // returns false if no request
   bool getRequest(int& device, int& file) {

      if (rand_0_1 (gen) < this->r_prob) {

         device = this->d_req.getRequest();

         file = this->f_req.getRequest();

         return true;
      }

      return false;

   }

   friend ostream& operator<<(ostream&, RequestController&);

};

ostream& operator<<(ostream& os, RequestController& x) {

   os << x.f_req;

   return os;
}

// The BS, which controls cellular resources
class BaseStation {

   // the number of resource blocks this BS has to distribute
   static constexpr double NUM_RBS = 300;

   // the rate of transmission for each resource block in portion of file
   // this corresponds to 100MB file size, and 0.1MB/s/rb
   static constexpr double RB_RATE = 0.001;

   // all devices in the system, in order of id
   vector<Device>& devices;

   // device id to file id of each device having data transmitted to it
   // from the BS
   map<int, int> in_transmission;

   public:

   BaseStation(vector<Device>& devices): devices(devices) { }

   friend ostream& operator<<(ostream&, const BaseStation&);

};

ostream& operator<<(ostream& os, const BaseStation& x) {

   os << "Base station in transmission: ";

   for (auto it = x.in_transmission.begin(); it != x.in_transmission.end(); it++) {

      os << "(" << it->first << ", " << it->second << ") ";
   }

   os << endl;

   return os;

}

// Handles the placement of files in the caches of devices
class CacheController {

   // holds the threshold for each cache rate
   // i.e., the ith entry is the min popularity place to get the
   // ith hit rate
   vector<int> thresholds;

   // holds the cache hit probability for the ith threshold
   vector<double> cache_hit_rates;

   // the ranking in popularity of the files
   FileRanking& file_ranking;

   // the contact graph we are working with
   Graph graph;

   // the theoretical cache graph construct
   CacheGraphMultiFile cache_graph;

   public:

   // thresholds takes the lower bound for the ith threshold
   // cache_hit_rates are the corresponding cache hit rates for each threshold
   // n is the number of nodes (number of devices)
   // c is the number of files a single node can cache
   CacheController(Graph& g, const int& n, const int& c, const double& epsilon, 
                   const int& num_thresholds, const int& threshold_size,
                   const double& top_rate, const double& rate_dec,
                   FileRanking& file_ranking): file_ranking(file_ranking),
                   graph (g), cache_graph (g, n, c, epsilon) {

      int threshold_upper_bound = threshold_size - 1;

      double cache_hit_rate = top_rate;

      for (int i = 0; i < num_thresholds; i++) {

         this->thresholds.push_back(threshold_upper_bound);
         this->cache_hit_rates.push_back(cache_hit_rate);

         threshold_upper_bound += threshold_size;
         cache_hit_rate -= rate_dec;
      }

   }

   // the initial setting of the file popularities and what devices cache them
   // cache is the ith file to what devices should cache it
   // file_pop takes the ith popularity spot to the file id
   bool initialCache(map<int, vector<int> >& cache) {

      // what threshold we are at
      int t = 0;

      // iterate in order of popularity over the files caching each

      for (int i = 0; i < this->file_ranking.getNumFiles(); i++) {

         if (i > this->thresholds[t]) {

            // we are below the min rank for this threshold
            t++;
         }

         if (t == this->thresholds.size()) {

            // don't cache beyond this level
            break;
         }

         // So we do need to cache this file at the threshold's rate

         vector<int> cache_nodes;

         if (this->cache_graph.cacheFile(this->file_ranking.getPopularFile(i), cache_hit_rates[t],
            cache_nodes)) {

            
         }

      }
   }

   friend ostream& operator<<(ostream&, const CacheController&);

};

ostream& operator<<(ostream& os, const CacheController& x) {

   os << "Thresholds: ";

   for (int i = 0; i < x.thresholds.size(); i++) {

      os << x.thresholds[i] << " ";
   }

   os << endl;

   os << "Cache hit rates: ";

   for (int i = 0; i < x.cache_hit_rates.size(); i++) {

      os << x.cache_hit_rates[i] << " ";
   }

   os << endl;

   os << x.cache_graph;

   os << endl;

}

// Has all of the classes above, controls everything
class D2DInstance {

   vector<Device> devices;

   D2DController d2d_cont;

   FileRanking file_rank;

   RequestController req_cont;

   BaseStation bs;

   CacheController cache_cont;

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
      const double& rate_dec):
      d2d_cont (devices), file_rank (m),
      req_cont (m, n, zipf, r_prob, file_rank), bs (devices),
      cache_cont (g, n, cache_size, epsilon, num_thresholds, threshold_size,
      top_rate, rate_dec, file_rank) {

      // add devices in with ids 0,..,n-1
      for (int i = 0; i < n; i++) {

         this->devices.push_back(Device(i, m));
      }

      map<int, vector<int>> to_cache;

      this->cache_cont.initialCache(to_cache);
   }

   friend ostream& operator<<(ostream&, D2DInstance&);

};

ostream& operator<<(ostream& os, D2DInstance& x) {

   os << endl << x.file_rank;

   for (int i = 0; i < x.devices.size(); i++) {

      os << x.devices[i];
   }

   os << x.d2d_cont;

   os << x.req_cont;

   os << x.bs;

   os << x.cache_cont;

   os << endl;

   return os;
}


