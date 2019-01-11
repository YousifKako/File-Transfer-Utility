CFLAGS = -g

objects = Server.cpp Serialization.cpp

Server : $(objects)
	g++ $(objects) -o Server -lboost_system -lboost_filesystem

.PHONY : clean
clean :
	rm Server $(objects)
