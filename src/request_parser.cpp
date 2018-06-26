//
// request_parser.cpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "request_parser.hpp"
#include "request.hpp"

namespace http {
namespace server {

request_parser::request_parser()
  : state_(method_start)
{
}

void request_parser::reset() {
  state_ = method_start;
}

boost::tribool request_parser::consume(Request& req, char input) {
  switch (state_) {
  case method_start:
    if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
      return false;
    }
    else {
      state_ = method;
      method_.push_back(input);
      return boost::indeterminate;
    }
  case method:
    if (input == ' ') {
      state_ = uri;
      return boost::indeterminate;
    }
    else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
      return false;
    }
    else {
      method_.push_back(input);
      return boost::indeterminate;
    }
  case uri:
    if (input == ' ') {
      state_ = http_version_h;
      return boost::indeterminate;
    }
    else if (is_ctl(input)) {
      return false;
    }
    else {
      uri_.push_back(input);
      return boost::indeterminate;
    }
  case http_version_h:
    if (input == 'H') {
      state_ = http_version_t_1;
      return boost::indeterminate;
    }
    else {
      return false;
    }
  case http_version_t_1:
    if (input == 'T') {
      state_ = http_version_t_2;
      return boost::indeterminate;
    }
    else {
      return false;
    }
  case http_version_t_2:
    if (input == 'T') {
      state_ = http_version_p;
      return boost::indeterminate;
    }
    else {
      return false;
    }
  case http_version_p:
    if (input == 'P') {
      state_ = http_version_slash;
      return boost::indeterminate;
    }
    else {
      return false;
    }
  case http_version_slash:
    if (input == '/') {
      http_version_major_ = 0;
      http_version_minor_ = 0;
      state_ = http_version_major_start;
      return boost::indeterminate;
    }
    else {
      return false;
    }
  case http_version_major_start:
    if (is_digit(input)) {
      http_version_major_ = http_version_major_ * 10 + input - '0';
      state_ = http_version_major;
      return boost::indeterminate;
    }
    else {
      return false;
    }
  case http_version_major:
    if (input == '.') {
      state_ = http_version_minor_start;
      return boost::indeterminate;
    }
    else if (is_digit(input)) {
      http_version_major_ = http_version_major_ * 10 + input - '0';
      return boost::indeterminate;
    }
    else {
      return false;
    }
  case http_version_minor_start:
    if (is_digit(input)) {
      http_version_minor_ = http_version_minor_ * 10 + input - '0';
      state_ = http_version_minor;
      return boost::indeterminate;
    }
    else {
      return false;
    }
  case http_version_minor:
    if (input == '\r') {
      state_ = expecting_newline_1;
      return boost::indeterminate;
    }
    else if (is_digit(input)) {
      http_version_minor_ = http_version_minor_ * 10 + input - '0';
      return boost::indeterminate;
    }
    else {
      return false;
    }
  case expecting_newline_1:
    if (input == '\n') {
      state_ = header_line_start;
      return boost::indeterminate;
    }
    else {
      return false;
    }
  case header_line_start:
    if (input == '\r') {
      state_ = expecting_newline_3;
      return boost::indeterminate;
    }
    else if (!headers.empty() && (input == ' ' || input == '\t')) {
      state_ = header_lws;
      return boost::indeterminate;
    }
    else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
      return false;
    }
    else {
      headers.push_back(header());
      headers.back().name.push_back(input);
      state_ = header_name;
      return boost::indeterminate;
    }
  case header_lws:
    if (input == '\r') {
      state_ = expecting_newline_2;
      return boost::indeterminate;
    }
    else if (input == ' ' || input == '\t') {
      return boost::indeterminate;
    }
    else if (is_ctl(input)) {
      return false;
    }
    else {
      state_ = header_value;
      headers.back().value.push_back(input);
      return boost::indeterminate;
    }
  case header_name:
    if (input == ':') {
      state_ = space_before_header_value;
      return boost::indeterminate;
    }
    else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
      return false;
    }
    else {
      headers.back().name.push_back(input);
      return boost::indeterminate;
    }
  case space_before_header_value:
    if (input == ' ') {
      state_ = header_value;
      return boost::indeterminate;
    }
    else {
      return false;
    }
  case header_value:
    if (input == '\r') {
      state_ = expecting_newline_2;
      return boost::indeterminate;
    }
    else if (is_ctl(input)) {
      return false;
    }
    else {
      headers.back().value.push_back(input);
      return boost::indeterminate;
    }
  case expecting_newline_2:
    if (input == '\n') {
      state_ = header_line_start;
      return boost::indeterminate;
    }
    else {
      return false;
    }
  case expecting_newline_3:
    return (input == '\n');
  default:
    return false;
  }
}

bool request_parser::is_char(int c) {
  return c >= 0 && c <= 127;
}

bool request_parser::is_ctl(int c) {
  return (c >= 0 && c <= 31) || (c == 127);
}

bool request_parser::is_tspecial(int c) {
  switch (c) {
  case '(': case ')': case '<': case '>': case '@':
  case ',': case ';': case ':': case '\\': case '"':
  case '/': case '[': case ']': case '?': case '=':
  case '{': case '}': case ' ': case '\t':
    return true;
  default:
    return false;
  }
}

bool request_parser::is_digit(int c) {
  return c >= '0' && c <= '9';
}

} // namespace server
} // namespace http