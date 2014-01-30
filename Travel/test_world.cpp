#include <iostream>
//#include <vector>
#include <thread>		// std::thread()
#include <future>		// std::async()

#include "traveler.h"

void action1(Travel::Traveler &traveler, Travel::Town &town1, Travel::Town &town2)
{
	std::clog << "action1 started by thread(" << std::this_thread::get_id() << ")" << std::endl;
	if (!traveler.anchor(town1))
	{	// If failed, try one more time and wait.
		std::this_thread::sleep_for(std::chrono::seconds(2));

		auto future = std::async(std::launch::async, &Travel::Traveler::anchor, std::ref(traveler), std::ref(town1));
		future.wait_for(std::chrono::seconds(1));
	}
	traveler.leave_for(town2);
	traveler.anchor(town2);
	std::clog << "action1 finished by thread(" << std::this_thread::get_id() << ")" << std::endl;
}

int main(void)
{	
	using namespace Travel;

	double lat, lon, azi;
	get_pos_LL(-33, -71.6, 31.4, 121.8, 18743, 0.5, lat, lon, azi);

	World world;
	world.travelers.push_back(Traveler("me"));
	world.travelers.push_back(Traveler("you"));
	world.towns.push_back(Town("San Jose", dms2dec(37, 20), dms2dec(-121, -54)));
	world.towns.push_back(Town("Dayton", dms2dec(39, 45, 34), dms2dec(-84, -11, -30)));
	world.towns.push_back(Town("Valparaíso", dms2dec(-33, -3), dms2dec(-71, -37)));
	world.towns.push_back(Town("Shanghai", dms2dec(39, 12), dms2dec(121, 30)));

	std::list<Town>::iterator it_town_1, it_town_2;
	std::list<Traveler>::iterator it_traveler_1, it_traveler_2;
	if (world.find_town("San Jose", it_town_1) && world.find_town("Dayton", it_town_2) && world.find_traveler("me", it_traveler_1) && world.find_traveler("you", it_traveler_2))
	{
		auto &town1 = *it_town_1;
		auto &town2 = *it_town_2;
		auto &traveler1 = *it_traveler_1;
		auto &traveler2 = *it_traveler_2;

		traveler1.latitude = town1.latitude - 0.1;
		traveler1.longitude = town1.longitude - 0.1;
		traveler2.latitude = town2.latitude + 0.1;
		traveler2.longitude = town2.longitude + 0.1;

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
	}

	//auto &town1 = *std::find_if(world.towns.begin(), world.towns.end(), [](const Town &t){ return t.name == "San Jose"; });
	//auto &town2 = *std::find_if(world.towns.begin(), world.towns.end(), [](const Town &t){ return t.name == "Dayton"; });
	//auto &traveler1 = *std::find_if(world.travelers.begin(), world.travelers.end(), [](const Traveler &t){ return t.name == "me"; });
	//auto &traveler2 = *std::find_if(world.travelers.begin(), world.travelers.end(), [](const Traveler &t){ return t.name == "you"; });

	//traveler1.latitude = town1.latitude - 0.1;
	//traveler1.longitude = town1.longitude - 0.1;
	//traveler2.latitude = town2.latitude + 0.1;
	//traveler2.longitude = town2.longitude + 0.1;

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
	//auto h1 = std::async(std::launch::async, action1, std::ref(traveler1), std::ref(town1), std::ref(town2));
	//auto h2 = std::async(std::launch::async, &Traveler::anchor, std::ref(traveler2), std::ref(town2));
	//h1.wait();
	//h2.wait();

	//Traveler &tv2 = *std::find_if(world.travelers.begin(), world.travelers.end(), [](const Traveler &tv){ return tv.name == "you"; });
	//std::list<Traveler>::iterator it = std::find_if(world.travelers.begin(), world.travelers.end(), [](const Traveler &tv){ return tv.name == "you_"; });
	//if (it == world.travelers.end())
	//	std::cout << "There is no match." << std::endl;

	// There will be a couple of threads for traveler update (as a group), and another thread for town update.
}
