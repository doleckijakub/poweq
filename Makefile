test: poweq
	./poweq "420+1234*12345"

poweq: poweq.cpp
	g++ -o $@ $^
