// Classes and functions related to D2D

using namespace std;

// The BS, which controls cellular resources
class BaseStation {

   // the number of resource blocks this BS has to distribute
   static constexpr double NUM_RBS = 250;

   // all devices in the system, in order of id
   vector<Device>& devices;

   // all current transmissions from BS to device
   // may be to single device, or multicast
   list<SingleBSTransmission> in_transmission;
   list<CacheBSTransmission> in_transmission_MC;

   public:

   BaseStation(vector<Device>& devices): devices(devices) { }

   int inTransmissionCount() {

      return this->in_transmission.size() + this->in_transmission_MC.size();
   }

   // update everything over a second

   void nextTimeStep() {

      clog << "There are " << this->in_transmission.size() + this->in_transmission_MC.size()
         << " things being transmitted by the BS" << endl;

      if ((this->in_transmission.size() != 0) || (this->in_transmission_MC.size() != 0)) {

         // count how many we have transmitted this time step. Can't send more than the number of RBs
         int transmition_count = 0;

         int rbs;

         if ((this->in_transmission.size() + this->in_transmission_MC.size()) > BaseStation::NUM_RBS) {

            // going to need to cut some
            rbs = 1;
         }
         else {

            rbs = BaseStation::NUM_RBS/(this->in_transmission.size() + this->in_transmission_MC.size());
         }

         clog << "Each communication will be allotted " << rbs << " resource blocks" << endl;

         for (auto it = this->in_transmission.begin(); it != this->in_transmission.end();) {

            transmition_count++;

            if (transmition_count > BaseStation::NUM_RBS) {

               clog << "The BS is overloaded. Some data could not be transmitted." << endl;
               break;
            }

            bool completed;

            completed = it->nextTimeStep(rbs);

            if (completed) {

               clog << "The BS giving file " << it->file << " to device " << it->device.id
                  << " is complete" << endl;

               it = this->in_transmission.erase(it);
            }
           else it++;
         }

            
         for (auto it = this->in_transmission_MC.begin(); it != this->in_transmission_MC.end(); it) {

            transmition_count++;

            if (transmition_count > BaseStation::NUM_RBS) {

               clog << "The BS is overloaded. Some data could not be transmitted." << endl;
               break;
            }

            bool completed;

            completed = it->nextTimeStep(rbs);

            if (completed) {

               clog << "The multicast of file " << it->file << " to " << it->devices.size()
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

         clog << "The BS is taking over failed D2D transmission of file " << failed[i].file
            << " to device " << failed[i].device_rec.id << endl;

         this->newRequest(failed[i].device_rec.id, failed[i].file);
      }

   }

   // do multicast of a file to a bunch of devices
   void newCache(const int& file, const vector<int>& device_ids) {

      clog << "Caching file " << file << "..." << endl;

      // get devices corresponding to those ids
      vector< reference_wrapper<Device> > multicast_devices;

      for (int i = 0; i < device_ids.size(); i++) {

         multicast_devices.push_back(this->devices[device_ids[i]]);
      }

      clog << "Multicast transmission of file " << file << " to "
         << multicast_devices.size() << " devices from the BS has been initiated." << endl;

      this->in_transmission_MC.push_back( CacheBSTransmission (multicast_devices, file) );

   }

   // send file to device
   void newRequest(const int& device_id, const int& file) {

      clog << "Transmission of file " << file << " to device " << device_id << " by the BS "
         << "has been initiated" << endl;

      assert ((device_id < this->devices.size()) && (device_id >= 0));

      this->in_transmission.push_back( SingleBSTransmission (this->devices[device_id], file) );

   }

};
