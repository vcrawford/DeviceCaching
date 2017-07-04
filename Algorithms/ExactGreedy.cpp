// The exact algorithm for minimum file placement

using namespace std;

namespace greedy_exact {

// Represents a leaf (a subset of nodes) that is some descendant of a node
class Descendant {

   public:

   // the number of vertices in this subset
   int size;

   vector<double> small_gamma;

   double big_gamma;

   // what nodes are in the set, takes nodeid to true if in
   vector<bool> nodes;

   Descendant(int& size, vector<double>& small_gamma, double& big_gamma, vector<bool>& nodes) {

      this->size = size;
      this->small_gamma;
      this->big_gamma = big_gamma;
      this->nodes = nodes;
   }

   Descendant() {}

   // Descendant of root node with maximum number of nodes (max=true) or minimum number of nodes (max=false)
   Descendant(CacheGraph& cgraph, bool max) {

      if (max) {

         this->size = cgraph.canCacheCount();

         this->nodes = cgraph.can_cache;

         gamma_util::computeSmallGamma(this->small_gamma, this->nodes, cgraph.graph);

         this->big_gamma = gamma_util::computeBigGamma(this->small_gamma, cgraph.graph);
      }
      else {

         this->size = cgraph.cacheCount();

         this->nodes = cgraph.cache_nodes;

         this->small_gamma = cgraph.small_gamma;

         this->big_gamma = cgraph.big_gamma;
      }

   }

   // Make a descendant like desc, but with node added (add=true) or removed (add=false)
   Descendant(const Descendant* desc, const Graph& graph, const int& node, bool add) {

      map<int, double> delta_small_gamma;
      double delta_big_gamma;

      if (add) {

         gamma_util::computeDeltaGammas(node, desc->nodes, desc->small_gamma, graph,
                                        delta_small_gamma, delta_big_gamma);

         this->nodes = desc->nodes;
         this->nodes[node] = true;
         this->size = desc->size + 1;
      }
      else {

         gamma_util::computeDeltaGammasRemove(node, desc->nodes, desc->small_gamma, graph,
                                              delta_small_gamma, delta_big_gamma);

         this->nodes = desc->nodes;
         this->nodes[node] = false;
         this->size = desc->size - 1;
      }

      this->small_gamma = desc->small_gamma;

      for (auto it = delta_small_gamma.begin(); it != delta_small_gamma.end(); it++) {

         this->small_gamma[it->first] = this->small_gamma[it->first] + it->second;
      }

      this->big_gamma = desc->big_gamma + delta_big_gamma;

   }

};

// Class that represents one node on the solution space tree
// Leafs correspond to subsets of nodes that are potential new cache nodes
class SolutionTreeNode {

   public:

   // descendant with the maximum big_gamma value/max number of nodes
   Descendant* max_descendant;

   // descendant with the minimum big_gamma value/min number of nodes
   Descendant* min_descendant;

   // whether this was a left child of its parent or not
   bool is_left;

   // Get root of solution tree
   // nodes is assumed to contain all potential cache nodes that are not already caching
   SolutionTreeNode(CacheGraph& cgraph) {

      this->max_descendant = new Descendant(cgraph, true);

      this->min_descendant = new Descendant(cgraph, false);

   }

   // Get a solution tree node from a parent
   // is_left is true is left node, or false if right
   // fix_node is the id of the node we are now fixing
   SolutionTreeNode(SolutionTreeNode& parent, Graph& graph, bool is_left, int& fix_node) {

      this->is_left = is_left;

      if (this->is_left) {

         // different max_descendant than parent
         this->max_descendant = new Descendant(parent.max_descendant, graph, fix_node,
                                               false);

         // but same min descendant
         this->min_descendant = parent.min_descendant;
      }
      else {

         // same max_descendant as parent
         this->max_descendant = parent.max_descendant;

         // different min_descendant
         this->min_descendant = new Descendant(parent.min_descendant, graph, fix_node,
                                               true);
      }

   }

   ~SolutionTreeNode() {

      if (is_left) {
         delete max_descendant;
      }
      else {
         delete min_descendant;
      }

   }

    // TESTING
   void printMembers() {

      cout << "Max Descendant Big Gamma " << this->max_descendant->big_gamma << endl;
      cout << "Min Descendant Size " << this->min_descendant->size << endl;
      cout << "Min Descendant Big Gamma " << this->min_descendant->big_gamma << endl;

   }
  
};
  
// Given a parent node parent, search its left and right children for descendant leafs
// that could possibly be the optimal solution to MFP
// Assumes that the parent node's min_descendant is not feasible, and its max_descendant is
// Note that that implies the parent has children (don't need to check for that)
// unfixed are all nodes that are unfixed at the node whose children we're inspecting (so the
// children will have one our of these set)
void searchChildrenSubTrees(const double& p, const int& n, Graph& graph, SolutionTreeNode& parent,
                            vector< vector<bool> >& leafs, int& upper_bound, list<int> unfixed) {

   if (unfixed.empty()) {

      throw invalid_argument("Can't search subtree if there are no children nodes.");
   }

   // Find node in unfixed that will affect big_gamma the most by being added

   double max_delta_gamma = -1;
   int fix_node = -1;
   map<int, double> delta_small_gamma;
   double delta_big_gamma;

   for (auto it = unfixed.begin(); it != unfixed.end(); it++) {

      gamma_util::computeDeltaGammas(*it, parent.min_descendant->nodes, parent.min_descendant->small_gamma, graph,
				     delta_small_gamma, delta_big_gamma);

      if (delta_big_gamma > max_delta_gamma) {

         max_delta_gamma = delta_big_gamma;
         fix_node = *it;
     }
   }

   unfixed.remove(fix_node);

   // Look at the left child

   SolutionTreeNode left_child (parent, graph, true, fix_node);

   // We know that the left_child min_descendant is still not feasible

   // But is its max_descendant still feasible?

   if (left_child.max_descendant->big_gamma >= p) {

      // It is, so search this
      searchChildrenSubTrees(p, n, graph, left_child, leafs, upper_bound, unfixed);
   }

   // Now look at the right child
   SolutionTreeNode right_child (parent, graph, false, fix_node);

   // We know that the right_child has a max_descendant that is feasible

   // But is its min_descendant still below the upper_bound?

   if (right_child.min_descendant->size < upper_bound) {

      // And is it feasible?

      if (right_child.min_descendant->big_gamma >= p) {

         // No need to search further, the min_descendant is feasible
         leafs.push_back(right_child.min_descendant->nodes);

      }
      else {

         searchChildrenSubTrees(p, n, graph, right_child, leafs, upper_bound, unfixed);
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

   // Get a list of all nodes that may cache
   list<int> new_cache;
   cgraph.newCache(new_cache);

   // Run greedy with p and epsilon = 0 in order to get a feasible solution, and therefore an
   // upper bound on the optimal solution size

   vector<int> greedy_nodes;
   greedy(cgraph, greedy_nodes, p, 0);
   int upper_bound = greedy_nodes.size();
   //int upper_bound = n + 1;

   // Now start the recursive search down the tree, get leafs that are possible optimal solutions

   vector< vector<bool> > leafs; // cache placements (vector<bool>) that we have narrowed down the search
                                 // for the optimal solution to

   // Add greedy soln to leafs
   vector<bool> greedy_soln (n, false);

   for (int i = 0; i < greedy_nodes.size(); i++) {

      greedy_soln[greedy_nodes[i]] = true;
   }

   leafs.push_back(greedy_soln);

   // Make root node
   SolutionTreeNode root (cgraph); 

   // Recursively check solution tree for possible solutions
   searchChildrenSubTrees(p, n, cgraph.graph, root, leafs, upper_bound, new_cache);

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

} // end namespace

