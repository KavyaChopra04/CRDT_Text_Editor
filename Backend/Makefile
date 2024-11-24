concurrentLL: headers/ConcurrentLL.hpp src/ConcurrentLL.cpp
	g++ -c src/ConcurrentLL.cpp -std=c++17 -Wall -Wextra -O2 -pthread -o bin/ConcurrentLL.o
commands: concurrentLL headers/commands.hpp  headers/ConcurrentLL.hpp src/commands.cpp bin/ConcurrentLL.o
	g++ -std=c++17 -Wall -Wextra -O2 -pthread -o bin/commands.o  -c bin/ConcurrentLL.o src/commands.cpp
testconcurrentLL: headers/ConcurrentLL.hpp bin/ConcurrentLL.o
	g++ -o bin/ConcurrentLLSerialTest bin/ConcurrentLL.o tests/ConcurrentLLserial.cpp
	./bin/ConcurrentLLSerialTest
testserialization: headers/ConcurrentLL.hpp bin/ConcurrentLL.o
	g++ -o bin/s_ds_test bin/ConcurrentLL.o tests/s_ds_test.cpp
	./bin/s_ds_test
testfront: headers/ConcurrentLL.hpp headers/commands.hpp bin/commands.o
	g++ -o bin/front_client tests/front_client.cpp 
	g++ -o bin/front_server src/ConcurrentLL.cpp tests/front_server.cpp src/commands.cpp
alice: commands
	g++ -std=c++17 -Wall -Wextra -O2 -pthread -o bin/alice src/alice.cpp src/ConcurrentLL.cpp src/commands.cpp
bob: commands
	g++ -std=c++17 -Wall -Wextra -O2 -pthread -o bin/bob src/bob.cpp src/ConcurrentLL.cpp src/commands.cpp
clean:
	rm bin/*