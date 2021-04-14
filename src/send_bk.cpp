/*
 * Generate and send CANFRAME.
 * Benjamin
*/

#include <functional>
#include <iostream>
#include <spdlog/spdlog.h>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <string.h>
#include <unistd.h>

// Linux CAN
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
extern "C" {
	#include <linux/can.h>
	#include <linux/can/raw.h>
}

int main(int ac, char* av[]) {
	spdlog::info("Start CANsend script\n");

	// Parse input
	std::string canbus;
	po::options_description desc("Options");
	desc.add_options()
		("help", "run with CANsend -canbus vcan0")
	        ("canbus,c", po::value<std::string>(&canbus));
	po::variables_map vm;
	po::store(po::parse_command_line(ac, av, desc), vm);
	po::notify(vm);
	
	if(vm.count("help")) {
		std::cout << desc << "\n";
		exit(0);
	}
	
	if(canbus.length() < 1) {
		spdlog::info("No canbus specified, using vcan0");
		canbus.clear();
		canbus = "vcan0";
	}
	spdlog::info("Opening CANBUS: {}", canbus);

	// Generate socket
	int s;
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame;

	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		spdlog::error("Unable to open CAN socket.");
		exit(1);
	}
	
	strcpy(ifr.ifr_name, canbus.c_str());
	ioctl(s, SIOCGIFINDEX, &ifr);
	memset(&addr, 0, sizeof(addr));
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if(bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		spdlog::error("Unable to bind to socket.");
		exit(1);
	}
	else {
		spdlog::info("Opened can_socket {}", s);
	}


	// Close socket
	if(close(s) < 0) {
		spdlog::error("Unable to close socket properly.");
		exit(1);
	}
	else {
		spdlog::info("Closed can_socket.");
	}

}
