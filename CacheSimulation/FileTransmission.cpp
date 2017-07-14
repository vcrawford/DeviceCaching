// Represents a transmission occurring, either by BS or D2D

class BSTransmission {

public:

   static constexpr double rate = 0.001;
};

class SingleBSTransmission: public BSTransmission {

public:

   // what device the BS is transferring files to
   Device& device;

   // what file is being transferred
   int file;

   SingleBSTransmission(Device& device, int file): device(device), file(file) { }

   bool nextTimeStep(const int& rbs) {

      return this->device.addFile(this->file, BSTransmission::rate*rbs);

   }

};

class MulticastBSTransmission: public BSTransmission {

public:

   vector<reference_wrapper<Device>> devices;

   int file;

   MulticastBSTransmission (vector< reference_wrapper<Device> >& devices, const int& file): file(file) {

      this->file = file;

      this->devices = devices;

   }

   bool nextTimeStep(const int& rbs) {

      bool all_complete = true;

      for (auto it = this->devices.begin(); it != this->devices.end(); it++) {
 
         bool single_complete = it->get().addFile(this->file, BSTransmission::rate*rbs);

         all_complete = single_complete && all_complete;
      }

      return all_complete;
   }

};

class D2DTransmission {

public:

   static constexpr double rate = 0.03;

   Device& device_send;

   Device& device_rec;

   int file;

   D2DTransmission(Device& device_send, Device& device_rec, int file):
      device_send (device_send), device_rec (device_rec), file (file) {

   }

};

