# Building the server
CXXFLAGS=-std=c++11 -Wall 
SRC_FILES=src/*.cpp nginx-configparser/config_parser.cc cpp-markdown/markdown.cpp cpp-markdown/markdown-tokens.cpp
SQL_FLAGS=-I/usr/include/cppconn -L/usr/lib -lmysqlcppconn -I/usr/include/mysql -DBIG_JOINS=1  -fno-strict-aliasing -g -DNDEBUG -L/usr/lib/x86_64-linux-gnu -lmysqlclient -lpthread -lz -lm -ldl -static

# Running unit tests
GTEST_PATH=nginx-configparser/googletest/googletest
GMOCK_PATH=nginx-configparser/googletest/googlemock
GTEST_IMPORT=-I$(GTEST_PATH)/include -I./src $(GTEST_PATH)/src/gtest_main.cc
GMOCK_IMPORT=-I$(GMOCK_PATH)/include
TEST_DIR=test
TEST_FILES=$(TEST_DIR)/*.cpp
BOOST_FLAGS=-static-libgcc -static-libstdc++ -lpthread -Wl,-Bstatic -lboost_system -lboost_regex -lboost_thread
RESULTS_TEST_DIR=results-unit-tests
RESULTS_COVERAGE_DIR=results-coverage

# Test file dependencies
REQUEST_HANDLER_ECHO_DEPENDENCIES=src/request_handler_echo.cpp src/response.cpp src/request.cpp src/request_parser.cpp src/request_handler.cpp
REQUEST_HANDLER_STATIC_DEPENDENCIES=src/request_handler_static.cpp src/mime_types.cpp src/response.cpp src/request.cpp src/request_parser.cpp \
src/request_handler.cpp cpp-markdown/markdown.cpp cpp-markdown/markdown-tokens.cpp
REQUEST_HANDLER_NOTFOUND_DEPENDENCIES=src/request_handler_notfound.cpp src/response.cpp src/request.cpp src/request_parser.cpp src/request_handler.cpp
REQUEST_HANDLER_SQL_DEPENDENCIES=src/request_handler_sql.cpp src/sql_engine.cpp src/mime_types.cpp src/request.cpp src/response.cpp src/request_handler.cpp src/request_parser.cpp
SERVER_DEPENDENCIES=src/server.cpp src/connection.cpp src/connection_manager.cpp src/response.cpp src/request.cpp src/request_handler_static.cpp \
src/request_handler_echo.cpp src/request_handler_notfound.cpp src/request_handler_status.cpp src/request_parser.cpp src/mime_types.cpp \
src/request_handler.cpp cpp-markdown/markdown.cpp cpp-markdown/markdown-tokens.cpp
REQUEST_PARSER_DEPENDENCIES=src/request_parser.cpp
RESPONSE_DEPENDENCIES=src/response.cpp
REQUEST_DEPENDENCIES=src/request.cpp src/request_parser.cpp
CONNECTION_DEPENDENCIES=src/connection.cpp src/connection_manager.cpp src/request_handler.hpp
HTTP_CLIENT_DEPENDENCIES=src/http_client.cpp src/request.cpp src/response.cpp src/request_parser.cpp
PROXY_HANDLER_DEPENDENCIES=src/request_handler_proxy.cpp src/request_handler.cpp src/request.cpp src/request_parser.cpp src/http_client.cpp src/response.cpp nginx-configparser/config_parser.cc
SQL_ENGINE_DEPENDENCIES=src/sql_engine.cpp

all: webserver

webserver: $(SRC_FILES)
	g++ $(CXXFLAGS) -I. $(SRC_FILES) -o webserver $(BOOST_FLAGS) $(SQL_FLAGS)

docker:
	rm -f deploy/webserver
	docker build -t httpserver.build .
	docker run httpserver.build > deploy/binary.tar
	cd deploy && tar -xvf binary.tar
	rm -f deploy/binary.tar
	docker build -t httpserver deploy
	docker run --net=host --rm -t -p 8080:8080 httpserver

aws:
	./aws_deploy.sh

test:
	g++ $(CXXFLAGS) $(GTEST_IMPORT) \
	$(TEST_DIR)/request_handler_echo_test.cpp $(REQUEST_HANDLER_ECHO_DEPENDENCIES) \
	$(TEST_DIR)/libgtest.a -o request_handler_echo_test $(BOOST_FLAGS)

	g++ $(CXXFLAGS) $(GTEST_IMPORT) \
	$(TEST_DIR)/request_handler_static_test.cpp $(REQUEST_HANDLER_STATIC_DEPENDENCIES) \
	$(TEST_DIR)/libgtest.a -o request_handler_static_test $(BOOST_FLAGS)

	g++ $(CXXFLAGS) $(GTEST_IMPORT) \
	$(TEST_DIR)/request_handler_notfound_test.cpp $(REQUEST_HANDLER_NOTFOUND_DEPENDENCIES) \
	$(TEST_DIR)/libgtest.a -o request_handler_notfound_test $(BOOST_FLAGS)

	g++ $(CXXFLAGS) $(GTEST_IMPORT) \
	$(TEST_DIR)/server_test.cpp $(SERVER_DEPENDENCIES) \
	$(TEST_DIR)/libgtest.a -o server_test $(BOOST_FLAGS)

	g++ $(CXXFLAGS) $(GTEST_IMPORT) \
	$(TEST_DIR)/request_parser_test.cpp $(REQUEST_PARSER_DEPENDENCIES) \
	$(TEST_DIR)/libgtest.a -o request_parser_test $(BOOST_FLAGS)

	g++ $(CXXFLAGS) $(GTEST_IMPORT) \
	$(TEST_DIR)/response_test.cpp $(RESPONSE_DEPENDENCIES) \
	$(TEST_DIR)/libgtest.a -o response_test $(BOOST_FLAGS)

	g++ $(CXXFLAGS) $(GTEST_IMPORT) \
	$(TEST_DIR)/request_test.cpp $(REQUEST_DEPENDENCIES) \
	$(TEST_DIR)/libgtest.a -o request_test $(BOOST_FLAGS)

	g++ $(CXXFLAGS) $(GTEST_IMPORT) \
	$(TEST_DIR)/http_client_test.cpp $(HTTP_CLIENT_DEPENDENCIES) \
	$(TEST_DIR)/libgtest.a -o http_client_test $(BOOST_FLAGS)

	g++ $(CXXFLAGS) $(GTEST_IMPORT) \
	$(TEST_DIR)/request_handler_proxy_test.cpp $(PROXY_HANDLER_DEPENDENCIES) \
	$(TEST_DIR)/libgtest.a -o request_handler_proxy_test $(BOOST_FLAGS)

	g++ $(CXXFLAGS) $(GTEST_IMPORT) \
	$(TEST_DIR)/sql_engine_test.cpp $(SQL_ENGINE_DEPENDENCIES) \
	$(TEST_DIR)/libgtest.a -o sql_engine_test $(BOOST_FLAGS) $(SQL_FLAGS)

	#g++ $(CXXFLAGS) $(GTEST_IMPORT) $(GMOCK_IMPORT) \
	#$(TEST_DIR)/connection_test.cpp $(CONNECTION_DEPENDENCIES) \
	#$(TEST_DIR)/libgtest.a -o connection_test $(BOOST_FLAGS)

	#g++ $(CXXFLAGS) $(GTEST_IMPORT) $(GMOCK_IMPORT) \
	#$(TEST_DIR)/connection_manager_test.cpp $(CONNECTION_DEPENDENCIES) \
	#$(TEST_DIR)/libgtest.a -o connection_manager_test $(BOOST_FLAGS)

	g++ $(CXXFLAGS) $(GTEST_IMPORT) \
	$(TEST_DIR)/request_handler_sql_test.cpp $(REQUEST_HANDLER_SQL_DEPENDENCIES) \
	$(TEST_DIR)/libgtest.a -o request_handler_sql_test $(BOOST_FLAGS) $(SQL_FLAGS)

	g++ $(CXXFLAGS) $(GTEST_IMPORT) \
	$(TEST_DIR)/sql_engine_test.cpp $(SQL_ENGINE_DEPENDENCIES) \
	$(TEST_DIR)/libgtest.a -o sql_engine_test $(BOOST_FLAGS) $(SQL_FLAGS)

coverage: CXXFLAGS+=-coverage
coverage:
	mkdir $(RESULTS_TEST_DIR)
	mkdir $(RESULTS_COVERAGE_DIR)

	g++ $(CXXFLAGS) $(GTEST_IMPORT) \
	$(TEST_DIR)/request_handler_echo_test.cpp $(REQUEST_HANDLER_ECHO_DEPENDENCIES) \
	$(TEST_DIR)/libgtest.a -o request_handler_echo_test $(BOOST_FLAGS)
	./request_handler_echo_test > $(RESULTS_TEST_DIR)/request_handler_echo_test_info
	gcov -r request_handler_echo.cpp > $(RESULTS_COVERAGE_DIR)/request_handler_echo_coverage

	g++ $(CXXFLAGS) $(GTEST_IMPORT) \
	$(TEST_DIR)/request_handler_static_test.cpp $(REQUEST_HANDLER_STATIC_DEPENDENCIES) \
	$(TEST_DIR)/libgtest.a -o request_handler_static_test $(BOOST_FLAGS)
	./request_handler_static_test > $(RESULTS_TEST_DIR)/request_handler_static_test_info
	gcov -r request_handler_static.cpp > $(RESULTS_COVERAGE_DIR)/request_handler_static_coverage

	g++ $(CXXFLAGS) $(GTEST_IMPORT) \
	$(TEST_DIR)/request_handler_notfound_test.cpp $(REQUEST_HANDLER_NOTFOUND_DEPENDENCIES) \
	$(TEST_DIR)/libgtest.a -o request_handler_notfound_test $(BOOST_FLAGS)
	./request_handler_notfound_test > $(RESULTS_TEST_DIR)/request_handler_notfound_test_info
	gcov -r request_handler_notfound.cpp > $(RESULTS_COVERAGE_DIR)/request_handler_notfound_coverage

	g++ $(CXXFLAGS) $(GTEST_IMPORT) \
	$(TEST_DIR)/server_test.cpp $(SERVER_DEPENDENCIES) \
	$(TEST_DIR)/libgtest.a -o server_test $(BOOST_FLAGS)
	./server_test > $(RESULTS_TEST_DIR)/server_test_info
	gcov -r server.cpp > $(RESULTS_COVERAGE_DIR)/server_coverage

	g++ $(CXXFLAGS) $(GTEST_IMPORT) \
	$(TEST_DIR)/request_parser_test.cpp $(REQUEST_PARSER_DEPENDENCIES) \
	$(TEST_DIR)/libgtest.a -o request_parser_test $(BOOST_FLAGS)
	./request_parser_test > $(RESULTS_TEST_DIR)/request_parser_test_info
	gcov -r request_parser.cpp > $(RESULTS_COVERAGE_DIR)/request_parser_coverage

	g++ $(CXXFLAGS) $(GTEST_IMPORT) \
	$(TEST_DIR)/response_test.cpp $(RESPONSE_DEPENDENCIES) \
	$(TEST_DIR)/libgtest.a -o response_test $(BOOST_FLAGS)
	./response_test > $(RESULTS_TEST_DIR)/response_test_info
	gcov -r response.cpp > $(RESULTS_COVERAGE_DIR)/response_coverage

	g++ $(CXXFLAGS) $(GTEST_IMPORT) \
	$(TEST_DIR)/request_test.cpp $(REQUEST_DEPENDENCIES) \
	$(TEST_DIR)/libgtest.a -o request_test $(BOOST_FLAGS)
	./request_test > $(RESULTS_TEST_DIR)/request_test_info
	gcov -r request.cpp > $(RESULTS_COVERAGE_DIR)/request_coverage

	# g++ $(CXXFLAGS) $(GTEST_IMPORT) $(GMOCK_IMPORT) \
	# $(TEST_DIR)/connection_test.cpp $(CONNECTION_DEPENDENCIES) \
	# $(TEST_DIR)/libgtest.a -o connection_test $(BOOST_FLAGS)
	# ./connection_test > $(RESULTS_TEST_DIR)/connection_test_info
	# gcov -r connection.cpp > $(RESULTS_COVERAGE_DIR)/connection_coverage

	# g++ $(CXXFLAGS) $(GTEST_IMPORT) $(GMOCK_IMPORT) \
	# $(TEST_DIR)/connection_manager_test.cpp $(CONNECTION_DEPENDENCIES) \
	# $(TEST_DIR)/libgtest.a -o connection_manager_test $(BOOST_FLAGS)
	# ./connection_manager_test > $(RESULTS_TEST_DIR)/connection_manager_test_info
	# gcov -r connection_manager.cpp > $(RESULTS_COVERAGE_DIR)/connection_manager_coverage

integration:
	./$(TEST_DIR)/integration_test.sh

run-tests:
	./$(TEST_DIR)/run_tests.sh

clean:
	rm -rf *.o webserver *_test $(RESULTS_TEST_DIR) $(RESULTS_COVERAGE_DIR) *.gcov *.gcda *.gcno

dist:
	tar -czvf webserver.tar.gz *

.PHONY: all test clean
