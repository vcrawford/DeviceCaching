// Cluster fractal points by transitive nearness

// Finds all points in search_points that have a path to start_point and
// puts them in connected_points
// Two nodes are connected if they are within r distance from each other
void searchForConnectedPoints(const int& r, const pair<int, int>& start_point,
                              const list< pair<int, int> >& search_points,
                              vector< pair<int, int> >& connected_points) {

   connected_points.clear();
   connected_points.push_back(start_point);

   // a list that tells us what things have not yet been added into connected_points
   list< pair<int, int> > not_connected (search_points.begin(), search_points.end());
   not_connected.remove(start_point);

   // queue of things in connected points that we should search for nearby points in
   // left_to_search
   queue< pair<int, int> > search_neighbors;
   search_neighbors.push(start_point);

   // Breadth first search
   while (!search_neighbors.empty()) {

      // Search the first element of search_neighbors
      pair<int, int> point_a = search_neighbors.front();
      search_neighbors.pop();

      // Look for neighbors of point_a that are connected
      for (auto it = not_connected.begin(); it != not_connected.end();) {

         pair<int, int> point_b = *it;
         float distance_sq = pow(point_a.first - point_b.first, 2) +
                             pow(point_a.second - point_b.second, 2);

         if ((distance_sq <= pow(r, 2)) && (distance_sq != 0)) {
            // New point that is connected
            // Need to make sure it's not the same point, since there are a
            // few repeats in points
            connected_points.push_back(point_b);
            search_neighbors.push(point_b);
            it = not_connected.erase(it);
         }
         else {
            it++;
         }

      }
   }

}

// Take in all points and cluster them
// Cluster is transitive, where two points are in the same cluster if
// their distance is below r
// Each cluster is a vector in clusters
void computeClusters(const int& r, const vector< pair<int, int> >& points,
                     vector< vector< pair<int, int> > >& clusters) {

   clusters.clear();

   // points that have not yet been clustered
   list< pair<int, int> > unclustered (points.begin(), points.end());

   while (!unclustered.empty()) {
      
      pair<int, int> point = unclustered.front();
      unclustered.pop_front();

      // this points cluster
      vector< pair<int, int> > cluster;
      searchForConnectedPoints(r, point, unclustered, cluster);

      // add this cluster
      clusters.push_back(cluster);

      // don't need to search in these points anymore, they can't be connected
      // to any that weren't already found in the cluster
      for (auto it = unclustered.begin(); it != unclustered.end();) {

         int j_val = -1;
         for (int j = 0; j < cluster.size(); j++) {

            if (cluster[j] == *it) {

               j_val = j;
               break;
            }
         }

         if (j_val != -1) {
            it = unclustered.erase(it);
         }
         else it++;

      }

   }

}
