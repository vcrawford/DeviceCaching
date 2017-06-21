#include <iostream>
#include <random>
#include <math.h>
#include <ctime>
#include <fstream>
#include <list>
#include <queue>
#include <unordered_set>
#include <map>
#include "SlawRandom.cpp"
#include "FractalPoints.cpp"
#include "ClusterFractalPoints.cpp"
#include "Walker.cpp"

using namespace std;

// Generates data for SLAW movement model
int main() {

   cout << "Running SLAW movement model ..." << endl;

   cout << "Generating fractal points ..." << endl;

   // Get fractal points for 1000 by 1000 m area
   vector< pair<int, int> > points;
   cout << getFractalPoints(1000, 1000, 2000, 5, points) << " fractal points have been generated "<< endl;

   // Test that all fractal points are unique
   for (int i = 0; i < points.size(); i++) {
      for (int j = 0; j < points.size(); j++) {
         if ((points[i] == points[j]) && (i != j)) {
            cout << "Repeated fractal points!" << endl;
         }
      }
   }

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
         //cout << clusters[i][j].first << "," << clusters[i][j].second << endl;
      } 

   }

   cout << "Generating random walkers ..." << endl; 
   int speed = 1;
   SlawRandom rand (clusters, points);

   // test the random generator

   ofstream random_stream;
   random_stream.open("random_points.txt");

   // WORKS
   //for (int i = 0; i < 100; i++) {
   //   pair<int, int> random_point;
   //   rand.getRandomPoint(random_point);
   //   random_stream << random_point.first << "," << random_point.second << endl;
   //}

   // WORKS
   //int cl;
   //rand.getRandomCluster(cl);
   //for (int i = 0; i < clusters[cl].size(); i++) {
   //   random_stream << clusters[cl][i].first << "," << clusters[cl][i].second << endl;
   //}

   // WORKS
   //int cl;
   //rand.getRandomCluster(cl);
   //vector< pair<int, int> > picked;
   //rand.getRandomPointsInCluster(cl, picked);
   //for (int i = 0; i < picked.size(); i++) {
   //   random_stream << picked[i].first << "," << picked[i].second << endl;
   //}

   Walker w (clusters, points, speed, rand);

   //WORKS
   //for (int i = 0; i < w.clusters.size(); i++) {
   //   int a = w.clusters[i];
   //   for (int j = 0; j < clusters[a].size(); j++) {
   //      pair<int, int> b = clusters[a][j];
   //      random_stream << b.first << "," << b.second << endl;
   //   }
   //}

   // WORKDS
   //for (int i = 0; i < w.waypoints.size(); i++) {

   //   random_stream << w.waypoints[i].first << "," << w.waypoints[i].second << endl;
   //}
}

