#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <set>

struct City {
	std::string name;
	double latitude;
	double longitude;
	unsigned long population;
};

struct Flight {
	std::string origin;
	std::string destination;
};

struct Airport {
	std::string name;
	std::string city;
	std::string country;
	std::string iata_code;
	double latitude;
	double longitude;
	unsigned long population_10km;
	unsigned long population_20km;
	unsigned long population_50km;
	unsigned long departures;
	unsigned long arrivals;
	unsigned long short_haul;
	unsigned long long_haul;
	std::set<std::string> neighbours;

	double Tourism() const {
		return static_cast<double>(arrivals) / population_50km;
	}

	unsigned long Deg() const {
		return  static_cast<unsigned long> (neighbours.size());
	}

	double ArrivalsToDeg() const {
		unsigned long deg = Deg();
		unsigned long tourism = Tourism();
		if (deg <= 1 || std::isnan(deg)) {
			deg = 1;
		}
		return static_cast<double>(arrivals / deg);
	}

	double LongToShort() const {
		unsigned long sh = short_haul;
		if (sh == 0) {
			sh = 1;
		}
		return static_cast<double>(long_haul / sh);
	}

	double ShortToLong() const {
		unsigned long lh = long_haul;
		if (lh == 0) {
			lh = 1;
		}
		return static_cast<double>(short_haul / lh);
	}
};

class Cities {
public:
	void Load(const char* f_name) {
		std::ifstream file_in(f_name);
		std::string line;

		while (getline(file_in, line))
			ParseLine(line);
	}

	const std::vector<City>& GetCities() const {
		return cities_;
	}

private:
	std::vector<City> cities_;

	void ParseLine(std::string& line) {
		std::stringstream ss_line(line);
		std::string segment;

		City city = {{0}};

		for (size_t i = 0; getline(ss_line, segment, '\t'); ++i) {
			switch(i) {
				case 1:
					city.name = segment;
					break;
				case 4:
					city.latitude = stod(segment);
					break;
				case 5:
					city.longitude = stod(segment);
					break;
				case 14:
					city.population = stoul(segment);
					break;
				default:
					break;
			}
		}

		cities_.push_back(city);
	}
};

class Airports {
public:
	void Load(const char* f_name) {
		std::ifstream file_in(f_name);
		std::string line;

		while (getline(file_in, line))
			ParseLine(line);
	}

//	1km		= 0.00898315658 deg
//	10km	= 0.08983156581 deg
//	20km	= 0.17966313162 deg
//	50km	= 0.44915782907 deg

	void LoadPopulations(const Cities& cities) {
		for (auto& airport_pair : airports_) {
			Airport& airport = airport_pair.second;
			for (const auto &city : cities.GetCities()) {
				double x = hypot(city.latitude - airport.latitude, city.longitude - airport.longitude);
				if (x > 0.44915782907) //50km
					continue;

				airport.population_50km += city.population;

				if (x > 0.17966313162) //20km
					continue;

				airport.population_20km += city.population;

				if (x > 0.0898315658) //10km
					continue;

				airport.population_10km += city.population;
			}
		}
	}

	void LoadFlights(const char* f_name) {
		std::ifstream file_in(f_name);
		std::string line;
		getline(file_in, line);
		while (getline(file_in, line))
			ParseLineFlights(line);
	}

	const std::unordered_map<std::string, Airport>& GetAirports() const {
		return airports_;
	}

	void SortByTourism() const {
		std::vector<Airport> airports;
		std::ofstream result_csv_file;
		result_csv_file.open("result_data/result_tourism.csv");
		for (const auto& airport : airports_)
			airports.push_back(airport.second);

		sort(airports.begin(), airports.end(), [](const Airport &x, const Airport &y){ return (x.Tourism() > y.Tourism());});

		int i = 0;
		for (const auto& airport : airports) {
			//drop corner cases where, population is low
			if(airport.population_10km < 2000)
				continue;

			i++;
			result_csv_file << i << "," <<  airport.name.c_str() << "," << airport.city.c_str()<< ","  << airport.country.c_str() << "," << airport.iata_code.c_str() << "," <<  airport.population_10km << "," << airport.population_20km << "," << airport.population_50km << "," << airport.departures << "," << airport.arrivals << "," << airport.latitude << "," << airport.longitude << "," << airport.Tourism() << "," << airport.Deg() << "," << airport.ArrivalsToDeg() << "," << airport.LongToShort() << "," << airport.ShortToLong() << "\n";
			if(i >= 25)
				break;
		}
		result_csv_file.close();
	}

	void SortByDeg() const {
		std::vector<Airport> airports;
		std::ofstream result_csv_file;
		result_csv_file.open("result_data/result_deg.csv");
		for (const auto& airport : airports_)
			airports.push_back(airport.second);

		sort(airports.begin(), airports.end(), [](const Airport &x, const Airport &y){ return (x.Deg() > y.Deg());});

		int i = 0;

		for (const auto& airport : airports) {
			//no need to drop low population cases, usually big vertice degree is near huge city
			i++;
			result_csv_file << i << "," <<  airport.name.c_str() << "," << airport.city.c_str()<< ","  << airport.country.c_str() << "," << airport.iata_code.c_str() << "," <<  airport.population_10km << "," << airport.population_20km << "," << airport.population_50km << "," << airport.departures << "," << airport.arrivals << "," << airport.latitude << "," << airport.longitude << "," << airport.Tourism() << "," << airport.Deg() << "," << airport.ArrivalsToDeg() << "," << airport.LongToShort() << "\n";
			if(i >= 25)
				break;
		}
		result_csv_file.close();
	}


	void SortByArrivalsToDeg() const {
		std::vector<Airport> airports;
		std::ofstream result_csv_file;
		result_csv_file.open("result_data/result_arrivals_to_deg.csv");
		for (const auto& airport : airports_)
			airports.push_back(airport.second);

		sort(airports.begin(), airports.end(), [](const Airport &x, const Airport &y){ return (x.ArrivalsToDeg() > y.ArrivalsToDeg());});

		int i = 0;

		for (const auto& airport : airports) {
			//drop corner cases where, population is low and degree is really low
			if(airport.population_10km < 10000 || airport.Deg() < 10)
				continue;

			i++;
			result_csv_file << i << "," <<  airport.name.c_str() << "," << airport.city.c_str()<< ","  << airport.country.c_str() << "," << airport.iata_code.c_str() << "," <<  airport.population_10km << "," << airport.population_20km << "," << airport.population_50km << "," << airport.departures << "," << airport.arrivals << "," << airport.latitude << "," << airport.longitude << "," << airport.Tourism() << "," << airport.Deg() << "," << airport.ArrivalsToDeg() << "," << airport.LongToShort() << "," << airport.ShortToLong() << "\n";
			if(i >= 25)
				break;
		}
		result_csv_file.close();
	}


	void SortByLongToShort() const {
		std::vector<Airport> airports;
		std::ofstream result_csv_file;
		result_csv_file.open("result_data/result_long_to_short.csv");
		for (const auto& airport : airports_)
			airports.push_back(airport.second);

		sort(airports.begin(), airports.end(), [](const Airport &x, const Airport &y){ return (x.LongToShort() > y.LongToShort());});

		int i = 0;

		for (const auto& airport : airports) {
			//drop corner cases where, population is low
			if(airport.population_10km < 2000)
				continue;
			i++;
			result_csv_file << i << "," <<  airport.name.c_str() << "," << airport.city.c_str()<< ","  << airport.country.c_str() << "," << airport.iata_code.c_str() << "," <<  airport.population_10km << "," << airport.population_20km << "," << airport.population_50km << "," << airport.departures << "," << airport.arrivals << "," << airport.latitude << "," << airport.longitude << "," << airport.Tourism() << "," << airport.Deg() << "," << airport.ArrivalsToDeg() << "," << airport.LongToShort() << "," << airport.ShortToLong() << "\n";
			if(i >= 25)
				break;
		}
		result_csv_file.close();
	}

	void SortByShortToLong() const {
		std::vector<Airport> airports;
		std::ofstream result_csv_file;
		result_csv_file.open("result_data/result_short_to_long.csv");
		for (const auto& airport : airports_)
			airports.push_back(airport.second);

		sort(airports.begin(), airports.end(), [](const Airport &x, const Airport &y){ return (x.ShortToLong() > y.ShortToLong());});

		int i = 0;

		for (const auto& airport : airports) {
			//drop corner cases where, population is low
			if(airport.population_10km < 2000)
				continue;
			i++;
			result_csv_file << i << "," <<  airport.name.c_str() << "," << airport.city.c_str()<< ","  << airport.country.c_str() << "," << airport.iata_code.c_str() << "," <<  airport.population_10km << "," << airport.population_20km << "," << airport.population_50km << "," << airport.departures << "," << airport.arrivals << "," << airport.latitude << "," << airport.longitude << "," << airport.Tourism() << "," << airport.Deg() << "," << airport.ArrivalsToDeg() << "," << airport.LongToShort() << "," << airport.ShortToLong() << "\n";
			if(i >= 25)
				break;
		}
		result_csv_file.close();
	}
private:
	std::unordered_map<std::string, Airport> airports_;

	void ParseLine(std::string line) {
		std::string segment;

		// workaround for commas in parentheses
		bool par = false;
		for (size_t i = 0; i < line.length(); ++i) {

			if(par && line[i] == ',') {
				line[i] = ';';
				continue;
			}
			else if(par && line[i] == '\"') {
				par = false;
				continue;
			}
			else if(line[i] == '\"')
				par = true;
		}

		// remove parentheses
		line.erase(std::remove(line.begin(), line.end(), '\"'), line.end());
		std::stringstream ss_line(line);

		Airport airport = {{0}};

		for (size_t i = 0; getline(ss_line, segment, ','); ++i) {
			switch(i) {
				case 1:
					airport.name = segment;
					break;
				case 2:
					airport.city = segment;
					break;
				case 3:
					airport.country = segment;
					break;
				case 4:
					airport.iata_code = segment;
					break;
				case 6:
					airport.latitude = stod(segment);
					break;
				case 7:
					airport.longitude = stod(segment);
					break;

				default:
					break;
			}
		}

		airports_.insert(std::make_pair(airport.iata_code, airport));
	}

	void ParseLineFlights(std::string line) {
		// workaround for commas in parentheses
		bool par = false;
		for (size_t i = 0; i < line.length(); ++i) {

			if(par && line[i] == ',') {
				line[i] = ';';
				continue;
			}
			else if(par && line[i] == '\"') {
				par = false;
				continue;
			}
			else if(line[i] == '\"')
				par = true;
		}

		// remove parentheses
		line.erase(std::remove(line.begin(), line.end(), '\"'), line.end());
		std::stringstream ss_line(line);
		std::string segment;
		std::string origin_airport;
		std::string dest_airport;
		double dist;

		for (size_t i = 0; getline(ss_line, segment, ','); ++i) {
			switch(i) {
				case 3: //origin
					origin_airport = segment;
					//airports_[segment].departures++; - now we have number of passangers from flight file
 					break;
				case 5: //destination
					dest_airport = segment;
					//airports_[segment].arrivals++; - ^^ same here
					airports_[origin_airport].neighbours.insert(dest_airport);
					break;
				case 8: //passangers
					segment.erase(std::remove(segment.begin(), segment.end(), '.'), segment.end());
					airports_[origin_airport].departures += stol(segment);
					airports_[dest_airport].arrivals += stol(segment);
					break;
				case 9: //distance, if > 2500 miles then flight is long-haul
					dist = stod(segment);
					if(dist > 2500){
						airports_[origin_airport].long_haul++;
						airports_[dest_airport].long_haul++;
					} else {
						airports_[origin_airport].short_haul++;
						airports_[dest_airport].short_haul++;
					}
					break;
				default:
					break;
			}
		}
	}
};

int main() {
	Cities cities;
	Airports airports;

	printf("Loading cities...\n");
	cities.Load("data/cities.dat");
	printf("Loading airports...\n");
	airports.Load("data/airports.dat");
	printf("Corelating populations...\n");
	airports.LoadPopulations(cities);
	printf("Loading flights...\n");
	airports.LoadFlights("data/flights.dat");
	printf("Generating result files...\n");
	airports.SortByTourism();
	airports.SortByDeg();
	airports.SortByArrivalsToDeg();
	airports.SortByLongToShort();
	airports.SortByShortToLong();
	printf("Results are available in result_data directory\n");
	return 0;
}
