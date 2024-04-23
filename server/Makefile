Server: main.o udp_server.o
	g++ -o Server main.o udp_server.o
main.o: main.cpp udp_server.h
	g++ -c main.cpp -I.
udp_server.o: udp_server.cpp udp_server.h thread_pool.h TunInterface.h Authenticator.h
	g++ -c udp_server.cpp -I.
clean:
	rm -f *.o Server
