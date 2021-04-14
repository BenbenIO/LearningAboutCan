/*
 * Very simplified dbs file parser to illustrate principle.
 * Benjamin, 2021/04
 * Missing: canID convertion, endianness writing, lenght and start writing.
 * Using: fixed can_id.
*/

#include "DBSparser.h"

namespace ssc {
  // Parser implementation:
  DBSParser::DBSParser() {
    spdlog::info("DBSparser created.");
  }

  DBSParserStatus DBSParser::load(std::string dbs_path) {
    spdlog::info("Loading {}", dbs_path);
    std::ifstream dbs_file_(dbs_path);
    std::string line;
    std::string next_line;
    spdlog::info("DBS file content:");
    while (std::getline(dbs_file_, line)) {
      if(line.find("BO_") != std::string::npos) {
        //get next line and check if signal:
        std::getline(dbs_file_, next_line);
        if(next_line.find("SG_") != std::string::npos) {
          auto msg = generate_message(line, next_line);

          // Check message contant:
          auto canid = msg.get_canid();
          auto name = msg.get_name();
          auto signal = msg.get_signal();
          auto sg_name = signal.get_name();
          auto sg_offset = signal.get_offset();
          auto sg_scale = signal.get_scale();
          auto sg_unit = signal.get_unit();
          spdlog::info("Extracted signal: {}({}) : {} [{}](scale: {} + offset: {})",
                        name, canid, sg_name, sg_unit, sg_scale, sg_offset);

          // Append to message list:
          message_list_.push_back(msg);
        }
      }
    }
    return ssc::DBSParserStatus::STATUS_OK;
  }

  std::vector<std::string> DBSParser::parse_space(std::string raw_line) {
    std::vector<std::string> tokens;
    std::string delimiter = " ";
    size_t pos = 0;
    std::string token;
    while ((pos = raw_line.find(delimiter)) != std::string::npos) {
      token = raw_line.substr(0, pos);
      tokens.push_back(token);
      raw_line.erase(0, pos + delimiter.length());
    }
    tokens.push_back(raw_line);

    return tokens;
  }

  DBSMessage DBSParser::generate_message(std::string msg_raw, std::string signal_raw) {
    // Parse msg_raw to extarct ID, name, lenght
    auto token_msg = parse_space(msg_raw);
    unsigned int msg_id = std::stoul(token_msg[1], nullptr, 16);
    auto name = token_msg[2];
    int lenght = std::stoi(token_msg[3]);

    // generate_signal with signal_raw
    auto signal = generate_signal(signal_raw);

    // Generate and return message class
    //spdlog::info("name: {} l: {} id: {}", name, msg_id, lenght);
    ssc::DBSMessage extracted(msg_id, name, lenght, signal);
    return extracted;
  }

  DBSSignal DBSParser::generate_signal(std::string signal_raw) {
    // Parse signal_raw to extract name, offset, scale, min-max, unit
    auto token_sg = parse_space(signal_raw);
    auto name = token_sg[2];
    auto endian_raw = token_sg[4];
    auto unit = token_sg[7];
    auto minmax_raw = token_sg[6];
    auto scaleoffset_raw = token_sg[5];

    // Raw extract offset and scale:
    auto pos = scaleoffset_raw.find(",");
    auto scale_str = scaleoffset_raw.substr(0, pos).erase(0,1);
    auto offset_str = scaleoffset_raw.erase(0, pos + 1).erase(scaleoffset_raw.size() -1);
    float scale = std::stof(scale_str);
    float offset = std::stof(offset_str);

    // Generate and return signal object.
    //spdlog::info("name: {} off: {} scale: {}, unit: {}", name, offset, scale, unit);
    ssc::DBSSignal extracted(name, offset, scale, unit);
    return extracted;
  }

  DBSParserStatus DBSParser::close(){
    //closing file.
    dbs_file_.close();
    return ssc::DBSParserStatus::STATUS_OK;
  }

  DBSParser::~DBSParser() {
    close();
    spdlog::info("DBSparser destroyed.");
  }

  void DBSParser::float_to_candata(float value, ssc::sscan_frame &frame) {
    uint8_t *sends;
  	sends = reinterpret_cast<uint8_t*>(&value);
    std::memcpy(frame.data, sends, sizeof(float));
  }

  float DBSParser::candata_to_float(ssc::sscan_frame const frame){
    float value;
  	std::memcpy(&value, frame.data, sizeof(float));
  	return value;
  }

  DBSParserStatus DBSParser::encode(std::string msg_name, float value, ssc::sscan_frame &frame) {
    // 1- Search if message in message_list_:
    auto canid = message_list_[0].get_canid();
    auto name = message_list_[0].get_name();
    auto signal = message_list_[0].get_signal();
    auto sg_name = signal.get_name();
    auto sg_offset = signal.get_offset();
    auto sg_scale = signal.get_scale();
    auto sg_unit = signal.get_unit();

    // 2- Encode message based on message'signal:
    auto encoded_value = (value * sg_scale) + sg_offset;

    // 3- write id / lenght / data to the frame:
    frame.can_id = 0x555;
  	frame.can_dlc = 8;
  	float_to_candata(encoded_value, frame);
    return ssc::DBSParserStatus::STATUS_OK;
  }

  DBSParserStatus DBSParser::decode(ssc::sscan_frame const frame){
    // 1- Extract canID and search if message in message_list_:
    auto id_to_search = frame.can_id;

    auto canid = message_list_[0].get_canid();
    auto name = message_list_[0].get_name();
    auto signal = message_list_[0].get_signal();
    auto sg_name = signal.get_name();
    auto sg_offset = signal.get_offset();
    auto sg_scale = signal.get_scale();
    auto sg_unit = signal.get_unit();

    // 2- Extract value from canframe:
    auto data_raw = candata_to_float(frame);

    // 3- Decode value based on message'signal:
    auto decoded_data = (data_raw / sg_scale) - sg_offset;

    // 4- Print out result:
    spdlog::info("Decoded: {} : {} {}", sg_name, decoded_data, sg_unit);
    return ssc::DBSParserStatus::STATUS_OK;
  }

  // DBSSignal ctor:
  DBSSignal::DBSSignal(std::string name, float offset, float scale, std::string unit) {
    name_ = name;
    offset_ = offset;
    scale_ = scale;
    unit_ = unit;
  }

  // DBSMessage ctor:
  DBSMessage::DBSMessage(int can_id, std::string name, int lenght, DBSSignal signal) : sg_(signal) {
    can_id_ = can_id;
    name_ = name;
    lenght_ = lenght;
    sg_ = signal;
  }

}
