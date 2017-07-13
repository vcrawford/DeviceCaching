// Classes and functions related to D2D

using namespace std;

// A single mobile device
class Device {

   public:

   // a map of file id to how much of that file this device has
   // (a value between 0 and 1)
   map<int, double> stored_files;

   // m is the number of files
   // initializes with 0 of every file
   Device() {}

};

ostream& operator<<(ostream& os, const Device& x) {

   os << " Stored files: ";

   for (auto it = x.stored_files.begin(); it != x.stored_files.end(); it++) {

      os << "(" << it->first << ", " << it->second << ") ";
   }

   os << endl;

}

