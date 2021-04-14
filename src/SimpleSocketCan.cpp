/*
 * SimpleSocketCan is a simple classe based on SocketCan, to init, send and read can message.
 * Benjamin, 2021/04
*/

#include "SimpleSocketCan.h"

namespace ssc {
	SimpleSocketCan::SimpleSocketCan() {
		spdlog::info("SimpleSocketCan created.");
	}

	SocketCanStatus SimpleSocketCan::open(std::string canbus) {
		// Bus to connect to.
		if(canbus.length() < 1) {
			spdlog::warn("No canbus specified, using vcan0");
			canbus.clear();
			canbus = "vcan0";
		}
		spdlog::info("Opening CANBUS: {}", canbus);

		// Open and bind socket.
		if ((socket_ = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
			spdlog::error("Unable to open CAN socket.");
			return SocketCanStatus::STATUS_ERROR_INIT;
		}

		strcpy(ifr_.ifr_name, canbus.c_str());
		ioctl(socket_, SIOCGIFINDEX, &ifr_);
		memset(&addr_, 0, sizeof(addr_));
		addr_.can_family = AF_CAN;
		addr_.can_ifindex = ifr_.ifr_ifindex;

		if(bind(socket_, (struct sockaddr *)&addr_, sizeof(addr_)) < 0) {
			spdlog::error("Unable to bind to socket.");
			return SocketCanStatus::STATUS_ERROR_BIND;
		}
		else {
			spdlog::info("Opened can_socket {}", socket_);
			return SocketCanStatus::STATUS_OK;
		}
	}

	SocketCanStatus SimpleSocketCan::close() {
		if(::close(socket_) < 0) {
			spdlog::error("Unable to close socket properly.");
			return SocketCanStatus::STATUS_ERROR_CLOSE;
		}
		else {
			spdlog::info("Closed can_socket.");
			return SocketCanStatus::STATUS_OK;
		}
	}

	SocketCanStatus SimpleSocketCan::write(sscan_frame frame) {
		if (::write(socket_, &frame, sizeof(struct sscan_frame)) != sizeof(struct sscan_frame)) {
			spdlog::error("Error while writing frame.");
			return SocketCanStatus::STATUS_ERROR_WRITE;
		}
		else {
			return SocketCanStatus::STATUS_OK;
		}
	}
	SocketCanStatus SimpleSocketCan::read(sscan_frame &frame) {
		if( ::read(socket_, &frame, sizeof(struct sscan_frame)) < 0) {
			spdlog::error("Error while reading");
			return SocketCanStatus::STATUS_ERROR_READ;
		}
		else {
			return SocketCanStatus::STATUS_OK;
		}
	}

	SimpleSocketCan::~SimpleSocketCan(){
		spdlog::info("SimpleSocketCan destroyed.");
		close();
	}
}
