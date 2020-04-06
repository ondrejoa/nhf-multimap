#include <iostream>
#include <string>
#include "multimap.hpp"

int main(int argc, char **argv) {
	nhf::multimap<std::string, int> asd;
	asd.emplace("Béla", 43);
	asd.emplace("Béla", 33);
	asd.emplace("Kecske", 2);
	asd.emplace("Retek", 34);
	asd.for_each([](std::string &key, int &val) {
				std::cout << key << "\t" << val << "\n";
			});
	return 0;
}

