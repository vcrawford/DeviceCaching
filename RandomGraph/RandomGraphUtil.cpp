// functions used by the various random graph generators

using namespace std;

namespace random_graph_util {

   // For every edge in input graph that is not 0, pick a random
   // weight uniformly between 0 and 1
   void edgeWeights(vector< vector<double> >& graph) {

      default_random_engine gen (time(NULL));

      uniform_real_distribution<double> dist (0, 1);

      for (int i = 0; i < graph.size(); i++) {

         for (int j = 0; j < i; j++) {

            if (graph[i][j] != 0) {

               double edge_weight = dist(gen);
               graph[i][j] = edge_weight;
               graph[j][i] = edge_weight;
            }
         }
      }

   }

   // Write graph to file
   void writeGraph(const string& filename, const vector< vector<double> >& graph) {

      ofstream file_strm;
      file_strm.open(filename); 

      for (int i = 0; i < graph.size(); i++) {

         for (int j = 0; j < graph[i].size(); j++) {

            file_strm << graph[i][j] << " ";
         }

         file_strm << endl;
      }

      file_strm.close();

   }

   // construct lattice with k neighbors for each node
   void lattice( vector< vector<double> >& lattice, int n, int k) {

      cout << "Generating a lattic of size " << n << " and each node having degree " << k << endl;

      if ((k%2 != 0) || !(k < n)) throw invalid_argument("Invalid lattice");

      vector< vector<double> > graph (n, vector<double> (n, 0));

      for (int i = 0; i < n; i++) {

         for (int j = i - k/2; j < i; j++) {

            if (j < 0) {

               graph[i][n + j] = 1;
               graph[n + j][i] = 1;
            }
            else {

               graph[i][j] = 1;
               graph[j][i] = 1;
            }
         }
      }

      lattice = graph;

   }

   // construct complete graph of size n
   void complete( vector< vector<double> >& complete, int n) {

      cout << "Generating a complete graph of size " << n << endl;

      vector< vector<double> > graph (n, vector<double> (n, 1));

      for (int i = 0; i < n; i++) {

	graph[i][i] = 0;

      }

      complete = graph;

   }

   // generate a random ER graph
   void randomER(vector< vector<double> >& er_graph, int n, double p) {

      if ((p < 0) || (p > 1)) throw invalid_argument("p for ER graph must be between 0 and 1");

      vector< vector<double> > graph (n, vector<double> (n, 0));

      default_random_engine gen (time(NULL));

      uniform_real_distribution<double> dist (0, 1);

      for (int i = 0; i < n; i++) {

         for (int j = 0; j < i; j++) {

            if (dist(gen) < p) {

               graph[i][j] = 1;
               graph[j][i] = 1;
            }
         }
      }

      er_graph = graph;
   }

   // used for debugging
   void printGraph(vector< vector<double> >& graph) {

      for (int i = 0; i < graph.size(); i++) {

         for (int j = 0; j < graph[i].size(); j++) {

            cout << setw(5) << graph[i][j];
         }

         cout << endl;
      }

   }

}

