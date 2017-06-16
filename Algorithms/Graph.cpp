// Represents a graph

#include <vector>
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_set>

using namespace std;

class Graph {

   public:

      // matrix where the ith, jth entry is a float representing the
      // edge between the ith and jth nodes
      vector< vector<float> > edge_matrix;

      // vector that takes the ith node to an unordered_set of its neighbors
      vector< unordered_set<int> > neighbors;

      // Empty graph
      Graph() {}

      // Pass in the edge_matrix
      Graph(vector< vector<float> >& matrix) {

         this->edge_matrix = matrix;

         this->buildNeighbors();
      }

      // Read the edge matrix from a file
      Graph(string& filename) {

         ifstream matrix_file;

         matrix_file.open(filename.c_str());

         string line;

         while (getline(matrix_file, line)) {

            stringstream line_strm;
            line_strm.str(line);

            vector<float> edge_weights;
            float edge_weight;

            while (line_strm >> edge_weight) {

               edge_weights.push_back(edge_weight);
            }

            this->edge_matrix.push_back(edge_weights);

         }

         this->buildNeighbors();
      }

      // builds the neighbors vector based upon the edge_matrix data
      void buildNeighbors() {

         this->neighbors.clear();

         for (int i = 0; i < this->edge_matrix.size(); i++) {
            // Looking at neighbors of the ith node

            unordered_set<int> node_neighbors;

            for (int j = 0; j < this->edge_matrix[0].size(); j++) {

               if (this->edge_matrix[i][j] > 0) {
                  // j is a neighbor

                  node_neighbors.insert(j);
               }
            }

            this->neighbors.push_back(node_neighbors);
         }
      }

      // prints the neighbors data to the screen
      void printNeighbors() {

         for (int i = 0; i < this->neighbors.size(); i++) {

            cout << setw(5) << i << ": ";

            for (auto it = this->neighbors[i].begin(); it != this->neighbors[i].end(); ++it) {

               cout << setw(5) << *it;
            }

            cout << endl;
         }

      }

      // Gets the neighbors of node and puts them in neighbors
      void getNeighbors(const int& node, unordered_set<int>& neighbors) {

         neighbors = this->neighbors[node];

      }

      // Return the weight of the edge between node i and node j
      float getEdgeWeight(const int& i, const int& j) {

         return this->edge_matrix[i][j];
      }

      // Checks whether node i and node j are neighbors
      bool areNeighbors(const int& i, const int& j) {

         return (this->edge_matrix[i][j] > 0);

      }

      // Get the count of nodes in the graph
      int getNodeCount() {

         return this->edge_matrix.size();

      }

      // Print a Graph object to a screen
      void print() {

         for (int i = 0; i < this->edge_matrix.size(); i++) {

            for (int j = 0; j < this->edge_matrix[0].size(); j++) {

               cout << setw(10) << this->edge_matrix[i][j];

            }

            cout << endl;
         }
      }

};


