CC = g++
CFLAGS = -Wall -g -O3 -std=c++20 -static-libstdc++

test:
	$(CC) $(CFLAGS) -c -o primes.o primes.cpp
	$(CC) $(CFLAGS) -c -o find-prime-constellations.o find-prime-constellations.cpp
	$(CC) $(CFLAGS) -o find-prime-constellations find-prime-constellations.o primes.o -lgmpxx -lgmp
# g++ -o find-prime-constellations -std=c++20 find-prime-constellations.o primes.o  -lgmpxx -lgmp