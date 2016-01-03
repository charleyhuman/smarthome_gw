CXXFLAGS= -std=c++11 -Wall -g -I/usr/include/ -I/usr/local/include
LDFLAGS=-L/usr/lib/x86_64-linux-gnu -lmosquittopp -lmosquitto -L/usr/local/lib -lrf24

.PHONY: all clean

all : smarthome_gateway

smarthome_gateway : smarthome_gateway.o openhab_inf.o sensors_inf.o
	${CXX} $^ -o $@ ${LDFLAGS}
	
%.o: %.cpp *.h
	$(CXX) -c -o $@ $< $(CXXFLAGS)
	
clean : 
	-rm -f *.o smarthome_gateway
