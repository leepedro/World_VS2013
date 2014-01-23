#include "traveler.h"

double correct(double value)
{
	auto temp = std::fmod(value, 360);
	return temp > 180 ? temp - 360 : (temp <= -180 ? temp + 360 : temp);
}

void TestLLCoordinate(void)
{
	double d1 = correct(1);
	double d2 = correct(-1);
	double d3 = correct(181);
	double d4 = correct(-181);
	double d5 = correct(361);
	double d6 = correct(-361);
}

int main(void)
{
	TestLLCoordinate();
}