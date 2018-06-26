//
// connection_manager.cpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "connection_manager.hpp"
#include <algorithm>
#include <boost/bind.hpp>

namespace http {
namespace server {

void ConnectionManager::start(connection_ptr c) {
  connections_.insert(c);
  c->start();
}

void ConnectionManager::stop(connection_ptr c) {
  connections_.erase(c);
  c->stop();
}

void ConnectionManager::stopAll() {
  std::for_each(connections_.begin(), connections_.end(),
      boost::bind(&Connection::stop, _1));
  connections_.clear();
}

} // namespace server
} // namespace http