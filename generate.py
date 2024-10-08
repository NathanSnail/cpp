src = f"""
#define NS_CONCAT(a, b) a##b
#define NS_REPEAT(n, f, ...) NS_CONCAT(NS_REPEAT, n) (f __VA_OPT__(,) __VA_ARGS__)
#define NS_REPEAT0(f, ...)
#define NS_NARGS_HELPER({"".join([f"__{str(i)}, " for i in range(100)])}N, ...) N
#define NS_NARGS(...) NS_NARGS_HELPER(__VA_ARGS__ __VA_OPT__(,) {"".join([f"{str(i)}, " for i in range(100 + 1)[::-1]])[:-2]})
#define NS_GET(n, ...) NS_CONCAT(NS_GET, n) (__VA_ARGS__)
"""
for i in range(99):
    src += f"""
#define NS_REPEAT{i + 1}(f, ...) NS_REPEAT{i}(f __VA_OPT__(,) __VA_ARGS__) f({i} __VA_OPT__(,) __VA_ARGS__)"""

for i in range(100):
    src += f"""
#define NS_GET{i}({"".join([f"__{str(j)}, " for j in range(i)])}N, ...) N"""

# we hit the max nesting really fast so no point autogenerating
src += """
#define NS_EVAL(...)  NS_EVAL1(NS_EVAL1(NS_EVAL1(__VA_ARGS__)))
#define NS_EVAL1(...) NS_EVAL2(NS_EVAL2(NS_EVAL2(__VA_ARGS__)))
#define NS_EVAL2(...) NS_EVAL3(NS_EVAL3(NS_EVAL3(__VA_ARGS__)))
#define NS_EVAL3(...) NS_EVAL4(NS_EVAL4(NS_EVAL4(__VA_ARGS__)))
#define NS_EVAL4(...) NS_EVAL5(NS_EVAL5(NS_EVAL5(__VA_ARGS__)))
#define NS_EVAL5(...) __VA_ARGS__
"""
open("repeat.cpp", "w").write(src)
