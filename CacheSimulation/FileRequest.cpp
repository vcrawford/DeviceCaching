// Classes and functions related to D2D

using namespace std;

// Returns requests for a file according to a Zipf distribution
class FileRequest {

   // popularity ranking of the files, which affects what devices
   // are requested
   FileRanking& file_ranking;

   // the zipf parameter
   // file requests follow a zipf distribution over the above ranking
   double zipf;

   // the denominator of the zipf probability function
   double denom;

   // random generator
   default_random_engine gen;

   // for picking random rank (and therefore random file)
   uniform_real_distribution<double> rand_0_1;

   public:

   // m is the number of files
   FileRequest(const int& m, const double& zipf, FileRanking& fr, const int& seed):
               gen (seed), rand_0_1 (0, 1), file_ranking (fr), zipf (zipf) {

      // calculate the denominator

      this->denom = 0;

      for (int i = 1; i <= m; i++) {

         this->denom += 1/pow(i, zipf);
      }

   }

   // gets a random request for a file
   // returns the id of that file
   int getRequest() {

      double rand_num = rand_0_1 (gen);

      // the popularity ranking we are on
      int pop_spot = 0;

      double prob_sum = (1/pow(1, this->zipf))/this->denom;

      while (rand_num > prob_sum) {

         pop_spot++;

         prob_sum += (1/pow(pop_spot + 1, this->zipf))/this->denom;
      }

      return this->file_ranking.getPopularFile(pop_spot);

   }

   friend ostream& operator<<(ostream&, const FileRequest&);

};

ostream& operator<<(ostream& os, FileRequest& x) {

   os << "File request samples: ";

   for (int i = 0; i < 20; i++) {

      os << x.getRequest() << " ";
   }

   os << endl;

}

