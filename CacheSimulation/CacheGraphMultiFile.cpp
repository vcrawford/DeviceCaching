using namespace std;

// Holds cache graphs for multiple files
class CacheGraphMultiFile {

   // the cache graphs for the ith file
   map<int, CacheGraph> cgs;

   // the graph we are working with
   Graph& graph;

   // devices that are able to cache at least one more file
   // takes device id to true if can cache at least one more file
   vector<bool> can_cache;

   // device ids to how many files they can still cache
   vector<int> available_cache;

   // the epsilon for running the greedy algorithm
   double epsilon;

   public:

   // g is the contact graph for our devices
   // n is the number of nodes (number of devices)
   // c is the number of files a single node can cache
   // epsilon is for the greedy algorithm
   // assumes we start out with all caches empty
   CacheGraphMultiFile(Graph& g, const int& n, const int& c, const double& epsilon):
                       graph (g), can_cache (n, true), available_cache (n, c), epsilon (epsilon) { }

   // Determine what nodes to cache a file in in order to get a cache hit rate of p
   // for that file, and then cache those nodes.
   // If we cannot get that rate, just don't cache.
   // Returns whether this was successful or not
   bool cacheFile (const int& file_id, const double& p, vector<int>& nodes) {

      return true;

   }

};

