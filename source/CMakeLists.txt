include_directories(${COMMON_INCLUDE_DIRS})
link_directories(${COMMON_LIBDIR})

set(serial_LIB_SRCS
    debug.cpp
    program_options.cpp
    serial_connection.cpp
    serial_server.cpp)

add_library(serial SHARED ${serial_LIB_SRCS})

if(UNIX)
  target_link_libraries(serial pthread ${Boost_LIBRARIES})
endif(UNIX)

install(TARGETS serial DESTINATION lib/${PROJECT_NAME})

install(FILES
            debug.h
            types_serial.h
            program_options.h
            serial_connection.h
            serial_server.h
            singleton.h
        DESTINATION include/${PROJECT_NAME})
