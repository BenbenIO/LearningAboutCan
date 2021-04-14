/*
 * SimpleSocketCan is a simple classe based on SocketCan, to init, send and read can message.
 * Benjamin, 2021/04
*/
#pragma once

#include <string.h>
#include <spdlog/spdlog.h>
#include <iostream>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
extern "C" {
#include <linux/can.h>
#include <linux/can/raw.h>
}

// Convert user sec to usec for usleep.
namespace tutil {
	constexpr size_t operator "" _sec(unsigned long long const sleep) {
		return static_cast<size_t>(sleep * 1000000);
	}
}

namespace ssc {

	struct sscan_frame {
    canid_t can_id;  /* 32 bit CAN_ID + EFF/RTR/ERR flags */
    __u8    can_dlc; /* frame payload length in byte (0 .. 8) */
    __u8    __pad;   /* padding */
    __u8    __res0;  /* reserved / padding */
    __u8    __res1;  /* reserved / padding */
    __u8    data[8] __attribute__((aligned(8)));
};

	enum class SocketCanStatus {
		STATUS_OK,
		STATUS_ERROR_INIT,
		STATUS_ERROR_BIND,
		STATUS_ERROR_WRITE,
		STATUS_ERROR_READ,
		STATUS_ERROR_CLOSE
	};

	class SimpleSocketCan {
		public:
			SimpleSocketCan();
			SocketCanStatus open(std::string canbus);
			SocketCanStatus close();
			//std::string get_info() const;
			SocketCanStatus write(sscan_frame frame);
			SocketCanStatus read(sscan_frame &frame);
			~SimpleSocketCan();

		private:
			int socket_;
			struct sockaddr_can addr_;
			struct ifreq ifr_;
			struct can_frame frame_;

	};

}
