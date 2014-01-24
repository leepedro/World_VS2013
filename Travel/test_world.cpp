#include "traveler.h"

void TestLLCoordinate(void)
{
	using namespace World;
	LLCoordinate ll1 = { 1, 1 }, ll2 = { -1, -1 }, ll3 = { 181, 181 }, ll4 = { -181, -181 }, ll5 = { 361, 361 }, ll6 = { -361, -361 };
	ll1.correct();
	ll2.correct();
	ll3.correct();
	ll4.correct();
	ll5.correct();
	ll6.correct();
}

int main(void)
{
	TestLLCoordinate();
	World::Traveler tv1;
	World::Town t1;
}