// Classes and functions related to D2D

using namespace std;

// Controls requests for files from devices
class RequestController {

   // the probability per device that a request will come in
   // at the busiest part of the day
   static constexpr double max_prob_device = 0.001;

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

   public:

   // assumed to be created as 11:59 pm (will be 12am when first request is possible)
   RequestController(const int& n, const int& m, const double& zipf,
      FileRanking& ranking, const double& start_time, const int& seed):
      f_req(m, zipf, ranking, seed), d_req (n, seed), gen (seed), rand_0_1 (0, 1) {

         // the maximum request probability we will have
         double max_prob = n*this->max_prob_device;

         // must reach max_prob at 1pm by increasing from 0 at 7am
         this->inc_rate = (max_prob)/(6*60*60);

         this->dec_rate = (max_prob)/(13*60*60);

         // rate at 11:59pm
         this->current_prob = max_prob - this->dec_rate*(6*60*60 - 1);

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

      if (time_in_day < 25200) {

         this->current_prob -= this->dec_rate;
      }
      else if (time_in_day < 46800) {

         this->current_prob += this->inc_rate;       
      }
      else if (time_in_day < 68400) {

         // do nothing
      }
      else {

         this->current_prob -= this->dec_rate;
      }

      assert ((this->current_prob >= 0) && (this->current_prob <= 1));

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

