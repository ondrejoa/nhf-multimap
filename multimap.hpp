#pragma once
#include <stdexcept>
#include <utility>
#include <functional>
#include <type_traits>
#include <iostream>
#include <algorithm> // ...

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
		public:
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
					bool operator==(const iterator &it) const { return li == it.li; }
					bool operator!=(const iterator &it) const { return li != it.li; }
					T &operator*() { return li->data; }
					T *operator->() { return &li->data; }
			};
			list() {}
			list(list &&val) {
				first = val.first;
				val.first = nullptr;
			}
			list(const list&) = delete;
			~list() {
				clear();
			}
			std::size_t size() { // Nem tudom const-ra :(
				std::size_t n = 0;
				for (auto it = begin(); it != end(); ++it) n++;
				return n;
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
	struct nohash {
		T operator()(T val) { return val; }
	};

	template<typename T, typename U, typename HASH = nohash<T>>
	class multimap {
		private:
			struct binarytree_node {
				binarytree_node *lc = nullptr;
				binarytree_node *rc = nullptr;
				list<U> values;
				struct ukey {
					T key;
					typename std::result_of<HASH(T)>::type hash;
					ukey(T key, typename std::result_of<HASH(T)>::type hash): key(key), hash(hash) {}
					ukey(const ukey &k): key(k.key), hash(k.hash) {}
					bool hash_matched = false;
					bool operator==(const ukey &k) {
						if (hash == k.hash) { 
							hash_matched = true;
							if (key == k.key) 
								return true;
						}
						return false;
					}
					bool operator>(const ukey &k) {
						if (hash_matched) 
							return key > k.key;
						return hash > k.hash;
					}
					bool operator<(const ukey &k) {
						if (hash_matched) 
							return key < k.key;
						return hash < k.hash;
					}
				} key;
				template<typename ... Args>
				binarytree_node(ukey &key, Args&& ... args): key(key) {
					values.emplace_front(std::forward<Args>(args)...);
				}
				template<typename ... Args>
				void insert(ukey &key, Args&& ... args) {
					if (key == this->key) {
						values.emplace_front(std::forward<Args>(args)...);
					}
					else if (key < this->key) {
						if (lc == nullptr) {
							lc = new binarytree_node(key, std::forward<Args>(args)...);
						}
						else {
							lc->insert(key, std::forward<Args>(args)...);
						}
					}
					else {
						if (rc == nullptr) {
							rc = new binarytree_node(key, std::forward<Args>(args)...);
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
						delete rc;
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
				binarytree_node *search(ukey &key) {
					if (key == this->key) {
						return this;
					}
					else if (key < this->key) {
						if (lc != nullptr) {
							return lc->search(key);
						}
					}
					else {
						if (rc != nullptr) {
							return rc->search(key);
						}
					}
					return nullptr;
				}
				binarytree_node *extract(ukey &key, binarytree_node **parent) {
					if (key == this->key) {
						std::size_t subsize = 0;
						if (lc != nullptr) subsize += lc->nodecount();
						if (rc != nullptr) subsize += rc->nodecount();
						builder b(subsize);
						if (lc != nullptr) b.pushr(lc);
						if (rc != nullptr) b.pushr(rc);
						*parent = b.get_root();
						return this;
					}
					else if(key < this->key) {
						if (lc != nullptr) {
							return lc->extract(key, &lc);
						}
					}
					else {
						if (rc != nullptr) {
							return rc->extract(key, &rc);
						}
					}
					return nullptr;
				}
				std::size_t size() {
					std::size_t s = 0;
					if (lc != nullptr)
						s += lc->size();
					if (rc != nullptr)
						s += rc->size();
					return s + values.size();
				}
				std::size_t nodecount() {
					std::size_t s = 0;
					if (lc != nullptr)
						s += lc->nodecount();
					if (rc != nullptr)
						s += rc->nodecount();
					return s + 1;
				}
			};
			binarytree_node *root = nullptr;
			class builder {
				private:
					binarytree_node **nodes;
					std::size_t size;
					std::size_t stored = 0;
				public:
					builder(std::size_t l): nodes(new binarytree_node*[l]), size(l) {
						std::cout << "Storing: " << l << "\n";
					}
					~builder() {
						delete []nodes;
					}
					void push(binarytree_node *n) {
						nodes[stored++] = n;
					}
					void pushr(binarytree_node *n) {
						push(n);
						if (n->lc != nullptr)
							pushr(n->lc);
						if (n->rc != nullptr)
							pushr(n->rc);
					}
					binarytree_node *get_root() { // CALL IT ONLY ONCE
						if (stored == 0) return nullptr;
						std::sort(nodes, nodes + stored,
								[](binarytree_node *lhs, binarytree_node *rhs) {
									if (lhs->key == rhs->key)
										return false;
									if (lhs->key < rhs->key) {
										lhs->key.hash_matched = false;
										return true;
									}
									return false;
								}
						);
						std::function<binarytree_node*(int, int)> getnr = [this, &getnr](int n, int off) -> binarytree_node* {
							if (n >= stored || n < 0) return nullptr;
							if (nodes[n] == nullptr) return nullptr;
							binarytree_node *ezitt = nodes[n];
							nodes[n] = nullptr;
							ezitt->lc = getnr(n - std::clamp(off / 2, 1, off), std::clamp(off / 2, 1, off));
							ezitt->rc = getnr(n + std::clamp(off / 2, 1, off), std::clamp(off / 2, 1, off));
							return ezitt;
						};
						binarytree_node *r = getnr(stored / 2, stored / 2);
						stored = 0;
						return r;
					}
			};
		public:
			~multimap() { clear(); }
			void clear() {
				if (root != nullptr) root->free();
				delete root;
				root = nullptr;
			}
			template<typename ... Args>
			void emplace(T key ,Args&& ... args) {
				typename binarytree_node::ukey ukey(key, HASH{}(key));
				if (root == nullptr) {
					root = new binarytree_node(ukey, std::forward<Args>(args)...);
				}
				else {
					root->insert(ukey, std::forward<Args>(args)...);
				}
			}
			void for_each(std::function<void(T&, U&)> func) {
				if (root != nullptr)
					root->for_each(func);
			}
			pair<typename list<U>::iterator, typename list<U>::iterator> equal_range(T &key) {
				typename binarytree_node::ukey ukey(key, HASH{}(key));
				binarytree_node *res = nullptr;
				if (root != nullptr)
					res = root->search(ukey);
				if (res != nullptr)
					return pair<typename list<U>::iterator, typename list<U>::iterator>(res->values.begin(), res->values.end());
				return pair<typename list<U>::iterator, typename list<U>::iterator>(nullptr, nullptr);
			}
			std::size_t size() {
				if (root != nullptr)
					return root->size();
				return 0;
			}
			void balance() {
				if (root == nullptr) return;
				builder b(root->nodecount());
				b.pushr(root);
				root = b.get_root();
			}
			void read(std::istream &is) {
				// TODO
			}
			void write(std::ostream &os) {
				// TODO
			}
			multimap &merge(multimap &m) {
				if (root == nullptr) {
					root = m.root;
					m.root = nullptr;
					return *this;;
				}
				if (m.root == nullptr)
					return *this;
				m.for_each([this](T &key, U &val){
							emplace(key, std::move(val));
						});
				m.clear();
				balance();
				return *this;
			}
			void erase(T key) {
				extract(key);
			}
			list<U> extract(T key) {
				typename binarytree_node::ukey ukey(key, HASH{}(key));
				binarytree_node *res = nullptr;
				if (root != nullptr) 
					res = root->extract(ukey, &root);
				if (res != nullptr) {
					list<U> l(std::move(res->values));
					delete res;
					return std::move(l);
				}
				return list<U>{};
			}
	};
}
