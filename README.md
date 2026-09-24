sudo apt update
sudo apt install libsqlite3-dev libyaml-cpp-dev libgoogle-glog-dev

docker run -d -p 3025:3025 -p 3110:3110 --name test-mail pti/greenmail