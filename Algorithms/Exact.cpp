// The exact algorithm for minimum file placement

using namespace std;

// Class that represents one node on the solution space tree
// Leafs correspond to subsets of nodes that are potential new cache nodes
class SolutionTreeNode {

   public:

   // Corresponding node for each level
   // Assumed to have nodes that can cache, and are not already caching (although there
   // is no particular logic for this, just doesn't check)
   vector<int>& nodes;

   // the level in the tree (0,...,n-1)
   // i.e., all nodes before the level-th are set from the above vector
   int level;

   // max_descendant is the descendant leaf with the maximum big_gamma value
   // (i.e. the one with the most nodes in it)
   // Note that this is in order of node id, not in the order of nodes!!
   double max_descendant_big_gamma; // what is the big gamma
   vector<double> max_descendant_small_gamma; // the small gamma
   vector<bool> max_descendant_cached; // and what nodes are cached (true if ith node is cached)
   
   // Similarly, the descendant leaf with the minimum number of nodes
   int min_descendant_size; // how many nodes it has overall (in nodes and already cached)
   double min_descendant_big_gamma;
   vector<double> min_descendant_small_gamma;
   vector<bool> min_descendant_cached;

   // Contact graph we are working with
   Graph& graph;

   // Get root of solution tree
   // nodes is assumed to contain all potential cache nodes that are not already caching
   SolutionTreeNode(CacheGraph& cgraph, vector<int>& nodes): graph(cgraph.graph), nodes(nodes) {

      this->level = 0;

      // this should correspond to including all available nodes
      this->max_descendant_cached = cgraph.can_cache;
      gamma_util::computeSmallGamma(this->max_descendant_small_gamma, this->max_descendant_cached,
                                    this->graph);
      this->max_descendant_big_gamma = gamma_util::computeBigGamma(this->max_descendant_small_gamma,
                                                                   this->graph);

      this->min_descendant_size = cgraph.cacheCount();
      this->min_descendant_cached = cgraph.cache_nodes;
      this->min_descendant_small_gamma = cgraph.small_gamma;
      this->min_descendant_big_gamma = cgraph.big_gamma;

   }

   // Get a solution tree node from a parent
   // is_left is true is left node, or false if right
   SolutionTreeNode(SolutionTreeNode& parent, bool is_left): graph(parent.graph), nodes(parent.nodes) {

      this->level = parent.level + 1;

      if (is_left) {

         // different max_descendant than parent

         map<int, double> delta_small_gamma;
         double delta_big_gamma;

         // max_gamma of parent, but without the (level - 1)-th node
         gamma_util::computeDeltaGammasRemove(this->nodes[this->level - 1], parent.max_descendant_cached,
                                              parent.max_descendant_small_gamma,
                                              this->graph, delta_small_gamma, delta_big_gamma);

         this->max_descendant_cached = parent.max_descendant_cached;
         this->max_descendant_cached[this->nodes[this->level - 1]] = false;
         this->max_descendant_small_gamma = parent.max_descendant_small_gamma;

         for (auto it = delta_small_gamma.begin(); it != delta_small_gamma.end(); it++) {

            this->max_descendant_small_gamma[it->first] = this->max_descendant_small_gamma[it->first] + it->second;
         }

         this->max_descendant_big_gamma = parent.max_descendant_big_gamma + delta_big_gamma;

         // same min_descendant as parent

         this->min_descendant_size = parent.min_descendant_size;
         this->min_descendant_cached = parent.min_descendant_cached;
         this->min_descendant_small_gamma = parent.min_descendant_small_gamma;
         this->min_descendant_big_gamma = parent.min_descendant_big_gamma;
      }
      else {

         // same max_descendant as parent

         this->max_descendant_small_gamma = parent.max_descendant_small_gamma;
         this->max_descendant_big_gamma = parent.max_descendant_big_gamma;
         this->max_descendant_cached = parent.max_descendant_cached;

         // different min_descendant

         map<int, double> delta_small_gamma;
         double delta_big_gamma;

         // the min_descendant now includes the (level - 1)-th node
         gamma_util::computeDeltaGammas(this->nodes[this->level - 1], parent.min_descendant_cached,
                                        parent.min_descendant_small_gamma,
                                        this->graph, delta_small_gamma, delta_big_gamma);

         this->min_descendant_cached = parent.min_descendant_cached;
         this->min_descendant_cached[this->nodes[this->level - 1]] = true;
         this->min_descendant_small_gamma = parent.min_descendant_small_gamma;

         for (auto it = delta_small_gamma.begin(); it != delta_small_gamma.end(); it++) {

            this->min_descendant_small_gamma[it->first] = this->min_descendant_small_gamma[it->first] + it->second;
         }

         this->min_descendant_big_gamma = parent.min_descendant_big_gamma + delta_big_gamma;

         this->min_descendant_size = parent.min_descendant_size + 1;
      }

   }

    // TESTING
   void printMembers() {

      cout << "Level " << this->level << endl;
      cout << "Max Descendant Big Gamma " << this->max_descendant_big_gamma << endl;
      cout << "Min Descendant Size " << this->min_descendant_size << endl;
      cout << "Min Descendant Big Gamma " << this->min_descendant_big_gamma << endl;

   }
  
};
  
// Given a parent node parent, search its left and right children for descendant leafs
// that could possibly be the optimal solution to MFP
// Assumes that the parent node's min_descendant is not feasible, and its max_descendant is
// Note that that implies the parent has children (don't need to check for that)
void searchChildrenSubTrees(const double& p, const int& n, SolutionTreeNode& parent,
                            vector< vector<bool> >& leafs, int& upper_bound) {

   // Look at the left child

   SolutionTreeNode left_child (parent, true);
   //left_child.printMembers();

   // We know that the left_child min_descendant is still not feasible

   // But is its max_descendant still feasible?

   if (left_child.max_descendant_big_gamma >= p) {

      // It is, so search this
      searchChildrenSubTrees(p, n, left_child, leafs, upper_bound);
   }

   // Now look at the right child

   SolutionTreeNode right_child (parent, false);
   //right_child.printMembers();

   // We know that the right_child has a max_descendant that is feasible

   // But is its min_descendant still below the upper_bound?

   if (right_child.min_descendant_size < upper_bound) {

      // And is it feasible?

      if (right_child.min_descendant_big_gamma >= p) {

         // No need to search further, the min_descendant is feasible
         leafs.push_back(right_child.min_descendant_cached);
      }
      else {

         searchChildrenSubTrees(p, n, right_child, leafs, upper_bound);
      }
   }

}


// Given a cache graph cgraph, add in to exact_nodes those nodes for an exact solution
// to minimum file placement
// Returns the big gamma of cgraph if exact_nodes were added in
double exact(CacheGraph& cgraph, vector<int>& exact_nodes, const double& p) {

   int n = cgraph.graph.getNodeCount();

   exact_nodes.clear();

   // Check if the cache graph already has big enough gamma

   if (cgraph.big_gamma >= p) {

      return cgraph.big_gamma;
   }

   // Make sure it is even possible to get a big gamma of p

   double max_gamma = gamma_util::computeBigGamma(cgraph.can_cache, cgraph.graph);

   if (max_gamma < p) {

      // not possible

      return cgraph.big_gamma;
   }

   // So by the checks above, there must exist an exact solution of at least one node

   // Run greedy algorithm with p ~ 1 to get an ordering of the nodes

   vector<int> greedy_nodes;

   greedy(cgraph, greedy_nodes, 0.99, 0.0001);

   // Add missing but cacheable nodes to the end of greedy_nodes (we need an ordering of all nodes)
   vector<int> new_cache;
   cgraph.newCache(new_cache);

   for (int i = 0; i < new_cache.size(); i++) {

      bool in_greedy = false;
      for (int j = 0; j < greedy_nodes.size(); j++) {

         if (new_cache[i] == greedy_nodes[j]) {
            in_greedy = true;
            break;
         }
      }

      if (!in_greedy) {

         greedy_nodes.push_back(new_cache[i]);
      }
   }

   // Run greedy again with p and epsilon = 0 in order to get a feasible solution, and therefore an
   // upper bound on the optimal solution size

   vector<int> greedy_nodes_2;
   greedy(cgraph, greedy_nodes_2, p, 0);
   int upper_bound = greedy_nodes_2.size();

   // Now start the recursive search down the tree, get leafs that are possible optimal solutions

   vector< vector<bool> > leafs; // cache placements (vector<bool>) that we have narrowed down the search
                                 // for the optimal solution to

   // Add greedy soln to leafs
   vector<bool> greedy_soln (n, false);

   for (int i = 0; i < greedy_nodes_2.size(); i++) {

      greedy_soln[greedy_nodes_2[i]] = true;
   }

   leafs.push_back(greedy_soln);

   // Make root node
   SolutionTreeNode root (cgraph, greedy_nodes); 

   // Recursively check solution tree for possible solutions
   searchChildrenSubTrees(p, n, root, leafs, upper_bound);

   int min_count = n + 1;
   int min_leaf;

   // Now find the leaf with the smallest number of elements
   for (int i = 0; i < leafs.size(); i++) {

      int count = 0;

      for (int j = 0; j < leafs[i].size(); j++) {

         if (leafs[i][j]) {

            count++;
         }
      }

      if (count < min_count) {

         min_count = count;
         min_leaf = i;
      }
   }

   for (int i = 0; i < leafs[min_leaf].size(); i++) {

      if (leafs[min_leaf][i]) {

         exact_nodes.push_back(i);
      }
   } 

   return gamma_util::computeBigGamma(leafs[min_leaf], cgraph.graph);

}



