/*
 * Generate, encode and  send CANFRAME.
 * Benjamin
*/

#include <functional>
#include <iostream>
#include <unistd.h>
#include <spdlog/spdlog.h>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <random>

#include "SimpleSocketCan.h"
#include "DBSparser.h"

// simple sec to usec convertor
using namespace tutil;

int main(int ac, char* av[]) {
	spdlog::info("Start CANsend script\n");

	// Parse input
	std::string canbus;
	std::string dbs_path;
	po::options_description desc("Options");
	desc.add_options()
		("help", "run with CANsend --canbus vcan0 --dbs dbs/simplified.dbs")
	  ("canbus,c", po::value<std::string>(&canbus), "path to socket")
		("dbs", po::value(&dbs_path), "path to dbs file");
	po::variables_map vm;
	po::store(po::parse_command_line(ac, av, desc), vm);
	po::notify(vm);

	if(vm.count("help")) {
		std::cout << desc << "\n";
		exit(0);
	}

	if(dbs_path.length() < 1) {
		spdlog::warn("No .dbs file specified.");
    std::cout << desc << "\n";
    exit(1);
	}

	if(canbus.length() < 1) {
		spdlog::warn("No canbus specified, using vcan0");
		canbus.clear();
		canbus = "vcan0";
	}
	spdlog::info("Opening CANBUS: {}", canbus);

	// Generate socket
	ssc::SimpleSocketCan vcan0;
	auto status = vcan0.open(canbus);
	if (status != ssc::SocketCanStatus::STATUS_OK) {
		spdlog::error("Error while opening the socket: {}", status);
	}

	// Load dbsfile:
	ssc::DBSParser dbsparser;
  dbsparser.load(dbs_path);

	// Generate and send message.
	std::random_device rd{};
	auto mtgen = std::mt19937{ rd() };
	auto ud = std::uniform_real_distribution<>{0.0, 250.0};
	while(true) {
		auto data = ud(mtgen);
		ssc::sscan_frame frame_to_write;
		auto encode_status = dbsparser.encode("test", data, frame_to_write);
		spdlog::info("Sending frame {}: data: {}",frame_to_write.can_id, data);
		auto write_status = vcan0.write(frame_to_write);
		if (write_status != ssc::SocketCanStatus::STATUS_OK) {
			spdlog::error("Error while writing frame: {}", status);
		}

		usleep(1_sec);
	}

}
