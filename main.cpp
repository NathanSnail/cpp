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
	const static size_t VEC_BASE_SIZE = 8;
	T *first;
	T *last;     // write to this
	T *capacity; // capacity cannot be written to

	void expand_checked() {
		if (this->capacity == this->last) {
			size_t len = this->len();
			size_t new_size = len * 2;

			// move buffer
			T *new_buffer = static_cast<T *>(
			    ::operator new[](new_size * sizeof(T)));
			for (size_t i = 0; i < this->len(); i++) {
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
	vector(const vector<T> &other) {
		size_t alloc_size = other.size();
		first = (T *)::operator new[](alloc_size);
		last = first;
		capacity = first + alloc_size;
	}
	vector(vector<T> &&other)
	    : first(other.first), last(other.last), capacity(other.capacity) {
		other.first = nullptr;
		other.last = nullptr;
		other.capacity = nullptr;
	}

	T *begin() const { return this->first; }
	T *end() const { return this->last; }
	~vector() {
		for (T *el = this->first; el < this->last; el++) {
			el->~T();
		}
		::operator delete[](this->first);
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

	T erase(size_t index) {}

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

	T &operator[](size_t index) { return this->first[index]; }
	const T &operator[](size_t index) const { return this->first[index]; }
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
	vector<vector<int>> recursive;
	recursive.push(a);
}
