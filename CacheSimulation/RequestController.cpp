// Classes and functions related to D2D

using namespace std;

// Controls requests for files from devices
class RequestController {

   default_random_engine gen;

   uniform_real_distribution<double> rand_0_1;

   FileRequest f_req;

   DeviceRequest d_req;

   // probability in a second there will be a request for a
   // file
   double r_prob;

   public:

   RequestController(const int& n, const int& m, const double& zipf, const double& r_prob,
      FileRanking& ranking):
      f_req(m, zipf, ranking), d_req (n), r_prob (r_prob), gen (time(NULL)), rand_0_1 (0, 1) { }

   // gets a request from a random uniformly chosen device
   // and a file from the zipf distribution
   // with probability r_prob
   // returns false if no request
   bool getRequest(int& device, int& file) {

      if (rand_0_1 (gen) < this->r_prob) {

         device = this->d_req.getRequest();

         file = this->f_req.getRequest();

         return true;
      }

      return false;

   }

   friend ostream& operator<<(ostream&, RequestController&);

};

ostream& operator<<(ostream& os, RequestController& x) {

   os << x.f_req;

   return os;
}


