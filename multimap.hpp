#pragma once
#include <utility>
#include <functional>
#include <type_traits>

namespace nhf {
	template<typename T, typename U>
	class pair {
		private:
			T f;
			U s;
		public:
			pair(const T&val1, const U&val2):
				f(val1), s(val2) {}
			T &first() { return f; }
			U &second() { return s; }
			T &begin() { return f; }
			U &end() { return s; }
	};

	template<typename T>
	class list {
		private:
			struct list_item {
				list_item *next = nullptr;
				T data;
				template<typename ... Args>
				list_item(list_item *next, Args&& ... args):
					next(next), data(std::forward<Args>(args)...) {}
			};
			list_item *first = nullptr;
			class iterator {
				private:
					list_item *li;
				public:
					iterator(list_item *li): li(li) {}
					iterator(const iterator &it): iterator(it.li) {} 
					iterator &operator++() { 
						li = li->next;
						return *this;
					}
					iterator operator++(int) { 
						iterator tmp = *this;
						operator++();
						return tmp;
					}
					bool operator==(const iterator &it) { return li == it.li; }
					bool operator!=(const iterator &it) { return li != it.li; }
					T &operator*() { return li->data; }
					T *operator->() { return &li->data; }
			};
		public:
			list() {}
			list(const list&) = delete;
			~list() {
				clear();
			}
			void clear() {
				while (first != nullptr) {
					auto tmp = first->next;
					delete first;
					first = tmp;
				}
			}
			template<typename ... Args>
			void emplace_front(Args&& ... args) {
				first = new list_item(first, std::forward<Args>(args)...);
			}
			iterator begin() { return iterator(first); }
			iterator end() { return iterator(nullptr); }
	};

	template<typename T>
	struct nohash_passthrough {
		T operator()(T val) { return val; }
	};

	template<typename T, typename U, typename HASH = nohash_passthrough<T>>
	class multimap {
		private:
			struct btree_node {
				btree_node *lc = nullptr;
				btree_node *rc = nullptr;
				list<U> values;
				struct ukey {
					T key;
					typename std::result_of<HASH(T)>::type hash;
					ukey(T key, typename std::result_of<HASH(T)>::type hash): key(key), hash(hash) {}
					ukey(const ukey&) = default;
					bool operator==(const ukey &k) {
						if (hash == k.hash) 
							if (key == k.key)
								return true;
						return false;
					}
					bool operator>(const ukey &k) {
						return hash > k.hash;
					}
					bool operator<(const ukey &k) {
						return hash < k.hash;
					}
				} key;
				template<typename ... Args>
				btree_node(ukey &key, Args&& ... args): key(key) {
					values.emplace_front(std::forward<Args>(args)...);
				}
				template<typename ... Args>
				void insert(ukey &key, Args&& ... args) {
					if (key == this->key) {
						values.emplace_front(std::forward<Args>(args)...);
					}
					else if (key < this->key) {
						if (lc == nullptr) {
							lc = new btree_node(key, std::forward<Args>(args)...);
						}
						else {
							lc->insert(key, std::forward<Args>(args)...);
						}
					}
					else {
						if (rc == nullptr) {
							rc = new btree_node(key, std::forward<Args>(args)...);
						}
						else {
							rc->insert(key, std::forward<Args>(args)...);
						}
					}
				}
				void free() {
					if (lc != nullptr) {
						lc->free();
						delete lc;
					}
					if (rc != nullptr) {
						rc->free();
						delete lc;
					}
				}
				void for_each(std::function<void(T&, U&)> func) {
					if (lc != nullptr)
						lc->for_each(func);
					for (auto &v: values)
						func(key.key, v);
					if (rc != nullptr)
						rc->for_each(func);
				}
			};
			btree_node *root = nullptr;
		public:
			~multimap() { clear(); }
			void clear() {
				if (root == nullptr) root->free();
				delete root;
				root = nullptr;
			}
			template<typename ... Args>
			void emplace(T &&key ,Args&& ... args) {
				typename btree_node::ukey ukey(key, HASH{}(key));
				if (root == nullptr) {
					root = new btree_node(ukey, std::forward<Args>(args)...);
				}
				else {
					root->insert(ukey, std::forward<Args>(args)...);
				}
			}
			void for_each(std::function<void(T&, U&)> func) {
				if (root != nullptr)
					root->for_each(func);
			}
	};
}
