#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cxxabi.h>
#include <iostream>
#include <new>
#include <ostream>
#include <utility>

#define dbg(expr)                                                              \
	std::cout << __FILE__ << ":" << __LINE__ << ": " << #expr << " = "     \
		  << (expr) << "\n"

#define todo() static_assert(false, "Function not implemented!");

#define debug_log(msg) std::cout << msg << "\n"

inline std::string demangle(const char *s) {
	const char *result = abi::__cxa_demangle(s, 0, 0, NULL);
	std::string str = std::string(result);
	free(const_cast<void *>(static_cast<const void *>(result)));
	return str;
}

template <typename T> inline std::string type_name() {
	return demangle(typeid(T).name());
}

template <typename T> class Vector {
      private:
	const static size_t VEC_BASE_SIZE = 8;
	T *first;
	T *last;     // write to this
	T *capacity; // capacity cannot be written to

	void expand_checked() {
		if (this->capacity == this->last) {
			debug_log("expanding");
			size_t len = this->len();
			size_t new_size = len * 2;

			// move buffer
			T *new_buffer = static_cast<T *>(
			    ::operator new(new_size * sizeof(T)));
			for (size_t i = 0; i < this->len(); i++) {
				new (&new_buffer[i]) T(std::move((*this)[i]));
				(*this)[i].~T();
			}
			::operator delete(this->first);

			this->first = new_buffer;
			this->capacity = this->first + new_size;
			this->last = this->first + len;
		}
	}

      public:
	Vector() {
		first = (T *)::operator new(sizeof(T) * VEC_BASE_SIZE);
		last = first;
		capacity = first + VEC_BASE_SIZE;
	}
	Vector(const Vector<T> &other) {
		debug_log("copied");
		size_t alloc_size = other.size() * sizeof(T);
		first = (T *)::operator new(alloc_size);
		size_t len = other.len();
		for (size_t i = 0; i < len; i++) {
			new (&first[i]) T(other[i]);
		}
		last = first + len;
		capacity = first + alloc_size;
	}
	Vector(Vector<T> &&other)
	    : first(other.first), last(other.last), capacity(other.capacity) {
		debug_log("moved");
		other.first = nullptr;
		other.last = nullptr;
		other.capacity = nullptr;
	}

	T *begin() const { return this->first; }
	T *end() const { return this->last; }
	~Vector() {
		debug_log("freeing");
		for (T *el = this->first; el < this->last; el++) {
			el->~T();
		}
		::operator delete(this->first);
	}
	size_t len() const { return this->last - this->first; }
	size_t size() const { return this->capacity - this->first; }

	void push(T &&elem) {
		expand_checked();
		new (this->last) T(std::move(elem));
		this->last++;
	}
	void push(const T &elem) {
		expand_checked();
		new (this->last) T(elem);
		this->last++;
	}

	T pop() {
		assert(this->last != this->first);
		this->last--;
		return std::move(*this->last);
	}

	T erase(size_t index) { T elem = std::move(this[index]); }

	void shrink() {
		/*if ((this->capacity - this->first) / 4 + this->first >=
			this->last &&
		    this->capacity - this->first > VEC_BASE_SIZE) {
			int len = this->len();
			int new_size = std::max<int>(this->last - this->first,
						     VEC_BASE_SIZE);
			this->first =
			    (T *)realloc(this->first, new_size * sizeof(T));
			this->last = this->first + len;
			this->capacity = this->first + new_size;
		}*/
		todo();
	}

	T &operator[](size_t index) {
		assert(index < this->len());
		return this->first[index];
	}
	const T &operator[](size_t index) const {
		assert(index < this->len());
		return this->first[index];
	}
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const Vector<T> &vec) {

	os << "Vector<" << type_name<T>() << ">[";
	for (size_t i = 0; i < vec.len(); i++) {
		os << vec[i];
		if (i != vec.len() - 1) {
			os << ", ";
		}
	}
	os << "]";
	return os;
}

void not_leak() {
	Vector<int> thing;
	const int test_size = 1 << 20;
	for (int i = 0; i < test_size; i++) {
		thing.push(i);
	}
}

int main() {
	Vector<int> a;

	a.push(10);
	dbg(a[0]);
	a[0] = 2;
	dbg(a[0]);

	int x = 7;
	a.push(x);
	a[1] = 6;
	dbg(x);
	dbg(a[1]);
	for (int elem : a) {
		dbg(elem);
	}
	Vector<int> b;
	const int test_size = 1 << 20;
	for (int i = 0; i < test_size; i++) {
		b.push(i);
	}
	for (int i = 0; i < test_size; i++) {
		assert(b.pop() == test_size - i - 1);
	}
	dbg(b.size());
	// b.shrink();
	dbg(b.size());
	for (int i = 0; i < 3; i++) {
		not_leak();
	}
	Vector<int> copy = a;
	dbg(copy);

	Vector<Vector<int>> recursive;
	dbg(a);
	a.push(1);
	recursive.push(a);
	dbg(a);
	recursive[0].push(20);
	recursive[0].push(30);
	dbg(recursive);
}
