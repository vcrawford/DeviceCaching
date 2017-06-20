#include <iostream>
#include <random>
#include <math.h>
#include <ctime>
#include <fstream>
#include <list>
#include <queue>
#include <unordered_set>
#include "FractalPoints.cpp"
#include "ClusterFractalPoints.cpp"

using namespace std;

// Generates data for SLAW movement model
int main() {

   cout << "Running SLAW movement model ..." << endl;

   cout << "Generating fractal points ..." << endl;

   // Get fractal points for 1000 by 1000 m area
   vector< pair<int, int> > points;
   cout << getFractalPoints(1000, 1000, 2000, 5, points) << " fractal points have been generated "<< endl;

   // Cluster the fractal points
   int fractal_radius = 20;
   cout << "Clustering fractal points using radius " << fractal_radius << " ..." << endl;
   vector< vector< pair<int, int> > > clusters;
   computeClusters(fractal_radius, points, clusters);

   int count_cluster_points = 0;
   for (int i = 0; i < clusters.size(); i++) {
      count_cluster_points += clusters[i].size();
   }

   cout << count_cluster_points << " fractal points have been put into " << clusters.size() << " clusters." << endl;

   // Print fractal clusters to file "fractal_points.txt"
   cout << "Writing fractal clusters to file fractal_points.txt" << endl;

   ofstream fractal_points_strm;

   fractal_points_strm.open("fractal_points.txt");

   for (int i = 0; i < clusters.size(); i++) {

      fractal_points_strm << "cluster" << i << endl;

      for (int j = 0; j < clusters[i].size(); j++) {

         fractal_points_strm << clusters[i][j].first << "," << clusters[i][j].second << endl;
      } 

   }

}

