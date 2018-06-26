//
// http_client.hpp
// ~~~~~~~~~~~~~~~~~~~
//

#ifndef HTTP_CLIENT_HPP
#define HTTP_CLIENT_HPP

#include "response.hpp"
#include "request.hpp"

#include <string>
#include <memory>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace http {
namespace server {

class HttpClient {
public:
	bool EstablishConnection(const std::string& host, const std::string& port);
	Response* SendRequest(const Request& req);
private:
	boost::asio::ip::tcp::socket* socket_;
};

} // namespace server
} // namespace http

#endif // HTTP_CLIENT_HPP