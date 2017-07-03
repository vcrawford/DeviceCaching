// Functions for computation with little and big gamma

using namespace std;

namespace gamma_util {

   // Compute small gamma from scratch (no updates)
   // Cached is a vector of length n (number of nodes) that takes the ith node to whether it is
   // cached or not
   // Store the ith node's small gamma value in small_gamma
   void computeSmallGamma(vector<double>& small_gamma, const vector<bool>& cached, const Graph& graph) {
  
      small_gamma.clear();

      // Compute small gamma for each node
      for (int i = 0; i < graph.getNodeCount(); i++) {

         if (cached[i]) {

            small_gamma.push_back(1);
         }
         else {
            // the product in the equation for little gamma (see paper)
            double product = 1;

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
   double computeBigGamma(const vector<double>& small_gamma, const Graph& graph) {

      double big_gamma = 0;

      for (int i = 0; i < graph.getNodeCount(); i++) {

         big_gamma += small_gamma[i];
      }

      return big_gamma/graph.getNodeCount();
   }

   // computes big gamma from scratch
   double computeBigGamma(const vector<bool>& cached, const Graph& graph) {

      // compute small gamma
      vector<double> small_gamma;
      computeSmallGamma(small_gamma, cached, graph);

      return computeBigGamma(small_gamma, graph);
   }

   // Compute how much small_gamma will change if new_cache_node is added to the cache nodes
   // store in delta_small_gamma as map from the node to the change
   // only contains entries for nodes that not change
   void computeDeltaSmallGamma(const int& new_cache_node, const vector<bool>& cached,
                               const vector<double>& small_gamma, const Graph& graph,
                               map<int, double>& delta_small_gamma) {

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

   // Compute how much small_gamma will change if new_cache_node is removed from the cache nodes
   // store in delta_small_gamma as map from the node to the change
   // only contains entries for nodes that not change
   void computeDeltaSmallGammaRemove(const int& remove_cache_node, const vector<bool>& cached,
                                     const vector<double>& small_gamma, const Graph& graph,
                                     map<int, double>& delta_small_gamma) {

      delta_small_gamma.clear();

      if (!cached[remove_cache_node]) {
         // nothing will change
         return;
      }

      // Need to re-compute the small_gamma of the removed node

      vector<int> neighbors;
      graph.getNeighbors(remove_cache_node, neighbors);

      // product will be the probability the node WON'T get a cache hit
      double product = 1;

      for (int j = 0; j < neighbors.size(); j++) {

         if (cached[neighbors[j]]) {

            product = product*(1 - graph.getEdgeWeight(remove_cache_node, neighbors[j]));
         }

      }

      // the change is from 1 to 1-product (which is just -product)
      delta_small_gamma[remove_cache_node] = -1*product;


      // Now see how much each of the removed node's neighbors will change

      for (int j = 0; j < neighbors.size(); j++) {

         // Only impact neighbors that are not cached

         if (!cached[neighbors[j]]) {
            double edge_weight = graph.getEdgeWeight(remove_cache_node, neighbors[j]);

            delta_small_gamma[neighbors[j]] = edge_weight*(small_gamma[neighbors[j]] - 1)/(1 - edge_weight);
         }
      }
   }


   // Compute how much big_gamma and small_gamma will change if node is added to cache_nodes
   // Stores change in delta_small_gamma and delta_big_gamma
   void computeDeltaGammas(const int& node, const vector<bool>& cached, const vector<double>& small_gamma,
                           const Graph& graph, map<int, double>& delta_small_gamma, double& delta_big_gamma) {

      computeDeltaSmallGamma(node, cached, small_gamma, graph, delta_small_gamma);

      delta_big_gamma = 0;

      for (auto it = delta_small_gamma.begin(); it != delta_small_gamma.end(); it++) {

         delta_big_gamma = delta_big_gamma + it->second;
      }

      delta_big_gamma = (1.0/graph.getNodeCount())*delta_big_gamma;

   }

   // Compute how much big_gamma and small_gamma will change if node is REMOVED from cache_nodes
   // Stores change in delta_small_gamma and delta_big_gamma
   void computeDeltaGammasRemove(const int& node, const vector<bool>& cached, const vector<double>& small_gamma,
                                 const Graph& graph, map<int, double>& delta_small_gamma, double& delta_big_gamma) {

      computeDeltaSmallGammaRemove(node, cached, small_gamma, graph, delta_small_gamma);

      delta_big_gamma = 0;

      for (auto it = delta_small_gamma.begin(); it != delta_small_gamma.end(); it++) {

         delta_big_gamma = delta_big_gamma + it->second;
      }

      delta_big_gamma = (1.0/graph.getNodeCount())*delta_big_gamma;

   }

}
