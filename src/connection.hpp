//
// connection.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_CONNECTION_HPP
#define HTTP_CONNECTION_HPP

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <map>
#include <vector>
#include <deque>
#include <string>
#include "response.hpp"
#include "request.hpp"
#include "request_handler_echo.hpp"
#include "request_handler_static.hpp"
#include "request_handler_status.hpp"
#include "request_handler_notfound.hpp"
#include <sstream>

namespace http {
namespace server {

class ConnectionManager;

/// Represents a single connection from a client.
class Connection
  : public boost::enable_shared_from_this<Connection>,
    private boost::noncopyable
{
public:
  /// Construct a connection with the given io_service.
  explicit Connection(boost::asio::io_service& io_service,
    ConnectionManager& manager,
    std::map<std::string, RequestHandler*> handlers,
    RequestHandler* default_handler,
    RequestHandler* status_handler);

  /// Get the socket associated with the connection.
  boost::asio::ip::tcp::socket& socket();

  /// Start the first asynchronous operation for the connection.
  void start();

  /// Stop all asynchronous operations associated with the connection.
  void stop();

private:
  /// Handle a partial reqad request.
  void handleReadPartial(const boost::system::error_code& e,
      std::size_t bytes_transferred);
  /// Handle completion of a read operation.
  void handleRead(const boost::system::error_code& e,
      std::size_t bytes_transferred);

  /// Handle completion of a write operation.
  void handleWrite(const boost::system::error_code& e);

  /// Fill buffer with null bytes
  void clearBuffer();

  //explodes string into vector
  std::deque<std::string> splitUri(std::string uri);

  //decides which handler to use
  RequestHandler* chooseHandler();

  //determines longest matching substring
  int longestSubstring(std::deque<std::string> req, std::deque<std::string> handler);

  /// Socket for the connection.
  boost::asio::ip::tcp::socket socket_;

  /// The manager for this connection.
  ConnectionManager& connection_manager_;

  /// Map of URL paths to Request Handlers.
  std::map<std::string, RequestHandler*> handlers_;

  /// The default request handler if no URL path matches.
  RequestHandler* default_handler_;

  /// The default request handler if no URL path matches.
  RequestHandler* status_handler_;

  /// Buffer for incoming data.
  enum { BUF_SIZE = 8192 };
  boost::asio::streambuf buffer_;

  /// The incoming request.
  std::unique_ptr<Request> request_;

  // string containing http resonse
  std::string response_string;

  // Buffers for write
  std::vector<boost::asio::const_buffer> buffers_;

  /// The response to be sent back to the client.
  Response* response_;
};

typedef boost::shared_ptr<Connection> connection_ptr;

} // namespace server
} // namespace http

#endif // HTTP_CONNECTION_HPP