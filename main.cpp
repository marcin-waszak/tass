#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <cmath>

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

	double Tourism() const {
		return static_cast<double>(arrivals) / population_50km;
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

				if (x > 0.08983156581) //10km
					continue;

				airport.population_10km += city.population;
			}
		}
	}

	void LoadFlights(const char* f_name) {
		std::ifstream file_in(f_name);
		std::string line;

		while (getline(file_in, line))
			ParseLineFlights(line);
	}

	const std::unordered_map<std::string, Airport>& GetAirports() const {
		return airports_;
	}

	void SortByTourism() const {
		std::vector<Airport> airports;

		for (const auto& airport : airports_)
			airports.push_back(airport.second);

		sort(airports.begin(), airports.end(), [](const Airport &x, const Airport &y){ return (x.Tourism() > y.Tourism());});

		printf("\nMost tourist airports:\n");
		int i = 0;

		for (const auto& airport : airports) {
			//drop corner cases where, population is low
			if(airport.population_10km < 10000)
				continue;

			i++;
			printf("%d. %s %s %s %s %lu %lu %lu %lu %lu\n",
						 i,
						 airport.name.c_str(),
						 airport.city.c_str(),
						 airport.country.c_str(),
						 airport.iata_code.c_str(),
						 airport.population_10km,
						 airport.population_20km,
						 airport.population_50km,
						 airport.departures,
						 airport.arrivals);

			if(i >= 25)
				break;
		}
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

		for (size_t i = 0; getline(ss_line, segment, ','); ++i) {
			switch(i) {
				case 3: //origin
					airports_[segment].departures++;
					break;
				case 5: //destination
					airports_[segment].arrivals++;
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
	printf("Sorting airports...\n");
	airports.SortByTourism();

	return 0;
}
