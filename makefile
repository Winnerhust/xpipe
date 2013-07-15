CXX=g++
all:
    $(CXX) -c xpipe.cpp
    $(CXX) test.cpp -o test
clean:
    rm xpipe.o test
