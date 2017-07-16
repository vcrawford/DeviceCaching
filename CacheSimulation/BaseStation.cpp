// Classes and functions related to D2D

using namespace std;

// The BS, which controls cellular resources
class BaseStation {

   // the number of resource blocks this BS has to distribute
   static constexpr double NUM_RBS = 300;

   // all devices in the system, in order of id
   vector<Device>& devices;

   // all current transmissions from BS to device
   // may be to single device, or multicast
   list<SingleBSTransmission> in_transmission;
   list<CacheBSTransmission> in_transmission_MC;

   public:

   BaseStation(vector<Device>& devices): devices(devices) { }

   // update everything over a second
   void nextTimeStep() {

      cout << "New time step for the BS" << endl;

      if ((this->in_transmission.size() != 0) || (this->in_transmission_MC.size() != 0)) {

         cout << "There are " << this->in_transmission.size() + this->in_transmission_MC.size()
            << " things being transmitted by the BS" << endl;
         // the number of resource blocks each communication should get
         // just split them up equally
         int rbs = BaseStation::NUM_RBS/(this->in_transmission.size() + this->in_transmission_MC.size());

         cout << "Each communication will be allotted " << rbs << " resource blocks" << endl;

         for (auto it = this->in_transmission.begin(); it != this->in_transmission.end();) {

            bool completed;

            completed = it->nextTimeStep(rbs);

            if (completed) {

               cout << "The BS giving file " << it->file << " to device " << it->device.id
                  << " is complete" << endl;

               it = this->in_transmission.erase(it);
            }
            else it++;
         }

         for (auto it = this->in_transmission_MC.begin(); it != this->in_transmission_MC.end(); it) {

            bool completed;

            completed = it->nextTimeStep(rbs);

            if (completed) {

               cout << "The multicast of file " << it->file << " to " << it->devices.size()
                  << " devices is complete " << endl;

               it = this->in_transmission_MC.erase(it);
            }
            else it++;
         }
      }

   }

   // any D2D communications that have failed will have the rest of
   // the file delivered by the BS
   void takeFailedD2D(vector<D2DTransmission>& failed) {

      for (int i = 0; i < failed.size(); i++) {

         cout << "The BS is taking over failed D2D transmission of file " << failed[i].file
            << " to device " << failed[i].device_rec.id << endl;

         this->newRequest(failed[i].device_rec.id, failed[i].file);
      }

   }

   // do multicast of a file to a bunch of devices
   void newCache(const int& file, const vector<int>& device_ids) {

      // get devices corresponding to those ids
      vector< reference_wrapper<Device> > multicast_devices;

      for (int i = 0; i < device_ids.size(); i++) {

         multicast_devices.push_back(this->devices[device_ids[i]]);
      }

      cout << "Adding transmission of file " << file << " to " << multicast_devices.size()
         << " devices to BS transmissions" << endl;

      this->in_transmission_MC.push_back( CacheBSTransmission (multicast_devices, file) );

   }

   // send file to device
   void newRequest(const int& device_id, const int& file) {

      cout << "BS has received request for file " << file << " from device " << device_id << endl;

      assert ((device_id < this->devices.size()) && (device_id >= 0));

      this->in_transmission.push_back( SingleBSTransmission (this->devices[device_id], file) );

   }

};
