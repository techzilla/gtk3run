
file(COPY main.glade DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

set(SOURCE
        ${SOURCE}
        ${CMAKE_CURRENT_SOURCE_DIR}/main.c
        )

include_directories(${GTK3_INCLUDE_DIRS})
add_executable(gtk3run main.c)
target_link_libraries(gtk3run ${GTK3_LIBRARIES})

