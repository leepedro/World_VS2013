#if !defined(TRAVELER_H)
#define TRAVELER_H

#include <string>		// std::string
#include <chrono>		// std::chrono::system_clock
#include <list>			// std::list<T>
#include <atomic>
#include <algorithm>	// std::move()
#include <cmath>		// std::atan(), ...
#include <stdexcept>	// std::invalid_argument
#include <functional>	// std::reference_wrapper<T>

namespace Travel
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
	double dms2dec(double degree, double minute, double second = 0)
	{
		if ((degree >= 0 && minute >= 0 && second >= 0) ||
			(degree <= 0 && minute <= 0 && second <= 0))
			return degree + (minute * 60 + second) / 3600;
		else
			throw std::invalid_argument(
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
		Town(void) = default;
		Town(const Town &src);
		Town(Town &&src);
		Town &operator=(const Town &src);
		Town &operator=(Town &&src) = delete;
		Town(const std::string &name, double latitude, double longitude);

		// properties (never changed).
		double &latitude = this->coordinates_.first;
		double &longitude = this->coordinates_.second;
		std::string &name = this->name_;

		// (occasionally changed) status
		std::list<std::reference_wrapper<Traveler>> travlers;

		// methods.
		double distance_to(const Town &dst) const;

	protected:
		// properties (never changed).
		std::pair<double, double> coordinates_;
		std::string name_;
	};

	// A dummy variable as a default value.
	Town nowhere;

	Town::Town(const Town &src) : coordinates_(src.coordinates_), name_(src.name_) {}

	Town::Town(Town &&src) : coordinates_(std::move(src.coordinates_)),
		name_(std::move(src.name_)) {}

	Town &Town::operator=(const Town &src)
	{
		this->coordinates_ = src.coordinates_;
		this->name_ = src.name_;
		return *this;
	}

	Town::Town(const std::string &name, double latitude, double longitude) :
		coordinates_(latitude, longitude), name_(name) {}

	double Town::distance_to(const Town &dst) const
	{
		return get_distance(this->latitude, this->longitude, dst.latitude, dst.longitude);
	}


	class Traveler
	{
	public:
		Traveler(void) = default;
		Traveler(const Traveler &src);
		Traveler(Traveler &&src);
		Traveler(const std::string &name);

		// properties (never changed)
		std::string &name = this->name_;
		//double speed;		// distance(degree) / time (day)

		// (occasionally changed) status
		//LLCoordinate destination;
		std::reference_wrapper<Town> last_town;
		std::chrono::system_clock::time_point last_depart_time;
		std::reference_wrapper<const Town> next_town;
		bool parked;

		// (real-time) status
		//double bearing_now;
		double &latitude = this->position_.first;
		double &longitude = this->position_.second;
		//double speed_now;

		// operators.
		bool operator==(const Traveler &rhs) const;

		// actions.
		bool anchor(Town &town);
		void depart(void);
		void leave_for(const Town &town);
		void move(const LLCoordinate &pos);
	protected:
		// properties (never changed)
		std::string name_ = "unknown";

		// (real-time) status
		std::pair<double, double> position_;
	};

	Traveler::Traveler(const Traveler &src) : last_town(src.last_town),
		last_depart_time(src.last_depart_time), next_town(src.next_town),
		parked(src.parked), name_(src.name_), position_(src.position_) {}

	Traveler::Traveler(Traveler &&src) : last_town(std::move(src.last_town)),
		last_depart_time(std::move(src.last_depart_time)),
		next_town(std::move(src.next_town)), parked(src.parked),
		name_(std::move(src.name_)), position_(std::move(src.position_)) {}

	Traveler::Traveler(const std::string &name) : last_town(nowhere), next_town(nowhere),
		name_(name) {}

	bool Traveler::operator==(const Traveler &rhs) const
	{
		// TODO: Use a unique ID instead of string.
		return this->name == rhs.name;
	}

	bool Traveler::anchor(Town &town)
	{
		// TODO: Check the approximity of the position.
		if (true)
		{
			this->last_town = town;
			this->parked = true;
			town.travlers.push_back(*this);
			return true;
		}
		return false;
	}

	void Traveler::depart(void)
	{
		if (this->parked)
		{
			// Remove the travler from the parking lot.
			Town &town = this->last_town.get();
			town.travlers.remove_if([this](const Traveler &tv){ return tv == *this; });
			this->last_depart_time = std::chrono::system_clock::now();
			this->parked = false;
		}
		// No change, if not parked to a town.
	}

	void Traveler::leave_for(const Town &town)
	{
		if (this->parked)
			this->depart();
		this->next_town = town;
	}


	class World
	{
	public:
		World(void) = default;
		World(const World&) = delete;
		World(World &&) = delete;

		//bool find_town(const std::string &name, Town &town);
		//Traveler &find_traveler(const std::string &name) const;

		std::list<Town> towns;
		std::list<Traveler> travelers;
	};

	//bool World::find_town(const std::string &name, Town &town)
	//{
	//	std::list<Town>::iterator it = std::find_if(this->towns.begin(), this->towns.end(), [name](const Town &t){ return t.name == name; });
	//	if (it == this->towns.end())
	//		return false;
	//	else
	//	{
	//		town = *it;
	//		return true;
	//	}
	//}
}
#endif
