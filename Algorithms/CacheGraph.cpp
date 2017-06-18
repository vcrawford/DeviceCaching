// A set of nodes in a graph that have some data cached

using namespace std;

class CacheGraph {

   public:

      // The graph these nodes correspond to
      Graph graph;

      // The nodes that are considered to have some data cached
      // Takes the id of each node to true or false depending on whether it is cached
      vector<bool> cache_nodes;

      // The probability of a cache hit 
      float big_gamma;

      // The probability of the ith node having a cache hit 
      vector<float> small_gamma;

      // Create instance with a subset of nodes cached
      CacheGraph(Graph& graph, vector<bool>& cache_nodes) {

         this->graph = graph;
         this->cache_nodes = cache_nodes;

         // compute this->small_gamma for cache_nodes
         this->computeSmallGamma();

         // compute this->big_gamma using this->small_gamma for cache_nodes
         this->computeBigGamma();

      }

      // Create instance where no nodes are yet caching
      CacheGraph(Graph& graph) {

         this->graph = graph;

         for (int i = 0; i < this->graph.getNodeCount(); i++) {
            this->cache_nodes.push_back(false);
         }

         // compute this->small_gamma for cache_nodes
         this->computeSmallGamma();

         // compute this->big_gamma using this->small_gamma for cache_nodes
         this->computeBigGamma();

      }

      // Add a node to the cache nodes
      // big_gamma and small_gamma must be updated accordingly
      // Note that big_gamma must be updated before small_gamma since it uses the small_gamma for
      // the previous cache_nodes
      void addCacheNode(int& node) {

         map<int, float> delta_small_gamma;
         this->computeDeltaSmallGamma(node, delta_small_gamma);

         float delta_big_gamma;
         delta_big_gamma = this->computeDeltaBigGamma(delta_small_gamma);

         // Set new values using the deltas
         for (auto it = delta_small_gamma.begin(); it != delta_small_gamma.end(); it++) {
            this->small_gamma[it->first] = this->small_gamma[it->first] + it->second;
         }

         this->big_gamma = this->big_gamma + delta_big_gamma;

         this->cache_nodes[node] = true;
      }

      // Checks whether this node is caching
      bool isCached(const int& node) {

         return this->cache_nodes[node];
      }

      // Compute small_gamma for each vertex in a graph
      // Store small_gamma for the ith vertex in small_gamma
      // Does not use any sort of update, just computes from scratch
      void computeSmallGamma() {
  
         this->small_gamma.clear();

         for (int i = 0; i < this->graph.getNodeCount(); i++) {

            if (this->isCached(i)) {

               this->small_gamma.push_back(1);
            }
            else {
  	       // the product in the equation for little gamma (see paper)
  	       float product = 1;

               unordered_set<int> neighbors;
               this->graph.getNeighbors(i, neighbors);

	       // Go through each neighbor and check if it's cached, update small gamma
               // accordingly
	       for (auto j = neighbors.begin(); j != neighbors.end(); j++) {

	          if (this->isCached(*j)) {

		     product = product*(1 - this->graph.getEdgeWeight(i, *j));
	          }  

	       }

	       // we have computed the product, so now we can set small gamma
	       this->small_gamma.push_back(1 - product);
            }

         }

      }

      // Compute big gamma from scratch, does not do any sort of updating
      // Relies on small_gamma being accurate
      // TODO: Make non-uniform probability distribution
      void computeBigGamma() {

         this->big_gamma = 0;

         for (int i = 0; i < this->graph.getNodeCount(); i++) {

            this->big_gamma += this->small_gamma[i];
         }

         this->big_gamma = this->big_gamma/this->graph.getNodeCount();

      }

      // Compute how much small_gamma will change if new_cache_node is added to the cache nodes
      // store in delta_small_gamma as map from the node to the change
      // only contains entries for nodes that not change
      // Does not store anything in any member functions!!!
      void computeDeltaSmallGamma(const int& new_cache_node, map<int, float>& delta_small_gamma) {

         delta_small_gamma.clear();

         if (this->isCached(new_cache_node)) {
            // nothing will change
            return;
         }

         // the newly added node will have its small gamma go to 1
         delta_small_gamma[new_cache_node] = 1 - this->small_gamma[new_cache_node];


         // Go through new_cache_node's neighbors, which are the only things that could
         // have their small gamma changed
         unordered_set<int> neighbors;
         this->graph.getNeighbors(new_cache_node, neighbors);

         for (auto it = neighbors.begin(); it != neighbors.end(); it++) {

            // Only impact neighbors that are not cached
            if (!this->isCached(*it)) {
               delta_small_gamma[*it] = this->graph.getEdgeWeight(*it, new_cache_node)*(1 - small_gamma[*it]);
            
            }

         }

      }

      // Compute how much big_gamma will change if small_gamma is changed by delta_small_gamma
      // Does not store anything in any member functions!!!
      // Relies on delta_small_gamma being accurate
      float computeDeltaBigGamma(const map<int, float>& delta_small_gamma) {

         float delta_big_gamma = 0;

         for (auto it = delta_small_gamma.begin(); it != delta_small_gamma.end(); it++) {

            delta_big_gamma = delta_big_gamma + it->second;
         }

         return (1.0/this->graph.getNodeCount())*delta_big_gamma;

      }


      void printSmallGamma() {

         for (int i = 0; i < this->small_gamma.size(); i++) {

            cout << this->small_gamma[i] << ", ";
         }
         cout << endl;

      }
};


