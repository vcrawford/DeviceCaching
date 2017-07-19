// Classes and functions related to D2D

using namespace std;

// Handles the placement of files in the caches of devices
class CacheController {

   // holds the threshold for each cache rate
   // i.e., the ith entry is the min popularity place to get the
   // ith hit rate (sorted from smallest to largest)
   vector<int> thresholds;

   // holds the cache hit probability for the ith threshold
   vector<double> cache_hit_rates;

   // the ranking in popularity of the files
   FileRanking& file_ranking;

   // the contact graph we are working with
   Graph graph;

   // the theoretical cache graph construct
   CacheGraphMultiFile cache_graphs;

   // a map of file id to device id that should be cached
   // and has not already been
   map<int, vector<int> > to_cache;

   // what algorithm we use to decide what and when to cache
   string alg;

public:

   // thresholds takes the lower bound for the ith threshold
   // cache_hit_rates are the corresponding cache hit rates for each threshold
   // n is the number of nodes (number of devices)
   // c is the number of files a single node can cache
   CacheController(Graph& g, const int& n, const int& c, const double& epsilon, 
                   vector<int>& thresholds, vector<double>& cache_hit_rates,
                   FileRanking& file_ranking, const string& alg): file_ranking(file_ranking),
                   graph (g), cache_graphs (g, n, c, epsilon), thresholds (thresholds),
                   cache_hit_rates (cache_hit_rates), alg (alg) {

      clog << "Cache controller created with thresholds ";

      for (int i = 0; i < this->thresholds.size(); i++) {

         clog << this->thresholds[i] << ", ";
      }

      clog << " and cache hit rates ";

      for (int i = 0; i < this->cache_hit_rates.size(); i++) {

         clog << this->cache_hit_rates[i] << ", ";
      }

      clog << endl;

      assert( this->cache_hit_rates.size() == this->thresholds.size());

      this->cacheFilesInThresholds();

   }

   string getAlgorithm() {

      return this->alg;
   }

   // get the cache hit rate that a file should be cached at
   double getTheoreticalCacheHitRate(const int& file_id) {

      int ranking = this->file_ranking.getRanking(file_id); 

      for (int i = 0; i < this->thresholds.size(); i++) {

         if (ranking <= thresholds[i]) {

            return this->cache_hit_rates[i];
         }
      }

      // did not meet any threshold
      return 0;

   }

   // adds any new caching that must be done
   // if something has been added, returns true
   bool nextTimeStep() {

      return this->cacheFilesInThresholds();
   }

   // adds to this->to_cache if needed
   // returns whether there is anything new to cache
   bool cacheFilesInThresholds() {

      // what threshold we are at
      int t = 0;

      // iterate in order of popularity over the files caching each
      // i is the ist most popular file, not the file with id i
      // (initially, they happen to be the same, but can't assume that in general)

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

         assert(t < cache_hit_rates.size());

         vector<int> file_cache_nodes;

         if (this->cache_graphs.cacheFile(this->file_ranking.getPopularFile(i), cache_hit_rates[t],
            file_cache_nodes)) {

            this->to_cache[this->file_ranking.getPopularFile(i)] = file_cache_nodes;
         }
      }

      return (this->to_cache.size() > 0);

   }

   // take a file and devices out of to_cache
   // subsequently delete them from to_cache
   // returns whether there is anything to take or not
   bool takeNextToCache(int& file, vector<int>& device_ids) {

      auto it = this->to_cache.begin();

      if (it != this->to_cache.end()) {

         file = it->first;

         device_ids = it->second;

         to_cache.erase(it);

         return true;
      }

      return false;
   }

   friend ostream& operator<<(ostream&, const CacheController&);

};

ostream& operator<<(ostream& os, const CacheController& x) {

   os << "Cache Controller" << endl;

   os << " Thresholds: ";

   for (int i = 0; i < x.thresholds.size(); i++) {

      os << x.thresholds[i] << " ";
   }

   os << endl;

   os << " Theoretical cache hit rates: ";

   for (int i = 0; i < x.cache_hit_rates.size(); i++) {

      os << x.cache_hit_rates[i] << " ";
   }

   os << endl;

}

