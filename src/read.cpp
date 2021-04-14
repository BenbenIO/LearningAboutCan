/*
 * Read and decode CANFRAME.
 * Benjamin
*/

#include <functional>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <spdlog/spdlog.h>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "SimpleSocketCan.h"
#include "DBSparser.h"
// simple sec to usec convertor
using namespace tutil;

int main(int ac, char* av[]) {
	spdlog::info("Start CANread script\n");

	// Parse input
	std::string canbus;
	std::string dbs_path;
	po::options_description desc("Options");
	desc.add_options()
		("help", "run with CANread -canbus vcan0")
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
	ssc::sscan_frame read_frame;
	while(true) {
		auto status_read = vcan0.read(read_frame);
		if (status_read != ssc::SocketCanStatus::STATUS_OK) {
			spdlog::error("Error while reading socket.");
		}

		// Print out the frame content.
		std::cout << std::hex << std::setfill('0');
		spdlog::info("Received: frame {} | {}:",read_frame.can_id, read_frame.can_dlc);
		for(auto i=0; i<read_frame.can_dlc;i++) {
			std::cout << std::setw(2) << read_frame.data[i] << " ";
		}
		std::cout << std::endl;

		// Decode data:
		auto decode_status = dbsparser.decode(read_frame);
	}

}
