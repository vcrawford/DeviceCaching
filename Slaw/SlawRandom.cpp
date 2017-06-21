// Helps with random numbers needing to be generated by the SLAW model

using namespace std;

class SlawRandom {

   public:

   // The clusters of fractal points
   vector< vector < pair<int, int> > >& clusters;

   // All the waypoints
   vector< pair<int, int> >& waypoints;

   // Random number generator
   mt19937 generator;

   // Uniform distribution between 0 and 1 
   uniform_real_distribution<float> uniform_0_1;

   // Uniform distribution to pick a point
   uniform_int_distribution<int> uniform_point;

   // Uniform distributions to pick a point from each cluster
   vector< uniform_int_distribution<int> > uniform_point_cluster;

   // exponential distribution in order to get pareto wait times
   exponential_distribution<float> exponential;

   SlawRandom (vector< vector < pair <int, int> > >& clusters, vector< pair<int, int> >& waypoints)
               : clusters(clusters), waypoints(waypoints), generator(time(NULL)), uniform_0_1 (0,1),
                 uniform_point (0, waypoints.size() - 1), exponential (0.25) {

      
      for (int i = 0; i < clusters.size(); i++) {

         this->uniform_point_cluster.push_back(uniform_int_distribution<int> (0, clusters[i].size() - 1));
      }

   }

   // Get number of seconds to wait, sampled from a pareto distribution with min 30 and max
   int getWait() {

      return (int) min(30*exp( this->exponential (this->generator) ), (float) 10000);
   }

   // Pick a random cluster according to its size
   void getRandomCluster(int& picked) {

      pair<int, int> random_point;
      this->getRandomPoint(random_point);

      picked = -1;
      for (int i = 0; i < this->clusters.size(); i++) {

         if (picked == -1) {
            for (int j = 0; j < this->clusters[i].size(); j++) {

               if (random_point == this->clusters[i][j]) {
                  picked = i;
                  break;
               }
            }
         }
         else break;
      }

   }

   // Pick random points from a certain cluster, altogether 10% of cluster size
   void getRandomPointsInCluster(const int& cluster, vector< pair<int, int> >& picked) {

      // add num_waypoints to new_waypoints
      int num_waypoints = ceil(0.1*this->clusters[cluster].size());

      while (picked.size() < num_waypoints) {

         pair<int, int> new_waypoint;
         bool is_new;

         // generate random points until we have a new one            
         do {

            int rand = this->uniform_point_cluster[cluster] (this->generator);
            new_waypoint = this->clusters[cluster][rand];

            // have we already picked this one?
            is_new = true;
            for (int j = 0; j < picked.size(); j++) {

               if (new_waypoint == picked[j]) {

                  is_new = false;
                  break;
               }
            }

            if (is_new) {

               picked.push_back(new_waypoint);
            }
         } while(!is_new);
      }         

   }

   // Pick a random waypoint, every point is equally probable
   void getRandomPoint(pair<int, int>& picked) {

      int rand = this->uniform_point (this->generator);

      picked = this->waypoints[rand];
   }

   // Pick one of the points in the map according to their given probabilities
   // Used for LATP algorithm
   void getRandomPoint ( const map< pair<int, int>, float >& probabilities, pair<int, int>& picked ) {

      // Generate a random number between 0 and 1
      float rand = this->uniform_0_1 (this->generator);

      // And use it in order to pick a point
      float probability_sum = 0;

      for (auto it = probabilities.begin(); it != probabilities.end(); it++) {

         probability_sum += it->second;

         if (rand <= probability_sum) {
            picked = it->first;
            break;
         }
      }

   }
};


