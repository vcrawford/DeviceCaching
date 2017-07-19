// Classes and functions related to D2D

using namespace std;

// A ranking of files in order of popularity
// Each file is just reprensented by an integer
class FileRanking {

   // the number of files
   int m;

   // the files (identified by an int) in order of popularity
   // i.e. the 0th spot is the most popular file
   vector<int> ranking;

   // whether we would like this ranking to evolve over time
   bool evolve;

   // probability that in any second a new file will be added to those
   // currently evolving
   double new_evolve_prob;

   // the probability over any second that a file will move up/down
   // a ranking if it is an unstable state (moves_up is non-zero, or 
   // it's moves_up is zero, it's stable time is 0, and its moves_down is
   // non-zero)
   static constexpr double move_prob = 0.015;

   // the probability we will pick a random file to evolve

   // used for randomness in evolving
   default_random_engine gen;
   uniform_real_distribution<double> rand;

   // the ids of the files we are currently evolving
   list<int> evolving;

   // the current ranks of each of those files;
   // WARNING: this is not always accurate (sometimes, evolving files swap
   // other evolving files) and so you must do a check in this->ranking before
   // using this
   map<int, int> evolving_ranks;

   // how many spots up in popularity the files have left to go
   map<int, int> moves_up;

   // how many moves down the files will have to go once they have
   // 0 moves up
   map<int, int> moves_down;

   // how long (in seconds) each file should stay in its stable spot (when moves_up reached 0
   // but moves_down is non-zero)
   map<int, int> stable_time;

   public:

   // m is the number of files
   // ranking is initialized to be in order of id
   // evolve is whether files evolve in popularity or not
   // evolve_port is how many files are expected to begin evolving in a day
   FileRanking(const int& m, const bool& evolve, const double& evolve_port,
      const int& seed): m (m), evolve (evolve), gen (seed), rand (0, 1) {

      for (int i = 0; i < m; i++) {

         this->ranking.push_back(i);
      }

      this->new_evolve_prob = (m*evolve_port)/(24*60*60);

      clog << "File ranking initialized in order of id. Each second, there is a "
         << this->new_evolve_prob << " probability that a new file will evolve." << endl;

   }

   // returns the id of the most popular file
   int topFile() {

      return this->ranking[0];
   }

   // swaps the files at the two rankings
   // If at least one rank is out of range, does nothing. The reason for this
   // is because sometimes other files will move an evolving file and make the
   // number of places it has to change impossible
   // WARNING: Could make this->evolving_ranks innaccurate
   void swapRanks(const int& rank_1, const int& rank_2) {

      if ((rank_1 < 0) || (rank_1 >= this->ranking.size()) || (rank_2 < 0) ||
         (rank_2 >= this->ranking.size())) {

         return;
      }

      int file_1 = this->ranking[rank_1];

      this->ranking[rank_1] = this->ranking[rank_2];

      this->ranking[rank_2] = file_1;
   }

   // moves file throughout ranking
   // returns whether any ranking has changed
   bool nextTimeStep() {

      if (this->evolve) {

         clog << "There are " << this->evolving.size() << " files currently evolving in popularity."
            << endl;

         if (this->rand(this->gen) < this->new_evolve_prob) {

            this->evolveNewFile();
         }

         return this->evolveFiles();
      }

      return false;

   }

   // randomly picks some file in bottom 90% and adds it to those evolving to the
   // top 10%
   void evolveNewFile() {

      // random low popularity rank
      int rank = m*(0.1 + 0.9*this->rand(this->gen));

      // random high popularity rank
      int new_rank = m*0.1*this->rand(this->gen);

      assert(rank >= 0);
      assert(rank < this->ranking.size());
      assert(new_rank < rank);
      assert(new_rank >= 0);
      assert(new_rank < this->ranking.size());

      // the file that we have randomly chosen to move
      int fileid = this->ranking[rank];

      // if this file is already evolving, don't add it
      for (auto it = this->evolving.begin(); it != this->evolving.end(); it++) {

         if (*it == fileid) {

            clog << "Will not begin evolving file " << fileid << ". Already evolving." << endl;
            return;
         }
      }

      // add it to those evolving

      this->evolving.push_back(fileid);

      this->evolving_ranks[fileid] = rank;

      this->moves_up[fileid] = rank - new_rank;

      this->moves_down[fileid] = rank - new_rank;

      // stable for a random, uniformly distributed time between 0 seconds and 1 day
      this->stable_time[fileid] = 86400*this->rand(this->gen);

      clog << "Evolving file " << fileid << " from rank " << rank
         << " to rank " << new_rank <<". It will move up " << this->moves_up[fileid] <<
         " and then move down " << this->moves_down[fileid] << ". It will stay stable for "
         << this->stable_time[fileid] << " seconds at its new destination." << endl;

   }

   // all currently evolving files are updated
   // returns whether anything has been moved
   bool evolveFiles() {

      bool moved = false;

      // go through each evolving file and update its state if needed
      for (auto it = this->evolving.begin(); it != this->evolving.end();) {

         // if it is time to move, this random number will decide if it should
         bool should_move = (this->rand(this->gen) < this->move_prob);

         // first, must check if evolving_ranks is accurate or needs an update
         if (!(this->ranking[this->evolving_ranks[*it]] == *it)) {

            // not accurate, need to update
            this->evolving_ranks[*it] = this->getRanking(*it);
         }

         if (this->moves_up[*it] != 0) {

            // should move up
            if (should_move) {
            
               this->swapRanks(this->evolving_ranks[*it], this->evolving_ranks[*it] - 1);

               this->moves_up[*it]--;

               this->evolving_ranks[*it]--;

               clog << "File " << *it << " has moved up a rank. It is now at rank "
                  << this->evolving_ranks[*it] <<". It has " << this->moves_up[*it]
                  << " moves up left." << endl;

               moved = true;
            }

            it++;
         }
         else if (this->stable_time[*it] != 0) {

            // is in stable, popular state
            this->stable_time[*it]--;

            it++;
         }
         else if (this->moves_down[*it] != 0) {

            // moving down in popularity
            if (should_move) {

               this->swapRanks(this->evolving_ranks[*it], this->evolving_ranks[*it] + 1);

               this->moves_down[*it]--;

               this->evolving_ranks[*it]++;              

               clog << "File " << *it << " has moved down a rank. It is now at rank "
               << this->evolving_ranks[*it] <<". It has " << this->moves_down[*it]
               << " moves down left." << endl;

               moved = true;           
            }

            it++;
         }
         else {

            // complete
            this->moves_up.erase(*it);

            this->moves_down.erase(*it);

            this->evolving_ranks.erase(*it);

            this->stable_time.erase(*it);

            clog << "File " << *it << " has completed its evolution." << endl;              

            it = this->evolving.erase(it);
       
         }

      }

      return moved;

   }

   // get the id of the i-st most popular file
   int getPopularFile(const int& i) {

      return this->ranking[i];
   }

   // get the number of files
   int getNumFiles() {

      return this->ranking.size();
   }

   // get the ranking of a particular file
   int getRanking(const int& file_id) {

      for (int i = 0; i < this->ranking.size(); i++) {

         if (this->ranking[i] == file_id) {

            return i;
         }
      }

      // should not ever be here, that means the file was not ranked
      assert(false);

   }

   friend ostream& operator<<(ostream&, const FileRanking&);

};

