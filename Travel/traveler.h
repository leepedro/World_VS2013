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

	class LLCoordinate
	{
	public:
		double latitude;	// horizontal lines presenting north(+) and south(-).
		double longitude;	// vertical lines presenting east(+) and west(-).
		//double &lambda = this->longitude;
		//double &phi = this->latitude;
		void correct(void);
	protected:
		static double correct_degree(double value);
	};

	void LLCoordinate::correct(void)
	{
		this->latitude = LLCoordinate::correct_degree(this->latitude);
		this->longitude = LLCoordinate::correct_degree(this->longitude);
	}

	// Adjusts given value (degree) between (-180, +180].
	double LLCoordinate::correct_degree(double value)
	{
		auto temp = std::fmod(value, 360);
		return temp > 180 ? temp - 360 : (temp <= -180 ? temp + 360 : temp);
	}

	class Town
	{
	public:
		// properties
		std::string name;
		LLCoordinate position;

		// (real-time) status
		std::list<Traveler> travlers;
	};

	class Traveler
	{
	public:
		// properties
		std::string name;
		double speed;		// distance(degree) / time (day)

		// records
		LLCoordinate destination;
		Town last_town;
		std::chrono::system_clock::time_point last_departure;

		// (real-time) status
		double speed_now;
		LLCoordinate position;

		void move(const LLCoordinate &pos);
		//LLCoordinate &position = this->position_;
	protected:
		//LLCoordinate position_;
	};

}
#endif
