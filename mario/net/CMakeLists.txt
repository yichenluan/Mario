set(net_SRCS
    EventLoop.cpp
    Channel.cpp
    Poller.cpp
    EventLoopThread.cpp
    )

add_library(mario_net ${net_SRCS})
target_link_libraries(mario_net mario_base)
set_target_properties(mario_net PROPERTIES COMPILE_FLAGS "-std=c++11")

install(TARGETS mario_net DESTINATION lib)

set(HEADERS
    EventLoop.h
    Poller.h
    Channel.h
    )

install(FILES ${HEADERS} DESTINATION include/mario/net)

if(NOT CMAKE_BUILD_NO_EXAMPLES)
    add_subdirectory(tests)
endif()
