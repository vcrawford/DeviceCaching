// Run main algorithm

#include <unordered_set>
#include <map>
#include <list>
#include <fstream>
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <iomanip>
#include "Graph.cpp"
#include "GammaUtil.cpp"
#include "CacheGraph.cpp"
#include "Greedy.cpp"

using namespace std;

int main(int argc, char** argv) {

   string filename = "contact_graph.txt";

   int node0 = 0;
   int node1 = 1;
   int node2 = 2;
   int node3 = 3;
   int node4 = 4;

   Graph graph = Graph(filename);

   CacheGraph cg = CacheGraph(graph);

   float p = stof(argv[1]);
   float epsilon = 0.0000001;

   cout << "Running the greedy algorithm for minimum file placement to get a cache hit rate "
        << "of " << p << " with epsilon " << epsilon << " ..." << endl;

   vector<int> greedy_nodes;
   float gamma = greedy(cg, greedy_nodes, p, epsilon);

   cout << greedy_nodes.size() << " nodes should cache for a cache hit rate of " << gamma << endl;

   ofstream write_greedy;
   write_greedy.open("cache_greedy.txt");

   for (int i = 0; i < greedy_nodes.size(); i++) {

      write_greedy << greedy_nodes[i] << ",";

   }
  
   write_greedy.close();
}

