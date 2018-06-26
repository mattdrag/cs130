# CS 130 Project - Web Server

// TODO: Team Name

Matt Dragotto, Andy Kuang, McKenna Galvin


A full HTTP web server, from the
[boost documentation](http://www.boost.org/doc/libs/1_53_0/doc/html/boost_asio/examples.html).
This code is an adapted version of the
[HTTP server](http://www.boost.org/doc/libs/1_62_0/doc/html/boost_asio/examples/cpp11_examples.html)
example.

The server is currently deployed on AWS under the public IP *54.190.63.110* and port 8080.
Try out some of the following links!
* Echo: [http://54.190.63.110:8080/echo](http://54.190.63.110:8080/echo)
* Static HTML: [http://54.190.63.110:8080/static1/index.html](http://54.190.63.110:8080/static1/index.html)
* Static image: [http://54.190.63.110:8080/static2/ducks.jpg](http://54.190.63.110:8080/static2/ducks.jpg)
* Status: [http://54.190.63.110:8080/status](http://54.190.63.110:8080/status)
* SQL interface: [http://54.190.63.110:8080/sql](http://54.190.63.110:8080/sql)
* Project 9 design doc: [http://54.190.63.110:8080/static1/design.md](http://54.190.63.110:8080/static1/design.md)

(We currently don't have the proxy handler up on our AWS instance because there was a bug in the proxy handler
implementation that was causing our server to crash when a favicon.ico was requested that didn't exist)

---

### Deploying
* To build and run the webserver in docker, run `make docker`
* To deploy the docker image to AWS and run it, run `make aws`
* To SSH into the EC2 instance, run `ssh -i webserver.pem ec2-user@54.190.63.110`
  (assumes you have the private key webserver.pem in the working directory)

---

### Usage - local development
* Edit `deploy/config` to define the port on which you want the server to run.
* Run `make` to build the source code
* Run server using `./webserver deploy/config`

The server can be accessed via a variety of methods:
* Use your browser to view the resource (for example, `http://localhost:8080/echo`).
* Use a curl request: `curl -i -s localhost:8080/echo`

---

### Running tests
* `make integration` runs the integration test only
* `make test` builds the test binaries for the individual files. You can then run each test separately, if you choose (for example, `./request_handler_echo_test`)
* `make all-tests` builds and runs all unit and integration tests

---

### Dealing with bound ports
If when running the server you get an error that the port is already bound,
execute `sudo lsof -i:8080` (change 8080 to whatever port number you're using).
Then find the PID of the process associated with the server and kill it using `sudo kill -9 <pid>`

---

### Source code explanation
* `main`: Parses the config file, initializes and runs the server
* `server_options`: Struct which contains the config options specified in the config file. Passed to the server upon creation so the server can properly set up request handlers
* `server`: Constructs a server to listen on the specified TCP address and port, and serves up files from the given directory
* `connection_manager`: Manages open connections so they may be cleanly stopped when the server needs to shut down
* `connection`: Represents a single TCP connection from a client
* `request_handler`: Processes a request and fills out a response (variations are `request_handler_echo`, `request_handler_static`, `request_handler_notfound`, and `request_handler_status`)
* `request`: Represents the contents of a HTTP request
* `response`: Represents the contents of an HTTP response

We also use the following helper classes taken from the boost example:
* `mime_types`
* `request_parser`
