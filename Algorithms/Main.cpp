// Run main algorithm

#include <unordered_set>
#include <map>
#include "Graph.cpp"
#include <vector>
#include <iostream>
#include <string>
#include "CacheGraph.cpp"
#include <iomanip>
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
   cg.addCacheNode(node0);
   cg.addCacheNode(node1);
   cg.addCacheNode(node2);
   cg.addCacheNode(node3);
   cg.addCacheNode(node4);

   cout << cg.big_gamma << endl;


}

