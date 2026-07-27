// WASM bindings za aksiomat_core — kompajlira se SAMO pod Emscripten toolchainom.
// Native (MSVC) build ovu datoteku ne uključuje (vidi core/CMakeLists.txt).
#ifdef EMSCRIPTEN

#include <emscripten/bind.h>

#include "aksiomat/LogicExpression.hpp"

namespace {

// Jednostavan demo API za web: evaluira (p ∧ ¬q) — kasnije zamijeniti parserom.
bool demoEvaluate(bool p, bool q) {
	using aksiomat::LogicExpression;
	auto expr = LogicExpression::conjunction(
		LogicExpression::variable("p"),
		LogicExpression::negation(LogicExpression::variable("q")));
	return expr->evaluate({{"p", p}, {"q", q}});
}

std::string demoToString() {
	using aksiomat::LogicExpression;
	auto expr = LogicExpression::conjunction(
		LogicExpression::variable("p"),
		LogicExpression::negation(LogicExpression::variable("q")));
	return expr->toString();
}

} // namespace

EMSCRIPTEN_BINDINGS(aksiomat_module) {
	emscripten::function("demoEvaluate", &demoEvaluate);
	emscripten::function("demoToString", &demoToString);
}

#endif // EMSCRIPTEN
