#include "repeat.cpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>

char *stringify(const char *ty, void *data) {
	if (!strcmp(ty, "int")) {
		char *s =
		    (char *)malloc(50); // this is bound to be enough digits
		sprintf(s, "%d", *(int *)data);
		return s;
	} else if (!strcmp(ty, "float")) {
		char *s = (char *)malloc(50);
		sprintf(s, "%.5f", *(float *)data);
		return s;
	} else if (!strcmp(ty, "char *") || !strcmp(ty, "const char *")) {
		char *s = (char *)malloc(strlen(*(char **)data));
		strcpy(s, *(char **)data);
		return s;
	} else {
		exit(1);
	}
}

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
#define SHOW(a, b)                                                             \
	{                                                                      \
		char *s = stringify(#a, &thing->b);                            \
		printf("thing->%s: %s = %s\n", #b, #a, s);                     \
		free(s);                                                       \
	}
#define STRUCTURE(name, ...)                                                   \
	typedef struct {                                                       \
		EACH_REDUCE(FIELD, __VA_ARGS__)                                \
	} name;                                                                \
	void NS_CONCAT(print_, name)(name * thing) {                           \
		EACH_REDUCE(SHOW, __VA_ARGS__)                                 \
	}
STRUCTURE(thingy, (int, a), (float, b), (const char *, c));

#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)
#define TYPE(x) STRINGIFY(NS_EVAL(typeof(x)))
int main() {
	// NS_REPEAT(25, PRINTER);
	EACH(PRINTER, 1, 3, 6, 10);
	thingy x = {.a = 10, .b = 3.14, .c = "hi"};
	print_thingy(&x);
}
