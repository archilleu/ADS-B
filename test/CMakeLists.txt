#---------------------------------------------------------------------------
#CMake minimum required version
cmake_minimum_required (VERSION 2.8)
#---------------------------------------------------------------------------
#look up dir's file, and add to var
aux_source_directory(. DIR_TESTS)
#---------------------------------------------------------------------------
#generate test
set(TESTBAE test_base)
add_executable(${TESTBAE} ${DIR_TESTS})
#---------------------------------------------------------------------------
#add include path
#include_directories(${LIBRARY_OUTPUT_PATH_INC})
#---------------------------------------------------------------------------
#add lib path
#link_directories(${CMAKE_INSTALL_PREFIX}/src})
#---------------------------------------------------------------------------
#add link lib
set(LIBBASE base)
target_link_libraries(${TESTBAE} ${LIBBASE} pthread rt)
#---------------------------------------------------------------------------
#install target 
#install(TARGETS json_test DESTINATION ${EXECUTABLE_OUTPUT_PATH})
#---------------------------------------------------------------------------
