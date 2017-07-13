// Classes and functions related to D2D

using namespace std;

// The BS, which controls cellular resources
class BaseStation {

   // the number of resource blocks this BS has to distribute
   static constexpr double NUM_RBS = 300;

   // the rate of transmission for each resource block in portion of file
   // this corresponds to 100MB file size, and 0.1MB/s/rb
   static constexpr double RB_RATE = 0.001;

   // all devices in the system, in order of id
   vector<Device>& devices;

   // all current transmissions to a single device
   list<BSTransmission> in_transmission;

   // all current transmissions that are multicast
   list<MulticastBSTransmission> in_transmission_multi;

   public:

   BaseStation(vector<Device>& devices): devices(devices) { }

   // do multicast of a file to a bunch of devices
   void newMulticast(const int& file, const vector<Device>& devices) {

      this->in_transmission.push_back( MulticastBSTransmission (devices, file) );

   }

   // send file to device
   void newRequest(const Device& device, const int& file) {

      this->in_transmission.push_back( BSTransmission (device, file) );
   }

   friend ostream& operator<<(ostream&, const BaseStation&);

};

ostream& operator<<(ostream& os, const BaseStation& x) {

   os << "Base station in transmission: ";

   for (auto it = x.in_transmission.begin(); it != x.in_transmission.end(); it++) {

      os << "(" << it->first << ", " << it->second << ") ";
   }

   os << endl;

   return os;

}

