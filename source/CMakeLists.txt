include_directories(${COMMON_INCLUDE_DIRS})
link_directories(${COMMON_LIBDIR})

set(planar_core_LIB_SRCS
    debug.cpp
    protocol.cpp
    serial_connection.cpp)

add_library(planar_core SHARED ${planar_core_LIB_SRCS})

if(UNIX)
  target_link_libraries(planar_core pthread ${Boost_LIBRARIES})
endif(UNIX)

install(TARGETS planar_core DESTINATION lib/${PROJECT_NAME})

# FIXME: This header must be in external directory
install(FILES
            serial_connection.h
        DESTINATION include/${PROJECT_NAME})
