# app : CLI aplikacija za razvoj i debug jezgre (samo native build)

add_executable(mathengine_app main.cpp)

target_link_libraries(mathengine_app PRIVATE aksiomat_core)
