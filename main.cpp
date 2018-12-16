#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cmath>

struct City {
	std::string name;
	double latitude;
	double longitude;
	unsigned long population;
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
		for (auto& airport : airports_) {
			for (const auto &city : cities.GetCities()) {
				double x = hypot(city.latitude - airport.latitude, city.longitude - airport.longitude);
				if (x < 0.08983156581); //10km
					printf("%s\t%s\n", airport.name.c_str(), city.name.c_str());
			}
		}
	}

private:
	std::vector<Airport> airports_;

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

		airports_.push_back(airport);
	}
};

int main() {
	Cities cities;
	Airports airports;

	cities.Load("data/cities500.txt");
	airports.Load("data/airports.dat");
	airports.LoadPopulations(cities);

	return 0;
}