set(DJ_FFT_HEADER_FILES
    include/dj_fft.h
)

add_library(fft INTERFACE)
target_include_directories(fft INTERFACE include)

