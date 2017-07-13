// Class that holds location data from a SLAW location file

using namespace std;

class Locations {

   ifstream locations_data;

   int n;

   public:

   Locations(const string& locations_file, const int& n): n (n) {

      this->locations_data.open(locations_file);

      if (!locations_data.good()) {

         throw invalid_argument("There is something wrong with the file for locations"
                                " data. A Locations object could not be built.");
      }

   }

   bool nextStep(vector< pair<int, int> >& locations) {

      locations.clear();

      int x, y;
      string line;
      stringstream line_strm;
      int node = 0;

      while (getline(this->locations_data, line)) {

         // read in point
         line_strm.clear();
         line_strm.str(line);
         line_strm >> x >> y;

         locations.push_back(make_pair(x, y));

         if (node == (this->n - 1)) {

            break;
         }

         node++;

      }

      if (locations.size() == this->n) return true;
      else return false;

   }

};


