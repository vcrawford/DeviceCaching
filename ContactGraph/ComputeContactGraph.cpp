#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

/**
 * Compute a contact graph from contact data
 */

using namespace std;

int main(int argc, char* argv[]) {

   /**Variables read in from the config file*/

   string filename; // name of our contact data filee

   int id_first; // which range of ids we should track for contact
   int id_last;

   int data_hour; // hour of the day (0,...,23) that the data starts at

   int day_start; // What day and hour ranges (0,1,...) we should track for contact
   int day_end;
   int hour_start;
   int hour_end;

   double threshold; // edges below this are dropped

   string contact_output; // where to store contact graph
   string plot; // where to save plot

   /**Read in config file and set variables*/

   if (argv[1] == "") {
      cout << "No config file was given" << endl;
      return 1;
   }

   ifstream config;
   config.open(argv[1]);

   if (config.is_open()) {
      cout << "Reading in config file " << argv[1] << endl;

      string line; // config has "type value" on each line
      string type;
      int value;
      stringstream line_strm;

      /**Read in each line and set appropriate variables*/
      while (getline(config, line)) {
         line_strm.clear();
         line_strm.str(line);
         line_strm >> type;
         
         if (type.compare("filename") == 0) {
            line_strm >> filename;
         }
         else if (type.compare("id_first") == 0) {
            line_strm >> id_first;
         }
         else if (type.compare("id_last") == 0) {
            line_strm >> id_last;
         }
         else if (type.compare("data_hour") == 0) {
            line_strm >> data_hour;
         }
         else if (type.compare("day_start") == 0) {
            line_strm >> day_start;
         }
         else if (type.compare("day_end") == 0) {
            line_strm >> day_end;
         }
         else if (type.compare("hour_start") == 0) {
            line_strm >> hour_start;
         }
         else if (type.compare("hour_end") == 0) {
            line_strm >> hour_end;
         }
         else if (type.compare("threshold") == 0) {
            line_strm >> threshold;
         }
         else if (type.compare("contact_output") == 0) {
            line_strm >> contact_output;
         }
         else if (type.compare("plot") == 0) {
            line_strm >> plot;
         }
      }
   }
   else {
      cout << "Config file of name " << argv[1] << " is not found." << endl;
      return 1;
   }

   // Now read in data and build the contact graph

   ifstream data;
   data.open(filename.c_str());

   if (data.is_open()) {
      cout << "Reading in contact data from " << filename << endl;
   }
   else {
      cout << "There was a problem opening the file " << filename << endl;
      return 1;
   }

   int num_users = id_last - id_first + 1; // the number of users we track

   vector< vector<double> > contact (num_users, vector<double> (num_users, 0)); // the contact graph

   /**Find the times region in seconds (that's what contact events are in)*/
   int first_day_seconds = (24 - data_hour)*60*60; // seconds from 0 that the first day starts

   int day_seconds = 24*60*60; // seconds in one day

   int day_start_seconds = 0; // what the day range corresponds to in seconds

   if (day_start > 0) {
      day_start_seconds = first_day_seconds + (day_start - 1)*day_seconds;
   }

   int day_end_seconds = first_day_seconds;

   if (day_end > 0) {
      day_end_seconds = first_day_seconds + (day_end)*day_seconds;
   }

   int hour_start_seconds = hour_start*60*60; // the seconds into a day our hour corresponds to
   int hour_end_seconds = hour_end*60*60;

   /**Go through contact data and find needed contact events, which each correspond to one line*/
   string line;
   stringstream line_strm;
   int node1, node2, contact_start, contact_end, contact_time;

   while (getline(data, line)) {

      line_strm.clear();
      line_strm.str(line);
      line_strm >> node1;
      line_strm >> node2;
      line_strm >> contact_start;
      line_strm >> contact_end;

      // check if this is in a desired time range
      if ( (contact_start >= day_start_seconds) &&
           (contact_end <= day_end_seconds) &&
           ((contact_start + (day_seconds - first_day_seconds))%day_seconds >= hour_start_seconds) &&
           ((contact_end + (day_seconds - first_day_seconds))%day_seconds <= hour_end_seconds)) {

         // also check it involves nodes we are tracking
         if ((node1 >= id_first) && (node1 <= id_last) && (node2 >= id_first) && (node2 <= id_last)) {
            // add this contact event
            contact_time = contact_end - contact_start;
            contact[node1 - id_first][node2 - id_first] += contact_time;
            //cout << "Node " << node1 << " and " << " Node " << node2 << " from " 
            //     << contact_start << " to " << contact_end << endl;
         }

      }
   }

   data.close();

   // write contact data to file
   ofstream output_strm;
   output_strm.open(contact_output.c_str());

   // also make graphviz plot
   ofstream plot_strm;
   plot_strm.open(plot.c_str());
   plot_strm << "graph contactplot {" << endl
             << "overlap = false" << endl
             << "splines = true" << endl
             << "node [shape=circle, fontsize=10, margin=0.05, height=0.3, style=filled, fillcolor=deepskyblue2,"
             << "color=deepskyblue3]" << endl
             << "edge [color=dimgray]";

   // the total time we are looking at
   int total_time = (day_end - day_start + 1)*(hour_end_seconds - hour_start_seconds);

   cout << "Outputting contact data to file " << contact_output << "." << endl;
   cout << "Outputting contact plot to file " << plot << "." << endl;

   // make all contact values be between 0 and 1
   for (int i = 0; i < contact.size(); i++) {
      for (int j = 0; j < contact[0].size(); j++){
         contact[i][j] = contact[i][j]/total_time;
         if (contact[i][j] >= threshold) {
            output_strm << contact[i][j] << " ";
            if (i < j) {
               plot_strm << " " << (i + 1) << " -- " << (j + 1)  << "[weight=" << 10*contact[i][j] 
                         << ", penwidth=" << 5*contact[i][j] << "]" << endl;
            }
         }
         else {
            output_strm << 0 << " ";
         }
      }
      output_strm << endl;
   }

   output_strm.close();

   plot_strm << "}";
   plot_strm.close();
}


