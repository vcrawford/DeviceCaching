// Classes and functions related to D2D

using namespace std;

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

   // all devices in order of id
   vector<Device>& devices;

   public:

   // thresholds takes the lower bound for the ith threshold
   // cache_hit_rates are the corresponding cache hit rates for each threshold
   // n is the number of nodes (number of devices)
   // c is the number of files a single node can cache
   CacheController(Graph& g, const int& n, const int& c, const double& epsilon, 
                   const int& num_thresholds, const int& threshold_size,
                   const double& top_rate, const double& rate_dec,
                   FileRanking& file_ranking, vector<Device>& devices): file_ranking(file_ranking),
                   graph (g), cache_graph (g, n, c, epsilon), devices (devices) {

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
   bool initialCache(map<int, vector<Device> >& cache) {

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

            for (int i = 0; i < 
         }

      }
   }

   bool getCacheNodes(const int& file, vector<int>& cache_nodes) {

      return this->cache_graph.getCacheNodes(file, cache_nodes);
   }

   void getDevices(vector<int>& device_ids, vector<Device>& ds) {

      

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

