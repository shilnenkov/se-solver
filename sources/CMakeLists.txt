find_package(Threads REQUIRED)

list(APPEND SOURCES
    consumer.cpp
    consumer.h
    main.cpp
    parameter_queue.h
    parameter_queue.cpp
    producer.cpp
    producer.h
    )

add_executable(se_solver ${SOURCES})
target_link_libraries(se_solver PRIVATE Threads::Threads)
