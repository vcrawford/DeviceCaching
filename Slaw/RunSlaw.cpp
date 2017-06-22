#include <iostream>
#include <random>
#include <math.h>
#include <ctime>
#include <fstream>
#include <list>
#include <queue>
#include <unordered_set>
#include <map>
#include <cstdlib>
#include "SlawRandom.cpp"
#include "FractalPoints.cpp"
#include "ClusterFractalPoints.cpp"
#include "Walker.cpp"

using namespace std;

// Generates data for SLAW movement model
int main(int argc, char* argv[]) {

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

   // Create all the walkers

   int speed = 1;
   SlawRandom rand (clusters, points);
   
   int walker_count = atoi(argv[1]);
   vector<Walker> walkers;

   cout << "Creating " << walker_count << " walkers..." << endl;

   for (int i = 0; i < walker_count; i++) {

      Walker w (clusters, points, speed, rand);
      walkers.push_back(w);
   }

   // Run simulation
   int days = atoi(argv[2]);
   int route_start = atoi(argv[3]);
   int total_time = days*86400;

   cout << "Running movement for " << days << " day(s) where routes start at hour "
        << route_start << " of each day ..."  << endl;


   ofstream walker_locations;
   walker_locations.open("locations.txt");

   for (int day = 0; day < days; day++) {

      for (int second = 0; second < 86400; second++) {

         if (second == route_start*60*60) {

            for (int i = 0; i < walker_count; i++) {

               walkers[i].pickRoute();
            }
         }

         for (int i = 0; i < walker_count; i++) {

            walkers[i].updateLocation();
            walker_locations << walkers[i].location.first << " "
                             << walkers[i].location.second << endl;
         }

         cout << "\r" << (int) 100*(day*86400 + second)/total_time << "%" << flush;
      }
   }

   cout << endl;
}

