// Classes and functions related to D2D

using namespace std;

// Handles the placement of files in the caches of devices
class CacheControllerRandom1: public CacheController {

   // takes file ranks to how many should be randomly cached
   vector<int> file_cache_count;

   // for choosing random devices for each file
   default_random_engine gen;
   uniform_int_distribution<int> rand;

public:

   // file_cache_count takes a ranking to how many of that file we should
   // randomly cache
   CacheControllerRandom1(const int& n, const int& c, FileRanking& file_ranking, const string& alg,
      const vector<int>& file_cache_count):
      CacheController(n, c, file_ranking, alg), file_cache_count (file_cache_count), gen(time(NULL)),
      rand(0, n - 1) {

      clog << "Cache controller random 1 created. Set to cache: ";

      for (int i = 0; i < this->file_cache_count.size(); i++) {

         clog << "file " << this->file_ranking.getPopularFile(i) << " on "
            << this->file_cache_count[i] << " devices, ";
      }

      clog << endl;

      this->cache();

      this->logToCache();

      clog << "Cache controller has finished being created" << endl << flush;

   }

   virtual bool nextTimeStep() {
      // not dynamic

      return false;
   }

   // randomly choose devices to cache each file in
   virtual bool cache() {

      // takes each device to how much space we think it has left
      vector<int> space_left (this->n, this->c);

      for (int i = 0; i < this->file_cache_count.size(); i++) {

         int num_devices_left = this->file_cache_count[i];

         int file = this->file_ranking.getPopularFile(i);

         while (num_devices_left > 0) {

            // find random device id

            int random_device = this->rand(this->gen);

            assert(random_device >= 0);
            assert(random_device < this->n);

            if (!this->alreadyAdded(file, random_device) &&
               (space_left[random_device] > 0)) {

               space_left[random_device]--;

               this->addToCache(file, random_device);

               num_devices_left--;
            }
         }
      }

   }

};
