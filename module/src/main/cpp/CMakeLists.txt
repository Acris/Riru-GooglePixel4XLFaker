cmake_minimum_required(VERSION 3.4.1)

SET(XHOOK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/libxhook/)

if (NOT DEFINED MODULE_NAME)
    message(FATAL_ERROR "MODULE_NAME is not set")
endif ()

message("Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_STANDARD 11)

set(LINKER_FLAGS "-ffixed-x18 -Wl,--hash-style=both")
set(C_FLAGS "-Werror=format -fdata-sections -ffunction-sections")

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(C_FLAGS "${C_FLAGS} -O2 -fvisibility=hidden -fvisibility-inlines-hidden")
    set(LINKER_FLAGS "${LINKER_FLAGS} -Wl,-exclude-libs,ALL -Wl,--gc-sections")
else ()
    set(C_FLAGS "${C_FLAGS} -O0")
endif ()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${C_FLAGS}")

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${LINKER_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${LINKER_FLAGS}")

SET(XHOOK_OBJS
        external/libxhook/xhook.c
        external/libxhook/xh_core.c
        external/libxhook/xh_elf.c
        external/libxhook/xh_jni.c
        external/libxhook/xh_log.c
        external/libxhook/xh_util.c
        external/libxhook/xh_version.c
        )
ADD_LIBRARY(libxhook STATIC ${XHOOK_OBJS})
set_target_properties(libxhook PROPERTIES LINKER_LANGUAGE C)

add_library(${MODULE_NAME} SHARED main.cpp hook.cpp riru.c)
target_link_libraries(${MODULE_NAME} log libxhook)
set_target_properties(${MODULE_NAME} PROPERTIES LINK_FLAGS_RELEASE -s)
