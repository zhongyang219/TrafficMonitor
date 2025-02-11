# Copyright (c) <2019> <copyright zhongyang219>

# "Anti 996" License Version 1.0 (Draft)

# Permission is hereby granted to any individual or legal entity
# obtaining a copy of this licensed work (including the source code,
# documentation and/or related items, hereinafter collectively referred
# to as the "licensed work"), free of charge, to deal with the licensed
# work for any purpose, including without limitation, the rights to use,
# reproduce, modify, prepare derivative works of, distribute, publish
# and sublicense the licensed work, subject to the following conditions:

# 1. The individual or the legal entity must conspicuously display,
# without modification, this License and the notice on each redistributed
# or derivative copy of the Licensed Work.

# 2. The individual or the legal entity must strictly comply with all
# applicable laws, regulations, rules and standards of the jurisdiction
# relating to labor and employment where the individual is physically
# located or where the individual was born or naturalized; or where the
# legal entity is registered or is operating (whichever is stricter). In
# case that the jurisdiction has no such laws, regulations, rules and
# standards or its laws, regulations, rules and standards are
# unenforceable, the individual or the legal entity are required to
# comply with Core International Labor Standards.

# 3. The individual or the legal entity shall not induce, suggest or force
# its employee(s), whether full-time or part-time, or its independent
# contractor(s), in any methods, to agree in oral or written form, to
# directly or indirectly restrict, weaken or relinquish his or her
# rights or remedies under such laws, regulations, rules and standards
# relating to labor and employment as mentioned above, no matter whether
# such written or oral agreements are enforceable under the laws of the
# said jurisdiction, nor shall such individual or the legal entity
# limit, in any methods, the rights of its employee(s) or independent
# contractor(s) from reporting or complaining to the copyright holder or
# relevant authorities monitoring the compliance of the license about
# its violation(s) of the said license.

# THE LICENSED WORK IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN ANY WAY CONNECTION WITH THE
# LICENSED WORK OR THE USE OR OTHER DEALINGS IN THE LICENSED WORK.

cmake_minimum_required(VERSION 3.11)

project(TrafficMonitor)

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

## options
option(ENABLED_LITE "debug switch" OFF)

## lite version
if(ENABLED_LITE)
add_definitions(-DWITHOUT_TEMPERATURE)
endif()

## enabled cxx 14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

## enable mfc
ADD_DEFINITIONS(-D_AFXDLL)
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)
SET(CMAKE_MFC_FLAG 2)

## add hardwaremonitorapi plugin
add_subdirectory(OpenHardwareMonitorApi)

## add trafficmonitor sources
file(GLOB_RECURSE traffic_SRC RELATIVE ${ROOT_DIR} ${ROOT_DIR}/include/PluginInterface.h
                    ${ROOT_DIR}/TrafficMonitor/*.h
                    ${ROOT_DIR}/TrafficMonitor/*.cpp
                    ${ROOT_DIR}/TrafficMonitor/*.rc
                    ${ROOT_DIR}/TrafficMonitor/res/*.bin
                    ${ROOT_DIR}/TrafficMonitor/res/*.rc2
                    ${ROOT_DIR}/TrafficMonitor/res/*.bmp
                    ${ROOT_DIR}/TrafficMonitor/res/menu_icon/*.ico
                    ${ROOT_DIR}/TrafficMonitor/res/*.ico)

## main executor 
add_executable(${PROJECT_NAME} ${traffic_SRC})

target_include_directories(${PROJECT_NAME} PRIVATE ${ROOT_DIR}/include/)

## enabled window executor
IF(MSVC)
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
ENDIF(MSVC)

#support unicode
target_compile_definitions(${PROJECT_NAME} PRIVATE _AFXDLL _UNICODE UNICODE)

## link hardware plugin
target_link_libraries(${PROJECT_NAME} PRIVATE OpenHardwareMonitorApi)

## copy skins resources  vcxproj run
file(COPY ${ROOT_DIR}/TrafficMonitor/skins DESTINATION ${CMAKE_BINARY_DIR})


#copy resources binary dir
if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
file(COPY ${ROOT_DIR}/TrafficMonitor/skins DESTINATION "${CMAKE_BINARY_DIR}/Debug")
elseif(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Release"))
file(COPY ${ROOT_DIR}/TrafficMonitor/skins DESTINATION "${CMAKE_BINARY_DIR}/Release")
else()
file(COPY ${ROOT_DIR}/TrafficMonitor/skins DESTINATION "${CMAKE_BINARY_DIR}/Debug")
endif()

string(TIMESTAMP COMPILE_TIME %Y%m%d_%H%M%S)
message(STATUS "==>> ${COMPILE_TIME}")
execute_process(COMMAND echo ${COMPILE_TIME} >> ${ROOT_DIR}/TrafficMonitor/compile_time.txt)

# has some problem
# add_custom_command(TARGET trafficmonitor
#             PRE_BUILD
#             COMMAND ${ROOT_DIR}/TrafficMonitor/print_compile_time.bat
#             WORKING_DIRECTORY ${ROOT_DIR}/TrafficMonitor/)

# set_target_properties(trafficmonitor PROPERTIES
#     COMPILE_DEFINITIONS _AFXDLL,_UNICODE,UNICODE,_BIND_TO_CURRENT_CRT_VERSION,_BIND_TO_CURRENT_MFC_VERSION
#     LINK_FLAGS "/ENTRY:\"wWinMainCRTStartup\"")