CC=g++
INCLUDES=-I. -I..
LIBS=-L. -L..
LDFLAGS=-lcrypto -lcryptopp
CFLAGS=-c -Wall -g
SOURCES=pass.cpp ../util/useful.cpp
OBJECTS=$(SOURCES:.cpp=.o)
MAIN=pass_main.cpp
TEST=pass_test.cpp

pass: pass.a pass_main.cpp
	$(CC) $(INCLUDES) $(LIBS) pass_main.cpp pass.a -o $@ $(LDFLAGS)

test: pass.a pass_test.cpp
	$(CC) $(INCLUDES) $(LIBS) pass_test.cpp pass.a -o $@ $(LDFLAGS)

pass.a: $(OBJECTS)
	ar rcs $@ $(OBJECTS)

.cpp.o:
	$(CC) $(INCLUDES) $(CFLAGS) $< -o $@
	
clean:
	rm -rf *.o *.a pass pass_test
