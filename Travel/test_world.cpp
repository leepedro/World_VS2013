#include <iostream>
//#include <vector>
#include <thread>		// std::thread()
#include <future>		// std::async()

#include "traveler.h"

void action1(Travel::Traveler &traveler, Travel::Town &town1, Travel::Town &town2)
{
	std::clog << "action1 started by thread(" << std::this_thread::get_id() << ")" << std::endl;
	traveler.anchor(town1);
	traveler.leave_for(town2);
	traveler.anchor(town2);
	std::clog << "action1 finished by thread(" << std::this_thread::get_id() << ")" << std::endl;
}

int main(void)
{	
	using namespace Travel;

	World world;
	world.travelers.push_back(Traveler("me"));
	world.travelers.push_back(Traveler("you"));
	world.towns.push_back(Town("San Jose", dms2dec(37, 20), dms2dec(-121, -54)));
	world.towns.push_back(Town("Dayton", dms2dec(39, 45, 34), dms2dec(-84, -11, -30)));

	// TODO: What if the finding fail?
	//auto it_towns = std::find_if(world.towns.begin(), world.towns.end(), [](const Town &t){ return t.name == "San Jose"; });
	//if (it_towns == world.towns.end())
	//	std::cerr << "Incorrect name" << std::endl;
	//auto &town1 = *it_towns;
	//it_towns = std::find_if(world.towns.begin(), world.towns.end(), [](const Town &t){ return t.name == "Dayton"; });
	//if (it_towns == world.towns.end())
	//	std::cerr << "Incorrect name" << std::endl;
	//auto &town2 = *it_towns;
	//auto it_travelers = std::find_if(world.travelers.begin(), world.travelers.end(), [](const Traveler &t){ return t.name == "me"; });
	//if (it_travelers == world.travelers.end())
	//	std::cerr << "Incorrect name" << std::endl;
	//auto &traveler1 = *it_travelers;
	//it_travelers = std::find_if(world.travelers.begin(), world.travelers.end(), [](const Traveler &t){ return t.name == "you"; });
	//if (it_travelers == world.travelers.end())
	//	std::cerr << "Incorrect name" << std::endl;
	//auto &traveler2 = *it_travelers;
	auto &town1 = *std::find_if(world.towns.begin(), world.towns.end(), [](const Town &t){ return t.name == "San Jose"; });
	auto &town2 = *std::find_if(world.towns.begin(), world.towns.end(), [](const Town &t){ return t.name == "Dayton"; });
	auto &traveler1 = *std::find_if(world.travelers.begin(), world.travelers.end(), [](const Traveler &t){ return t.name == "me"; });
	auto &traveler2 = *std::find_if(world.travelers.begin(), world.travelers.end(), [](const Traveler &t){ return t.name == "you"; });

	// serial work flow.
	//traveler1.anchor(town1);
	//traveler2.anchor(town2);
	//traveler1.leave_for(town2);
	//traveler1.anchor(town2);

	// parallel work flow using std::thread
	//std::thread th1(action1, std::ref(traveler1), std::ref(town1), std::ref(town2));
	//std::thread th2(&Traveler::anchor, std::ref(traveler2), std::ref(town2));
	//th1.join();
	//th2.join();

	// parallel work flow using std::async
	auto h1 = std::async(std::launch::async, action1, std::ref(traveler1), std::ref(town1), std::ref(town2));
	auto h2 = std::async(std::launch::async, &Traveler::anchor, std::ref(traveler2), std::ref(town2));
	h1.wait();
	h2.wait();

	//Traveler &tv2 = *std::find_if(world.travelers.begin(), world.travelers.end(), [](const Traveler &tv){ return tv.name == "you"; });
	//std::list<Traveler>::iterator it = std::find_if(world.travelers.begin(), world.travelers.end(), [](const Traveler &tv){ return tv.name == "you_"; });
	//if (it == world.travelers.end())
	//	std::cout << "There is no match." << std::endl;
}
