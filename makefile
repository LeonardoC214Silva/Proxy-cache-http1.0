make:  servidor cliente inf
 
 servidor: main.cpp
	g++ -std=c++11 -o servidor main.cpp ItemCash.cpp ItemCash.h Cache.cpp Cache.h CacheHost.cpp CacheHost.h CacheControl.cpp CacheControl.h Estatic.cpp Estatic.h -lpthread
 
cliente: client.cpp
	g++ -std=c++11 -o cliente client.cpp
inf: client2.cpp
	g++ -std=c++11 -o inf client2.cpp

 clean:
	rm cliente
	rm servidor
	rm inf
