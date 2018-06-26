//
// response.cpp
// ~~~~~~~~~
//

#include "response.hpp"
#include <string>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

namespace http {
namespace server {

Response::ResponseCode Response::GetStatus() {
  return status_;
}

bool Response::GetRedirectHostAndPath(std::string& host, std::string& path, std::string& protocol) {
  std::string location = "";
  unsigned int i = 0;
  for (; i < headers_.size(); ++i) {
    if (headers_[i].first == "Location") {
      location = headers_[i].second;
      break;
    }
  }
  if (i == headers_.size()) {
    std::cout << "Redirect location failed to be found" << std::endl;
    return false;
  }

  // ignoring possibly https://, everything before '/' character is the host, everything after is the path
  size_t double_slash_index = location.find("//");
  size_t slash_index;

  // search after first occurence of '//' if it exists
  if(double_slash_index == std::string::npos) {
    slash_index = location.find('/');
    std::cout << "No protocol specified in Location field, default to http." << std::endl;
    protocol = "http";
  }
  else {
    protocol = location.substr(0, double_slash_index-1); //before the colon as well
    slash_index = location.find('/', double_slash_index+2);
  }

  // '/' not found
  if (slash_index == std::string::npos) {
    std::cout << "could not find slash to separate host and path." << std::endl;
    return false;
  }

  host = location.substr(double_slash_index+2, slash_index-double_slash_index-2);
  path = location.substr(slash_index);

  std::cout << "Redirecting, with: Protocol=" << protocol << ", Host=" << host << ", Path=" << path << std::endl;

  return true;
}

void Response::SetStatus(const ResponseCode response_code) {
  status_ = response_code;
}

void Response::AddHeader(const std::string& header_name, const std::string& header_value) {
  headers_.push_back(std::make_pair(header_name, header_value));
}

void Response::SetBody(const std::string& body) {
  body_ = body;
}

std::string Response::ToString() {
  std::string response_string = "";
  const std::string carriage_return = "\r\n";

  // Response code
  switch(status_) {
    case OK:
      response_string += "HTTP/1.1 200 OK" + carriage_return;
      break;
    case FOUND:
      response_string += "HTTP/1.1 302 Found" + carriage_return;
      break;
    case BAD_REQUEST:
      response_string += "HTTP/1.1 400 Bad Request" + carriage_return;
      break;
    case NOT_FOUND:
      response_string += "HTTP/1.1 404 Not Found" + carriage_return;
      break;
    default:
      response_string += "HTTP/1.1 500 Internal Server Error" + carriage_return;
      break;
  }

  // HTTP headers
  for (auto it = headers_.begin(); it != headers_.end(); it++) {
    response_string += it->first + ": " + it->second + carriage_return;
  }
  response_string += carriage_return;

  // Response content
  response_string += body_;

  return response_string;
}

Response* Response::Parse(const std::string& raw_res)
{
  Response* response = new Response();
  if(response->parse_raw_response(raw_res))
  {
    return response;
  }
  else
  {
    return nullptr;
  }
}

//parse the first line of the response
bool Response::parse_first_line(const std::string& line) {
  std::vector<std::string> tokens;
  boost::split(tokens, line, boost::is_any_of(" "));

  unsigned int expected_num_of_tokens = 2;
  if(tokens.size() <= expected_num_of_tokens)
    return false;

  int response_code;
  try {
    response_code = boost::lexical_cast<int>(tokens[1]);
  }
  catch (const boost::bad_lexical_cast&) {
    return false;
  }
  status_ = IntToResponseCode(response_code);

  return true;
}

Response::ResponseCode Response::IntToResponseCode(int code) {
  switch(code) {
    case 200:
      return ResponseCode::OK;
    case 302:
      return ResponseCode::FOUND;
    case 400:
      return ResponseCode::BAD_REQUEST;
    case 404:
      return ResponseCode::NOT_FOUND;
    case 500:
      return ResponseCode::INTERNAL_SERVER_ERROR;
    default:
      return ResponseCode::NOT_FOUND;
  }
}

//parse the entire raw response and update the private member variables
bool Response::parse_raw_response(const std::string& res) {
  std::vector<std::string> lines;

  //separate the request body, denoted by \r\n\r\n.
  //if there is any content after, update value of body_.
  size_t end_fields_index = res.find("\r\n\r\n");

  if(end_fields_index == std::string::npos)
  {
    std::cerr << "error in raw response: couldn't find the end of the response fields preceding response body" << std::endl;
    return false;
  }
  size_t begin_body_index = end_fields_index + 4; //Add 4 to skip to the body.
  if(begin_body_index < res.size())
    body_ = res.substr(begin_body_index);

  //truncate res to everything before the \r\n\r\n
  std::string new_res = res.substr(0, end_fields_index + 1);

  //split raw resuest based on /r/n
  //boost::split(lines, res, boost::is_any_of("\n"));
  boost::regex re("(\r\n)+");
  boost::sregex_token_iterator it(new_res.begin(), new_res.end(), re, -1);
  boost::sregex_token_iterator j;
  while(it != j)
    lines.push_back(*it++);


  //parse the method and path separately, return 0 if it fails
  if(!parse_first_line(lines[0]))
  {
    std::cerr << "error in raw response format: first line not exactly 3 tokens or response code not an integer" << std::endl;
    return false;
  }
  //populate the map with the fields
  for(unsigned int i = 1; i < lines.size(); ++i)
  {
    std::size_t index = lines[i].find(": ");
    if(index == std::string::npos)
    {
      std::cerr << "error in raw response format: field line doesn't contain a colon character" << std::endl;
      return false;
    }
    std::string key = lines[i].substr(0, index);
    std::string value = lines[i].substr(index+2); //add 2 to skip the ": "

    std::pair<std::string, std::string> field_pair(key, value);
    headers_.push_back(field_pair);
  }

  return true;
}

} // namespace server
} // namespace http
