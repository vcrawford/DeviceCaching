// Classes and functions related to D2D

using namespace std;

// Returns requests from a device
class DeviceRequest {

   // random generator
   default_random_engine gen;

   // for which device
   uniform_int_distribution<int> rand_0_n;

   public:

   // n is the number of devices
   DeviceRequest(const int& n): gen (time(NULL)), rand_0_n (0, n) { }

   // get a request from a random device
   // returns the device's id
   int getRequest() {

      return rand_0_n (gen);
   }

};


