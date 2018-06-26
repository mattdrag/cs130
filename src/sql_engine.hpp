//
// sql_engine.hpp
//

#include <string>

#ifndef SQL_ENGINE_HPP
#define SQL_ENGINE_HPP



class SqlEngine {
public:
	std::string HandleRequest(std::string& field, int mode);

private:

};

#endif // SQL_ENGINE_HPP