// Classes and functions related to D2D

using namespace std;

// A ranking of files in order of popularity
// Each file is just reprensented by an integer
class FileRanking {

   // the files (identified by an int) in order of popularity
   // i.e. the 0th spot is the most popular file
   vector<int> ranking;

   public:

   // m is the number of files
   // ranking is initialized to be in order of id
   FileRanking(const int& m) {

      for (int i = 0; i < m; i++) {

         this->ranking.push_back(i);
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
