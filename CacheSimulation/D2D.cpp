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

};

// A single mobile device
class Device {

   public:

   // every device is identified by an id
   int id;

   // the current location (x,y) of this device
   int x;
   int y;

   // a map of each file id to how much of that file this device has
   // (a value between 0 and 1)
   vector<double> stored_files;

   // m is the number of files
   // initializes with 0 of every file
   Device(const int& id, const int& m) : id(id), stored_files (m, 0) {}

};

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

   D2DController(vector<Device>& devices): devices(devices) {}

};

// Returns requests for certain files by certain devices
class FileRequest {

   // the probability that any file is requested per second
   double req_prob;

   // popularity ranking of the files, which affects what devices are
   // are requested
   FileRanking& file_ranking;

   // the zipf parameter
   // file requests follow a zipf distribution over the above ranking
   double zipf_param;

   // the denominator for the zipf distribution
   // pre-computed to save time
   double denom;

   // random generator
   default_random_engine gen;

   // random number for whether there was a request
   uniform_real_distribution<double> rand_0_1;

   // which file it was for
   uniform_int_distribution<int> rand_0_f;

   // and which device
   uniform_int_distribution<int> rand_0_n;

   public:

   // n is the number of devices
   FileRequest(const int& m, const int& n, const double& zipf_param, const double& req_prob,
               FileRanking& fr): gen (time(NULL)), rand_0_1 (0, 1), rand_0_f (0, m),
               rand_0_n (0, n), req_prob (req_prob), file_ranking (fr) {

      // compute the denominator in the zipf probability distribution
      this->denom = 0;

      for (int i = 1; i <= m; i++) {

         this->denom += 1/(pow(i, zipf_param));
      }

   }

   // get a request from a device with req_prob probability
   // returns whether there was a request or not
   // If there was, stores which device and which file
   bool getRequest(int& device, int& file) {

      if (rand_0_1(gen) < this->req_prob) {

         // there was a request
         // get which file, f

         double rand_num = rand_0_1 (gen);

         // the popularity ranking we are on
         int pop_spot = 0;

         double prob_sum = (1/pow(1, this->zipf_param))/this->denom;

         while (rand_num > prob_sum) {

            pop_spot++;

            prob_sum += (1/pow(pop_spot + 1, this->zipf_param))/this->denom;
         }

         file = this->file_ranking.getPopularFile(pop_spot);

         return true;
      }

      else return false;

   }

};


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

};

// Handles the placement of files in the caches of devices
class CacheController {

   // holds the threshold for each cache rate
   // i.e., the ith entry is the min popularity place to get the
   // ith hit rate
   vector<int>& thresholds;

   // holds the cache hit probability for the ith threshold
   vector<double>& cache_hit_rates;

   // the ranking in popularity of the files
   FileRanking& file_ranking;

   // the theoretical cache graph construct
   CacheGraphMultiFile& cache_graph;

   public:

   // thresholds takes the lower bound for the ith threshold
   // cache_hit_rates are the corresponding cache hit rates for each threshold
   // n is the number of nodes (number of devices)
   // c is the number of files a single node can cache
   CacheController(vector<int>& thresholds, vector<double>& cache_hit_rates,
                  FileRanking& file_ranking, CacheGraphMultiFile& cache_graph):
                  thresholds(thresholds), cache_hit_rates(cache_hit_rates), file_ranking(file_ranking),
                  cache_graph(cache_graph) {}

   // the initial setting of the file popularities and what devices cache them
   // cache is the ith file to what devices should cache it
   // file_pop takes the ith popularity spot to the file id
   bool initialCache(map<int, vector<int> >& cache) {

      // what threshold we are at
      int t = 0;

      // iterate in order of popularity over the files caching each

      for (int i = 0; i < this->file_ranking.getNumFiles(); i++) {

         if (i < this->thresholds[t]) {

            // we are below the min rank for this threshold
            t++;
         }

         if (t == this->thresholds.size()) {

            // don't cache beyond this level
            break;
         }

         // So we do need to cache this file at the threshold's rate

         vector<int> cache_nodes;

         this->cache_graph.cacheFile(this->file_ranking.getPopularFile(i), cache_hit_rates[t],
                                      cache_nodes);

      }
   }


};

