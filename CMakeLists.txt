cmake_minimum_required(VERSION 3.5)
project(LaTeX)


# check if compiler has c++11 support

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
	message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no c++11 support. Please use a different one that supports c++11.")
endif()

# source file

file(GLOB_RECURSE SRC "src/*.cpp")


# check operating system

if(WIN32)
	message(STATUS "We are work on windows")
	include_directories("src")
	set(LINK_LIBS gdiplus)
elseif(UNIX AND NOT APPLE)
	message(STATUS "We are work on linux")
	# include gtkmm module
	find_package(PkgConfig)
	pkg_check_modules(GTKMM gtkmm-3.0)
	link_directories(${GTKMM_LIBRARY_DIRS})
	include_directories(${GTKMM_INCLUDE_DIRS} "src")
	set(LINK_LIBS ${GTKMM_LIBRARIES} fontconfig)
else()
	message(STATUS "We are work on a unknow platform")
	# other platforms...
endif()


add_executable(LaTeX ${SRC})
target_link_libraries(LaTeX ${LINK_LIBS})