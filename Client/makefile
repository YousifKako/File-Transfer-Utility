CFLAGS = -g

objects = Client.cpp Serialization.cpp

Client : $(objects)
	g++ $(objects) -o Client -lboost_system -lboost_filesystem

.PHONY : clean
clean :
	rm Client $(objects)
