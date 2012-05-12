include_directories(${COMMON_INCLUDE_DIRS})
link_directories(${COMMON_LIBDIR})

set(serial_LIB_SRCS
    debug.cpp
    debug_client.cpp
    program_options.cpp
    serial_connection.cpp
    serial_receive.cpp
    serial_server.cpp
    serial_send.cpp
    state_client_wrap.cpp)

add_library(serial SHARED ${serial_LIB_SRCS})

if(UNIX)
  target_link_libraries(serial pthread mysql_state ${Boost_LIBRARIES})
endif(UNIX)

install(TARGETS serial DESTINATION lib/${PROJECT_NAME})

install(FILES
            debug.h
            debug_client.h
            types_serial.h
            program_options.h
            serial_connection.h
            serial_receive.h
            serial_server.h
            serial_send.h
            singleton.h
        DESTINATION include/${PROJECT_NAME})
