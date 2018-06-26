//
// request_handler_static.cpp
// ~~~~~~~~~~~~~~~~~~~
//

#include <iostream>
#include "request_handler_static.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <boost/lexical_cast.hpp>
#include "mime_types.hpp"
#include "response.hpp"
#include "request.hpp"
#include "../cpp-markdown/markdown.h"

namespace http {
namespace server {

RequestHandler::Status StaticHandler::Init(const std::string& uri_prefix, const NginxConfig& config) {
  uri_prefix_ = uri_prefix;
  std::vector<std::shared_ptr<NginxConfigStatement>> statements = config.statements_;

  // Get root directory from config.
  if (statements.size() == 1) {
    std::vector<std::string> statement_tokens = statements.at(0)->tokens_;
    if (statement_tokens.size() == 2 && statement_tokens.at(0) == "root") {
      root_ = statement_tokens.at(1);
      return RequestHandler::OK;
    }
  }

  return RequestHandler::BAD_CONFIG;
}

// Serve the static file that is requested.
RequestHandler::Status StaticHandler::HandleRequest(const Request& request, Response* response) {

  // Decode url to path.
  std::string request_string;
  if (!url_decode(request.uri(), request_string)) {
    response->SetStatus(Response::BAD_REQUEST);
    return RequestHandler::BAD_REQUEST;
  }

  // If path ends in slash (i.e. is a directory) then add "index.html".
  if (request_string[request_string.size() - 1] == '/') {
    request_string += "index.html";
  }

  // Determine the file extension and file name
  std::size_t last_slash_pos = request_string.find_last_of("/");
  std::size_t last_dot_pos = request_string.find_last_of(".");
  std::string extension;
  if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos) {
    extension = request_string.substr(last_dot_pos + 1);
  }

  std::string file_name = request_string.substr(last_slash_pos);

  // Request path must be absolute and not contain "..".
  if (file_name.empty() || file_name[0] != '/'
      || file_name.find("..") != std::string::npos) {
    return RequestHandler::BAD_REQUEST;
  }

  // TODO: dont open full path, only the file that we are asking for
  // Open the file to send back.
  std::string full_path = root_ + file_name;

  std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
  if (!is) {
    response->SetStatus(Response::NOT_FOUND);
    return RequestHandler::NOT_FOUND;
  }

  // Fill out the response to be sent to the client.
  response->SetStatus(Response::OK);
  char buf[512];
  std::string file_content = "";
  while (is.read(buf, sizeof(buf)).gcount() > 0) {
    file_content.append(buf, is.gcount());
  }

  // Convert markdown files to HTML
  if (extension == "md") {
    markdown::Document doc;
    doc.read(file_content);
    std::ostringstream stream;
    doc.write(stream);
    file_content = stream.str();
  }

  response->SetBody(file_content);
  response->AddHeader("Content-Length", boost::lexical_cast<std::string>(file_content.length()));
  response->AddHeader("Content-Type", mime_types::extension_to_type(extension));
  return RequestHandler::OK;
}

bool StaticHandler::url_decode(const std::string& in, std::string& out) {
  out.clear();
  out.reserve(in.size());
  for (std::size_t i = 0; i < in.size(); ++i) {
    if (in[i] == '%') {
      if (i + 3 <= in.size()) {
        int value = 0;
        std::istringstream is(in.substr(i + 1, 2));
        if (is >> std::hex >> value) {
          out += static_cast<char>(value);
          i += 2;
        }
        else {
          return false;
        }
      }
      else {
        return false;
      }
    }
    else if (in[i] == '+') {
      out += ' ';
    }
    else {
      out += in[i];
    }
  }
  std::cout << in << std::endl;
  std::cout << out << std::endl;
  return true;
}

REGISTER_REQUEST_HANDLER(StaticHandler);

} // namespace server
} // namespace http
