// A set of nodes in a graph that have some data cached

using namespace std;

class CacheGraph {

   public:

      // The graph these nodes correspond to
      Graph graph;

      // The nodes that are considered to have some data cached
      // Takes the id of each node to true or false depending on whether it is cached
      vector<bool> cache_nodes;

      // Nodes that are consider able to cache data
      // takes the id of each node to true or false
      // assumed to include nodes that already have data cached
      vector<bool> available_nodes;

      // The probability of a cache hit 
      float big_gamma;

      // The probability of the ith node having a cache hit 
      vector<float> small_gamma;

      // Create instance with a subset of nodes cached, and a subset unavailable
      CacheGraph(Graph& graph, vector<bool>& cache_nodes, vector<bool>& available_nodes) {

         this->graph = graph;
         this->cache_nodes = cache_nodes;
         this->available_nodes = available_nodes;

         // compute this->small_gamma for cache_nodes
         this->computeSmallGamma(this->small_gamma, this->cache_nodes);

         // compute this->big_gamma using this->small_gamma for cache_nodes
         this->big_gamma = this->computeBigGamma(this->small_gamma);

      }

      // Create instance where no nodes are yet caching, all nodes assumed available
      CacheGraph(Graph& graph) {

         this->graph = graph;

         for (int i = 0; i < this->graph.getNodeCount(); i++) {
            this->cache_nodes.push_back(false);
            this->available_nodes.push_back(true);
         }

         // compute this->small_gamma for cache_nodes
         this->computeSmallGamma(this->small_gamma, this->cache_nodes);

         // compute this->big_gamma using this->small_gamma for cache_nodes
         this->big_gamma = this->computeBigGamma(this->small_gamma);

      }

      // Add a node to the cache nodes
      // big_gamma and small_gamma must be updated accordingly
      // Note that big_gamma must be updated before small_gamma since it uses the small_gamma for
      // the previous cache_nodes
      void addCacheNode(int& node) {

         map<int, float> delta_small_gamma;
         float delta_big_gamma;
         delta_big_gamma = this->computeDeltaBigGamma(node, delta_small_gamma);

         // Set new values using the deltas
         for (auto it = delta_small_gamma.begin(); it != delta_small_gamma.end(); it++) {
            this->small_gamma[it->first] = this->small_gamma[it->first] + it->second;
         }

         this->big_gamma = this->big_gamma + delta_big_gamma;

         this->cache_nodes[node] = true;
      }

      // Make a node unavailable for caching
      void makeUnavailable(const int& node) {

         this->available_nodes[node] = false;

      }

      // Checks whether this node is caching
      bool isCached(const int& node) const {

         return this->cache_nodes[node];
      }

      bool isAvailable(const int& node) const {

         return this->available_nodes[node];

      }

      // Compute small_gamma for each vertex in a graph with cached vertices cached
      // Store small_gamma for the ith vertex in small_gamma
      void computeSmallGamma(vector<float>& small_gamma, vector<bool>& cached) const {
  
         small_gamma.clear();

         for (int i = 0; i < this->graph.getNodeCount(); i++) {

            if (cached[i]) {

               small_gamma.push_back(1);
            }
            else {
  	       // the product in the equation for little gamma (see paper)
  	       float product = 1;

               vector<int> neighbors;
               this->graph.getNeighbors(i, neighbors);

	       // Go through each neighbor and check if it's cached, update small gamma
               // accordingly
	       for (int j = 0; j < neighbors.size(); j++) {

                  int neighbor = neighbors[j];

	          if (cached[neighbor]) {

		     product = product*(1 - this->graph.getEdgeWeight(i, neighbor));
	          }  

	       }

	       // we have computed the product, so now we can set small gamma
	       small_gamma.push_back(1 - product);
            }

         }

      }

      // Compute big gamma from scratch, does not do any sort of updating
      // Relies on small_gamma being accurate
      // TODO: Make non-uniform probability distribution
      float computeBigGamma(const vector<float>& small_gamma) const {

         float big_gamma = 0;

         for (int i = 0; i < this->graph.getNodeCount(); i++) {

            big_gamma += small_gamma[i];
         }

         return big_gamma/this->graph.getNodeCount();

      }

      // Compute how much small_gamma will change if new_cache_node is added to the cache nodes
      // store in delta_small_gamma as map from the node to the change
      // only contains entries for nodes that not change
      // Does not store anything in any member functions!!!
      void computeDeltaSmallGamma(const int& new_cache_node, map<int, float>& delta_small_gamma) const {

         delta_small_gamma.clear();

         if (this->isCached(new_cache_node)) {
            // nothing will change
            return;
         }

         // the newly added node will have its small gamma go to 1
         delta_small_gamma[new_cache_node] = 1 - this->small_gamma[new_cache_node];

         // Go through new_cache_node's neighbors, which are the only things that could
         // have their small gamma changed
         vector<int> neighbors;
         this->graph.getNeighbors(new_cache_node, neighbors);

         for (int j = 0; j < neighbors.size(); j++) {

            int neighbor = neighbors[j];

            // Only impact neighbors that are not cached
            if (!this->isCached(neighbor)) {
               delta_small_gamma[neighbor] = this->graph.getEdgeWeight(neighbor,
                  new_cache_node)*(1 - small_gamma[neighbor]);
            
            }

         }


      }

      // Compute how much big_gamma will change if node is added to cache_nodes
      // Stores changes in small gamma in delta_small_gamma
      float computeDeltaBigGamma(int& node, map<int, float>& delta_small_gamma) const {

         this->computeDeltaSmallGamma(node, delta_small_gamma);

         float delta_big_gamma = 0;

         for (auto it = delta_small_gamma.begin(); it != delta_small_gamma.end(); it++) {

            delta_big_gamma = delta_big_gamma + it->second;
         }

         return (1.0/this->graph.getNodeCount())*delta_big_gamma;

      }

      // Finds the max big_gamma possible
      // Since gamma is monotonic, just add all possible nodes in and compute big_gamma
      // adds nodes needed to get this gamma (that aren't already cached) to potential_cache_nodes
      float maxGamma(list<int>& potential_cache_nodes) const {

         potential_cache_nodes.clear();

         // Set all possible nodes to cached
         vector<bool> all_cached;

         for (int i = 0; i < this->graph.getNodeCount(); i++) {

            if (this->isAvailable(i)) {

               all_cached.push_back(true);

               if (!this->isCached(i)) {
                  potential_cache_nodes.push_back(i);
               }
            }
            else {

               all_cached.push_back(false);
            }
         }

         vector<float> small_gamma;
         this->computeSmallGamma(small_gamma, all_cached);

         return this->computeBigGamma(small_gamma);
      }

      void printSmallGamma() const {

         for (int i = 0; i < this->small_gamma.size(); i++) {

            cout << this->small_gamma[i] << ", ";
         }
         cout << endl;

      }
};


