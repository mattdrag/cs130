#!/bin/bash

# aws_deploy.sh
# Deploy the docker image to AWS and run it.
# This assumes that the docker image has already been created using 'make docker' and is named httpserver
# This also assumes that the EC2 public key, webserver.pem, is in the root directory of the repo.

echo 'Saving docker image as tar file'
docker save -o httpserver.tar httpserver

echo 'Copying the docker image to AWS'
scp -i webserver.pem httpserver.tar ec2-user@54.190.63.110:/home/ec2-user

echo 'SSHing into AWS'
ssh -i webserver.pem ec2-user@54.190.63.110 << EOF

	echo 'Loading the image into docker'
	docker load -i httpserver.tar

	echo 'Killing existing running containers'
	docker kill $(docker ps -q)

	echo 'Running the server'
	docker run --rm -t -p 8080:8080 httpserver

EOF
