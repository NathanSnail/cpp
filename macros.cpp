#include "repeat.cpp"
#include <cstdio>

#define PRINTER(x) printf("%d\n", x);
#define GETTER(n, f, ...) f(NS_GET(n __VA_OPT__(, ) __VA_ARGS__))
#define GETTER_REDUCE(n, f, ...) f NS_GET(n __VA_OPT__(, ) __VA_ARGS__)
#define EACH(f, ...)                                                           \
	NS_EVAL(NS_REPEAT(NS_NARGS(__VA_ARGS__), GETTER,                       \
			  f __VA_OPT__(, ) __VA_ARGS__))
#define EACH_REDUCE(f, ...)                                                    \
	NS_EVAL(NS_REPEAT(NS_NARGS(__VA_ARGS__), GETTER_REDUCE,                \
			  f __VA_OPT__(, ) __VA_ARGS__))
#define FIELD(a, b) a b;
#define SHOW(a, b) printf("%s: %s\n", #b, #a);
#define STRUCTURE(name, ...)                                                   \
	typedef struct {                                                       \
		EACH_REDUCE(FIELD, __VA_ARGS__)                                \
	} name;                                                                \
	void NS_CONCAT(print_, name)() { EACH_REDUCE(SHOW, __VA_ARGS__) }

STRUCTURE(thingy, (int, a), (float, b), (const char *, c));

int main() {
	// NS_REPEAT(25, PRINTER);
	EACH(PRINTER, 1, 3, 6, 10);
	thingy x = {.a = 10, .b = 3.14, .c = "hi"};
	print_thingy();
}
