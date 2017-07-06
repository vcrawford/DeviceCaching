// Classes and functions related to D2D

using namespace std;

// A file that mobile devices request
class File {

   // unique id
   int id;

   // the probability that this file will be requested over a time period
   double request_prob;

};

// A single mobile device
class Device {

   // The D2D transmission rate in MB per second
   static constexpr double TRANS_RATE = 3;

   // the current location of this device
   pair<double, double> location;

   // a map of each file to how much of that file this device has
   map<File*, double> stored_files;

   // other device that this device is transmitting a file to, and how much
   // it has transmitted
   Device* communicating_device = NULL;
   int trans_file;

   Device() {

   }
};

// Returns requests for certain files by certain devices
class FileRequest {

   // each file to the probability of a request being for it (follows Zipf)
   map<File*, double> file_dist;

   // all the devices
   vector<Device*> devices;

   // the probability that any file is requested per second
   double req_dist;

   // random generator
   default_random_engine gen;

   // used for all random numbers, even device
   uniform_real_distribution<double> rand_0_1;

   FileRequest() {

   }

};


// The BS, which controls cellular resources
class BaseStation {

   // the number of resource blocks this BS has to distribute
   static constexpr double NUM_RBS = 300;

   // the rate of transmission for each resource block in MB
   static constexpr double RB_RATE = 0.1;

   // device to file of each device having a transmission
   map<Device*, File*> trans_device;

   BaseStation() {

   }

};


