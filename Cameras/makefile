all: test

CFLAGS=-fPIC -g -Wall `pkg-config --cflags opencv` -std=c++11
LIBS = `pkg-config --libs opencv` -lusb-1.0 -l pthread
INCLUDE = -I/usr/local/include/libfreenect
FREE_LIBS = -L/usr/local/lib -lfreenect

test:  opencvwrapper.cpp
	$(CXX) $(INCLUDE) $(CFLAGS) $? -o $@  $(LIBS) $(FREE_LIBS)

%.o: %.cpp
	$(CXX) -c $(CFLAGS) $< -o $@

clean:
	rm -rf *.o test

