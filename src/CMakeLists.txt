
set(SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/main.c
        )

set(HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/main.h
)

include_directories(${GTK3_INCLUDE_DIRS})
add_executable(gtk3run ${SOURCE} ${HEADER})
target_link_libraries(gtk3run ${GTK3_LIBRARIES})

