# tests : GoogleTest testovi jezgre (samo native build)

include(FetchContent)

FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# Windows: koristi isti runtime kao ostatak projekta.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(core_tests
	test_logic_expression.cpp
	test_logic_parser.cpp
	test_truth_table.cpp
	test_logic_analysis.cpp
	test_normal_forms.cpp
	test_predicate_logic.cpp
	test_arithmetic.cpp
	test_rational.cpp
	test_percentages.cpp
	test_number_theory.cpp
	test_numeral_systems.cpp
	test_algebra_parser.cpp
	test_algebra_simplifier.cpp
	test_equation_solver.cpp
	test_inequality_solver.cpp
	test_linear_system_solver.cpp
	test_polynomial.cpp
	test_function_analyzer.cpp
	test_geometry_unit_conversion.cpp
	test_geometry_plane_shapes.cpp
	test_geometry_triangles.cpp
	test_geometry_solids_coordinates.cpp
	test_trigonometry_angles.cpp
	test_trigonometry_functions.cpp
	test_trigonometry_right_triangle.cpp
	test_trigonometry_general_triangle.cpp
	test_trigonometry_identities_equations.cpp
)

target_link_libraries(core_tests PRIVATE
	aksiomat_core
	GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(core_tests)
