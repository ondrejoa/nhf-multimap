#pragma once
#include <iostream>
#include <ostream>

class noisyboi {
	private:
		std::ostream &os;
		int a, b;
	public:
		noisyboi(std::ostream &os, int a = 0, int b = 0): os(os), a(a), b(b) {
			os << "ctor0\t";
		}
		noisyboi(const noisyboi &n): os(n.os), a(n.a), b(n.b) {
			os << "ctor1\t";
		}
		noisyboi(noisyboi &&n): os(n.os), a(n.a), b(n.b) {
			os << "ctor2\t";
			n.a = n.b = 0;
		}
		~noisyboi() {
			os << "dtor\t";
		}
		std::ostream &write(std::ostream &os) {
			this->os << "write\t";
			return os << a << "\t" << b << "\t";
		}
		std::istream &read(std::istream &is) {
			os << "read\t";
			return is >> a >> b;
		}
};
