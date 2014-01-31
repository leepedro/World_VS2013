#include <iostream>
#include <thread>		// std::thread()
#include <future>		// std::async()

#include "traveler.h"

void action1(Travel::Traveler &traveler, Travel::Town &town1, Travel::Town &town2)
{
	traveler.anchor(town1);
	if (traveler.anchor(town1))
		std::clog << traveler.name << " is parked at " << town1.name << std::endl;
	else
		std::clog << traveler.name << " is NOT parked at " << town1.name << std::endl;
	traveler.speed = 1224.0;	// (km/h)
	traveler.leave_for(town2);
	std::clog << traveler.name << " is leaving for " << traveler.next_town.get().name << " with " << traveler.speed << "km/hr" << std::endl;
}

// Use async for only timer.
void run_serial(Travel::World &world, Travel::Traveler &traveler1, Travel::Traveler &traveler2, Travel::Town &town1, Travel::Town &town2)
{
	std::cout << std::endl << "Use std::async for only timer." << std::endl << std::endl;
	using namespace Travel;

	// Place travelers at the initial position.
	traveler1.latitude = town1.latitude - 0.1;
	traveler1.longitude = town1.longitude - 0.1;
	traveler2.latitude = town2.latitude + 0.1;
	traveler2.longitude = town2.longitude + 0.1;

	// One traveler parks at a town.
	if (traveler1.anchor(town1))
		std::clog << traveler1.name << " is parked at " << town1.name << std::endl;
	else
		std::clog << traveler1.name << " is NOT parked at " << town1.name << std::endl;

	// One traveler parks at a town.
	if (traveler2.anchor(town2))
		std::clog << traveler2.name << " is parked at " << town2.name << std::endl;
	else
		std::clog << traveler2.name << " is NOT parked at " << town2.name << std::endl;

	// One traveler leaves a town for another town.
	traveler1.speed = 1224.0;	// (km/h)
	traveler1.leave_for(town2);
	std::clog << traveler1.name << " is leaving for " << traveler1.next_town.get().name << " with " << traveler1.speed << "km/hr" << std::endl;

	// The status of all travelers are continuously updated.
	auto stop = std::async(std::launch::async, [](){ std::this_thread::sleep_for(std::chrono::seconds(3)); });
	while (stop.wait_for(std::chrono::milliseconds(1)) != std::future_status::ready)
		world.update_travelers();

	if (traveler1.parked)
		std::clog << traveler1.name << " has arrived at " << traveler1.last_town.get().name << std::endl;
	else
		std::clog << traveler1.name << " has NOT arrived at " << traveler1.next_town.get().name << std::endl;
}

// Use async for timer and each traveler.
void run_parallel_async(Travel::World &world, Travel::Traveler &traveler1, Travel::Traveler &traveler2, Travel::Town &town1, Travel::Town &town2)
{
	std::cout << std::endl << "Use std::async for timer and each traveler." << std::endl << std::endl;
	using namespace Travel;

	// Place travelers at the initial position.
	traveler1.latitude = town1.latitude - 0.1;
	traveler1.longitude = town1.longitude - 0.1;
	traveler2.latitude = town2.latitude + 0.1;
	traveler2.longitude = town2.longitude + 0.1;

	// One traveler parks at a town, and then leaves for another town.
	auto h1 = std::async(std::launch::async, action1, std::ref(traveler1), std::ref(town1), std::ref(town2));

	// One traveler parks at a town.
	auto h2 = std::async(std::launch::async, &Traveler::anchor, std::ref(traveler2), std::ref(town2));

	// The status of all travelers are continuously updated.
	auto stop = std::async(std::launch::async, [](){ std::this_thread::sleep_for(std::chrono::seconds(3)); });
	while (stop.wait_for(std::chrono::milliseconds(1)) != std::future_status::ready)
		world.update_travelers();

	h1.wait();
	if (traveler1.parked)
		std::clog << traveler1.name << " has arrived at " << traveler1.last_town.get().name << std::endl;
	else
		std::clog << traveler1.name << " has NOT arrived at " << traveler1.next_town.get().name << std::endl;

	if (h2.get())
		std::clog << traveler2.name << " is parked at " << traveler2.last_town.get().name << std::endl;
	else
		std::clog << traveler2.name << " is NOT parked at " << traveler2.next_town.get().name << std::endl;
}

// Use thread for each traveler and async for timer.
void run_parallel_thread(Travel::World &world, Travel::Traveler &traveler1, Travel::Traveler &traveler2, Travel::Town &town1, Travel::Town &town2)
{
	std::cout << std::endl << "Use std::thread for each traveler and std::async for timer." << std::endl << std::endl;
	using namespace Travel;

	// Place travelers at the initial position.
	traveler1.latitude = town1.latitude - 0.1;
	traveler1.longitude = town1.longitude - 0.1;
	traveler2.latitude = town2.latitude + 0.1;
	traveler2.longitude = town2.longitude + 0.1;

	// One traveler parks at a town, and then leaves for another town.
	std::thread th1(action1, std::ref(traveler1), std::ref(town1), std::ref(town2));

	// One traveler parks at a town.
	std::thread th2(&Traveler::anchor, std::ref(traveler2), std::ref(town2));

	// The status of all travelers are continuously updated.
	auto stop = std::async(std::launch::async, [](){ std::this_thread::sleep_for(std::chrono::seconds(3)); });
	while (stop.wait_for(std::chrono::milliseconds(1)) != std::future_status::ready)
		world.update_travelers();

	th1.join();
	if (traveler1.parked)
		std::clog << traveler1.name << " has arrived at " << traveler1.last_town.get().name << std::endl;
	else
		std::clog << traveler1.name << " has NOT arrived at " << traveler1.next_town.get().name << std::endl;

	th2.join();
	if (traveler2.parked)
		std::clog << traveler2.name << " is parked at " << traveler2.last_town.get().name << std::endl;
	else
		std::clog << traveler2.name << " is NOT parked at " << traveler2.next_town.get().name << std::endl;
}


int main(void)
{	
	using namespace Travel;

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

		run_serial(world, traveler1, traveler2, town1, town2);
		run_parallel_async(world, traveler1, traveler2, town1, town2);
		run_parallel_thread(world, traveler1, traveler2, town1, town2);

	}
}
