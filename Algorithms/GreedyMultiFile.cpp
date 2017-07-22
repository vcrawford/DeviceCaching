// Greedy algorithm to get max cache hit rate while filling up all caches
// Assumes no caches have anything in them, and all possible file requests come
// from the Zipf distribution

using namespace std;

// c is the cache space per node
// zipf_dist takes each file id to its probability of being the requested file
// puts the file to who should cache it in greedy_nodes
// returns the theoretical overall cache hit rate
double greedyMultiFile(Graph& graph, const int& c, vector<double>& zipf_dist,
   map<int, vector<int> >& greedy_nodes) {

   greedy_nodes.clear();

   // number of files
   int m = zipf_dist.size();

   // number of nodes
   int n = graph.getNodeCount();

   // cache vectors for every file up to any point in greedy algo
   vector< vector<bool> > cache (m, vector<bool> (n, false));

   // vector from each node to how much cache space is left
   vector<int> space (n, c);

   int cache_count = 0; // how many caches we have chosen

   // small gammas of each file
   vector< vector<double> > small_gammas (m, vector<double> (n, 0));

   // and the overall cache hit rate up to any point
   double cache_hit_rate = 0;

   while ((cache_count < n*c) && (cache_hit_rate != 1)) {

      // look for the next cache
      // want to find the file and device such that caching that file in that device gives
      // the maximum new overall cache hit rate compared to any other pair

      // max change to the cache hit rate we have found
      double max_delta_hit_rate = -1;

      // corresponding gammas
      map<int, double> delta_small_gamma_corr;
      double delta_big_gamma_corr;

      // and the corresponding file/device
      int corr_file;
      int corr_dev;

      // search through all possibilities to find the max

      // loop through files
      for (int i = 0; i < m; i++) {  

         // loop through all devices
         for (int j = 0; j < n; j++) {

            // don't need to check any that can't fit this device,
            // or already have it cached
            if (!cache[i][j] && (space[j] > 0)) {

               // check caching file i in node j
               map<int, double> delta_small_gamma;
               double delta_big_gamma;
               gamma_util::computeDeltaGammas(j, cache[i], small_gammas[i], graph, delta_small_gamma,
                  delta_big_gamma);

               // check if it's the best so far
               if (zipf_dist[i]*delta_big_gamma > max_delta_hit_rate) {

                  max_delta_hit_rate = zipf_dist[i]*delta_big_gamma;
                  delta_small_gamma_corr = delta_small_gamma;
                  delta_big_gamma_corr = delta_big_gamma;
                  corr_dev = j;
                  corr_file = i;
               }

            }
      
         }

      }

      // finished looking through all file/device pairs
      // add the max so far

      space[corr_dev]--;
      cache[corr_file][corr_dev] = true;
      cache_count++;
      cache_hit_rate += zipf_dist[corr_file]*delta_big_gamma_corr;

      for (auto it = delta_small_gamma_corr.begin(); it != delta_small_gamma_corr.end(); it++) {

         small_gammas[corr_file][it->first] += it->second;       
      }

      //clog << "Caching file " << corr_file << " in device " << corr_dev
      //  << " to get a cache hit rate of " << cache_hit_rate << ". There are "
      //   << cache_count << " caches chosen." << endl;

   }

   // now add all choices to greedy nodes

   for (int i = 0; i < m; i++) {

      vector<int> to_cache;

      for (int j = 0; j < n; j++) {

         if (cache[i][j]) {

            to_cache.push_back(j);
         }
      }

      if (to_cache.size() > 0) {

         greedy_nodes[i] = to_cache;
      }
   }

   return cache_hit_rate;

}
