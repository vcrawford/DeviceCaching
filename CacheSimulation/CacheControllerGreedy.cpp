// Classes and functions related to D2D

using namespace std;

// Handles the placement of files in the caches of devices
class CacheControllerGreedy: public CacheController {

   // holds the threshold for each cache rate
   // i.e., the ith entry is the min popularity place to get the
   // ith hit rate (sorted from smallest to largest)
   vector<int> thresholds;

   // holds the cache hit probability for the ith threshold
   vector<double> cache_hit_rates;

   // the contact graph we are working with
   Graph graph;

   // the theoretical cache graph construct
   CacheGraphMultiFile cache_graphs;

   // the last attempted cache rate for each file (by id)
   vector<double> attempted_rates;

public:

   // thresholds takes the lower bound for the ith threshold
   // cache_hit_rates are the corresponding cache hit rates for each threshold
   // n is the number of nodes (number of devices)
   // c is the number of files a single node can cache
   CacheControllerGreedy(Graph& g, const int& n, const int& c, const double& epsilon,
                  vector<int>& thresholds, vector<double>& cache_hit_rates,
                  FileRanking& file_ranking, const string& alg):
                  CacheController(n, c, file_ranking, alg),
                  cache_graphs (g, n, c, epsilon), thresholds (thresholds),
                  cache_hit_rates (cache_hit_rates),
                  attempted_rates (file_ranking.getNumFiles(), 0.0) {

      clog << "Cache controller has popularity thresholds ";

      for (int i = 0; i < this->thresholds.size(); i++) {

         clog << this->thresholds[i] << ", ";
      }

      clog << " and corresponding cache hit rates ";

      for (int i = 0; i < this->cache_hit_rates.size(); i++) {

         clog << this->cache_hit_rates[i] << ", ";
      }

      clog << endl;

      assert( this->cache_hit_rates.size() == this->thresholds.size());

      this->cache();

   }

   // return the rank of the least popular, but cached, file
   // if there doesn't exist one, returns -1
   int getLeastPopularCached() {

      for (int i = this->file_ranking.getNumFiles() - 1; i >= 0; i--) {

         if (this->cache_graphs.isCached(this->file_ranking.getPopularFile(i))) { 
 
            return i;
         }
      }

      return -1;
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

      bool uncache = this->uncache();

      bool cache = this->cache();

      return (uncache || cache);
   }

   // adds to this->to_cache if needed
   bool cache() {

      // iterate in order of popularity over the files caching each
      // i is the ist most popular file, not the file with id i

      for (int i = 0; i < this->file_ranking.getNumFiles(); i++) {

         // cache hit rate this file should be at
         double rate = this->getRate(i);

         // check if we are beyond files that get cached
         if (rate == 0) break;

         // we are at a rank that should be cached

         // the corresponding file
         int file = this->file_ranking.getPopularFile(i);

         // check what rate it has been cached at (successful or not)
         if (this->attempted_rates[file] == rate) {

            // already attempted (successful or not) to cache it at the appropriate rate
            continue;
         }
         else {

            // more caching must be done

            clog << "File " << file << " should be cached at a higher rate ("
               << rate << ")..." << endl;

            this->attempted_rates[file] = rate;

            vector<int> file_cache_nodes;

            if (this->cache_graphs.cacheFile(file, rate, file_cache_nodes)) {

               // there was room to cache it
               this->to_cache[file] = file_cache_nodes;

               clog << "There was enough room to cache it." << endl;

            }
            else {

               clog << "There was not enough room to cache it." << endl;
            }        
         }
      }

      return (this->to_cache.size() > 0);

   }

   bool uncache() {

      // iterate in order of popularity over the files caching each
      // i is the ist most popular file, not the file with id i

      for (int i = 0; i < this->file_ranking.getNumFiles(); i++) {

         // determine cache hit rate for ith ranked file

         double rate = this->getRate(i);

         // the file corresponding to this rate
         int file = this->file_ranking.getPopularFile(i);

         if (this->attempted_rates[file] > rate) {

            // cached at too high of a rate

            clog << "Should uncache file " << file << " which is currently at rank "
               << i << endl;

            for (auto it = this->to_uncache.begin(); it != this->to_uncache.end(); it++) {

               assert(*it != file);                    
            }

            // despite the attempted rate being of a certain value, it's still possible a file
            // was never actually cached
            if (this->cache_graphs.isCached(file)) {

               this->cache_graphs.uncache(file);

               this->to_uncache.push_back(file);
            }

            this->attempted_rates[file] = 0.0;
         }
      }

      return (this->to_uncache.size() > 0);

   }

   // the rate that we want a certain rank to be cached at
   double getRate(const int& rank) {

      for (int i = 0; i < this->thresholds.size(); i++) {

         if (this->thresholds[i] >= rank) return cache_hit_rates[i];
      }

      return 0;
   }

};

