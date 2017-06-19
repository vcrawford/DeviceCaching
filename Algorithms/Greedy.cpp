// Greedy algorithm to find minimum number of nodes to get big_gamma in
// cache graph above some threshold

using namespace std;

// Input a cache graph cgraph where some subset of nodes are assumed
// to already be cache_nodes
// Find nodes to cache in, cache_nodes, that results in big_gamma of at least p
// we add the cache nodes in order to choosing into greedy_nodes
// Actually does add nodes to the graph
// Returns the gamma of the graph once greedy_nodes are added
float greedy(CacheGraph& cgraph, vector<int>& greedy_nodes, const float& p, const float& epsilon) {

   // First, we should test if there exists a feasible solution to get p
   
   // nodes that we consider for caching
   list<int> potential_cache_nodes;

   float max_gamma = cgraph.maxGamma(potential_cache_nodes);

   if ((max_gamma < p) || (epsilon <= 0) || (epsilon >= p)) {
      return cgraph.big_gamma;
   }

   map<int, float> delta_small_gamma;
   float max_delta_gamma;
   int add_next;

   while (cgraph.big_gamma < (p - epsilon)) {

      max_delta_gamma = -1;

      for (auto it = potential_cache_nodes.begin(); it != potential_cache_nodes.end(); it++) {

         float delta_big_gamma = cgraph.computeDeltaBigGamma(*it, delta_small_gamma);

         if (delta_big_gamma > max_delta_gamma) {

            max_delta_gamma = delta_big_gamma;
            add_next = *it;

         }

      }

      greedy_nodes.push_back(add_next);
      potential_cache_nodes.remove(add_next);
      cgraph.addCacheNode(add_next);
   }
    
   return cgraph.big_gamma; 

}


