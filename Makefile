CC=g++
CFLAGS=-c -Wall
LDFLAGS=-lboost_graph -lboost_program_options
SOURCES=Main.cpp Graph.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=ap

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o $(EXECUTABLE)