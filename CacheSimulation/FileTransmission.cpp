
class BSTransmission {

public:

   double rate = 0.001;
};

class SingleBSTransmission: public BSTransmission {

public:

   // what device the BS is transferring files to
   Device& device;

   // what file is being transferred
   int file;

   SingleBSTransmission(Device& device, int file): device(device), file(file) { }

   bool nextTimeStep(const int& rbs) {

      return this->device.downloadFile(this->file, BSTransmission::rate*rbs, false);

   }

};

class CacheBSTransmission: public BSTransmission {

public:

   vector<reference_wrapper<Device>> devices;

   int file;

   bool cancelled;

   // has any data actually been submitted yet
   bool started;

   CacheBSTransmission (vector< reference_wrapper<Device> >& devices, const int& file):
      file(file), cancelled (false), started (false) {

      this->file = file;

      this->devices = devices;

   }

   // returns whether the devices are ready to begin transmission
   // i.e., none are downloading
   // can only be called if no data has yet been transferred
   bool devicesReady() {

      assert(!this->started);

      for (int i = 0; i < this->devices.size(); i++) {

         if (this->devices[i].get().is_downloading) return false;
      }

      return true;
   }

   // does not clear file from node's storage
   // just turns off downloading flag
   void cancelTransmission() {

      assert(!this->cancelled);

      this->cancelled = true;

      for (auto it = this->devices.begin(); it != this->devices.end(); it++) {

         it->get().cancelDownload(this->file);
      }
   }

   bool nextTimeStep(const int& rbs) {

      assert (!this->cancelled);

      bool all_complete = true;

      for (auto it = this->devices.begin(); it != this->devices.end(); it++) {

         if (!it->get().hasFile(this->file)) {

            bool single_complete = it->get().downloadFile(this->file, BSTransmission::rate*rbs, true);

            all_complete = single_complete && all_complete;
         }
      }

      this->started = true;

      return all_complete;
   }

};

class D2DTransmission {

public:

   double rate = 0.03;

   Device& device_send;

   Device& device_rec;

   int file;

   D2DTransmission(Device& device_send, Device& device_rec, int file):
      device_send (device_send), device_rec (device_rec), file (file) {

      assert (this->device_send.hasFile(this->file));

   }

   bool nextTimeStep() {

      return device_rec.downloadFile(this->file, D2DTransmission::rate, false);

   }

};

