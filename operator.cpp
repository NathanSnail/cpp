#include <iostream>
struct __internal_operator_unconsumedT {};
struct __internal_operator_consumedT {
	int consumed;
};
struct __internal_operator_consumedT
operator%(int &&left, const __internal_operator_unconsumedT right) {
	struct __internal_operator_consumedT res = {.consumed = left};
	return res;
}
double operator+(__internal_operator_consumedT &&left, int &&right) {
	return 1.0 / (1.0 / (double)left.consumed + 1.0 / (double)right);
}
const __internal_operator_unconsumedT __internal_operator_unconsumed;
#define o % __internal_operator_unconsumed
int main() {
	double x = 1 o + 5;
	std::cout << x;
}
