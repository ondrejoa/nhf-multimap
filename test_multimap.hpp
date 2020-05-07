#pragma once
#include <typeinfo>
#include <vector>
#include <iostream>
#include <sstream>
#include <utility>
#include <stdexcept>
#include "multimap.hpp"
#include "noisy.hpp"

#ifdef __GNUC__
#include <cxxabi.h>
#endif

#include "gtest_lite.h"

template<typename T, typename U, typename HASH = nhf::nohash<T>>
void multimap_test1(std::vector<std::pair<T, U>> data) {
	std::cout << "\nMULTIMAP TEST1 TYPE: "
#ifdef __GNUC__
		;
		int status;
		char *name = abi::__cxa_demangle(typeid(nhf::multimap<T, U, HASH>).name(), 0, 0, &status);
		std::cout << name;
		free(name);
#else
		<< typeid(nhf::multimap<T, U, HASH>).name();
#endif

		if (data.size() == 0)
			throw std::logic_error("");

		{
			nhf::multimap<T, U, HASH> multimap;

			TEST(multimap1, defsize) {
				EXPECT_EQ(0, multimap.size());
			} END

			TEST(multimap1, emplace) {
				for (auto [key, value]: data)
					EXPECT_NO_THROW(multimap.emplace(key, value));
			} END

			TEST(multimap1, contains) {
				auto contains_kv_pair = [&multimap](T &key, U &value) {
					for (auto &v: multimap.equal_range(key))
						if (v == value) return true;
					return false;
				};
				for (auto [key, value]: data)
					EXPECT_EQ(true, contains_kv_pair(key, value));
			} END

			TEST(multimap1, size) {
				EXPECT_EQ(data.size(), multimap.size());
			} END

			TEST(multimap1, extract) {
				std::size_t oldsize = multimap.size();
				auto list = multimap.extract(data[0].first);
				std::size_t lsize = 0;
				for (auto &l: list) lsize++;
				EXPECT_GE(lsize, 1);
				EXPECT_LT(multimap.size(), oldsize);
			} END

			TEST(multimap1, merge) {
				std::size_t oldsize = multimap.size();
				
				std::cout << "M1:\n";
				multimap.for_each([](T &key, U &val) {
							std::cout << "\t" << key << "\t" << val << "\n";
						});

				nhf::multimap<T, U, HASH> m2;
				T key = T();
				m2.emplace(key, U());
				multimap.merge(m2);
				EXPECT_EQ(oldsize + 1, multimap.size());
			} END

			TEST(multimap1, read/write) {
				std::stringstream ss;
				multimap.write(ss);
				EXPECT_GT(ss.str().size(), 0);
				nhf::multimap<T, U, HASH> m2;
				m2.read(ss);
				EXPECT_EQ(multimap.size(), m2.size());
			} END
		}
}
