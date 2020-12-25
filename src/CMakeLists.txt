add_subdirectory(gui)
add_subdirectory(logic)
add_subdirectory(misc)

set(SOURCE
    ${SOURCE}
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    PARENT_SCOPE
)
set(HEADERS
    ${HEADERS}
    PARENT_SCOPE
)






