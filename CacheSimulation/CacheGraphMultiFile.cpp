using namespace std;

// Holds cache graphs for multiple files
class CacheGraphMultiFile {

   // the cache graphs for the ith file
   map<int, CacheGraph> cache_graphs;

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

      if (this->cache_graphs.find(file_id) == this->cache_graphs.end()) {

         this->cache_graphs.insert( make_pair(file_id, CacheGraph (this->graph, this->can_cache)) );
      } 

      if (greedy(this->cache_graphs.at(file_id), nodes, p, this->epsilon) >= p - this->epsilon) {

         cout << nodes.size() << endl;

         this->cache_graphs.at(file_id).addCacheNodes(nodes);

         for (int i = 0; i < nodes.size(); i++) {

            this->available_cache[nodes[i]]--;

            if (this->available_cache[nodes[i]] == 0) {

               this->can_cache[nodes[i]] = false;
            }
         }

         return true;
      }

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

   os << "Cache space left: ";

   for (int i = 0; i < x.available_cache.size(); i++) {

      os << "(" << i << ", " << x.available_cache[i] << ") ";
   }

   os << endl;

   os << "Can cache: ";

   for (int i = 0; i < x.can_cache.size(); i++) {

      if (x.can_cache[i]) {

         os << i << ", ";
      }
   }

   os << endl;

   os << "Cache graphs: ";

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
  
