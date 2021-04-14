/*
 * Test dbs simple parser.
 * Benjamin
*/

#include <functional>
#include <iostream>
#include <spdlog/spdlog.h>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "DBSparser.h"

int main(int ac, char* av[]) {
	spdlog::info("Start test DBSparser script\n");

	// Parse input
	std::string dbs_path;
	po::options_description desc("Options");
	desc.add_options()
		("help", "run with test_parser --dbs dbs/simplified.dbs")
	        ("dbs", po::value(&dbs_path));
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

  // Create and load dbsparser
  ssc::DBSParser dbsparser;
  dbsparser.load(dbs_path);

  // Try to encode message
	ssc::sscan_frame frame_to_write;
	auto encode_status = dbsparser.encode("test", 100, frame_to_write);

  // Try to decode message
	auto decode_status = dbsparser.decode(frame_to_write);

}
