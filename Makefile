all: webserver

webserver: webserver.cpp
	c++ -I ~/cs130/boost-server/boost_1_61_0 webserver.cpp -o webserver \
	~/cs130/boost-server/boost_1_61_0/~/cs130/boost-server/lib/libboost_regex.a

clean:
	rm -rf *.o webserver

dist:
	tar -czvf webserver.tar.gz *