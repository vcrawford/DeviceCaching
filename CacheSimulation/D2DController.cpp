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

   // takes the file to the number of successful D2D transmissions
   map<int, int> success;

   D2DController(vector<Device>& devices, const int& radius, vector< pair<int, int> >& current_locations)
      : devices (devices), radius (radius), current_locations (current_locations) { }

   // updates the total number of cache hits for this file
   void countSuccess(const int& file) {

      if (this->success.find(file) == this->success.end()) {

         this->success.insert( make_pair(file, 0) );
      }

      this->success[file]++;
   }

   double getDistance(const int& device_1, const int& device_2) {

      assert ((device_1 < this->current_locations.size()) && (device_2 < this->current_locations.size()));

       pair<int, int> loc_1 = this->current_locations[device_1];
       pair<int, int> loc_2 = this->current_locations[device_2];

       return sqrt(pow(loc_1.first - loc_2.first, 2.0) + pow(loc_1.second - loc_2.second, 2.0));
   }

   // update all D2D transmissions
   // if any fail (for example, out of range) then add them to failed and remove
   // them from self
   void nextTimeStep(vector<D2DTransmission>& failed) {

      clog << "There are " << this->in_transmission.size()
         << " d2d communications currently." << endl;

      failed.clear();
 
      for (auto it = this->in_transmission.begin(); it != this->in_transmission.end();) {

         // have the devices moved out of range?

         if (!this->withinRadius(it->device_send.id, it->device_rec.id)) {

            clog << "Device " << it->device_send.id << " and device " << it->device_rec.id
               << " are no longer in contact. D2D fail.";

            failed.push_back(*it);

            it = this->in_transmission.erase(it);

            break;
         }
         else {

            // now just have them transmit more of the file
            // remove if finished

            bool completed;

            completed = it->nextTimeStep();

            if (completed) {

               clog << "Transmission between " << it->device_send.id << " and "
                  << it->device_rec.id << " is complete" << endl;

               this->countSuccess(it->file);

               it = this->in_transmission.erase(it);
            }
            else it++;
         }
      }

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

         clog << "D2D transmission of file " << file << " from device " << device_1
            << " to " << device_2 << " initiated" << endl;

         this->in_transmission.push_back( D2DTransmission(this->getDevice(device_1),
            this->getDevice(device_2), file) );         

         return true;
      }

      return false;

   }


};

