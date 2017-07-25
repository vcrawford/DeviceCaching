// Class to handle all statistics for the D2D instance

class Statistics {

public:

	// number of cache hits over last hour
	int hits_hour;

	// number of requests over last hour
	int requests_hour;

	// sum of in transmission at every second over the last hour
	int BS_in_transmission_hour_sum;

	// file id to number hits over entire simulation
	vector<int> hits_file;

	// same but with requests
	vector<int> requests_file;

	// the cache hit rate for each hour in the simulation
	vector<double> hour_hit_rate;

	vector<double> hour_hit_rate_so_far;

	// the average number of files in transmission in a second from the BS
	// for each hour in the simulation
	vector<double> BS_in_transmission;

	int num_hits;

	int num_requests;

	Statistics(int const& n, int const& m): hits_hour (0), requests_hour (0),
		hits_file (m, 0), requests_file (m, 0), BS_in_transmission_hour_sum (0),
		num_hits (0), num_requests (0) { }

	void nextTimeStep(const int& time, int BS_transmission_count) {

		this->BS_in_transmission_hour_sum += BS_transmission_count;

		if ((time % (60*60)) == 59) {

			// last second of an hour

			this->BS_in_transmission.push_back(BS_in_transmission_hour_sum/(60.0*60.0));

			if (this->requests_hour > 0) {

				this->hour_hit_rate.push_back(double(hits_hour)/requests_hour);				
			}
			else {

				this->hour_hit_rate.push_back(1);
			}

			if (this->num_requests > 0) {

				this->hour_hit_rate_so_far.push_back(double(this->num_hits)/this->num_requests);
			}
			else {

				this->hour_hit_rate_so_far.push_back(1);
			}

			this->hits_hour = 0;
			this->requests_hour = 0;
			this->BS_in_transmission_hour_sum = 0;

		}
	}

	void hit(const int& file) {

		this->hits_file[file]++;

		this->num_hits++;

		this->hits_hour++;
	}

	void request(const int& file) {

		this->requests_file[file]++;

		this->num_requests++;

		this->requests_hour++;
	}

	bool hasrequest(const int& file) {

		return (this->requests_file[file] > 0);
	}

	double hitrate(const int& file) {

		assert (this->requests_file[file] > 0);

		return double(this->hits_file[file])/this->requests_file[file];
	}

	double hitrateall() {

           int num_hits = 0;

           for (int i = 0; i < this->hits_file.size(); i++) {

              num_hits += this->hits_file[i];
           }

           int num_requests = 0;

           for (int i = 0; i < this->requests_file.size(); i++) {

              num_requests += this->requests_file[i];
           }

           return double(num_hits)/num_requests;
	}

};
