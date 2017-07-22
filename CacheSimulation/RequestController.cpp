// Classes and functions related to D2D

using namespace std;

// Controls requests for files from devices
class RequestController {

public:

   // the probability per device that a request will come in
   // at the busiest part of the day
   static constexpr double max_prob_device = 0.001;

   static constexpr int _7AM = 25200;

   static constexpr int _1PM = 46800;

   static constexpr int _7PM = 68400;

   static constexpr int DAY = 86400;

   default_random_engine gen;

   uniform_real_distribution<double> rand_0_1;

   // controls what file is requested
   FileRequest f_req;

   // controls what device requests the file
   DeviceRequest d_req;

   // the probability a request will come in (roughly)
   double current_prob;

   // how much the request probability will increase per second from 7am to
   // 1pm
   double inc_rate;

   // how much the request probability will decrease per second from 6pm to 7am
   double dec_rate;

   RequestController(const int& n, const int& m, const double& zipf,
      FileRanking& ranking, const double& start_time, const int& seed):
      f_req(m, zipf, ranking, seed), d_req (n, seed), gen (seed), rand_0_1 (0, 1) {

         // the maximum request probability we will have
         double max_prob = n*this->max_prob_device;

         // must reach max_prob at 1pm by increasing from 0 at 7am
         this->inc_rate = (max_prob)/(RequestController::_1PM - RequestController::_7AM);

         this->dec_rate = (max_prob)/(RequestController::_7AM
            + RequestController::DAY - RequestController::_7PM);

         // rate at 12am
         this->current_prob = max_prob - this->dec_rate*(5*60*60);

         clog << "RequestController created with current request probability " << this->current_prob
            << ". Increase rate is set at " << this->inc_rate << " and decrease rate is set at "
            << this->dec_rate << endl;

   }

   bool nextTimeStep(const int& current_time, int& device, int& file) {

      this->updateRequestProbability(current_time);

      return this->getRequest(device, file);

   }

   // updates the request probability based on the current time
   // linear rise from 7am (time = 25200) to 1pm (time = 46800)
   // remains at peak traffic until 6pm (time = 68400)
   // and then linear
   // decrease from 1pm until 7am
   void updateRequestProbability(const int& current_time) {

      int time_in_day = current_time%86400;

      if (time_in_day == _7AM) {

         this->current_prob = 0.0;
      }
      else if (time_in_day < RequestController::_7AM) {

         this->current_prob -= this->dec_rate;
      }
      else if (time_in_day < RequestController::_1PM) {

         this->current_prob += this->inc_rate;       
      }
      else if (time_in_day < RequestController::_7PM) {

         // do nothing
      }
      else {

         this->current_prob -= this->dec_rate;
      }

      if (this->current_prob < 0) this->current_prob = 0;
      if (this->current_prob > 1) this->current_prob = 1;

      clog << "The current request probability per second is " << this->current_prob << endl;

   }

   // gets a request from a random uniformly chosen device
   // and a file from the zipf distribution
   // with probability r_prob
   // returns false if no request
   bool getRequest(int& device, int& file) {

      if (rand_0_1 (gen) < this->current_prob) {

         device = this->d_req.getRequest();

         file = this->f_req.getRequest();

         return true;
      }

      return false;

   }

};

