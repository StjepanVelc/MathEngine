# tests : GoogleTest testovi jezgre (samo native build)

include(FetchContent)

FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
# Windows: koristi isti runtime kao ostatak projekta.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(core_tests
	test_logic_expression.cpp
)

target_link_libraries(core_tests PRIVATE
	aksiomat_core
	GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(core_tests)
