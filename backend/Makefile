concurrentLL: headers/concurrentLL.hpp src/concurrentLL.cpp
	g++ -c src/concurrentLL.cpp -std=c++17 -Wall -Wextra -O3 -pthread -o bin/concurrentLL.o

commands: concurrentLL headers/commands.hpp src/commands.cpp
	g++ -std=c++17 -Wall -Wextra -O3 -pthread -o bin/commands.o  -c bin/concurrentLL.o src/commands.cpp

testconcurrentLL: concurrentLL
	g++ -o bin/concurrentLLSerialTest bin/concurrentLL.o tests/concurrentLLserial.cpp
	./bin/concurrentLLSerialTest

testserialization: concurrentLL
	g++ -o bin/s_ds_test bin/concurrentLL.o tests/s_ds_test.cpp
	./bin/s_ds_test

testfront: headers/concurrentLL.hpp headers/commands.hpp bin/commands.o
	g++ -o bin/front_client tests/front_client.cpp 
	g++ -o bin/front_server src/concurrentLL.cpp tests/front_server.cpp src/commands.cpp

alice: commands
	g++ -std=c++17 -Wall -Wextra -O3 -pthread -o bin/alice src/alice.cpp src/concurrentLL.cpp src/commands.cpp

bob: commands
	g++ -std=c++17 -Wall -Wextra -O3 -pthread -o bin/bob src/bob.cpp src/concurrentLL.cpp src/commands.cpp

clean:
	rm -f bin/*

all: clean alice bob