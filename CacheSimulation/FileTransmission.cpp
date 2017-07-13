// represents a transmission of file, either D2D or from the BS

class FileTransmission {

   protected:

   // the rate this transmission is occurring at, in parts of a whole file
   // i.e. 0.5 would imply 2 seconds to transmit 
   static constexpr double rate = 0.001;

   FileTransmission() { }

};

class BSTransmission: public FileTransmission {

   static constexpr double rate = 0.001;

   // what device the BS is transferring files to
   Device& device;

   // what file is being transferred
   int file;

   public:

   BSTransmission(Device& device, int file): device(device), file(file) { }

};

class MulticastBSTransmission: public FileTransmission {

   static constexpr double rate = 0.001;

   vector<Device*> devices;

   int file;

   public:

   MulticastBSTransmission (vector<Device>& devices, const int& file): file(file) {

      for (int i = 0; i < devices.size(); i++) {

         this->devices.push_back(&devices[i]);
      }

   }

};

class D2DTransmission: public FileTransmission {

   static constexpr double rate = 0.03;

   Device& device_send;

   Device& device_rec;

   int file;

   public:

   D2DTransmission(Device& device_send, Device& device_rec, int file):
      device_send (device_send), device_rec (device_rec), file (file) {

   }

};

