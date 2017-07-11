// A set of nodes in a graph that have some data cached

using namespace std;

class CacheGraph {

   public:

      // The graph these nodes correspond to
      Graph& graph;

      // The nodes that are considered to have a file cached
      // Takes the id of each node to true or false depending on whether it is cached
      vector<bool> cache_nodes;

      // Nodes that are consider able to cache data
      // takes the id of each node to true or false
      // assumed to include nodes that already have data cached
      vector<bool>& can_cache;

      // The probability of a cache hit 
      double big_gamma;

      // The probability of the ith node having a cache hit 
      vector<double> small_gamma;

      // Create instance with a subset of nodes cached, and a subset unavailable
      CacheGraph(Graph& graph, vector<bool>& cache_nodes, vector<bool>& can_cache_nodes):
         graph (graph), can_cache (can_cache_nodes) {

         this->cache_nodes = cache_nodes;

         // compute this->small_gamma for cache_nodes
         gamma_util::computeSmallGamma(this->small_gamma, this->cache_nodes, this->graph);

         // compute this->big_gamma using this->small_gamma for cache_nodes
         this->big_gamma = gamma_util::computeBigGamma(this->small_gamma, this->graph);

      }

      // Create instance where no nodes are yet caching
      CacheGraph(Graph& graph, vector<bool>& can_cache_nodes): graph(graph), can_cache(can_cache_nodes) {

         for (int i = 0; i < this->graph.getNodeCount(); i++) {
            this->cache_nodes.push_back(false);
            this->small_gamma.push_back(0);
         }

         this->big_gamma = 0;
      }

      // Add all nodes in nodes to cache, assuming they are all allowed to cache
      // the gammas are updated
      // returns whether all nodes were new nodes that were cached
      bool addCacheNodes(vector<int>& nodes) {

         bool all_added = true;

         for (int i = 0; i < nodes.size(); i++) {

            all_added = all_added && addCacheNode(nodes[i]);
         }

         return all_added;
      }

      // Add a node to the cache nodes, assuming it is allowed to cache
      // big_gamma and small_gamma must be updated accordingly
      // Note that big_gamma must be updated before small_gamma since it uses the small_gamma for
      // the previous cache_nodes
      // Returns whether a new node was cached or not
      bool addCacheNode(int& node) {

         if (isCached(node) || !this->can_cache[node]) {

            return false;
         }

         map<int, double> delta_small_gamma;
         double delta_big_gamma;
         gamma_util::computeDeltaGammas(node, this->cache_nodes, this->small_gamma, this->graph,
                                        delta_small_gamma, delta_big_gamma);

         // Set new values using the deltas
         for (auto it = delta_small_gamma.begin(); it != delta_small_gamma.end(); it++) {
            this->small_gamma[it->first] = this->small_gamma[it->first] + it->second;
         }

         this->big_gamma = this->big_gamma + delta_big_gamma;

         this->cache_nodes[node] = true;

         return true;
      }


      // Checks whether this node is caching
      bool isCached(const int& node) const {

         return this->cache_nodes[node];
      }

      // Get a list of all nodes that are able to cache, and are not already caching
      void newCache(list<int>& new_cache) const {

         new_cache.clear();

         for (int i = 0; i < this->graph.getNodeCount(); i++) {

            if (!this->cache_nodes[i] && this->can_cache[i]) {

               new_cache.push_back(i);
            }
         }

      }

      // get a vector filled with the ids of nodes able to cache, but not already caching
      void newCache(vector<int>& new_cache) const {

         new_cache.clear();

         for (int i = 0; i < this->graph.getNodeCount(); i++) {

            if (!this->cache_nodes[i] && this->can_cache[i]) {

               new_cache.push_back(i);
            }
         }

      }

      // number of nodes caching
      int cacheCount() const {

         int count = 0;

         for (int i = 0; i < this->cache_nodes.size(); i++) {

            if (this->cache_nodes[i]) {

               count++;
            }
         }

         return count;

      }

      // number of nodes that can cache
      int canCacheCount() const {

         int count = 0;

         for (int i = 0; i < this->cache_nodes.size(); i++) {

            if (this->can_cache[i]) {

               count++;
            }
         }

         return count;

      }


};


