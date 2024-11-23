concurrentLL: headers/ConcurrentLL.hpp src/ConcurrentLL.cpp
	g++ -c src/ConcurrentLL.cpp -o bin/ConcurrentLL.o
commands: headers/commands.hpp  headers/ConcurrentLL.hpp src/commands.cpp bin/ConcurrentLL.o
	g++ -o bin/commands.o  -c bin/ConcurrentLL.o src/commands.cpp
testconcurrentLL: headers/ConcurrentLL.hpp bin/ConcurrentLL.o
	g++ -o bin/ConcurrentLLSerialTest bin/ConcurrentLL.o tests/ConcurrentLLserial.cpp
	./bin/ConcurrentLLSerialTest
testserialization: headers/ConcurrentLL.hpp bin/ConcurrentLL.o
	g++ -o bin/s_ds_test bin/ConcurrentLL.o tests/s_ds_test.cpp
	./bin/s_ds_test
testfront: headers/ConcurrentLL.hpp headers/commands.hpp bin/commands.o
	g++ -o bin/front_client tests/front_client.cpp 
	g++ -o bin/front_server src/ConcurrentLL.cpp tests/front_server.cpp src/commands.cpp
alice: 
	g++ -o bin/alice src/alice.cpp src/ConcurrentLL.cpp src/commands.cpp
bob:
	g++ -o bin/bob src/bob.cpp src/ConcurrentLL.cpp src/commands.cpp
clean:
	rm bin/*