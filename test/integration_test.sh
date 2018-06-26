#!/bin/bash
# Build the binary
make clean
make

# Start the server
echo "Running an integration test on the server"
echo "port 8080;
path /echo EchoHandler {}
path /static1 StaticHandler { root www; }
path /static2 StaticHandler { root pics; }
path /test ProxyHandler { proxy_pass localhost:8081/example; }
" > test_config

echo "port 8081;
path /echo EchoHandler {}
path /static1 StaticHandler { root www; }
path /static2 StaticHandler { root pics; }
path /example ProxyHandler { proxy_pass www.example.com; }

" > test_config2

./webserver test_config &>/dev/null &
./webserver test_config2 &>/dev/null &

# Send a request to the server using curl
echo "Sending a curl request"
curl -i -s localhost:8080/echo > test_output_echo
curl -s localhost:8080/static1/index.html > test_output_static1
curl -s localhost:8080/static2/ducks.jpg > test_output_static2_jpg
curl -s localhost:8080/static2/giphy.gif > test_output_static2_gif
curl -s localhost:8080/static2/mario.jpeg > test_output_static2_jpeg

#proxy test
curl -i -s localhost:8080/test > test_proxy_echo2
curl -i -s localhost:8081/example > test_proxy_echo

# Verify the response from the server works as expected
diff test/expected_echo_output test_output_echo > diff_echo_output
EXIT_STATUS+=$?
diff www/index.html test_output_static1 > diff_static1_output
EXIT_STATUS+=$?
diff pics/ducks.jpg test_output_static2_jpg > diff_static2_jpg_output
EXIT_STATUS+=$?
diff pics/giphy.gif test_output_static2_gif > diff_static2_gif_output
EXIT_STATUS+=$?
diff pics/mario.jpeg test_output_static2_jpeg > diff_static2_jpeg_output
EXIT_STATUS+=$?

diff test_proxy_echo test_proxy_echo2 > diff_proxy_echo_output
num_lines="$(wc -l < diff_proxy_echo_output)"
#assert that no less than 10 lines be different to allow for some minor spacing changes.
if [ "$num_lines" -gt 10 ]
then
  EXIT_STATUS+=1
fi

# Indicate success or failure with an appropriate exit code
# Diff returns 0 if there are no differences, 1 if there are,
# and another number if something else went wrong
if [ "$EXIT_STATUS" -eq 0 ]
then
	echo "SUCCESS: Integration test produced expected output"
else
	echo "Integration test did not produce the expected result"
	echo "Diff of curl output:"
	cat diff_echo_output
	cat diff_static1_output
	cat diff_static2_jpg_output
	cat diff_static2_gif_output
	cat diff_static2_jpeg_output
	cat diff_proxy_echo_output
fi

# Shutdown the webserver
echo "Cleaning up and shutting down"
pkill webserver
make clean
rm test_config
rm test_config2
rm test_output_echo
rm test_output_static1
rm test_output_static2_jpg
rm test_output_static2_gif
rm test_output_static2_jpeg
rm diff_echo_output
rm diff_static1_output
rm diff_static2_jpg_output
rm diff_static2_gif_output
rm diff_static2_jpeg_output
rm diff_proxy_echo_output
rm test_proxy_echo
rm test_proxy_echo2

exit "$EXIT_STATUS"
