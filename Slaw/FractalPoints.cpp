// Get fractal points for the SLAW mobility model

using namespace std;

// Find count number of fractal waypoints to distribute over a square
// Basically, we split up the square by choosing a random midpoint and then placing a random
// portion of the count points in each square
// Levels is how many times we should split the squares until finally placing the points
// Add fractal waypoints to the vector points
// Returns the number of points that were placed successfully
// Need to pass in random number generator
int addFractalPoints(const int& x_min, const int& x_max, const int& y_min, const int& y_max,
                     vector< pair<int, int> >& points, const int& levels, const int& level,
                     const int& count, const int& total_count, default_random_engine& generator) {

   if ((count == 0) || (x_min >= x_max) || (y_min >= y_max)) {
      // can't place points here
      return 0;
   }

   // Find actual midpoint of square
   float mid_x = (x_min + x_max)/2.0;
   float mid_y = (y_min + y_max)/2.0;

   //cout << endl;
   //cout << "Range: " << x_min << ", " << x_max << " and " << y_min << ", " << y_max << endl;
   //cout << "Actual midpoint: " << mid_x << ", " << mid_y << endl;

   if ((level > levels) || ((x_max - x_min) < 4) || ((y_max - y_min) < 4)) {

      // No more splitting, time to place the points

      // Get normal distributions from which the points will be drawn
      float variance = min(x_max - x_min, y_max - y_min);
      normal_distribution<float> distribution_x (mid_x, 0.3*(x_max - x_min));
      normal_distribution<float> distribution_y (mid_y, 0.3*(y_max - y_min));

      // Draw each point randomly

      for (int i = 0; i < count; i++) {
         
         pair<int, int> point = make_pair((int) round(distribution_x(generator)),
                                          (int) round(distribution_y(generator)));

         //cout << "Adding point " << point.first << ", " << point.second << endl;
         points.push_back(point);

      }

      // We placed all the points
      return count;
   }

   // We split again
   // Randomly move that midpoint a bit to introduce some variation in the
   // square sizes
   normal_distribution<float> distribution_x (mid_x, 0.1*(x_max - x_min));
   normal_distribution<float> distribution_y (mid_y, 0.1*(y_max - y_min));
   float random_mid_x = distribution_x(generator);
   float random_mid_y = distribution_y(generator);


   // Randomly determine how the total number of points will be distributed
   // amongst the squares
   normal_distribution<float> distribution_points (count/4.0, (0.15*total_count)/(pow(2, 0.8*level)));

   // sample 4 values, one for each square
   vector<float> count_samples;

   // Also will need to normalize by this, so the sum is count
   float sample_sum = 1;

   // Get random counts
   //cout << "Sampling the sub-square counts ..." << endl;
   for (int j = 0; j < 4; j++) {

      float count_sample = distribution_points(generator);
      count_sample = max((float)0, count_sample);
      //cout << " " << count_sample << endl;
      sample_sum += count_sample;
      count_samples.push_back(count_sample);
   }

   // Normalize
   vector<int> count_samples_normalized;

   for (int j = 0; j < 4; j++) {

      count_samples_normalized.push_back((int) round((count_samples[j]/sample_sum)*count));
   }

   
   // Place points on the smaller squares
   int p1 = addFractalPoints(x_min, random_mid_x, y_min, random_mid_y, points, levels, level + 1,
                             count_samples_normalized[0], total_count, generator);
   int p2 = addFractalPoints(x_min, random_mid_x, random_mid_y, y_max, points, levels, level + 1,
                             count_samples_normalized[1], total_count, generator);
   int p3 = addFractalPoints(random_mid_x, x_max, y_min, random_mid_y, points, levels, level + 1,
                             count_samples_normalized[2], total_count, generator);
   int p4 = addFractalPoints(random_mid_x, x_max, random_mid_y, y_max, points, levels, level + 1,
                             count_samples_normalized[3], total_count, generator);

   return p1 + p2 + p3 + p4;
}

// Get all fractal points over the square region [0, width] by [0, height] and put in points vector
// count is the total number of fractal points there should be
// levels is how many times we should split into smaller squares before placing points
// Returns the number of points placed
int getFractalPoints(int width, int height, int count, int levels, vector< pair<int, int> >& points) {

   points.clear();

   // get random number generator
   default_random_engine generator (time(NULL));

   return addFractalPoints(0, width, 0, height, points, levels, 1, count, count, generator);
}

