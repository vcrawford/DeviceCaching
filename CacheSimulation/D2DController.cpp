// Classes and functions related to D2D

using namespace std;

// controls device to device transfer of files
class D2DController {

   // The D2D transmission rate in part of file per second
   // (Assuming file size is 100MB and transmission rate is 3MB per second)
   static constexpr double TRANS_RATE = 0.03;

   // all devices, in order of id
   vector<Device>& devices;

   // device id to device id, takes transmitter to receiver for all
   // current communications
   map<int, int> in_transmission;

   public:

   D2DController(vector<Device>& devices): devices(devices) { }

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

