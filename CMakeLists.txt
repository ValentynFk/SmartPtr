cmake_minimum_required(VERSION 3.15)
project(SmartPtr C)

set(CMAKE_C_STANDARD 11)
#set(SmartPtr_SOURCE_DIR "C:\\Users\\faitc\\OneDrive - lpnu.ua\\Cplusplus STUFF\\SmartPtr")
#add_custom_target(SmartPtr COMMAND cd ${SmartPtr_SOURCE_DIR}; make)
add_executable(SmartPtr main.c smart-ptr.c smart-ptr.h)

