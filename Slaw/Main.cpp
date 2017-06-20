#include <iostream>
#include <random>
#include <math.h>
#include <ctime>
#include <fstream>
#include "FractalPoints.cpp"

using namespace std;

// Generates data for SLAW movement model
int main() {

   cout << "Running SLAW movement model ..." << endl;

   cout << "Generating fractal points ..." << endl;
   // Get fractal points for 1000 by 1000 m area
   vector< pair<int, int> > points;
   cout << getFractalPoints(1000, 1000, 2000, 5, points) << " fractal points have been generated "<< endl;

   // Print fractal points to file "fractal_points.txt"
   cout << "Writing fractal points to file fractal_points.txt" << endl;

   ofstream fractal_points_strm;

   fractal_points_strm.open("fractal_points.txt");

   for (int i = 0; i < points.size(); i++) {

      fractal_points_strm << points[i].first << "," << points[i].second << endl;
   } 
}

