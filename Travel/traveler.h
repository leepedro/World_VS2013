#if !defined(TRAVELER_H)
#define TRAVELER_H

#include <string>

namespace World
{
	class LLCoordinate
	{
	public:
		double latitude;	// horizontal lines presenting north(+) and south(-).
		double longitude;	// vertical lines presenting east(+) and west(-).
		//double &lambda = this->longitude;
		//double &phi = this->latitude;
		void correct(void);
	protected:
		static double correct_(double value);
	};

	void LLCoordinate::correct(void)
	{
		this->latitude = LLCoordinate::correct_(this->latitude);
		this->longitude = LLCoordinate::correct_(this->longitude);
	}

	// Adjusts given value (degree) between (-180, +180].
	double LLCoordinate::correct_(double value)
	{
		auto temp = std::fmod(value, 360);
		return temp > 180 ? temp - 360 : (temp <= -180 ? temp + 360 : temp);
	}

	class Traveler
	{
	public:
		std::string name;
		void move(const LLCoordinate &pos);
		LLCoordinate &position = this->position_;
	protected:
		LLCoordinate position_;
	};

	class Town
	{
	public:
		std::string name;
		LLCoordinate position;
	};
}
#endif
