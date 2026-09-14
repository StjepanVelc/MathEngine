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
	src/analytic_geometry/PointsVectors.cpp
	src/analytic_geometry/LinesDistances.cpp
	src/analytic_geometry/Circles.cpp
	src/analytic_geometry/Conics.cpp
	src/exponential_logarithmic/PowersRoots.cpp
	src/exponential_logarithmic/ExponentialFunctions.cpp
	src/exponential_logarithmic/Logarithms.cpp
	src/exponential_logarithmic/EquationSolvers.cpp
	src/exponential_logarithmic/Applications.cpp
	src/combinatorics_probability_statistics/Counting.cpp
	src/combinatorics_probability_statistics/ProbabilityBasics.cpp
	src/combinatorics_probability_statistics/DescriptiveStatistics.cpp
	src/combinatorics_probability_statistics/DataVisualization.cpp
	src/calculus_basics/Limits.cpp
	src/calculus_basics/Derivatives.cpp
	src/calculus_basics/DerivativeApplications.cpp
	src/calculus_basics/DefiniteIntegral.cpp
	src/mathematical_analysis/FormalLimits.cpp
	src/mathematical_analysis/AdvancedDerivatives.cpp
	src/mathematical_analysis/AdvancedIntegrals.cpp
	src/mathematical_analysis/FunctionSeries.cpp
	src/mathematical_analysis/MultivariableCalculus.cpp
	src/mathematical_analysis/DifferentialEquations.cpp
	src/linear_algebra/SpaceVectorsPlanes.cpp
	src/linear_algebra/Quadrics.cpp
	src/linear_algebra/Matrices.cpp
	src/linear_algebra/EigenAnalysis.cpp
	src/linear_algebra/VectorSpaces.cpp
	src/discrete_math/SetsRelations.cpp
	src/discrete_math/GraphTheory.cpp
	src/discrete_math/Recurrences.cpp
	src/discrete_math/AdvancedCombinatorics.cpp
	src/probability_statistics/Distributions.cpp
	src/probability_statistics/StatisticalInference.cpp
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
