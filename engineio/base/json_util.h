#include <sstream>
#include <map>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;

void Json2Map(std::string& json, std::map<std::string, std::string>& m) {
  ptree pt;
  std::istringstream is(json);
  read_json(is, pt);
  for (ptree::const_iterator it = pt.begin(); it != pt.end(); it ++) {
    m[it->first] = it->second;
  }
}

std::string Map2json (const std::map<std::string, std::string>& m) {
  ptree pt;
  std::map<std::string, std::string>::const_iterator it;
  for (it = m.begin(); it != m.end(); it ++) {
    pt.put(it->first, it->second);
  }
  std::ostringstream buf; 
  write_json (buf, pt, false); 
  return buf.str();
}
