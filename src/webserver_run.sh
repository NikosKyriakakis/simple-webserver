make clean
rm -f webserver
make
sudo chown root ./webserver
sudo chmod u+s ./webserver
./webserver