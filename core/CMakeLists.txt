# core : matematička jezgra (aksiomat_core)

add_library(aksiomat_core STATIC
	src/arithmetic/ArithmeticExpression.cpp
	src/arithmetic/ArithmeticParser.cpp
	src/arithmetic/ArithmeticEvaluator.cpp
	src/arithmetic/Arithmetic.cpp
	src/arithmetic/Rational.cpp
	src/arithmetic/Percentages.cpp
	src/arithmetic/NumberTheory.cpp
	src/arithmetic/NumeralSystems.cpp
	src/algebra/AlgebraExpression.cpp
	src/algebra/AlgebraParser.cpp
	src/algebra/AlgebraFormatter.cpp
	src/algebra/AlgebraSimplifier.cpp
	src/algebra/LinearForm.cpp
	src/algebra/EquationSolver.cpp
	src/algebra/InequalitySolver.cpp
	src/algebra/LinearSystemSolver.cpp
	src/algebra/Polynomial.cpp
	src/algebra/FunctionAnalyzer.cpp
	src/geometry/UnitConversion.cpp
	src/geometry/PlaneShapes.cpp
	src/geometry/Triangles.cpp
	src/geometry/Solids.cpp
	src/geometry/Coordinates.cpp
	src/trigonometry/Angles.cpp
	src/trigonometry/TrigFunctions.cpp
	src/trigonometry/RightTriangle.cpp
	src/trigonometry/GeneralTriangle.cpp
	src/trigonometry/IdentitiesEquations.cpp
	src/sequences/SequenceAnalysis.cpp
	src/sequences/ArithmeticSequence.cpp
	src/sequences/GeometricSequence.cpp
	src/sequences/Recurrences.cpp
	src/sequences/Applications.cpp
	src/logic/LogicExpression.cpp
	src/logic/LogicParser.cpp
	src/logic/TruthTable.cpp
	src/logic/LogicAnalysis.cpp
	src/logic/NormalForms.cpp
	src/predicate/PredicateExpression.cpp
	src/predicate/PredicateParser.cpp
	src/predicate/Interpretation.cpp
)

target_include_directories(aksiomat_core PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(aksiomat_core PUBLIC cxx_std_20)

if (EMSCRIPTEN)
	# WASM modul — builda se samo pod Emscripten toolchainom.
	add_executable(aksiomat_wasm src/wasm_bindings.cpp)
	target_link_libraries(aksiomat_wasm PRIVATE aksiomat_core)
	set_target_properties(aksiomat_wasm PROPERTIES
		OUTPUT_NAME "aksiomat"
		SUFFIX ".js"
	)
	# C++ iznimke moraju biti uključene da ih bindings mogu uhvatiti u WASM-u.
	target_compile_options(aksiomat_core PRIVATE "-fexceptions")
	target_compile_options(aksiomat_wasm PRIVATE "-fexceptions")
	target_link_options(aksiomat_wasm PRIVATE
		"-sMODULARIZE=1"
		"-sEXPORT_NAME=createAksiomatModule"
		"--bind"
		"-fexceptions"
	)

	# Nakon svakog builda automatski kopiraj aksiomat.js i aksiomat.wasm u web/.
	add_custom_command(TARGET aksiomat_wasm POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_FILE_DIR:aksiomat_wasm>/aksiomat.js
			$<TARGET_FILE_DIR:aksiomat_wasm>/aksiomat.wasm
			${CMAKE_SOURCE_DIR}/web
		COMMENT "Kopiram aksiomat.js i aksiomat.wasm u web/"
	)
endif()
