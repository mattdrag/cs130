//
// http_client.cpp
// ~~~~~~~~~~~~~~~~~~~
//

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "http_client.hpp"
#include "response.hpp"
#include "request.hpp"

namespace http {
namespace server {

bool HttpClient::EstablishConnection(const std::string& host, const std::string& port) {
  boost::asio::io_service io_service;

  // Get a list of endpoints corresponding to the server name.
  tcp::resolver resolver(io_service);
  tcp::resolver::query query(host, port);
  tcp::resolver::iterator endpoint_iterator = resolver.resolve(query), end;

  // Try each endpoint until we successfully establish a connection.
  boost::system::error_code error;
  socket_ = new boost::asio::ip::tcp::socket(io_service);

  boost::asio::connect(*socket_, endpoint_iterator, end, error);

  if(error) {
    //error_code means binding the socket failed
    std::cerr << "Unable to bind socket to website " + host + " for " + port << std::endl;
    return false;
  }

  return true;
}

Response* HttpClient::SendRequest(const Request& req) {
  // Form the request from the req string
  boost::asio::streambuf request;
  std::ostream request_stream(&request);
  request_stream << req.ToString();

  // Send the request.
  boost::asio::write(*socket_, request);

  // Read the response back and store in Response object
  boost::asio::streambuf raw_response_stream;
  boost::system::error_code error;

  std::string raw_response_string;
  std::size_t bytes_read;
  while((bytes_read = boost::asio::read(*socket_, raw_response_stream, boost::asio::transfer_at_least(1), error))) {
    // Read the values from the buffer (all bytes read)
    std::string read_data = std::string(boost::asio::buffers_begin(raw_response_stream.data()),
                      boost::asio::buffers_begin(raw_response_stream.data()) + bytes_read);
    raw_response_string += read_data;
    // Remove the bytes read from the buffer
    raw_response_stream.consume(bytes_read);
  }


  if(error != boost::asio::error::eof) {
    // Error reading.
    std::cerr << "Reading failed." << std::endl;
    return nullptr;
  }

  auto resp = Response::Parse(raw_response_string);
  return resp;
}

} // namespace server
} // namespace http
