// WASM bindings za aksiomat_core — kompajlira se SAMO pod Emscripten toolchainom.
// Native (MSVC) build ovu datoteku ne uključuje (vidi core/CMakeLists.txt).
#ifdef __EMSCRIPTEN__

#include <emscripten/bind.h>

#include "aksiomat/Arithmetic.hpp"
#include "aksiomat/LogicExpression.hpp"
#include "aksiomat/LogicParser.hpp"
#include "aksiomat/PredicateLogic.hpp"
#include "aksiomat/TruthTable.hpp"

namespace {

// Parsira formulu i vraća njen normalizirani prikaz ili "GRESKA: ...".
std::string logicToString(std::string formula) {
	try {
		return aksiomat::LogicParser::parse(formula)->toString();
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

// Vraća varijable formule odvojene zarezom (npr. "p,q,r") ili "GRESKA: ...".
std::string logicVariables(std::string formula) {
	try {
		auto expr = aksiomat::LogicParser::parse(formula);
		std::string result;
		for (const auto& name : aksiomat::LogicParser::collectVariables(expr)) {
			if (!result.empty()) {
				result += ',';
			}
			result += name;
		}
		return result;
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

// Evaluira formulu uz valuaciju oblika "p=1,q=0"; vraća "T", "F" ili "GRESKA: ...".
std::string logicEvaluate(std::string formula, std::string valuation) {
	try {
		auto expr = aksiomat::LogicParser::parse(formula);
		aksiomat::LogicExpression::Valuation values;
		std::size_t pos = 0;
		while (pos < valuation.size()) {
			std::size_t eq = valuation.find('=', pos);
			if (eq == std::string::npos) {
				break;
			}
			std::size_t end = valuation.find(',', eq);
			if (end == std::string::npos) {
				end = valuation.size();
			}
			values[valuation.substr(pos, eq - pos)] = (valuation.substr(eq + 1, end - eq - 1) == "1");
			pos = end + 1;
		}
		return expr->evaluate(values) ? "T" : "F";
	} catch (const std::out_of_range&) {
		return "GRESKA: Nedostaje vrijednost varijable";
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

// Evaluira aritmetički izraz; vraća rezultat kao string ili poruku greške s prefiksom "GRESKA: ".
std::string evaluateArithmetic(std::string expression) {
	try {
		double result = aksiomat::Arithmetic::evaluate(expression);
		std::string text = std::to_string(result);
		// Skrati suvišne nule (npr. "5.000000" -> "5").
		text.erase(text.find_last_not_of('0') + 1);
		if (!text.empty() && text.back() == '.') {
			text.pop_back();
		}
		return text;
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

// Generira tablicu istinitosti kao JSON: {"variables":[...],"rows":[{"values":[0,1,...],"result":1},...]}
// ili "GRESKA: ...".
std::string truthTable(std::string formula) {
	try {
		auto expr = aksiomat::LogicParser::parse(formula);
		auto table = aksiomat::TruthTable::generate(expr);
		std::string json = "{\"variables\":[";
		for (std::size_t i = 0; i < table.variables.size(); ++i) {
			if (i > 0) json += ',';
			json += '"' + table.variables[i] + '"';
		}
		json += "],\"rows\":[";
		for (std::size_t r = 0; r < table.rows.size(); ++r) {
			if (r > 0) json += ',';
			json += "{\"values\":[";
			for (std::size_t i = 0; i < table.variables.size(); ++i) {
				if (i > 0) json += ',';
				json += table.rows[r].valuation.at(table.variables[i]) ? '1' : '0';
			}
			json += "],\"result\":";
			json += table.rows[r].result ? '1' : '0';
			json += '}';
		}
		return json + "]}";
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

// Provjerava logičku ekvivalenciju dviju formula; vraća "T", "F" ili "GRESKA: ...".
std::string logicEquivalent(std::string lhs, std::string rhs) {
	try {
		return aksiomat::TruthTable::areEquivalent(aksiomat::LogicParser::parse(lhs),
												   aksiomat::LogicParser::parse(rhs))
				   ? "T" : "F";
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

// Parsira predikatnu formulu i vraća normalizirani prikaz ili "GRESKA: ...".
std::string predicateToString(std::string formula) {
	try {
		return aksiomat::PredicateParser::parse(formula)->toString();
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

// Vraća predikate formule kao "P/1,Q/2" (ime/mjesnost) ili "GRESKA: ...".
std::string predicatePredicates(std::string formula) {
	try {
		auto expr = aksiomat::PredicateParser::parse(formula);
		std::string result;
		for (const auto& [name, arity] : aksiomat::PredicateParser::collectPredicates(expr)) {
			if (!result.empty()) result += ',';
			result += name + '/' + std::to_string(arity);
		}
		return result;
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

// Evaluira predikatnu formulu nad interpretacijom.
// domain: elementi odvojeni zarezom, npr. "1,2,3"
// facts:  činjenice odvojene točka-zarezom, npr. "P(1);Q(1,2);Q(2,3)"
// Vraća "T", "F" ili "GRESKA: ...".
std::string predicateEvaluate(std::string formula, std::string domain, std::string facts) {
	try {
		auto expr = aksiomat::PredicateParser::parse(formula);

		aksiomat::Interpretation interp;
		std::vector<std::string> elements;
		std::size_t pos = 0;
		while (pos <= domain.size() && !domain.empty()) {
			std::size_t end = domain.find(',', pos);
			if (end == std::string::npos) end = domain.size();
			if (end > pos) elements.push_back(domain.substr(pos, end - pos));
			pos = end + 1;
		}
		interp.setDomain(std::move(elements));

		pos = 0;
		while (pos < facts.size()) {
			std::size_t end = facts.find(';', pos);
			if (end == std::string::npos) end = facts.size();
			const std::string fact = facts.substr(pos, end - pos);
			pos = end + 1;
			const std::size_t open = fact.find('(');
			const std::size_t close = fact.rfind(')');
			if (open == std::string::npos || close == std::string::npos || close < open) {
				continue;
			}
			const std::string name = fact.substr(0, open);
			std::vector<std::string> args;
			std::size_t argPos = open + 1;
			while (argPos < close) {
				std::size_t argEnd = fact.find(',', argPos);
				if (argEnd == std::string::npos || argEnd > close) argEnd = close;
				if (argEnd > argPos) args.push_back(fact.substr(argPos, argEnd - argPos));
				argPos = argEnd + 1;
			}
			interp.addFact(name, std::move(args));
		}

		return interp.evaluate(expr) ? "T" : "F";
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

} // namespace

EMSCRIPTEN_BINDINGS(aksiomat_module) {
	emscripten::function("logicToString", &logicToString);
	emscripten::function("logicVariables", &logicVariables);
	emscripten::function("logicEvaluate", &logicEvaluate);
	emscripten::function("truthTable", &truthTable);
	emscripten::function("logicEquivalent", &logicEquivalent);
	emscripten::function("predicateToString", &predicateToString);
	emscripten::function("predicatePredicates", &predicatePredicates);
	emscripten::function("predicateEvaluate", &predicateEvaluate);
	emscripten::function("evaluateArithmetic", &evaluateArithmetic);
}

#endif // __EMSCRIPTEN__
