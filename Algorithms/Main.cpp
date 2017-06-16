// Run main algorithm

#include "Graph.cpp"
#include <vector>
#include <iostream>
#include <string>
#include "CacheGraph.cpp"
#include <iomanip>
#include <unordered_set>

using namespace std;

int main() {

   string filename = "contact2.txt";

   Graph graph2 = Graph(filename);

   vector<bool> cache_nodes;
   cache_nodes.push_back(false); //0
   cache_nodes.push_back(true); //1
   cache_nodes.push_back(false); //2
   cache_nodes.push_back(true); //3
   cache_nodes.push_back(true); //4

   CacheGraph cn = CacheGraph(graph2, cache_nodes);

   cn.graph.printNeighbors();

   int node = 0;

   vector<float> delta_small_gamma;
   cn.computeDeltaSmallGamma(node, delta_small_gamma);

   //cout << "OLD BIG GAMMA: " << cn.big_gamma << endl;

   //cout << "DELTA BIG GAMMA: " << cn.computeDeltaBigGamma(delta_small_gamma) << endl;


   vector<bool> cache_nodes2;
   cache_nodes2.push_back(true); //0
   cache_nodes2.push_back(true); //1
   cache_nodes2.push_back(false); //2
   cache_nodes2.push_back(true); //3
   cache_nodes2.push_back(true); //4

   CacheGraph cn2 = CacheGraph(graph2, cache_nodes2);

   //cout << "NEW BIG GAMMA: " << cn2.big_gamma << endl;

}

