import subprocess
import os
import shutil
import sys
import telnetlib

webserver = subprocess.Popen(['./webserver', 'config']);

#Echo
request = 'curl -i localhost:8080/echo'
curl_proc = subprocess.Popen(request, stdout=subprocess.PIPE, shell=True)
response = curl_proc.stdout.read().decode('utf-8')

expected_response = 'HTTP/1.1 200 OK\r\n\
Content-Length: 82\r\n\
Content-Type: text/plain\r\n\r\n\
GET /echo HTTP/1.1\r\n\
User-Agent: curl/7.35.0\r\n\
Host: localhost:8080\r\n\
Accept: */*\r\n\r\n'


#Static
request_static = 'curl -i localhost:8080/static2/'
curl_proc = subprocess.Popen(request_static, stdout=subprocess.PIPE, shell=True)
response_static = curl_proc.stdout.read().decode('utf-8')

expected_response_static = 'HTTP/1.1 200 OK\r\n\
Content-Length: 141\r\n\
Content-Type: text/html\r\n\r\n\
<!DOCTYPE html>\n\
<html>\n\
<body>\n\n\
<h1>This is default.html</h1>\n\n\
<p>The webserver successfully served this static file YAY</p>\n\n\
</body>\n\
</html>\n'


#Multithread
host = "localhost"
expected_response_thread = 'HTTP/1.1 200 OK\r\n\
Content-Length: 77\r\n\
Content-Type: text/plain\r\n\r\n\
GET /echo HTTP/1.1\r\n\
User-Agent: telnet\r\n\
Host: localhost:8080\r\n\
Accept: */*\r\n\r\n'

first_half_message = "GET /echo HTTP/1.1\r\n\
User-Agent: telnet\r\n\
Host: localhost:8080\r\n"
second_half_message = "Accept: */*\r\n\r\n"

tn1 = telnetlib.Telnet(host, 8080, 5)
tn1.write(first_half_message)
tn1_response = tn1.read_eager() 

tn2 = telnetlib.Telnet(host, 8080, 5)
tn2.write(first_half_message + second_half_message)
tn2_response = tn2.read_all()

tn1.write(second_half_message)
tn1_response = tn1_response + tn1.read_all()

webserver.kill()

print('----------------------------------------------------------')

if response != expected_response:
	print('Echo output does not match')
	print('Expected: ' + str(len(expected_response)) + '\n' + expected_response)
	print('Response: ' + str(len(response)) + '\n' + response)
else:
	print('Echo Success')

if response_static != expected_response_static:
        print('Static output does not match')
        print('Expected: ' + str(len(expected_response_static)) + '\n' + expected_response_static)
        print('Response: ' + str(len(response_static)) + '\n' + response_static)
        exit(1)
else:
        print('Static Success')
        

if tn1_response != expected_response_thread or tn2_response != expected_response_thread:
    print('Multithreaded output does not match')
    print("Expected first response: " + str(len(expected_response_thread)) + '\n' + expected_response_thread)
    print("First response: " + str(len(tn1_response)) + '\n' + tn1_response)
    print("Second response: " + str(len(tn2_response)) + '\n' + tn2_response)
    exit(1)
else:
    print("Multithread Success")
    exit(0)