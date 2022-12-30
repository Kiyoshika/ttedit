add_library(ttewindow window.c)
target_include_directories(ttewindow PUBLIC ${TTEdit_SOURCE_DIR}/include)
target_link_libraries(ttewindow ncurses)

add_library(ttecursor cursor.c)
target_include_directories(ttecursor PUBLIC ${TTEdit_SOURCE_DIR}/include)

add_executable(ttedit main.c)
target_link_libraries(ttedit ncurses ttewindow ttecursor)
