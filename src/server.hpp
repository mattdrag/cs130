//
// server.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <boost/asio.hpp>
#include <string>
#include <boost/noncopyable.hpp>
#include "connection.hpp"
#include "connection_manager.hpp"
#include "request_handler_echo.hpp"
#include "request_handler_static.hpp"
#include "request_handler_notfound.hpp"

namespace http {
namespace server {

/// The top-level class of the HTTP server.
class Server
  : private boost::noncopyable
{
public:
  /// Construct the server to listen on the specified TCP address and port
  /// Serve static files from the directory specified by static_file_root
  explicit Server(const std::string& address, const NginxConfig* config);

  // Parses config
  bool parseConfig(std::string& err);

  // Prints out config specs
  void printParsedConfig();

  /// Run the server's io_service loop.
  void run();

private:
  /// Initiate an asynchronous accept operation.
  void startAccept();

  /// Handle completion of an asynchronous accept operation.
  void handleAccept(const boost::system::error_code& e);

  /// Handle a request to stop the server.
  void handleStop();

  /// The io_service used to perform asynchronous operations.
  boost::asio::io_service io_service_;

  /// The signal_set is used to register for process termination notifications.
  boost::asio::signal_set signals_;

  /// Acceptor used to listen for incoming connections.
  boost::asio::ip::tcp::acceptor acceptor_;

  /// The connection manager which owns all live connections.
  ConnectionManager connection_manager_;

  /// The next connection to be accepted.
  connection_ptr new_connection_;

  /// Map of URL paths to Request Handlers.
  std::map<std::string, RequestHandler*> handlers_;

  // Pairs of handler names and their paths
  std::vector<std::pair<std::string, std::string>> all_handlers;

  /// The status handler 
  RequestHandler* status_handler_;

  /// The default request handler if no URL path matches.
  std::string default_handler_name;
  RequestHandler* default_handler_;

  /// The port on which the server is running
  std::string port_;

  // Threads
  int numThreads_;

  //pointer to config
  const NginxConfig* config_;
};

} // namespace server
} // namespace http

#endif // HTTP_SERVER_HPP