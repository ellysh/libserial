include_directories(${COMMON_INCLUDE_DIRS})
link_directories(${COMMON_LIBDIR})

set(serial_LIB_SRCS
    debug_serial.cpp
    serial_connection.cpp
    serial_receive.cpp
    serial_server.cpp
    serial_send.cpp)

add_library(serial SHARED ${serial_LIB_SRCS})

if(UNIX)
  target_link_libraries(serial pthread ${Boost_LIBRARIES})
endif(UNIX)

install(TARGETS serial DESTINATION lib/${PROJECT_NAME})

install(FILES
            debug_serial.h
            types_serial.h
            serial_connection.h
            serial_server.h
        DESTINATION include/${PROJECT_NAME})
