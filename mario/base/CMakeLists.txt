set(base_SRCS
    CountDownLatch.cpp
    Thread.cpp
    CurrentThread.cpp
    easylogging++.cc
    )

add_library(mario_base ${base_SRCS})
target_link_libraries(mario_base pthread)
set_target_properties(mario_base PROPERTIES COMPILE_FLAGS "-std=c++11")

install(TARGETS mario_base DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/mario/base)

if(NOT CMAKE_BUILD_NO_EXAMPLES)
    add_subdirectory(tests)
endif()
