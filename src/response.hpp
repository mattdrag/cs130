//
// response.hpp
// ~~~~~~~~~
//
// Represents an HTTP response.
//
// Usage:
//   Response r;
//   r.SetStatus(RESPONSE_200);
//   r.SetBody(...);
//   return r.ToString();
//
// Constructed by the RequestHandler, after which the server should call ToString
// to serialize.

#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include <string>
#include <vector>
#include <boost/asio.hpp>

namespace http {
namespace server {

class Response {
 public:
  enum ResponseCode {
    OK = 200,
    FOUND = 302,
    BAD_REQUEST = 400,
    NOT_FOUND = 404,
    INTERNAL_SERVER_ERROR = 500
  };

  static Response* Parse(const std::string& raw_res);
  ResponseCode GetStatus();
  bool GetRedirectHostAndPath(std::string& host, std::string& path, std::string& protocol);
  void SetStatus(const ResponseCode response_code);
  void AddHeader(const std::string& header_name, const std::string& header_value);
  void SetBody(const std::string& body);
  // void AddRelativePaths(const std::string& uri_prefix); // Not implemented due to errors

  std::string ToString();

private:
  // parse first line of the request
  bool parse_first_line(const std::string& line);

  // parse entire raw response and update private member variables
  bool parse_raw_response(const std::string& raw_res);

  ResponseCode IntToResponseCode(int code);

  ResponseCode status_;
  std::vector<std::pair<std::string, std::string>> headers_;
  std::string body_;
};

} // namespace server
} // namespace http

#endif // HTTP_RESPONSE_HPP
