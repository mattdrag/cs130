//
// request.cpp
// ~~~~~~~~~
//

#include "request.hpp"
#include "request_parser.hpp"
#include <string>
#include <memory>

namespace http {
namespace server {

std::unique_ptr<Request> Request::Parse(const std::string& raw_request) {

  // Use auxiliary class request_parser to perform parsing.
  request_parser request_parser_;
  Request *parsed_request = new Request();
  boost::tribool result;
  boost::tie(result, boost::tuples::ignore) = request_parser_.parse(*parsed_request, raw_request.c_str(), raw_request.c_str() + raw_request.length());

  //TODO: use result in some useful way

  // Get useful information from parser
  parsed_request->raw_request_ = raw_request;
  parsed_request->method_ = request_parser_.getMethod();
  parsed_request->uri_ = request_parser_.getUri();
  parsed_request->version_ = request_parser_.getVersion();
  parsed_request->headers_ = request_parser_.getHeaders();

  // Unique pointers are fun
  std::unique_ptr<Request> pointer_to_req(parsed_request);

  // Return a pointer, not object
  return pointer_to_req;
}

std::string Request::raw_request() const {
  return raw_request_;
}

std::string Request::method() const {
  return method_;
}

std::string Request::uri() const {
  return uri_;
}

std::string Request::version() const {
  return version_;
}

Request::Headers Request::headers() const {
  return headers_;
}

std::string Request::body() const {
  return body_;
}

std::string Request::ToString() const {
  std::string request_string;

  // Prepare the first line
  request_string.append(method_);
  request_string.append(" ");
  request_string.append(uri_);
  request_string.append(" ");
  request_string.append(version_);
  request_string.append("\r\n");

  // Attach the headers
  for(auto const& header : headers_) {
    request_string.append(header.first);
    request_string.append(": " );
    request_string.append(header.second);
    request_string.append("\r\n");
  }

  // Beginning of Body
  request_string.append("\r\n");
  request_string.append(body_);

  return request_string;
}

void Request::set_header(std::string first, std::string second) {
  for(auto& header : headers_) {
    if(header.first == first) {
      // Found existing header, so set and return
      header.second = second;
      return;
    }
  }
  // Header not set, so make new
  headers_.push_back(std::make_pair(first, second));
}

void Request::remove_header(std::string key) {
  unsigned int i = 0;
  for(; i < headers_.size(); i++) {
      if (headers_[i].first == key)
        break;
  }

  if(i != headers_.size()) {
    headers_.erase(headers_.begin()+i);
  }
  return;
}

void Request::set_uri(std::string uri) {
  uri_ = uri;
}

} // namespace server
} // namespace http
