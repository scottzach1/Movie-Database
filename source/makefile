dbserver: dbserver.o
	g++ -o dbserver nw.o mt.o

dbserver.o: nw.cc nw.hh mt.cc mt.hh
	g++ -c nw.cc mt.cc

clean:
	rm -rf *.o dbserver