#pragma once
#include <typeinfo>
#include <vector>
#include <iostream>
#include <sstream>
#include <utility>
#include "multimap.hpp"
#include "noisy.hpp"

#ifdef __GNUC__
#include <cxxabi.h>
#endif

#include "gtest_lite.h"

template<typename T>
void list_test1(std::vector<T> data) {
	std::cout << "\nLIST TEST1 TYPE: "
#ifdef __GNUC__
		;
		int status;
		char *name = abi::__cxa_demangle(typeid(nhf::list<T>).name(), 0, 0, &status);
		std::cout << name;
		free(name);
#else
		<< typeid(nhf::list<T>).name();
#endif

		TEST(list1, empty) {
			nhf::list<T> list;
			// Nem tudta egyből összehasonlítani őket
			EXPECT_EQ(list.begin() == list.begin(), true);
		} END

		nhf::list<T> list;

		TEST(list1, emplace) {
			for (auto &d: data)
				EXPECT_NO_THROW(list.emplace_front(d));
		} END

		TEST(list1, stored) {
			auto it = data.rbegin();
			for (auto &l: list) {
				EXPECT_EQ(l, *it++);
			}
		} END

		TEST(list1, clear) {
			EXPECT_NO_THROW(list.clear());
			EXPECT_EQ(list.begin() == list.begin(), true);
		} END


}

void list_testnoisy() {
	std::vector<std::string> out = {"ctor0", "ctor0", "ctor0", "ctor1", "ctor2", "dtor", "dtor", "dtor", "dtor", "dtor"};
	std::stringstream ss;

	{
		nhf::list<noisyboi> list;

		// ctor0, default
		list.emplace_front(ss);

		// ctor0, default
		list.emplace_front(ss, 2);

		{
			// ctor0, default
			noisyboi n(ss, 4, 2);

			// ctor1, copy
			list.emplace_front(n);

			// ctor2, move
			list.emplace_front(std::move(n));

			// dtor
		}

		// dtor dtor dtor dtor
	}

	TEST(list2, noisy) {
		for (auto &o: out) {
			std::string str;
			ss >> str;
			EXPECT_EQ(o, str);
		}
	} END
}
