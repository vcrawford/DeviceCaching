// Run main algorithm

#include <unordered_set>
#include <map>
#include <list>
#include "Graph.cpp"
#include <vector>
#include <iostream>
#include <string>
#include "CacheGraph.cpp"
#include <iomanip>
#include "Greedy.cpp"

using namespace std;

int main() {

   string filename = "contact2.txt";

   int node0 = 0;
   int node1 = 1;
   int node2 = 2;
   int node3 = 3;
   int node4 = 4;

   Graph graph = Graph(filename);

   CacheGraph cg = CacheGraph(graph);
   //cg.addCacheNode(node0);
   //cg.addCacheNode(node1);

   //cg.addCacheNode(node2);
   //cg.addCacheNode(node3);
   //cg.addCacheNode(node4);

   //cg.makeUnavailable(node0);
   cg.makeUnavailable(node4);

   float p = 0.6;
   float epsilon = 0.01;

   vector<int> greedy_nodes;
   cout << "Gamma: " << greedy(cg, greedy_nodes, p, epsilon) << endl;
   //cout << "Nodes: ";

   for (int i = 0; i < greedy_nodes.size(); i++) {

      //cout << greedy_nodes[i] << ", ";

   }

   cout << endl;
}

