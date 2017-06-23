// Greedy algorithm to find minimum number of nodes to get big_gamma in
// cache graph above some threshold

using namespace std;

// Input a cache graph cgraph where some subset of nodes are assumed
// to already be cache_nodes
// Find nodes to cache in, cache_nodes, that results in big_gamma of at least p
// we add the cache nodes in order to choosing into greedy_nodes
// Returns the gamma of the graph once greedy_nodes are added
float greedy(CacheGraph& cgraph, vector<int>& greedy_nodes, const float& p, const float& epsilon) {

   // First, we should test if there exists a feasible solution to get p

   float max_gamma = gamma_util::computeBigGamma(cgraph.can_cache, cgraph.graph);

   if ((max_gamma < p) || (epsilon <= 0) || (epsilon >= p)) {
      return cgraph.big_gamma;
   }

   // get nodes that we consider for caching

   list<int> potential_cache_nodes;
   cgraph.newCache(potential_cache_nodes);

   map<int, float> delta_small_gamma; // how much small gamma will change if add node
   float delta_big_gamma; // same with big gamma
   float max_delta_gamma; // the biggest gamma change found so far in greedy
   map<int, float> max_delta_small_gamma; // delta_small_gamma for the max_delta_gamma
   vector<float> small_gamma = cgraph.small_gamma; // small gamma of graph with greedy_nodes added
   float big_gamma = cgraph.big_gamma; // big gamma of graph with greedy_nodes added
   vector<bool> cache_nodes = cgraph.cache_nodes; // nodes cached so far
   int add_next; // what node we are looking at in greedy

   // go until big_gamma is big enough
   while (big_gamma < (p - epsilon)) {

      max_delta_gamma = -1;

      for (auto it = potential_cache_nodes.begin(); it != potential_cache_nodes.end(); it++) {

         gamma_util::computeDeltaGammas(*it, cache_nodes, small_gamma, cgraph.graph,
                                        delta_small_gamma, delta_big_gamma);

         if (delta_big_gamma > max_delta_gamma) {

            max_delta_gamma = delta_big_gamma;
            max_delta_small_gamma = delta_small_gamma;
            add_next = *it;

         }

      }

      if (max_delta_gamma > 0) {

         // add the greedy node that had biggest delta big gamma
         greedy_nodes.push_back(add_next);
         potential_cache_nodes.remove(add_next);
         cache_nodes[add_next] = true;

         // update big and small gammas
         big_gamma = big_gamma + max_delta_gamma;

         for (int i = 0; i < small_gamma.size(); i++) {

            small_gamma[i] = small_gamma[i] + max_delta_small_gamma[i];
         }
      }
   }
    
   return big_gamma; 

}


