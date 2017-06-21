
using namespace std;

// Calculate the distance of each point to location
// Used for determining how far different waypoints are
// Store in distances map, where each point is taken to its distance
void getDistances(const list< pair<int, int> >& points, const pair<int, int>& location,
                  map< pair<int, int>, float>& distances) {
   
   distances.clear();

   for (auto iter = points.begin(); iter != points.end(); iter++) {

      pair<int, int> point = *iter;

      float distance_sq = pow(point.first - location.first, 2) + pow(point.second - location.second, 2);

      distances.insert( make_pair(point, sqrt(distance_sq)) );
   }

}

// Pick a route including all elements in points, beginning with start_point, according to
// the "Least Action Trip Planning" algorithm (LATP)
// start_point is an element of points
// Must pass in a random number generator
void getRoute(const vector< pair<int, int> >& points, const pair<int, int>& start_point,
              vector< pair<int, int> >& route, SlawRandom& rand) {

   // Always start route with start_points
   route.clear();
   route.push_back(start_point);

   // Remaining points to be routed
   list< pair<int, int> > unrouted (route.begin(), route.end());
   unrouted.remove(start_point);

   // Add points to route until there are no more
   while (!unrouted.empty()) {

      // Get distances from most recently added point to all unrouted points
      map< pair<int, int>, float> distances;
      getDistances(unrouted, route[route.size() - 1], distances);

      // Get probability of picking each point for the next step according to LATP

      // First of all, sum 1/x over all x where x is the distance
      float dist_inv_sum = 0;
      for (auto it = distances.begin(); it != distances.end(); it++) {

         dist_inv_sum += 1/it->second;
      }

      // Now calculate the probability of picking each one
      map< pair<int, int>, float> probabilities;
      for (auto it = distances.begin(); it != distances.end(); it++) {

         probabilities.insert(make_pair(it->first, (1/it->second)/dist_inv_sum));
      }

      // Now pick a random next stop according to those probabilities
      pair<int, int> next_stop;
      rand.getRandomPoint(probabilities, next_stop);
      route.push_back(next_stop);
      unrouted.remove(next_stop);
   }

   // End with the beginning
   route.push_back(start_point);

}

// Walker class for the SLAW movement model
class Walker {

   public:

   // ids of clusters that this walker regularly goes to
   // comes from the indices of the clusters vector pass in the instructor
   vector<int> clusters;

   // points that this walker regularly goes to
   vector< pair<int, int> > waypoints;

   // The home point, where the walker starts and ends its routes
   pair<int, int> home;

   // The location that the walker currently is
   pair<float, float> location;

   // The distance the walker will move with every update
   int speed;

   // The route the walker is taking
   vector< pair<int,int> > route;

   // What point in the route the walker is going towards
   int route_step;

   // How long to wait at current location
   int seconds_to_wait;

   // What we use to get random things
   SlawRandom& rand;

   Walker(const vector< vector< pair<int, int> > >& clusters, const vector< pair<int, int> >& waypoints,
          const int& speed, SlawRandom& rand): rand(rand) {

      this->pickClusters();
      this->pickWaypoints();      
      this->home = this->waypoints[0];
      this->location = this->home;
      this->speed = speed;
      this->seconds_to_wait = 0;
      this->route.push_back(this->home);
      this->route_step = 1;
   }

   // Pick 5 random clusters that this walker will visit
   void pickClusters() {

      this->clusters.clear();

      while (this->clusters.size() < 5) {

         int random_cluster;
         bool is_new;

         // keep generatig random cluster until it is new
         do {

            // new random cluster
            this->rand.getRandomCluster(random_cluster);

            
            // compare to all existing clusters
            is_new = true;
            for (int i = 0; i < this->clusters.size(); i++) {

               if (random_cluster == this->clusters[i]) {
                  is_new = false;
                  break;
               }
            }

            // didn't match any, so add to clusters
            if (is_new) {

               this->clusters.push_back(random_cluster);
            }

         } while (!is_new);
      }


   }

   // pick the regular waypoints, i.e. 10% of each cluster
   void pickWaypoints() {

      // for each cluster, add 10% of waypoints to this->waypoints
      for (int i = 0; i < this->clusters.size(); i++) {

         vector< pair<int, int> > new_waypoints;

         this->rand.getRandomPointsInCluster(this->clusters[i], new_waypoints);

         for (int k = 0; k < new_waypoints.size(); k++) {

            this->waypoints.push_back(new_waypoints[k]);
         }
      }

   }

   // Pick a new route
   void pickRoute() {

      // One extra cluster we go to
      int extra_cluster;
      vector< pair<int,int> > extra_waypoints;

      // keep generating random cluster until it is one we don't have
      bool is_new;
      do {

         // new random cluster
         this->rand.getRandomCluster(extra_cluster);

         // compare to all existing clusters
         is_new = true;
         for (int i = 0; i < this->clusters.size(); i++) {

            if (extra_cluster == this->clusters[i]) {
               is_new = false;
               break;
            }
         }
      } while (!is_new);


      this->rand.getRandomPointsInCluster(extra_cluster, extra_waypoints); 

      // Now combine the extra waypoints with usual ones and get a route
      vector< pair<int, int> > all_points = this->waypoints;
      all_points.insert(all_points.end(), extra_waypoints.begin(), extra_waypoints.end());

      getRoute(all_points, this->home, this->route, this->rand); 

      // The walker is sent to the beginning of the route (if it isn't there already)
      this->route_step = 0;
      this->location = this->home;
   }


   // Computes the new location for the next second, assuming the walker is heading at 1 m/s to the next
   // point on its route at any point in time
   // Also pauses at each waypoint according to some distribution
   void updateLocation() {

      if (this->route_step >= this->route.size()) {
         // We have completed our route
      }
      else if (this->seconds_to_wait > 0) {
         // We are waiting at a waypoint
         this->seconds_to_wait = this->seconds_to_wait - 1;
      }
      else {
         // Move towards our next stop
         pair<int, int> next_waypoint = this->route[this->route_step];
         pair<float, float> route_vector = make_pair(next_waypoint.first - this->location.first,
                                                     next_waypoint.second - this->location.second);
         float route_mag = sqrt(pow(route_vector.first, 2) + pow(route_vector.second, 2));
      
         if (route_mag < this->speed) {
            // Close enough
            this->seconds_to_wait = this->rand.getWait();
            this->route_step = this->route_step + 1;
         }
         else {
            // Move forward along vector speed distance
            float move_distance = this->speed*(1.0/route_mag);
            this->location = make_pair(this->location.first + move_distance*route_vector.first,
                                       this->location.second + move_distance*route_vector.second);
         }
      }

   }

};

