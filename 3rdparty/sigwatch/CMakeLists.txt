if(UNIX)

    set(SIGWATCH_HEADER_FILES
        include/sigwatch.h
    )

    set(SIGWATCH_SOURCE_FILES
        src/sigwatch.cpp
    )

    include_directories(include)

    add_library(sigwatch STATIC
        ${SIGWATCH_HEADER_FILES}
        ${SIGWATCH_SOURCE_FILES})
    target_link_libraries(sigwatch PRIVATE Qt5::Core)

endif()
