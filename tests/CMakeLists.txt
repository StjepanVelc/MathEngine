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
)

target_link_libraries(core_tests PRIVATE
	aksiomat_core
	GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(core_tests)
