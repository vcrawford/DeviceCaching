using namespace std;

// Holds cache graphs for multiple files
class CacheGraphMultiFile {

   // the cache graphs for the ith file
   map<int, CacheGraph> cache_graphs;

   // the graph we are working with
   Graph graph;

   // devices that are able to cache at least one more file
   // takes device id to true if can cache at least one more file
   vector<bool> can_cache;

   // device ids to how many files they can still cache
   vector<int> available_cache;

   // cache space
   int c;

   // the epsilon for running the greedy algorithm
   double epsilon;

   public:

   // g is the contact graph for our devices
   // n is the number of nodes (number of devices)
   // c is the number of files a single node can cache
   // epsilon is for the greedy algorithm
   // assumes we start out with all caches empty
   CacheGraphMultiFile(Graph& g, const int& n, const int& c, const double& epsilon):
                       graph (g), can_cache (n, true), available_cache (n, c), epsilon (epsilon),
                       c (c) {

      assert (epsilon > 0);
   }

   // returns whether a file is supposed to be cached or not
   bool isCached(const int& fileid) {

      if (this->cache_graphs.find(fileid) == this->cache_graphs.end()) return false;

      return (this->cache_graphs.at(fileid).big_gamma > 0);      
   }

   // uncache a file that is cached
   void uncache(const int& fileid) {

      assert(this->cache_graphs.find(fileid) != this->cache_graphs.end());

      vector<int> cache_nodes;

      this->cache_graphs.at(fileid).getCacheNodes(cache_nodes);

      assert(cache_nodes.size() > 0);

      this->cache_graphs.erase(fileid);

      for (int i = 0; i < cache_nodes.size(); i++) {

         if (!this->can_cache[cache_nodes[i]]) {

            clog << "Device " << cache_nodes[i] << " has available space now." << endl;
         }

         this->can_cache[cache_nodes[i]] = true;
         this->available_cache[cache_nodes[i]]++;

         assert(this->available_cache[cache_nodes[i]] <= this->c);
      }

   }


   // Determine what nodes to cache a file in in order to get a cache hit rate of p
   // for that file, and then cache those nodes.
   // If we cannot get that rate, just don't cache.
   // Returns false if it is not possible to cache at this rate
   bool cacheFile (const int& file_id, const double& p, vector<int>& nodes) {

      assert((p < 1) && (this->epsilon < p));

      if (this->cache_graphs.find(file_id) == this->cache_graphs.end()) {

         this->cache_graphs.insert( make_pair(file_id, CacheGraph (this->graph, this->can_cache)) );
      }

      if (greedy(this->cache_graphs.at(file_id), nodes, p, this->epsilon) >= p - this->epsilon) {

         this->cache_graphs.at(file_id).addCacheNodes(nodes);

         for (int i = 0; i < nodes.size(); i++) {

            this->available_cache[nodes[i]]--;

            if (this->available_cache[nodes[i]] == 0) {

               clog << "Device " << nodes[i] << " no longer has available cache space." << endl;

               this->can_cache[nodes[i]] = false;
            }
         }

         return true;
      }

      // were not able to cache this file at the appropriate rate
      return false;

   }

   // get reference to cache_nodes vector for a file
   // returns whether there were any
   bool getCacheNodes(const int& file, vector<int>& cache_nodes) {

      cache_nodes.clear();

      if (this->cache_graphs.find(file) != this->cache_graphs.end()) {

         for (int i = 0; i < this->cache_graphs.at(file).cache_nodes.size(); i++) {

            if (this->cache_graphs.at(file).cache_nodes[i]) {

               cache_nodes.push_back(i);
            }
         }

         return true;
      }

      return false;

   }

   friend ostream& operator<<(ostream&, const CacheGraphMultiFile&);

};

ostream& operator<<(ostream& os, const CacheGraphMultiFile& x) {

   os << "Cache graph" << endl;

   os << " Cache space left: ";

   for (int i = 0; i < x.available_cache.size(); i++) {

      os << "(" << i << ", " << x.available_cache[i] << ") ";
   }

   os << endl;

   os << " Can cache: ";

   for (int i = 0; i < x.can_cache.size(); i++) {

      if (x.can_cache[i]) {

         os << i << ", ";
      }
   }

   os << endl;

   os << " Cache graphs: ";

   os << endl;

   for (int i = 0; i < x.cache_graphs.size(); i++) {

      os << " File " << i << " (gamma = " << x.cache_graphs.at(i).big_gamma << "): ";

      for (int j = 0; j < x.cache_graphs.at(i).cache_nodes.size(); j++) {

         if (x.cache_graphs.at(i).cache_nodes[j]) {

            os << j << ", ";
         }
      }

      os << endl;
   }

   os << endl;

   return os;

}
  
