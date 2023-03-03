find_package(Threads REQUIRED)

list(APPEND SOURCES
    main.cpp
    parameter_queue.h
    parameter_queue.cpp
    )

add_executable(se_solver ${SOURCES})
target_link_libraries(se_solver PRIVATE Threads::Threads)
