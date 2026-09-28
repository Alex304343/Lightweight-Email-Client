sudo apt update
sudo apt install libsqlite3-dev libyaml-cpp-dev libgoogle-glog-dev

docker run -d \
  -p 3025:3025 \
  -p 3110:3110 \
  -e GREENMAIL_OPTS="-Dgreenmail.setup.test.all -Dgreenmail.hostname=0.0.0.0 -Dgreenmail.users=acc1@localhost:123,acc2@localhost:321" \
  --name test-mail \
  greenmail/standalone:latest