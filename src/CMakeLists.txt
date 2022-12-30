add_library(ttewindow window.c)
target_include_directories(ttewindow PUBLIC ${TTEdit_SOURCE_DIR}/include)
target_link_libraries(ttewindow ncurses)

add_library(ttecursor cursor.c)
target_include_directories(ttecursor PUBLIC ${TTEdit_SOURCE_DIR}/include)

add_library(tteline line.c)
target_include_directories(tteline PUBLIC ${TTEdit_SOURCE_DIR}/include)

add_library(ttelinearray line_array.c)
target_include_directories(ttelinearray PUBLIC ${TTEdit_SOURCE_DIR}/include)

add_library(ttescreen screen_buffer.c)
target_include_directories(ttescreen PUBLIC ${TTEdit_SOURCE_DIR}/include)
target_link_libraries(ttescreen ttelinearray)

add_library(tteeditmode edit_mode.c)
target_include_directories(tteeditmode PUBLIC ${TTEdit_SOURCE_DIR}/include)
target_link_libraries(tteeditmode ttecursor ttescreen tteline)

add_executable(ttedit main.c)
target_link_libraries(ttedit 
	ncurses 
	ttewindow 
	ttecursor 
	tteline 
	ttelinearray 
	ttescreen
	tteeditmode)
