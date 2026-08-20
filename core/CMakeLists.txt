# core : matematička jezgra (aksiomat_core)

add_library(aksiomat_core STATIC
	src/LogicExpression.cpp
	src/LogicParser.cpp
	src/TruthTable.cpp
	src/LogicAnalysis.cpp
	src/NormalForms.cpp
	src/PredicateExpression.cpp
	src/PredicateParser.cpp
	src/Interpretation.cpp
	src/Arithmetic.cpp
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
