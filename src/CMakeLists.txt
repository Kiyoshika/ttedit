add_library(ttecursor cursor.c)
target_include_directories(ttecursor PUBLIC ${TTEdit_SOURCE_DIR}/include)

add_library(ttescreen screen_buffer.c)
target_include_directories(ttescreen PUBLIC ${TTEdit_SOURCE_DIR}/include)
target_link_libraries(ttescreen ttecommand m) # m = math library

add_library(tteeditmode edit_mode.c)
target_include_directories(tteeditmode PUBLIC ${TTEdit_SOURCE_DIR}/include)
target_link_libraries(tteeditmode ttecursor ttescreen)

add_library(ttecommand command_buffer.c)
target_include_directories(ttecommand PUBLIC ${TTEdit_SOURCE_DIR}/include)
target_link_libraries(ttecommand ttecursor ttescreen)

add_executable(ttedit main.c)
target_link_libraries(ttedit 
	ncurses 
	ttecursor 
	ttescreen
	tteeditmode
	ttecommand)
