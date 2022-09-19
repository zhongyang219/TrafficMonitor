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

## minimum version
cmake_minimum_required(VERSION 3.11)

## plugin version
project(OpenHardwareMonitorApi)

## export lib vs need
add_definitions(-DOPENHARDWAREMONITOR_EXPORTS)

## sources
file(GLOB harewaremonitor_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h
                              ${CMAKE_CURRENT_SOURCE_DIR}/app.rc)

add_library(${PROJECT_NAME} SHARED ${harewaremonitor_SRC})

# c# library
add_library(LibreHardwareMonitorLib SHARED IMPORTED)

target_compile_options(${PROJECT_NAME} PRIVATE /clr)

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DOTNET_TARGET_FRAMEWORK_VERSION "4.7.2")
LIST(APPEND VS_DOTNET_REFERENCES "mscorlib")
LIST(APPEND VS_DOTNET_REFERENCES "System")
LIST(APPEND VS_DOTNET_REFERENCES "System.Xml")
LIST(APPEND VS_DOTNET_REFERENCES "System.Data")

set_target_properties(${PROJECT_NAME}  PROPERTIES
    VS_DOTNET_REFERENCES "${VS_DOTNET_REFERENCES}"
    VS_DOTNET_REFERENCE_LibreHardwareMonitorLib "${CMAKE_CURRENT_SOURCE_DIR}/LibreHardwareMonitorLib.dll")

target_compile_definitions(${PROJECT_NAME} PRIVATE _UNICODE UNICODE)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${ROOT_DIR}/include)
