// Classes and functions related to D2D

using namespace std;

// A ranking of files in order of popularity
// Each file is just reprensented by an integer
class FileRanking {

   // the files (identified by an int) in order of popularity
   // i.e. the 0th spot is the most popular file
   vector<int> ranking;

   // whether we would like this ranking to evolve over time
   bool evolve;

   // the probability we will pick a random file to evolve

   // used for randomness in evolving
   default_random_engine gen;
   uniform_real_distribution rand;

   // the ids of the files we are evolving
   list<int> evolving;

   // the current ranks of each of those files;
   map<int, int> evolving_ranks;

   // how many spots up the files have left to go
   map<int, int> moves_up;

   // how many moves down the files will have to go once they have
   // 0 moves up
   map<int, int> moves_down;

   public:

   // m is the number of files
   // ranking is initialized to be in order of id
   FileRanking(const int& m, const bool& evolve, const double& evolve_prob):
      evolve (evolve), evolve_prob (evolve_prob), gen (time(NULL)), rand (0, 1) {

      for (int i = 0; i < m; i++) {

         this->ranking.push_back(i);
      }

   }

   // swaps the files at the two rankings
   // if one ranking is out of bounds, does nothing (by design)
   void swapRanks(const int& rank_1, const int& rank_2) {

      if ((rank_1 < 0) || (rank_1 >= this->ranking.size())) return;

      if ((rank_2 < 0) || (rank_2 >= this->ranking.size())) return;

      int file_1 = this->ranking[rank_1];

      this->ranking[rank_1] = this->ranking[rank_2];

      this->ranking[rank_2] = file_1;
   }

   // moves file throughout ranking
   void nextTimeStep() {

      if (rand(gen))

      if (!this->evolve) {

         return;
      }

      for (auto it = this->ranks_evolving.begin(); it != this->ranks_evolving.end();) {

         if (this->moves_up[*it] != 0) {

            this->swapRanks(this->evolving_ranks[*it], this->evolving_ranks[*it] - 1);

            this->moves_up[*it]--;

            it++;
         }
         else if (this->moves_down[*it] != 0) {

            this->swapRanks(this->evolving_ranks[*it], this->evolving_ranks[*it] + 1);

            this->moves_down[*it]--;

            it++;
         }
         else {

            this->moves_down.erase(*it);

            this->moves_down.erase(*it);

            it = this->ranks_evolving.erase(it);
         }
      }

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

ostream& operator<<(ostream& os, const FileRanking& x) {

   os << "File ranking: ";

   for (int i = 0; i < x.ranking.size(); i++) {

      os << i << ", ";
   }

   os << endl;

   return os;

}
