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

      if ((this->in_transmission.size() != 0) || (this->in_transmission_MC.size() != 0)) {

         // the number of resource blocks each communication should get
         // just split them up equally
         int rbs = BaseStation::NUM_RBS/(this->in_transmission.size() + this->in_transmission_MC.size());

         for (auto it = this->in_transmission.begin(); it != this->in_transmission.end(); it++) {

            bool completed;

            completed = it->nextTimeStep(rbs);

            if (completed) {

               it = this->in_transmission.erase(it);
            }
         }

         for (auto it = this->in_transmission_MC.begin(); it != this->in_transmission_MC.end(); it++) {

            bool completed;

            completed = it->nextTimeStep(rbs);

            if (completed) {

               it = this->in_transmission_MC.erase(it);
            }
         }
      }

   }

   // any D2D communications that have failed will have the rest of
   // the file delivered by the BS
   void takeFailedD2D(vector<D2DTransmission>& failed) {

      for (int i = 0; i < failed.size(); i++) {

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

      this->in_transmission_MC.push_back( CacheBSTransmission (multicast_devices, file) );

   }

   // send file to device
   void newRequest(const int& device_id, const int& file) {

      assert ((device_id < this->devices.size()) && (device_id >= 0));

      this->in_transmission.push_back( SingleBSTransmission (this->devices[device_id], file) );

   }

};
