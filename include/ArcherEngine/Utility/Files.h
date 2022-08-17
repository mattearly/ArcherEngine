#pragma once
#include <fstream>
#include <sstream>
#include <memory>
#include <string>
namespace AA {
namespace Files {

inline void ReadToString(const char* filepath, std::string& out) {
  std::ifstream input_file_stream(filepath);
  if (!input_file_stream.is_open())
    throw("invalid file path");
  std::stringstream data_stream;
  data_stream << input_file_stream.rdbuf();
  out = data_stream.str();
}

inline std::string ReadToString(const char* filepath) {
  std::ifstream input_file_stream(filepath);
  if (!input_file_stream.is_open())
  {
    throw("invalid file path");
  }
  std::stringstream data_stream;
  data_stream << input_file_stream.rdbuf();
  return data_stream.str();
}

inline const char* extract_prog_name(const char* full) {
  std::string p1 = full;
  static std::string p2;
  p2.clear();
  p2 = p1.substr(
    p1.find_last_of("/\\") + 1,  // from after last slash
    (p1.length() - p1.find_last_of("/\\")) - (p1.length() - (p1.find_last_of(".")) + 1)  // up to last dot
  );
  return p2.c_str();
}

#ifdef _DEBUG
inline void write_log(const char* msg) {
  std::ofstream logs;
  logs.open("our_log.txt", std::ofstream::app | std::ofstream::out);
  logs << msg << '\n';
  logs.close();
}

#else

inline void write_log(const char* msg) {}

#endif

}  // end namespace Files
}  // end namespace AA
