#include <algorithm>
#include <array>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cxxabi.h>
#include <functional>
#include <iostream>
#include <memory>
#include <new>
#include <optional>
#include <ostream>
#include <type_traits>
#include <typeinfo>
#include <utility>
// #include "macros.cpp"

#define dbg(expr)                                                              \
	std::cout << __FILE__ << ":" << __LINE__ << ": " << #expr << " = "     \
		  << (expr) << "\n"

#define todo() static_assert(false, "Function not implemented!");

#define debug_log(msg) std::cout << msg << "\n"

std::string operator""_s(const char *src, size_t len) {
	return std::string(src, len);
}

constexpr size_t operator""_szt(const unsigned long long src) {
	return static_cast<size_t>(src);
}

inline std::string demangle(const char *s) {
	const char *result = abi::__cxa_demangle(s, 0, 0, NULL);
	std::string str = std::string(result);
	free(const_cast<void *>(static_cast<const void *>(result)));
	return str;
}

inline std::string typeid_name(const std::type_info &T) {
	return demangle(T.name());
}

template <typename T> inline std::string type_name() {
	return typeid_name(typeid(T));
}

template <typename T, typename = void>
struct can_stringify : std::false_type {};
template <typename T>
struct can_stringify<T, std::void_t<decltype(std::declval<std::ostream &>()
					     << std::declval<T &>())>>
    : std::true_type {};

class Any {
      private:
	struct Generic {
		virtual ~Generic() =
		    default; // we need the vftable to include a destructor
		virtual std::ostream &psuedo_lshift(std::ostream &os) {
			os << "Unprintable Type";
			return os;
		};
		virtual const std::type_info &type() const = 0;
	};

	template <typename T> struct Specific : Generic {
		T data;
		Specific(const T &value) : data(value) {}
		virtual const std::type_info &type() const { return typeid(T); }
		virtual std::ostream &psuedo_lshift(std::ostream &os) {
			return lshift_impl(
			    os,
			    std::integral_constant<bool,
						   can_stringify<T>::value>{});
		}

	      private:
		std::ostream &lshift_impl(std::ostream &os, std::false_type) {
			return Generic::psuedo_lshift(os);
		}

		std::ostream &lshift_impl(std::ostream &os, std::true_type) {
			os << this->data;
			return os;
		}
	};

      public:
	std::unique_ptr<Generic> value;

	template <typename T>
	Any(const T &value) : value(std::make_unique<Specific<T>>(value)) {}
	template <typename T> std::optional<std::reference_wrapper<T>> data() {
		if (typeid(T) == value->type()) {
			return {static_cast<Specific<T> &>(*value).data};
		}
		return {std::nullopt};
	}
	const std::type_info &type() const { return value->type(); }
};

std::ostream &operator<<(std::ostream &os, Any &any) {
	os << "Any<" << typeid_name(any.type()) << ">(";
	any.value.get()->psuedo_lshift(os);
	os << ")";
	return os;
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

	void free_elements() {
		for (T *el = this->first; el < this->last; el++) {
			el->~T();
		}
		::operator delete(this->first);
	}

	void copy(const Vector<T> &other) {
		debug_log("copied");
		size_t alloc_size = other.size() * sizeof(T);
		this->first = static_cast<T *>(::operator new(alloc_size));
		size_t len = other.len();
		for (size_t i = 0; i < len; i++) {
			new (&this->first[i]) T(other[i]);
		}
		this->last = this->first + len;
		this->capacity = this->first + alloc_size;
	}

      public:
	Vector() {
		first =
		    static_cast<T *>(::operator new(sizeof(T) * VEC_BASE_SIZE));
		last = first;
		capacity = first + VEC_BASE_SIZE;
	}
	Vector(const Vector<T> &other) { copy(other); }
	template <size_t N>
	Vector<T> &operator=(const std::array<T, N> &other) {
		free_elements();
		constexpr size_t shift =
		    sizeof(size_t) * 8_szt - __builtin_clzl(N | 1);
		constexpr size_t shifted = 1_szt << shift;
		constexpr size_t alloc =
		    std::max(shifted, VEC_BASE_SIZE) * sizeof(T);
		this->first = static_cast<T *>(::operator new(alloc));
		this->last = this->first + N;
		for (size_t i = 0; i < N; i++) {
			new (&this->first[i]) T(other[i]);
		}
		this->capacity = this->first + shifted;
		return *this;
	}
	Vector<T> &operator=(const Vector<T> &other) {
		debug_log("copy assign");
		if (this == &other) {
			return *this;
		}
		free_elements();
		copy(other);
		return *this;
	}
	Vector<T> &operator=(Vector<T> &&other) {
		if (this == &other) {
			return *this;
		}
		free_elements();
		this->first = other.first;
		this->last = other.last;
		this->capacity = other.capacity;
		other.first = nullptr;
		other.last = nullptr;
		other.capacity = nullptr;
		return *this;
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
		if (this->first == nullptr) {
			return;
		}
		free_elements();
		debug_log("freeing");
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
		T elem = std::move(*this->last);
		this->last->~T();
		return elem;
	}

	T erase(size_t index) {
		assert(index < this->len());
		T elem = std::move((*this)[index]);
		(*this)[index].~T();
		for (size_t i = index; i < this->len() - 1; i++) {
			new (&(*this)[i]) T(std::move((*this)[i + 1]));
			(*this)[i + 1].~T();
		}
		this->last--;
		return elem;
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
	const int test_size = 1 << 16;
	for (int i = 0; i < test_size; i++) {
		thing.push(i);
	}
}

void test_arr() {
	std::array<int, 10> arr = {1, 2, 3, 4, 10, 9, 8, 7, 6, 5};
	Vector<int> arrvec;
	arrvec = arr;
	arrvec = arr;
	dbg(arrvec);
}

int main() {
	std::string s = "hello world"_s;
	std::cout << s << std::endl;
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
	const int test_size = 1 << 16;
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
	debug_log("hia");
	Vector<int> copy = a;
	dbg(a);
	dbg(copy);
	a.push(10);
	dbg(a);
	dbg(copy);
	copy.push(20);
	dbg(a);
	dbg(copy);
	Vector<int> copy2;
	copy2 = a;

	Vector<Vector<int>> recursive;
	dbg(a);
	a.push(1);
	recursive.push(a);
	dbg(a);
	recursive[0].push(20);
	a.push(11);
	recursive[0].push(30);
	recursive.push(a);
	dbg(recursive);
	recursive[1].erase(1);
	dbg(recursive);
	recursive[0].erase(1);
	dbg(recursive);
	recursive[0].erase(2);
	dbg(recursive);
	recursive[0].erase(2);
	dbg(recursive);

	test_arr();

	Any any(10);
	auto data = any.data<int>();
	if (data.has_value()) {
		data->get() = 20;
	}
	dbg(any);

	struct empty {};
	struct empty trash;
	any = trash;
	dbg(any);
}
