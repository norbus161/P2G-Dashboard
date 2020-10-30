set(P2G_HEADER_FILES
    include/COMPort.h
    include/EndpointCalibration.h
    include/EndpointRadarAdcxmc.h
    include/EndpointRadarBase.h
    include/EndpointRadarDoppler.h
    include/EndpointRadarErrorCodes.h
    include/EndpointRadarFmcw.h
    include/EndpointRadarIndustrial.h
    include/EndpointRadarP2G.h
    include/EndpointTargetDetection.h
    include/Protocol.h
)

set(P2G_SOURCE_FILES
    src/EndpointCalibration.c
    src/EndpointRadarAdcxmc.c
    src/EndpointRadarBase.c
    src/EndpointRadarDoppler.c
    src/EndpointRadarErrorCodes.c
    src/EndpointRadarFmcw.c
    src/EndpointRadarIndustrial.c
    src/EndpointRadarP2G.c
    src/EndpointTargetDetection.c
    src/Protocol.c
)

if(LINUX)
    set(P2G_SERIAL_INTERFACE src/COMPort_Unix.c)
elseif(WIN32)
    set(P2G_SERIAL_INTERFACE src/COMPort_Windows.c)
else()
    message( FATAL_ERROR "Invalid platform for the P2G serial interface." )
endif()

include_directories(include)

add_library(p2g STATIC
    ${P2G_HEADER_FILES}
    ${P2G_SOURCE_FILES}
    ${P2G_SERIAL_INTERFACE})
