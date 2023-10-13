set(COMPILE_ID "main")

set(SHARED_DIR_NAME "shared")

# if no target given, use Debug
if (NOT DEFINED CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()

# defines used in ninja / cmake ndk builds
if (NOT DEFINED CMAKE_ANDROID_NDK)
	if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/ndkpath.txt")
		file (STRINGS "ndkpath.txt" CMAKE_ANDROID_NDK)
	else()
		if(EXISTS $ENV{ANDROID_NDK_HOME})
			set(CMAKE_ANDROID_NDK $ENV{ANDROID_NDK_HOME})
		elseif(EXISTS $ENV{ANDROID_NDK_LATEST_HOME})
			set(CMAKE_ANDROID_NDK $ENV{ANDROID_NDK_LATEST_HOME})
		endif()
	endif()
endif()
if (NOT DEFINED CMAKE_ANDROID_NDK)
	message(Big time error buddy, no NDK)
endif()
message(Using NDK ${CMAKE_ANDROID_NDK})
string(REPLACE "\\" "/" CMAKE_ANDROID_NDK ${CMAKE_ANDROID_NDK})

set(ANDROID_PLATFORM 24)
set(ANDROID_ABI arm64-v8a)
set(ANDROID_STL c++_static)
set(ANDROID_USE_LEGACY_TOOLCHAIN_FILE OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_ANDROID_NDK}/build/cmake/android.toolchain.cmake)
# define used for external data, mostly just the qpm dependencies
set(EXTERN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${EXTERN_DIR_NAME})
set(SHARED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${SHARED_DIR_NAME})
# get files by filter recursively
MACRO(RECURSE_FILES return_list filter)
	FILE(GLOB_RECURSE new_list ${filter})
	SET(file_list "")
	FOREACH(file_path ${new_list})
		SET(file_list ${file_list} ${file_path})
	ENDFOREACH()
	LIST(REMOVE_DUPLICATES file_list)
	SET(${return_list} ${file_list})
ENDMACRO()

cmake_minimum_required(VERSION 3.21)
project(${COMPILE_ID})

# c++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

# define that stores the actual source directory
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# compile options used
add_compile_options(-fPIE -flto -fPIC -fno-rtti -fno-exceptions -O3 -Oz -fvisibility=hidden -Wall -Wextra -Wpedantic -Werror)

# recursively get all src files
RECURSE_FILES(cpp_file_list ${SOURCE_DIR}/*.cpp)
RECURSE_FILES(c_file_list ${SOURCE_DIR}/*.c)

# add all src files to compile
add_library(
    ${COMPILE_ID} 
    SHARED 
    ${cpp_file_list}
    ${c_file_list}
)

# add src dir as include dir
target_include_directories(${COMPILE_ID} PRIVATE ${SOURCE_DIR})
# add include dir as include dir
target_include_directories(${COMPILE_ID} PRIVATE ${INCLUDE_DIR})
# add shared dir as include dir
target_include_directories(${COMPILE_ID} PUBLIC ${SHARED_DIR})

target_link_libraries(${COMPILE_ID} PRIVATE -llog)

add_custom_command(TARGET ${COMPILE_ID} POST_BUILD
            COMMAND ${CMAKE_STRIP} -g -S -d --strip-all
            "lib${COMPILE_ID}.so" -o "stripped_lib${COMPILE_ID}.so"
            COMMENT "Strip debug symbols done on final binary.")
        
add_custom_command(TARGET ${COMPILE_ID} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory debug
        COMMENT "Make directory for debug symbols"
        )

add_custom_command(TARGET ${COMPILE_ID} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E rename lib${COMPILE_ID}.so debug/lib${COMPILE_ID}.so
            COMMENT "Move and rename the debug lib to debug folder since it has debug symbols"
            )

add_custom_command(TARGET ${COMPILE_ID} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E rename stripped_lib${COMPILE_ID}.so lib${COMPILE_ID}.so
            COMMENT "Rename the stripped lib to regular"
            )

foreach(so_file ${so_list})
    cmake_path(GET so_file FILENAME file)

    add_custom_command(TARGET ${COMPILE_ID} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${so_file} debug/${file}
        COMMENT "Copy so files for ndk stack"
    )

    add_custom_command(TARGET ${COMPILE_ID} POST_BUILD
            COMMAND ${CMAKE_STRIP} -g -S -d --strip-all ${so_file} -o ${file}
            COMMENT "Strip debug symbols from the dependencies")
endforeach()

foreach(a_file ${a_list})
cmake_path(GET a_file FILENAME file)

add_custom_command(TARGET ${COMPILE_ID} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${a_file} debug/${file}
    COMMENT "Copy a files for ndk stack")
endforeach()