// Classes and functions related to D2D

using namespace std;

// Handles the placement of files in the caches of devices
class CacheController {

protected:
   FileRanking& file_ranking;

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
   CacheController(const int& n, const int& c,
                   FileRanking& file_ranking, const string& alg): file_ranking(file_ranking),
                   alg (alg) {

      clog << "Cache controller created using algorithm " << this->alg << endl;

   }

   string getAlgorithm() {

      return this->alg;
   }

   // adds any new caching that must be done
   // if something has been added, returns true
   virtual bool nextTimeStep() { assert(false); }

   // adds to this->to_cache if needed
   // returns whether there is anything new to cache
   virtual bool cache() { assert(false); }

   virtual double getTheoreticalCacheHitRate(const int& file) { assert(false); };

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

};
