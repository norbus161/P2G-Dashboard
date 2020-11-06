set(PERSISTENCE1D_HEADER_FILES
    include/persistence1d.hpp
)

add_library(persistence1d INTERFACE)
target_include_directories(persistence1d INTERFACE include)

