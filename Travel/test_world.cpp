#include <algorithm>
#include <iostream>
#include <vector>

#include "traveler.h"

int main(void)
{	
	using namespace Travel;

	Town tw1 = { "San Jose", dms2dec(37, 20), dms2dec(-121, -54) };
	std::string &txt1 = tw1.name;
	World world;
	world.travelers.push_back(Traveler("me"));
	world.travelers.push_back(Traveler("you"));
	//world.towns.push_back(Town("San Jose", dms2dec(37, 20), dms2dec(-121, -54)));
	//world.towns.push_back(tw1);
	world.towns.push_back(std::move(tw1));
	world.towns.push_back(Town("Dayton", dms2dec(39, 45, 34), dms2dec(-84, -11, -30)));
	// TODO: Check out the above two lines produce different result.
	// TODO: Check out Linux.

	Town &t1 = world.towns.front();
	std::string &txt2 = t1.name;
	Traveler &tv1 = world.travelers.front();
	tv1.anchor(world.towns.front());
	tv1.leave_for(world.towns.back());
	tv1.anchor(world.towns.back());

	
	//Traveler &tv2 = *std::find_if(world.travelers.begin(), world.travelers.end(), [](const Traveler &tv){ return tv.name == "you"; });
	//std::list<Traveler>::iterator it = std::find_if(world.travelers.begin(), world.travelers.end(), [](const Traveler &tv){ return tv.name == "you_"; });
	//if (it == world.travelers.end())
	//	std::cout << "There is no match." << std::endl;
}