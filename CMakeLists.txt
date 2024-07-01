# CMakeList.txt : CMake project for DependencyInjection, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.18)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project("DependencyInjection")

if (MSVC)
  add_compile_options(/W4 /WX /permissive-)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

include_directories(src)

add_subdirectory(test)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET DependencyInjection PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
