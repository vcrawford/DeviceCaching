// Classes and functions related to D2D

using namespace std;

// controls device to device transfer of files
class D2DController {

   // the communication radius
   int radius;

   // all devices, in order of id
   vector<Device>& devices;

   // all ongoing D2D transmissions 
   list<D2DTransmission> in_transmission;

   // current locations of all devices
   vector< pair<int, int> >& current_locations;

   public:

   D2DController(vector<Device>& devices, const int& radius, vector< pair<int, int> >& current_locations)
      : devices (devices), radius (radius), current_locations (current_locations) { }

   double getDistance(const int& device_1, const int& device_2) {

       pair<int, int> loc_1 = this->current_locations[device_1];
       pair<int, int> loc_2 = this->current_locations[device_2];

       return sqrt(pow(loc_1.first - loc_2.first, 2.0) + pow(loc_1.second - loc_2.second, 2.0));
   }

   // go through in_transmission to see if a certain device is already transmitting
   bool isTransmitting(const Device& dev) {

      for (auto it = this->in_transmission.begin(); it != this->in_transmission.end(); it++) {

         if (it->device_send.id == dev.id) {

            return true;
         }
      }

      return false;

   }

   // can a device with id dev overhear an ongoing transmission
   bool canHear(const int& dev) {

      for (auto it = this->in_transmission.begin(); it != this->in_transmission.end(); it++) {

         if (this->withinRadius(dev, it->device_send.id)) {

            return true;
         }
      }

      return false;

   }

   // are two devices within radius of each other
   bool withinRadius(const int& d1, const int& d2) {

      if (this->getDistance(d1, d2) <= this->radius) {

         return true;
      }

      return false;

   }

   // get Device object from device id
   Device& getDevice(const int& device_id) {

      return this->devices[device_id];

   }

   // attempt a D2D connection between two devices
   // data is assumed to go from device_1 to device_2
   bool tryD2D(const int& device_1, const int& device_2, const int& file) {

      // is device_1 already transmitting?

      if (this->isTransmitting(this->devices[device_1])) {

         return false;
      }

      // is device_2 already overhearing some transmission?

      if (this->canHear(device_2)) {

         return false;
      }

      // Are they within contact radius of each other?

      if (this->withinRadius(device_1, device_2)) {

         this->in_transmission.push_back( D2DTransmission(this->getDevice(device_1),
            this->getDevice(device_2), file) );         

         return true;
      }

      return false;

   }

};

