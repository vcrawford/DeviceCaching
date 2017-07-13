// Classes and functions related to D2D

using namespace std;

// controls device to device transfer of files
class D2DController {

   // The D2D transmission rate in part of file per second
   // (Assuming file size is 100MB and transmission rate is 3MB per second)
   static constexpr double TRANS_RATE = 0.03;

   // the communication radius
   int radius;

   // all devices, in order of id
   vector<Device>& devices;

   // device id to device id, takes transmitter to receiver for all
   // current communications
   map<int, int> in_transmission;

   // takes transmitting device id to what file it is transmitting
   map<int, int> file_transmission;

   public:

   D2DController(vector<Device>& devices, const int& radius): devices(devices), radius (radius) { }

   double getDistance(const int& device_1, const int& device_2,
                      const vector< pair<int, int>>& locations) {

       pair<int, int> loc_1 = locations[device_1];
       pair<int, int> loc_2 = locations[device_2];

       return sqrt(pow(loc_1.first - loc_2.first, 2.0) + pow(loc_1.second - loc_2.second, 2.0));
   }

   // attempt a D2D connection between two devices
   // data is assumed to go from device_1 to device_2
   bool tryD2D(const int& device_1, const int& device_2, const int& file,
               const vector< pair<int, int> >& locations) {

      // is device_1 already transmitting?

      if (this->in_transmission.find(device_1) != this->in_transmission.end()) {

         return false;
      }

      // is device_2 already overhearing some transmission?

      for (auto it = this->in_transmission.begin(); it != this->in_transmission.end();
           it++) {

         if (this->getDistance(device_2, it->first, locations) <= this->radius) {

            return false;
         }
      }

      // Are they within contact radius of each other?

      if (this->getDistance(device_1, device_2, locations) <= this->radius) {

         this->in_transmission.insert( make_pair(device_1, device_2));         
         this->file_transmission.insert( make_pair(device_1, file));

         return true;
      }

      return false;

   }

   friend ostream& operator<<(ostream&, const D2DController&);

};

ostream& operator<<(ostream& os, const D2DController& x) {

   os << "D2D in transmission: ";

   for (auto it = x.in_transmission.begin(); it != x.in_transmission.end(); it++) {

      os << "(" << it->first << ", " << it->second << ") ";
   }

   os << endl;

   return os;

}

