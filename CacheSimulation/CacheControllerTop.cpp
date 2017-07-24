
using namespace std;

// Handles the placement of files in the caches of devices
// In this one, every device caches the top c files at any point in time
class CacheControllerTop: public CacheController {

   // holds the top c files (not necessarily in order)
   list<int> top_c;

   // use this to cache on all devices
   vector<int> all_device_ids;

public:

   // n is the number of nodes (number of devices)
   // c is the number of files a single node can cache
   CacheControllerTop(const int& n, const int& c,
      FileRanking& file_ranking, const string& alg):
      CacheController(n, c, file_ranking, alg) {

      clog << "Cache controller that maintains the top " << c << " files at all times "
         << " has been initiated." << endl;

      // get all device ids
      for (int i = 0; i < n; i++) {

         this->all_device_ids.push_back(i);
      }

      // add top ten to cache
      for (int i = 0; i < c; i++) {

         int file = this->file_ranking.getPopularFile(i);

         clog << "Adding file " << file << " to be cached." << endl;

         this->top_c.push_back(file);

         this->to_cache[file] = this->all_device_ids;
      }

   }

   // checks if the top c has changed
   // if it has, make appropriate changes to caches
   bool nextTimeStep() { 

      // search for each of our top c in the ranking
      for (auto it = this->top_c.begin(); it != this->top_c.end();) {

         // did we find the ith thing in our top c in the 
         // ranking's top c
         bool found = false;

         for (int j = 0; j < this->c; j++) {

            if (this->file_ranking.getPopularFile(j) == *it) {

               found = true;
               break;
            }
         }

         if (!found) {

            clog << "File " << *it << " is no longer in the top " << this->c
               << " most popular files. Need to uncache." << endl;

            this->to_uncache.push_back(*it);
            it = this->top_c.erase(it);
         }
         else it++;
      }

      // now search for the ranking's top c in ours
      for (int i = 0; i < this->c; i++) {

         bool found = false;

         for (auto it = this->top_c.begin(); it != this->top_c.end(); it++) {

            if (this->file_ranking.getPopularFile(i) == *it) {

               found = true;
               break;
            }
         }

         if (!found) {

            int file = this->file_ranking.getPopularFile(i);

            clog << "File " << file << " is new in the top " << this->c
               << " most popular files. Need to cache." << endl;

            this->top_c.push_back(file);

            this->to_cache[file] = this->all_device_ids;
         }
      } 

      assert (this->to_cache.size() == this->to_uncache.size());
      assert (this->top_c.size() == this->c);

      return (this->to_cache.size() > 0);

   }

};
