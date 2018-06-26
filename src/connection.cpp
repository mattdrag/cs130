//
// connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "connection.hpp"
#include <vector>
#include <deque>
#include <string>
#include <algorithm>
#include <iostream>
#include <memory>
#include <boost/bind.hpp>
#include "connection_manager.hpp"
#include "request_handler.hpp"
#include "request.hpp"
#include "response.hpp"
#include "request_handler_status.hpp"
#include <boost/lexical_cast.hpp>
#include <iostream>

namespace http {
namespace server {

Connection::Connection(boost::asio::io_service& io_service,
    ConnectionManager& manager,
    std::map<std::string, RequestHandler*> handlers,
    RequestHandler* default_handler,
    RequestHandler* status_handler)
  : socket_(io_service),
    connection_manager_(manager),
    handlers_(handlers),
    default_handler_(default_handler),
    status_handler_(status_handler)
{
}

boost::asio::ip::tcp::socket& Connection::socket() {
  return socket_;
}



void Connection::start() {
  boost::asio::async_read_until(socket_, buffer_, "\r\n\r\n",
                                boost::bind(&Connection::handleReadPartial, shared_from_this(),
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
}

void Connection::stop() {
  socket_.close();
}

std::deque<std::string> Connection::splitUri(std::string uri) {
  std::deque<std::string> ret;
  std::size_t last_slash_pos;
  std::string token;
  while ((last_slash_pos = uri.find_last_of("/")) != std::string::npos) {
    token = uri.substr(last_slash_pos);
    ret.push_front(token);
    uri.erase(last_slash_pos, last_slash_pos+token.length());
  }
  return ret;
}

int Connection::longestSubstring(std::deque<std::string> req, std::deque<std::string> handler) {
  int ret = 0;
  int upperBound = std::min(req.size(), handler.size());

  for (int i = 0; i < upperBound; i++){
    if (req[i] == handler[i])
      ret++;
    else
      return ret;
  }
  return ret;
}

RequestHandler* Connection::chooseHandler() {
  // Create a response
  response_ = new Response();

  std::deque<std::string> request_uri_tokens = splitUri(request_->uri());

  // Decide which handler to use.
  RequestHandler* best_handler = default_handler_; 

  // Loop through handlers.
  int quality;
  int max = 0;
  for (auto it = handlers_.begin(); it != handlers_.end(); ++it) {
    // Get data from it
    std::string uri_prefix = it->first;
      
    std::deque<std::string> handler_uri_tokens = splitUri(uri_prefix);

    quality = longestSubstring(request_uri_tokens, handler_uri_tokens);
    if (quality > max) {
      max = quality;
      best_handler = it->second;
    }
  }

  // if still equal to default handler, look for root path "/"
  if(best_handler == default_handler_) {
    for (auto it = handlers_.begin(); it != handlers_.end(); ++it) {
      std::string uri_prefix = it->first;
      if(uri_prefix == "/") {
        best_handler = it->second;
        break;
      }
    }
  }

  // Invoke the handler.
  return best_handler;
}

void Connection::handleReadPartial(const boost::system::error_code& e,
    std::size_t bytes_transferred) {

  if (!e) {
    // Get the request.
    std::ostringstream oss;
    oss << &buffer_;
    std::string raw_req = oss.str();

    request_ = Request::Parse(raw_req);
    RequestHandler::Status handler_status;

    // Pick a handler and handle the request.
    if (request_){
      std::size_t contentLength;
      std::string contentLStr = request_.get()->GetContentLength();
      if (contentLStr.empty()){
        contentLength = 0;
      }
      else{
        contentLength = boost::lexical_cast<std::size_t>(contentLStr);
      }

      if (request_.get()->body().length() < contentLength){
        boost::asio::async_read(socket_, buffer_, boost::asio::transfer_exactly(contentLength - request_.get()->body().length()),
                            boost::bind(&Connection::handleRead, shared_from_this(),
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
      }
    }
    
        handler_status = chooseHandler()->HandleRequest(*request_, response_);
        // If request handling failed, invoke NotFound handler
        // (In the future, we could invoke different handlers based on the type of
        // error that occured, but for now just send everything bad to 404!) 
        if (handler_status != RequestHandler::OK) {
          std::cerr << "Error: handler returned status code " << handler_status << ".\n";
          std::cerr << "Invoking default handler.\n";
          default_handler_->HandleRequest(*request_, response_);
          }

    // Let status handler know the result.
    dynamic_cast<StatusHandler*>(status_handler_)->update(request_->uri(), handler_status);

    // Write response to socket.
    response_string = response_->ToString();
    buffers_.push_back(boost::asio::buffer(response_string));
    boost::asio::async_write(socket_, buffers_,
          boost::bind(&Connection::handleWrite, shared_from_this(),
            boost::asio::placeholders::error));
  }

  else if (e != boost::asio::error::operation_aborted) {
    connection_manager_.stop(shared_from_this());
  }

}

void Connection::handleRead(const boost::system::error_code& e,
    std::size_t bytes_transferred) {

  if (!e) {
    // Get the request.
    std::ostringstream oss;
    oss << &buffer_;
    std::string remainingReq = oss.str();

    request_.get()->appendBody(remainingReq);
    RequestHandler::Status handler_status = chooseHandler()->HandleRequest(*request_, response_);
    
    // If request handling failed, invoke NotFound handler
    // (In the future, we could invoke different handlers based on the type of
    // error that occured, but for now just send everything bad to 404!) 
    if (handler_status != RequestHandler::OK) {
      std::cerr << "Error: handler returned status code " << handler_status << ".\n";
      std::cerr << "Invoking default handler.\n";
      default_handler_->HandleRequest(*request_, response_);
    }

    // Let status handler know the result.
    dynamic_cast<StatusHandler*>(status_handler_)->update(request_->uri(), handler_status);

    // Write response to socket.
    response_string = response_->ToString();
    buffers_.push_back(boost::asio::buffer(response_string));
    boost::asio::async_write(socket_, buffers_,
          boost::bind(&Connection::handleWrite, shared_from_this(),
            boost::asio::placeholders::error));
  }
  else if (e != boost::asio::error::operation_aborted) {
    connection_manager_.stop(shared_from_this());
  }

}

void Connection::handleWrite(const boost::system::error_code& e) {
  if (!e) {
    // Initiate graceful connection closure.
    boost::system::error_code ignored_ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
  }

  if (e != boost::asio::error::operation_aborted) {
    connection_manager_.stop(shared_from_this());
  }
}

} // namespace server
} // namespace http