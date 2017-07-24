// Classes and functions related to D2D

using namespace std;

// No organized caching
class CacheControllerNone: public CacheController {

public:

   // n is the number of nodes (number of devices)
   // c is the number of files a single node can cache
   CacheControllerNone(const int& n, const int& c,
                   FileRanking& file_ranking, const string& alg):
                   CacheController(n, c, file_ranking, alg) { }

   bool nextTimeStep() {

      return false;
   }

};
