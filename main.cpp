#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <new>
#include <utility>
#include <vector>

#define dbg(expr)                                                              \
	std::cout << __FILE__ << ":" << __LINE__ << ": " << #expr << " = "     \
		  << (expr) << "\n"

#define todo() static_assert(false, "Function not implemented!");

template <typename T> class vector {
      private:
	const int VEC_BASE_SIZE = 8;
	T *first;
	T *last;     // write to this
	T *capacity; // capacity cannot be written to

	void expand_checked() {
		if (this->capacity == this->last) {
			int len = this->len();
			int new_size = len * 2;

			// move buffer
			T *new_buffer = static_cast<T *>(
			    ::operator new[](new_size * sizeof(T)));
			for (int i = 0; i < this->last - this->first; i++) {
				new (&new_buffer[i]) T(std::move((*this)[i]));
				(*this)[i].~T();
			}
			::operator delete[](this->first);

			this->first = new_buffer;
			this->capacity = this->first + new_size;
			this->last = this->first + len;
		}
	}

      public:
	vector() {
		first = (T *)::operator new[](sizeof(T) * VEC_BASE_SIZE);
		last = first;
		capacity = first + VEC_BASE_SIZE;
	}
	vector(vector<T> &other) {
		int alloc_size = other.size();
		first = (T *)::operator new[](alloc_size);
		last = first;
		capacity = first + alloc_size;
	}
	vector(vector<T> &&other)
	    : first(other.first), last(other.last), capacity(other.capacity) {}

	T *begin() { return this->first; }
	T *end() { return this->last; }
	~vector() {
		for (T *el = this->first; el < this->last; el++) {
			el->~T();
		}
		::operator delete[](this->first);
	}
	int len() { return this->last - this->first; }
	int size() { return this->capacity - this->first; }

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

	T &operator[](int index) { return this->first[index]; }
};

void not_leak() {
	vector<int> thing;
	const int test_size = 1 << 20;
	for (int i = 0; i < test_size; i++) {
		thing.push(i);
	}
}

int main() {
	vector<int> a;
	std::vector<int> std_vec;

	a.push(1);
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
	vector<int> b;
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
	for (int i = 0; i < 10; i++) {
		not_leak();
	}
}
