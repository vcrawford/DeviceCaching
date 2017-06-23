// Functions for computation with little and big gamma

using namespace std;

namespace gamma_util {

   // Compute small gamma from scratch (no updates)
   // Cached is a vector of length n (number of nodes) that takes the ith node to whether it is
   // cached or not
   // Store the ith node's small gamma value in small_gamma
   void computeSmallGamma(vector<float>& small_gamma, const vector<bool>& cached, const Graph& graph) {
  
      small_gamma.clear();

      // Compute small gamma for each node
      for (int i = 0; i < graph.getNodeCount(); i++) {

         if (cached[i]) {

            small_gamma.push_back(1);
         }
         else {
            // the product in the equation for little gamma (see paper)
            float product = 1;

            vector<int> neighbors;
            graph.getNeighbors(i, neighbors);

            // Go through each neighbor and check if it's cached, update small gamma
            // accordingly
            for (int j = 0; j < neighbors.size(); j++) {

               int neighbor = neighbors[j];

               if (cached[neighbor]) {

                  product = product*(1 - graph.getEdgeWeight(i, neighbor));
               }

            }

            // we have computed the product, so now we can set small gamma
            small_gamma.push_back(1 - product);
         }

      }
   }


   // Compute big gamma from scratch, does not do any sort of updating
   // TODO: Make non-uniform probability distribution
   float computeBigGamma(const vector<float>& small_gamma, const Graph& graph) {

      float big_gamma = 0;

      for (int i = 0; i < graph.getNodeCount(); i++) {

         big_gamma += small_gamma[i];
      }

      return big_gamma/graph.getNodeCount();
   }

   // computes big gamma from scratch
   float computeBigGamma(const vector<bool>& cached, const Graph& graph) {

      // compute small gamma
      vector<float> small_gamma;
      computeSmallGamma(small_gamma, cached, graph);

      return computeBigGamma(small_gamma, graph);
   }

   // Compute how much small_gamma will change if new_cache_node is added to the cache nodes
   // store in delta_small_gamma as map from the node to the change
   // only contains entries for nodes that not change
   void computeDeltaSmallGamma(const int& new_cache_node, const vector<bool>& cached,
                               const vector<float>& small_gamma, const Graph& graph,
                               map<int, float>& delta_small_gamma) {

      delta_small_gamma.clear();

      if (cached[new_cache_node]) {
         // nothing will change
         return;
      }

      // the newly added node will have its small gamma go to 1
      delta_small_gamma[new_cache_node] = 1 - small_gamma[new_cache_node];

      // Go through new_cache_node's neighbors, which are the only things that could
      // have their small gamma changed
      vector<int> neighbors;
      graph.getNeighbors(new_cache_node, neighbors);

      for (int j = 0; j < neighbors.size(); j++) {

         int neighbor = neighbors[j];

         // Only impact neighbors that are not cached
         if (!cached[neighbor]) {

            delta_small_gamma[neighbor] = graph.getEdgeWeight(neighbor, new_cache_node)*
                                          (1 - small_gamma[neighbor]);
         }
      }
   }


   // Compute how much big_gamma and small_gamma will change if node is added to cache_nodes
   // Stores change in delta_small_gamma and delta_big_gamma
   void computeDeltaGammas(const int& node, const vector<bool>& cached, const vector<float>& small_gamma,
                           const Graph& graph, map<int, float>& delta_small_gamma, float& delta_big_gamma) {

      computeDeltaSmallGamma(node, cached, small_gamma, graph, delta_small_gamma);

      delta_big_gamma = 0;

      for (auto it = delta_small_gamma.begin(); it != delta_small_gamma.end(); it++) {

         delta_big_gamma = delta_big_gamma + it->second;
      }

      delta_big_gamma = (1.0/graph.getNodeCount())*delta_big_gamma;

   }

}
