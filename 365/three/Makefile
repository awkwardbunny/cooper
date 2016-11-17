CC = g++
CFLAGS = -std=c++11 -fpermissive

dijkstra: dijkstra.o heap.o hash.o graph.o
	$(CC) -o $@ $^

dijkstra.o: dijkstra.cpp
	$(CC) $(CFLAGS) -c $^

heap.o: heap.cpp heap.h
	$(CC) $(CFLAGS) -c $<

hash.o: hash.cpp hash.h
	$(CC) $(CFLAGS) -c $<

graph.o: graph.cpp graph.h
	$(CC) $(CFLAGS) -c $<

#debug:
#   $(CC) $(CFLAGS) -g -o dijkstraDebug dijkstra.cpp heap.cpp hash.cpp graph.cpp

clean:
	rm -f *.exe *.o *.stackdump *~ 

backup:
	test -d backups || mkdir backups
	cp *.cpp backups
	cp *.h backups
	cp Makefile backups
