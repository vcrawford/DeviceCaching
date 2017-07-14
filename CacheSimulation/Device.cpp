// Classes and functions related to D2D

using namespace std;

// A single mobile device
class Device {

   public:

   int id;

   // a map of file id to how much of that file this device has
   // (a value between 0 and 1)
   map<int, double> stored_files;

   // m is the number of files
   // initializes with 0 of every file
   Device(const int& id): id(id) {}

   bool hasFile(const int& file) {

      if (this->stored_files.find(file) != this->stored_files.end()) {

         return (this->stored_files.at(file) == 1.0);
      }

      return false;

   }

   // adds how_much of file, capping at 1
   // returns whether it has the entire file or not
   bool addFile(const int& file, const double& how_much) {

      if (this->stored_files.find(file) == this->stored_files.end()) {

         this->stored_files.insert( make_pair(file, 0) );
      }

      this->stored_files[file] += how_much;

      if (this->stored_files[file] >= 1) {

         this->stored_files[file] = 1;

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

