concurrentLL: headers/ConcurrentLL.hpp src/ConcurrentLL.cpp
	g++ -c src/ConcurrentLL.cpp -o bin/ConcurrentLL.o  
testconcurrentLL: headers/ConcurrentLL.hpp bin/ConcurrentLL.o
	g++ -o bin/ConcurrentLLSerialTest bin/ConcurrentLL.o tests/ConcurrentLLserial.cpp
	./bin/ConcurrentLLSerialTest
clean:
	rm bin/*