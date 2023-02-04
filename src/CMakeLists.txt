##### @fbcpX
project(fbcpX LANGUAGES CXX)
cmake_minimum_required(VERSION 3.10)
# set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -lstdc++fs")
##### @fbcpX

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

include_directories(/opt/vc/include)
link_directories(/opt/vc/lib)

file(GLOB sourceFiles *.cpp)

message(STATUS "Doing a ${CMAKE_BUILD_TYPE} build")
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    message(STATUS "Pass -DCMAKE_BUILD_TYPE=Release to do a fast optimized build.")
endif()

# Detect if current hardware is Raspberry Pi Zero/Zero W, and enable targeting Zero with -DSINGLE_CORE_BOARD=ON if so.
execute_process(COMMAND cat /proc/cpuinfo OUTPUT_VARIABLE CPU_INFO)
STRING(REGEX MATCH "Revision[\t ]*:[\t ]*([0-9a-f]+)" BOARD_REVISION ${CPU_INFO})
set(BOARD_REVISION "${CMAKE_MATCH_1}")
message(STATUS "Board revision: ${CMAKE_MATCH_1}")

set(DEFAULT_TO_SINGLE_CORE_BOARD OFF)
set(DEFAULT_TO_ARMV6Z OFF)
set(DEFAULT_TO_ARMV7A OFF)
set(DEFAULT_TO_ARMV8A OFF)

# http://ozzmaker.com/check-raspberry-software-hardware-version-command-line/
if (BOARD_REVISION MATCHES "(0002)|(0003)|(0004)|(0005)|(0006)|(0007)|(0008)|(0009)" OR BOARD_REVISION MATCHES "(000d)|(000e)|(000f)|(0010)|(0011)|(0012)" OR BOARD_REVISION MATCHES "(900092)|(900093)|(9000c1)")
	message(STATUS "Detected this Pi to be one of: Pi A, A+, B rev. 1, B rev. 2, B+, CM1, Zero or Zero W, with single hardware core and ARMv6Z instruction set CPU.")
	set(DEFAULT_TO_SINGLE_CORE_BOARD ON)
	set(DEFAULT_TO_ARMV6Z ON)
elseif(BOARD_REVISION MATCHES "(a01041)|(a21041)")
	message(STATUS "Detected this board to be a Pi 2 Model B < rev 1.2 with ARMv7-A instruction set CPU.")
	set(DEFAULT_TO_ARMV7A ON)
elseif(BOARD_REVISION MATCHES "(a02082)|(a22082)|(a020d3)|(9020e0)|(a03111)|(b03111)|(b03140)|(c03111)" OR BOARD_REVISION MATCHES "(c03130)|(d03114)")
	message(STATUS "Detected this Pi to be one of: Pi 2B rev. 1.2, 3B, 3B+, 3A+, CM3, CM3 lite, CM4, 4B(1GB,2GB,4GB,8GB RAM) or Pi400, with 4 hardware cores and ARMv8-A instruction set CPU.")
	set(DEFAULT_TO_ARMV8A ON)
else()
	message(WARNING "The board revision of this hardware is not known. Please add detection to this board in CMakeLists.txt. (proceeding to compile against a generic multicore CPU)")
endif()

option(SINGLE_CORE_BOARD "Target a Raspberry Pi with only one hardware core (Pi Zero)" ${DEFAULT_TO_SINGLE_CORE_BOARD})
if (SINGLE_CORE_BOARD)
  message(STATUS "Targeting a Raspberry Pi with only one hardware core")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSINGLE_CORE_BOARD=1")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -marm -mabi=aapcs-linux -mhard-float -mfloat-abi=hard -mlittle-endian -mtls-dialect=gnu2 -funsafe-math-optimizations")

option(ARMV6Z "Target a Raspberry Pi with ARMv6Z instruction set (Pi 1A, 1A+, 1B, 1B+, Zero, Zero W)" ${DEFAULT_TO_ARMV6Z})
if (ARMV6Z)
    message(STATUS "Enabling optimization flags that target ARMv6Z instruction set (Pi Model A, Pi Model B, Compute Module 1, Pi Zero/Zero W)")
    # Currently disabled, no test data to know if this would be faster
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv6z -mfpu=vfp")
endif()

option(ARMV7A "Target a Raspberry Pi with ARMv7-A instruction set (Pi 2B < rev 1.2)" ${DEFAULT_TO_ARMV7A})
if (ARMV7A)
    message(STATUS "Enabling optimization flags that target ARMv7-A instruction set (Pi 2B < rev 1.2)")
    # Currently disabled, no test data to know if this would be faster
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=cortex-a7 -mcpu=cortex-a7 -mtune=cortex-a7 -mfpu=neon-vfpv4")
endif()

option(ARMV8A "Target a Raspberry Pi with ARMv8-A instruction set (Pi 2B >= rev. 1.2, 3B, 3B+, CM3 or CM3 lite)" ${DEFAULT_TO_ARMV8A})
if (ARMV8A)
    message(STATUS "Enabling optimization flags that target ARMv8-A instruction set (Pi 2B >= rev. 1.2, 3B, 3B+, CM3 or CM3 lite)")
    # N.B. Here should be possible to set -mfpu=neon-vfpv4, though for some reason that generates really slow code, tested on gcc (Raspbian 6.3.0-18+rpi1+deb9u1) 6.3.0 20170516
    # Currently disabled, seems to be a tiny 1-2% slower (or no difference, drown in noise)
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8-a+crc -mcpu=cortex-a53 -mtune=cortex-a53")
endif()

message(NOTICE "\nBuild options: ${CMAKE_CXX_FLAGS}\n")

add_executable(fbcpx ${sourceFiles})

target_link_libraries(fbcpx pthread bcm_host atomic)
