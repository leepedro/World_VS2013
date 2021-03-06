#if !defined(TRAVELER_H)
#define TRAVELER_H

#include <string>		// std::string
#include <chrono>		// std::chrono::system_clock
#include <list>			// std::list<T>
#include <atomic>
#include <algorithm>	// std::move()
#include <cmath>		// std::atan2(), ...
#include <stdexcept>	// std::invalid_argument
#include <functional>	// std::reference_wrapper<T>

namespace Travel
{
	// Forward Declaration.
	//class LLCoordinate;
	class Traveler;
	//class Town;

	// constexpr is NOT supported by VS2013.
	//constexpr double PI = std::atan(1) * 4;	// (radian)
	const double PI = std::atan(1) * 4;	// (radian)
	const double EARTH_RADIUS = 6371;	// mean earth radius (km)
	const double TIME_FACTOR = 3600.0;	// sim hour ~= sec

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

	// Computes the great-circle distance or orthodromic distance (km) between two
	// geo-positions as (latitude, longitude) using Spherical law of Cosines with mean earth
	// radius. All coordinates are in degree.
	double get_distance(double lat1, double lon1, double lat2, double lon2)
	{
		double lat1_r = degree2rad(lat1);
		double lat2_r = degree2rad(lat2);
		double diff_lon = degree2rad(lon2 - lon1);
		using namespace std;
		return acos(sin(lat1_r) * sin(lat2_r) + cos(lat1_r) * cos(lat2_r) * cos(diff_lon)) *
			EARTH_RADIUS;
	}

	// Computes the current geo-position as (latitude, longitude) based on the origin, intended
	// destination, and the surface distance(km). All coordinates are in degree.
	void get_position(double lat1, double lon1, double lat2, double lon2, double dist,
		double &lat, double &lon, double &azi)
	{
		double la_1 = degree2rad(lat1);
		double la_2 = degree2rad(lat2);
		double lo_12 = degree2rad(lon2 - lon1);
		using namespace std;
		// NOTE: atan2 returns [-pi, ..., pi].
		
		// bearing at point 1 and reference point 0 at the equator.
		double b_1 = atan2(sin(lo_12), cos(la_1) * tan(la_2) - sin(la_1) * cos(lo_12));
		double b_0 = atan2(sin(b_1) * cos(la_1), sqrt(cos(b_1) * cos(b_1) + sin(b_1) * sin(b_1) * sin(la_1) * sin(la_1)));

		// angular distance from the reference point 0.
		double ad_1 = atan2(tan(la_1), cos(b_1));
		double ad_dt = dist / EARTH_RADIUS;
		double ad = ad_1 + ad_dt;

		// current latitude
		double la = atan2(cos(b_0) * sin(ad), sqrt(cos(ad) * cos(ad) + sin(b_0) * sin(b_0) * sin(ad) * sin(ad)));
		assert(la >= -PI / 2 && la <= PI / 2);

		// current longitude
		double lo_01 = atan2(sin(b_0) * sin(ad_1), cos(ad_1));
		double lo_0 = degree2rad(lon1) - lo_01;
		double lo = atan2(sin(b_0) * sin(ad), cos(ad)) + lo_0;

		// current bearing
		double b = atan2(tan(b_0), cos(ad));

		lat = rad2degree(la);
		lon = rad2degree(lo);
		azi = fmod(rad2degree(b) + 360, 360);	// (-pi, ..., pi] -> [0, ..., 360)
	}

	double get_bearing_(double lat1, double lon1, double lat2, double lon2)
	{
		double lat1_r = degree2rad(lat1);
		double lat2_r = degree2rad(lat2);
		double diff_lon = degree2rad(lon2 - lon1);
		using namespace std;
		return atan2(std::sin(diff_lon) * cos(lat2_r),
			cos(lat1_r) * sin(lat2_r) - sin(lat1_r) * cos(lat2_r) * cos(diff_lon));
	}

	double get_bearing(double lat1, double lon1, double lat2, double lon2)
	{
		double lat1_r = degree2rad(lat1);
		double lat2_r = degree2rad(lat2);
		double diff_lon = degree2rad(lon2 - lon1);
		using namespace std;
		double bearing_r = atan2(std::sin(diff_lon) * cos(lat2_r),
			cos(lat1_r) * sin(lat2_r) - sin(lat1_r) * cos(lat2_r) * cos(diff_lon));
		return fmod(rad2degree(bearing_r) + 360, 360);	// (-pi, ..., pi] -> [0, ..., 360)
	}

	class LLCoordinate
	{
	public:
		double latitude;	// horizontal lines presenting north(+) and south(-).
		double longitude;	// vertical lines presenting east(+) and west(-).
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

	////////////////////////////////////////////////////////////////////////////////////////
	// Town
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
		const std::pair<double, double> &coordinate = this->coordinates_;
		const double &latitude = this->coordinates_.first;
		const double &longitude = this->coordinates_.second;
		const std::string &name = this->name_;

		// (occasionally changed) status
		bool can_park = true;
		std::list<std::reference_wrapper<Traveler>> travelers;

		// methods.
		double distance_to(const Town &dst) const;

	protected:
		// properties (never changed).
		std::pair<double, double> coordinates_;
		std::string name_ = "unknown";
	};

	Town::Town(const Town &src) : can_park(src.can_park), travelers(src.travelers),
		coordinates_(src.coordinates_), name_(src.name_) {}

	Town::Town(Town &&src) : can_park(src.can_park), travelers(std::move(src.travelers)),
		coordinates_(std::move(src.coordinates_)), name_(std::move(src.name_)) {}

	Town &Town::operator=(const Town &src)
	{
		this->can_park = src.can_park;
		this->travelers = src.travelers;
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

	// Town
	////////////////////////////////////////////////////////////////////////////////////////

	// A dummy variable as a default value.
	Town nowhere;


	////////////////////////////////////////////////////////////////////////////////////////
	// Traveler
	class Traveler
	{
	public:
		Traveler(void) = default;
		Traveler(const Traveler &src);
		Traveler(Traveler &&src);
		Traveler(const std::string &name);

		// properties (never changed)
		const std::string &name = this->name_;

		// (occasionally changed) status
		//double dist_est;	// (km)
		std::reference_wrapper<Town> last_town;
		std::reference_wrapper<Town> next_town;
		double distance_plan;	// (km)
		bool parked;

		// (real-time) status
		const std::pair<double, double> &coordinates = this->coordinates_;
		double &latitude = this->coordinates_.first;
		double &longitude = this->coordinates_.second;
		double distance_moved;	// accumulated distance (km)
		double speed = 0.0;		// (km/h)
		std::chrono::system_clock::time_point time_last;

		// operators.
		bool operator==(const Traveler &rhs) const;

		// actions.
		bool anchor(Town &town);
		void depart(void);
		void leave_for(Town &town);
		void update(void);

	protected:
		// properties (never changed)
		std::string name_ = "unknown";

		// (real-time) status
		std::pair<double, double> coordinates_;
	};

	Traveler::Traveler(const Traveler &src) :
		last_town(src.last_town),
		next_town(src.next_town),
		distance_plan(src.distance_plan),
		parked(src.parked),
		distance_moved(src.distance_moved),
		speed(src.speed),
		time_last(src.time_last),
		name_(src.name_),
		coordinates_(src.coordinates_) {}

	Traveler::Traveler(Traveler &&src) :
		last_town(std::move(src.last_town)),
		next_town(std::move(src.next_town)),
		distance_plan(distance_plan),
		parked(src.parked),
		distance_moved(src.distance_moved),
		speed(src.speed),
		time_last(std::move(src.time_last)),
		name_(std::move(src.name_)),
		coordinates_(std::move(src.coordinates_)) {}

	Traveler::Traveler(const std::string &name) : last_town(nowhere), next_town(nowhere),
		name_(name) {}

	bool Traveler::operator==(const Traveler &rhs) const
	{
		// TODO: Use a unique ID instead of string.
		return this->name == rhs.name;
	}

	bool Traveler::anchor(Town &town)
	{
		// Traveler must be close to the town, and parking must be available at the town.		
		if (get_distance(this->latitude, this->longitude, town.latitude, town.longitude) < 50.0 &&
			town.can_park)
		{
			this->last_town = town;
			this->speed = 0;
			this->parked = true;
			this->coordinates_ = town.coordinate;
			town.travelers.push_back(*this);
			return true;
		}
		return false;
	}

	void Traveler::depart(void)
	{
		if (this->parked)
		{	// Undo anchor().
			Town &town = this->last_town.get();
			town.travelers.remove_if([this](const Traveler &tv){ return tv == *this; });
			this->parked = false;
		}
		// No change, if not parked to a town.
	}

	void Traveler::leave_for(Town &dst)
	{
		this->depart();
		this->next_town = dst;
		this->time_last = std::chrono::system_clock::now();
		auto &orgn = this->last_town.get();
		this->distance_plan = orgn.distance_to(dst);			// (km)
		this->distance_moved = 0;								// (km)
		//double time_est = dist / this->speed_cruise;	// (sim hour)
	}

	void Traveler::update(void)
	{
		if (!this->parked)
		{
			// Compute the advanced distance since the last update with the current speed.
			auto now = std::chrono::system_clock::now();
			auto t = std::chrono::duration<double>(now - this->time_last);	// (sec)
			auto hours = t.count() * TIME_FACTOR / 3600.0;	// (sec) -> (sim hour)
			auto distance = this->speed * hours;			// (km)
			this->distance_moved += distance;				// accumulated distance
			this->time_last = now;
			//double progress = this->distance_moved / this->distance_plan;

			// Compute current coordinates and update it.
			auto &orgn = this->last_town.get();
			auto &dst = this->next_town.get();
			double lat, lon, azi;
			get_position(orgn.latitude, orgn.longitude, dst.latitude, dst.longitude, this->distance_moved,
				lat, lon, azi);
			this->latitude = lat;
			this->longitude = lon;

			// Try to park at every opportunity.
			this->anchor(dst);
		}
		// No change, if parked to a town.
	}

	// Traveler
	////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////
	// World
	class World
	{
	public:		
		World(void) = default;
		World(const World&) = delete;
		World(World &&) = delete;

		bool find_town(const std::string &name, std::list<Town>::iterator &it);
		bool find_traveler(const std::string &name, std::list<Traveler>::iterator &it);
		void update_travelers(void);

		std::list<Town> towns;
		std::list<Traveler> travelers;
	};

	bool World::find_town(const std::string &name, std::list<Town>::iterator &it)
	{
		it = std::find_if(this->towns.begin(), this->towns.end(), [name](const Town &t){ return t.name == name; });
		if (it == this->towns.end())
			return false;
		else
			return true;
	}

	bool World::find_traveler(const std::string &name, std::list<Traveler>::iterator &it)
	{
		it = std::find_if(this->travelers.begin(), this->travelers.end(), [name](const Traveler &t){ return t.name == name; });
		if (it == this->travelers.end())
			return false;
		else
			return true;
	}

	void World::update_travelers(void)
	{
		for (auto &traveler : this->travelers)
			traveler.update();
	}
	// World
	////////////////////////////////////////////////////////////////////////////////////////
}
#endif
