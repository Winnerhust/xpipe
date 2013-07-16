CXX=g++
all:
    $(CXX) -c xpipe.cpp
    $(CXX) test.cpp -o test xpipe.o
clean:
    rm xpipe.o test
