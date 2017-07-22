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

   // number of devices
   int n;

   // cache size
   int c;

public:

   // n is the number of nodes (number of devices)
   // c is the number of files a single node can cache
   CacheController(const int& n, const int& c,
                   FileRanking& file_ranking, const string& alg): file_ranking(file_ranking),
                   alg (alg), n(n), c(c) { }

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

   virtual double getTheoreticalCacheHitRate() { assert(false); };

   // add a single device to cache for a file
   void addToCache(const int& file, const int& device) {

      if (this->to_cache.find(file) == this->to_cache.end()) {

         this->to_cache[file] = vector<int> ();
      }

      for (int i = 0; i < this->to_cache[file].size(); i++) {

         if (this->to_cache[file][i] == device) {

            assert (false);
         }
      }

      this->to_cache[file].push_back(device);
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

   // checks if device already has been added to cache file
   bool alreadyAdded(const int& file, const int& device) {

      if (this->to_cache.find(file) == this->to_cache.end()) {

         return false;
      }

      for (int i = 0; i < this->to_cache.at(file).size(); i++) {

         if (this->to_cache.at(file)[i] == device) {

            return true;
         }
      }

      return false;

   }

   void logToCache() {

      clog << "Cache controller has the following queued to be cached:" << endl;

      for (auto it = this->to_cache.begin(); it != this->to_cache.end(); it++) {

         clog << "File " << it->first << ": ";

         for (int i = 0; i < it->second.size(); i++) {

            clog << it->second[i] << ", ";
         }

         clog << endl;
      }

   }   

};
