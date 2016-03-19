cmake_minimum_required (VERSION 2.6 FATAL_ERROR)
project (ArduinoWebControl C)
find_package(Threads)

if(CMAKE_C_COMPILER_ID MATCHES GNU OR
	CMAKE_C_COMPILER_ID MATCHES "Clang")
	set(CMAKE_C_FLAGS "-Wall -Wfatal-errors -pedantic -std=gnu99")
	set(CMAKE_C_FLAGS_DEBUG "-O0 -g3")
	set(CMAKE_C_FLAGS_RELEASE "-O3")

endif()


add_library(arduino_serial_lib ${PROJECT_SOURCE_DIR}/arduino-serial/arduino-serial-lib.c)
#add_custom_target( OUTPUT arduino_serial-lib.o
#					COMMAND make
#					WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/arduino_serial)

#add_custom_target(arduino_serial_lib_target DEPENDS arduino-serial-lib.o)

#add_library(arduino_serial_lib STATIC IMPORTED GLOBAL)

#add_dependencies(arduino_serial_lib arduino_serial_lib_target)

#set_target_properties(arduino_serial_lib PROPERTIES IMPORTED_LOCATION arduino_serial-lib.o)

include_directories(${PROJECT_SOURCE_DIR}/arduino-serial)
					

add_executable(ArduinoWebControl controller.c webcontent.c)
target_link_libraries(ArduinoWebControl arduino_serial_lib)

