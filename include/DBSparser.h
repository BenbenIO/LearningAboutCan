/*
 * Very simplified dbs file parser to illustrate principle.
 * Benjamin, 2021/04
 * Missing: canID convertion, endianness writing, lenght and start writing.
 * Using: fixed can_id.
*/
#pragma once

#include <string.h>
#include <spdlog/spdlog.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <vector>
#include "SimpleSocketCan.h"

namespace ssc {

  enum class DBSParserStatus {
		STATUS_OK,
		STATUS_ERROR_INIT,
		STATUS_ERROR_MSG,
    STATUTS_ERROR_MSG_NOT_SUPPORTED,
		STATUS_ERROR_SIGNAL,
		STATUS_ERROR_MINMAX,
		STATUS_ERROR_CLOSE
	};

  class DBSSignal {
    public:
      DBSSignal(std::string name, float offset, float scale, std::string unit);
      float get_scale() const {return scale_;}
      float get_offset() const {return offset_;}
      std::string get_unit() const {return unit_;}
      std::string get_name() const {return name_;}
    private:
      std::string name_;
      float offset_;
      float scale_;
      std::string unit_;
  };


  class DBSMessage {
    public:
      DBSMessage(int can_id, std::string name, int lenght, ssc::DBSSignal signal);
      DBSParserStatus encode_value();
      DBSParserStatus decode_value();
      int get_canid() const { return can_id_;}
      std::string get_name() const {return name_;}
      DBSSignal get_signal() const {return sg_;}

    private:
      int can_id_;
      std::string name_;
      int lenght_;
      DBSSignal sg_;
  };


  class DBSParser {
    public:
      DBSParser();
      DBSParserStatus load(std::string dbs_path);
      DBSParserStatus close();
      ~DBSParser();
      std::vector<std::string> parse_space(std::string raw_line);
      DBSMessage generate_message(std::string msg_raw, std::string signal_raw);
      DBSSignal generate_signal(std::string signal_raw);
      void float_to_candata(float value, ssc::sscan_frame &frame);
      float candata_to_float(ssc::sscan_frame const frame);
      DBSParserStatus encode(std::string msg_name, float value, ssc::sscan_frame &frame);
      DBSParserStatus decode(ssc::sscan_frame const frame);

    private:
      std::ifstream dbs_file_;
      std::vector<DBSMessage> message_list_;
  };

}
