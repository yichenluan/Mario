set(net_SRCS
    EventLoop.cpp
    Channel.cpp
    Poller.cpp
    EventLoopThread.cpp
    Timer.cpp
    TimerQueue.cpp
    )

add_library(mario_net ${net_SRCS})
target_link_libraries(mario_net mario_base)
set_target_properties(mario_net PROPERTIES COMPILE_FLAGS "-std=c++11")

install(TARGETS mario_net DESTINATION lib)

set(HEADERS
    EventLoop.h
    Poller.h
    Channel.h
    Callbacks.h
    EventLoopThread.h
    Timer.h
    TimerId.h
    TimerQueue.h
    )

install(FILES ${HEADERS} DESTINATION include/mario/net)

if(NOT CMAKE_BUILD_NO_EXAMPLES)
    add_subdirectory(tests)
endif()
