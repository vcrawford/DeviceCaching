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

      // matrix where the ith, jth entry is a double representing the
      // edge between the ith and jth nodes
      vector< vector<double> > edge_matrix;

      // vector that takes the ith node to a vector of its neighbors
      vector< vector<int> > neighbors;

      // Empty graph
      Graph() {}

      // Pass in the edge_matrix
      Graph(vector< vector<double> >& matrix) {

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

            vector<double> edge_weights;
            double edge_weight;

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

         vector<int> node_neighbors;

         for (int i = 0; i < this->getNodeCount(); i++) {
            // Looking at neighbors of the ith node
            node_neighbors.clear();

            for (int j = 0; j < this->getNodeCount(); j++) {

               if (this->edge_matrix[i][j] > 0) {
                  // j is a neighbor
                  node_neighbors.push_back(j);
               }
            }

            this->neighbors.push_back(node_neighbors);

         }


      }

      // prints the neighbors data to the screen
      void printNeighbors() const {

         for (int i = 0; i < this->neighbors.size(); i++) {

            cout << setw(5) << i << ": ";

            for (int j = 0; j < this->neighbors[i].size(); ++j) {

               cout << setw(5) << this->neighbors[i][j];
            }

            cout << endl;
         }

      }

      // Gets the neighbors of node and puts them in neighbors
      void getNeighbors(const int& node, vector<int>& neighbors) const {

         neighbors = this->neighbors[node];

      }

      // Return the weight of the edge between node i and node j
      double getEdgeWeight(const int& i, const int& j) const {

         return this->edge_matrix[i][j];
      }

      // Checks whether node i and node j are neighbors
      bool areNeighbors(const int& i, const int& j) const {

         return (this->edge_matrix[i][j] > 0);

      }

      // Get the count of nodes in the graph
      int getNodeCount() const {

         return this->edge_matrix.size();

      }

      // Print a Graph object to a screen
      void print() const {

         for (int i = 0; i < this->edge_matrix.size(); i++) {

            for (int j = 0; j < this->edge_matrix[0].size(); j++) {

               cout << setw(10) << this->edge_matrix[i][j];

            }

            cout << endl;
         }
      }

};


