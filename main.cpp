#include <iostream>
#include <ostream>
#include <string>
#include <utility>
#include "multimap.hpp"
#include "test_list.hpp"
#include "test_multimap.hpp"

struct point {
	int a, b;
	point(int a = 0, int b = 0): a(a), b(b) {}
};

int main() {
	{
		std::vector<int> data_i = {8, 9, 55, 32, 8, 10};
		list_test1(data_i);
		std::vector<std::string> data_s = {"Te!", "sötétben bújkáló", "rémhír terjesztő", "ellenforradalmár", "reszkess!"};
		list_test1(data_s);
		list_testnoisy();
	}

	{
		std::vector<std::pair<int, int>> data = {
			std::make_pair(3, 4),
			std::make_pair(3, 5),
			std::make_pair(89, 4),
			std::make_pair(5, 9)
		};
		multimap_test1(data);
	}
	{
		std::vector<std::pair<std::string, int>> data = {
			std::make_pair("Kossuth Lajos azt üzente,", 0),
			std::make_pair("Elfogyott a regimentje!!", 1),
			std::make_pair("Ha még egyszer, azt üzeni, Mindnyájunknak el kell menni,", 2),
			std::make_pair("Éljen a magyar szabadság,", 3),
			std::make_pair("Éljen a haza!!!", 4),
			std::make_pair("Esik eső karikára,", 5),
			std::make_pair("Kossuth Lajos kalapjára!!", 6),
			std::make_pair("sikkasztott", -1),
			std::make_pair("Valahány csepp esik rája,", 7),
			std::make_pair("Annyi áldás szálljon rája!!", 8),
			std::make_pair("Éljen a magyar szabadság,", 9),
			std::make_pair("Éljen a haza!!!", 10)
		};
		multimap_test1(data);
		multimap_test1<std::string, int, std::hash<std::string>>(data);
	}
	*/

	return 0;
}


