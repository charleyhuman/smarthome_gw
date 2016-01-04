CXXFLAGS= -std=c++11 #-Wall -g
LDFLAGS=-lmosquittopp -lmosquitto -lrf24 -lpthread

.PHONY: all clean

all : smarthome_gateway

smarthome_gateway : smarthome_gateway.o openhab_inf.o sensors_inf.o
	${CXX} $^ -o $@ ${LDFLAGS}
	
%.o: %.cpp *.h
	$(CXX) -c -o $@ $< $(CXXFLAGS)
	
clean : 
	-rm -f *.o smarthome_gateway
