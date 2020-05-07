all: multimap

multimap: main.cpp multimap.hpp noisy.hpp test_list.hpp test_multimap.hpp
	g++ -std=c++2a -g -o multimap *.cpp
