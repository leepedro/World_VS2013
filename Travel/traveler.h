#if !defined(TRAVELER_H)
#define TRAVELER_H

#include <string>
#include <chrono>
#include <list>
#include <atomic>

namespace World
{
	// Forward Declaration.
	//class LLCoordinate;
	class Traveler;
	//class Town;

	const double PI = std::atan(1) * 4;	// (radian)
	const double EARTH_RADIUS = 6371;	// (km)

	double degree2rad(double degree)
	{
		return degree * PI / 180.0;
	}

	double rad2degree(double radian)
	{
		return radian / PI * 180.0;
	}

	// Converts DMS {degree, minute, second} to decimal degree.
	// NOTE: If west, all variables must be negative values.
	double dms2dec(double degree, double minute, double second = 0.0)
	{
		if ((degree >= 0 && minute >= 0 && second >= 0) ||
			(degree < 0 && minute < 0 && second < 0))
			return degree + (minute * 60 + second) / 3600;
		else
			throw std::logic_error(
			"The signs of all {degree, minute, second} must be identical.");
	}

	// Compute the distance (km) between two geo-positions (degree) using Spherical law of
	// Cosines with mean earth radius.
	double get_distance(double lat1, double lon1, double lat2, double lon2)
	{
		double lat1_r = degree2rad(lat1);
		double lat2_r = degree2rad(lat2);
		double diff_lon = degree2rad(lon2 - lon1);
		using namespace std;
		return acos(sin(lat1_r) * sin(lat2_r) + cos(lat1_r) * cos(lat2_r) * cos(diff_lon)) *
			EARTH_RADIUS;
	}

	double get_bearing(double lat1, double lon1, double lat2, double lon2)
	{
		double lat1_r = degree2rad(lat1);
		double lat2_r = degree2rad(lat2);
		double diff_lon = degree2rad(lon2 - lon1);
		using namespace std;
		double bearing_r = atan2(std::sin(diff_lon) * cos(lat2_r),
			cos(lat1_r) * sin(lat2_r) - sin(lat1_r) * cos(lat2_r) * cos(diff_lon));
		return fmod(rad2degree(bearing_r) + 180, 360);	// [-pi, ..., pi] -> [0, ..., 360).
	}

	class LLCoordinate
	{
	public:
		double latitude;	// horizontal lines presenting north(+) and south(-).
		double longitude;	// vertical lines presenting east(+) and west(-).
		//double &lambda = this->longitude;
		//double &phi = this->latitude;
		void correct(void);
	protected:
		static double correct_latitude(double value);
		static double correct_longitude(double value);
	};

	void LLCoordinate::correct(void)
	{
		this->latitude = LLCoordinate::correct_latitude(this->latitude);
		this->longitude = LLCoordinate::correct_longitude(this->longitude);
	}

	// Adjusts given longitude value (degree).
	double LLCoordinate::correct_latitude(double value)
	{
		throw std::logic_error("Not implemented.");
	}

	// Limits given longitude value (degree) between (-180, ..., +180].
	double LLCoordinate::correct_longitude(double value)
	{
		auto temp = std::fmod(value, 360);
		return temp > 180 ? temp - 360 : (temp <= -180 ? temp + 360 : temp);
	}

	class Town
	{
	public:
		Town(const std::string &name, double latitude, double longitude);

		// properties
		std::string name;
		//LLCoordinate coordinates;
		double &latitude = this->coordinates_.first;
		double &longitude = this->coordinates_.second;

		// (real-time) status
		std::list<Traveler> travlers;

	protected:
		std::pair<double, double> coordinates_;
	};

	Town::Town(const std::string &name, double latitude, double longitude) : name(name), coordinates_(latitude, longitude) {}

	class Traveler
	{
	public:
		Traveler(void) = default;

		// properties
		std::string name;
		double speed;		// distance(degree) / time (day)

		// records
		LLCoordinate destination;
		Town last_town;
		std::chrono::system_clock::time_point last_departure;

		// (real-time) status
		double speed_now;
		double bearing_now;
		//LLCoordinate position;		

		double &latitude = this->position_.first;
		double &longitude = this->position_.second;

		void move(const LLCoordinate &pos);
		//LLCoordinate &position = this->position_;
	protected:
		//LLCoordinate position_;
		std::pair<double, double> position_;
	};

}
#endif
