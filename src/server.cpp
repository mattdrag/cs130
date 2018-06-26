//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "server.hpp"
#include <boost/bind.hpp>
#include <signal.h>
#include <iostream>
#include "request_handler_status.hpp"
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

namespace http {
namespace server {

// Given a parsed config file, set the port and handlers
bool Server::parseConfig(std::string& err) {

  // Iterate over statements in parsed config.
  for (unsigned int i = 0; i < config_->statements_.size(); i++) {
    std::shared_ptr<NginxConfigStatement> config_line = config_->statements_.at(i);
    std::vector<std::string> statement = config_line->tokens_;

    // Port
    if (statement.size() == 2 && statement.at(0) == "port") {
      port_ = statement.at(1);
    }

    // Threads
    if (statement.size() == 2 && statement.at(0) == "threads") {
      numThreads_ = std::stoi(statement.at(1));
    }

    // Handlers
    if (statement.size() == 3 && statement.at(0) == "path") {
      // Use registerer to try and create by name
      RequestHandler* handler = RequestHandler::CreateByName(statement.at(2).c_str());

      // nullptr signifies that no handler of that type was registered
      if (handler == nullptr) {
        err = "ERROR: Handler: " + statement.at(2) + " Not Found\n";
        return false;
      }
      else if (handlers_.find(statement.at(1)) != handlers_.end()) {
        err = "ERROR: Duplicate Path: " + statement.at(1) + "\n";
        return false;
      }
      else{
        handler->Init(statement.at(1), *config_line->child_block_);
        handlers_[statement.at(1)] = handler;
        all_handlers.push_back(std::make_pair(statement.at(2), statement.at(1)));
        // Special case: status handler
        if (statement.at(2) == "StatusHandler"){
            status_handler_ = handler;
        }
      }
    }

    // Default
    if (statement.size() == 2 && statement.at(0) == "default") {
      // Use registerer to try and create by name
      default_handler_ = RequestHandler::CreateByName(statement.at(1).c_str());
      // nullptr signifies that no handler of that type was registered
      if (default_handler_ == nullptr) {
        err = "ERROR: Handler: " + statement.at(1) + " Not Found\n";
        return false;
      }
      else{
        default_handler_->Init("", *config_line->child_block_);
        handlers_[""] = default_handler_;
        //all_handlers.push_back(std::make_pair(statement.at(2), statement.at(1)));
        default_handler_name = statement.at(1);
      }
    }
  }

  if (port_ == "") {
    err = "ERROR: Please specify a port.\n";
    return false;
  }

  int port_as_num = std::stoi(port_);
  if (port_as_num > 65535 || port_as_num < 0) {
    err = "ERROR: Port must be a valid number ranging from 0 to 65535.\n";
    return false;
  }

  return true;
}

void Server::printParsedConfig() {
  std::cout << "******** PARSED CONFIG ********\n";
  std::cout << "Port: " << port_ << "\n";
  std::cout << "Threads: " << std::to_string(numThreads_) << "\n";
  for (std::vector<int>::size_type i = 0; i != all_handlers.size(); i++) {
    std::cout << all_handlers[i].first << " " << all_handlers[i].second << "\n";
  }
  std::cout << default_handler_name << " is the default handler\n";
  std::cout << "*******************************" << "\n";
}

Server::Server(const std::string& address, const NginxConfig* config)
  : io_service_(),
    signals_(io_service_),
    acceptor_(io_service_),
    connection_manager_(),
    new_connection_(),
    config_(config) {

  // Parse the config, setting port, numThreads and creating handlers
  std::string err;
  if (!parseConfig(err)) {
    std::cerr << "ERROR: failed to parse config.\n";
    std::cerr << err << "\n";
  }

  printParsedConfig();

  // Check if a status handler was specified 
  if (status_handler_ != nullptr) {
    // Tell status handler about the other handlers
    dynamic_cast<StatusHandler*>(status_handler_)->SetHandlers(all_handlers);
  }

  // Register to handle the signals that indicate when the server should exit.
  // It is safe to register for the same signal multiple times in a program,
  // provided all registration for the specified signal is made through Asio.
  signals_.add(SIGINT);
  signals_.add(SIGTERM);
#if defined(SIGQUIT)
  signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)
  signals_.async_wait(boost::bind(&Server::handleStop, this));

  // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  boost::asio::ip::tcp::resolver resolver(io_service_);
  boost::asio::ip::tcp::resolver::query query(address, port_);
  boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();

  startAccept();
}

void Server::run() {
  // The io_service::run() call will block until all asynchronous operations
  // have finished. While the server is running, there is always at least one
  // asynchronous operation outstanding: the asynchronous accept call waiting
  // for new incoming connections.
  std::vector<boost::shared_ptr<boost::thread>> threadArray;

  for (int i = 0; i < numThreads_; i++){
    boost::shared_ptr<boost::thread> temp(new boost::thread(boost::bind(&boost::asio::io_service::run, &io_service_)));
    threadArray.push_back(temp);
  }

  for (int i = 0; i < numThreads_; i++){
    threadArray[i]->join();
  }
}

void Server::startAccept() {
  new_connection_.reset(new Connection(io_service_,
        connection_manager_, handlers_, default_handler_, status_handler_));
  acceptor_.async_accept(new_connection_->socket(),
      boost::bind(&Server::handleAccept, this,
        boost::asio::placeholders::error));
}

void Server::handleAccept(const boost::system::error_code& e) {
  // Check whether the server was stopped by a signal before this completion
  // handler had a chance to run.
  if (!acceptor_.is_open()) {
    return;
  }

  if (!e) {
    connection_manager_.start(new_connection_);
  }

  startAccept();
}

void Server::handleStop() {
  // The server is stopped by cancelling all outstanding asynchronous
  // operations. Once all operations have finished the io_service::run() call
  // will exit.
  acceptor_.close();
  connection_manager_.stopAll();
}

} // namespace server
} // namespace http
