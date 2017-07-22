
using namespace std;

// Handles the placement of files in the caches of devices according to max cache
// hit rate at a moment in time
class CacheControllerMaxHitRate: public CacheController {

   Graph graph;

   FileRequest& file_req;

   double theoretical_rate;

public:

   // n is the number of nodes (number of devices)
   // c is the number of files a single node can cache
   CacheControllerMaxHitRate(const Graph& g, const int& n, const int& c,
      FileRanking& file_ranking, const string& alg, FileRequest& file_req):
      CacheController(n, c, file_ranking, alg), graph (g), file_req (file_req) {

      clog << "Cache controller for max hit rate created" << endl;

      this->cache();
   }

   bool nextTimeStep() {

      return false;
   }

   double getTheoreticalCacheHitRate() {

      return this->theoretical_rate;
   }

   bool cache() {

      vector<double> zipf_dist;

      this->file_req.getZipf(zipf_dist);

      this->theoretical_rate = greedyMultiFile(this->graph, this->c, zipf_dist, this->to_cache);

      clog << "The greedy algorithms for caching files to get the max cache hit rate "
         << "has completed. The cache hit rate is expected to be "
         << this->theoretical_rate << endl;

   }

};
