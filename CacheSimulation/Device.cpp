// Classes and functions related to D2D

using namespace std;

// A single mobile device
class Device {

   public:

   int id;

   // a map of file id to how much of that file this device has
   // (a value between 0 and 1)
   // some files stay after being downloaded (like cache files) while
   // others are gotten rid of once the user has the full file
   map<int, double> stored_files;

   // whether this device is currently downloading a file (from BS or another device)
   bool is_downloading;

   // the size of this device's cache
   int cache_size;

   // a queue of recently downloaded files
   // only accurate if devices always cache
   queue<int> file_downloads;

   // do we always cache the lastest files
   bool always_cache;

   // m is the number of files
   // initializes with 0 of every file
   Device(const int& id, const int& cache_size, bool always_cache):
      id(id), is_downloading (false), cache_size (cache_size),
      always_cache (always_cache) {}

   void cancelDownload(const int& file) {

      assert(this->is_downloading);
      assert(this->stored_files.find(file) != this->stored_files.end());

      this->is_downloading = false;

   }

   void removeFile(const int& file) {

      assert (this->stored_files.find(file) != this->stored_files.end());

      this->stored_files.erase(file);
   }

   // compute the number of files cached, meaning stored entirely
   // if a device is downloading a file, and not caching it, it's storage won't
   // be 1
   int numberCached() {

      int number_cached = 0;

      for (auto it = this->stored_files.begin(); it != this->stored_files.end();
         it++) {

         if (it->second == 1) {

            number_cached += 1;
         }
      }

      return number_cached;
   }

   bool hasFile(const int& file) {

      if (this->stored_files.find(file) != this->stored_files.end()) {

         return (this->stored_files.at(file) == 1.0);
      }

      return false;

   }

   // has any part of file, not necessarily the whole thing
   bool hasFilePart(const int& file) {

     if (this->stored_files.find(file) != this->stored_files.end()) {

         return true;
      }

      return false;
   }

   // adds how_much of file, capping at 1
   // returns whether it has the entire file or not
   // has option to cache or not when finished
   bool downloadFile(const int& file, const double& how_much, const bool& cache) {

      assert ((this->cache_size - (int)this->stored_files.size()) >= -1);

      assert ((how_much > 0) && (how_much <= 1));

      this->is_downloading = true;

      if (this->stored_files.find(file) == this->stored_files.end()) {

         this->stored_files.insert( make_pair(file, 0) );
      }

      //assert (this->stored_files[file] != 1);

      this->stored_files[file] += how_much;

      if (this->stored_files[file] >= 1) {

         if (cache || this->always_cache) {

            clog << "Device " << this->id << " is caching file " << file << endl;

            this->stored_files[file] = 1.0;

            if (this->always_cache) {

               this->file_downloads.push(file);

               if (this->file_downloads.size() > this->cache_size) {

                  int oldest = this->file_downloads.front();

                  assert(this->stored_files.find(oldest) != this->stored_files.end());

                  clog << "Device " << this->id << " is uncaching file " << oldest << endl;

                  this->stored_files.erase(oldest);
                  this->file_downloads.pop();

                  assert(this->file_downloads.size() == this->cache_size);
               }
            }
         }
         else {

            // not caching, get rid of it
            this->stored_files.erase(file);
         }

         this->is_downloading = false;

         return true;
      }

      return false;

   }

};

ostream& operator<<(ostream& os, const Device& x) {

   os << "Device " << x.id << " stored files: ";

   for (auto it = x.stored_files.begin(); it != x.stored_files.end(); it++) {

      os << "(" << it->first << ", " << it->second << ") ";
   }

   os << endl;

}

