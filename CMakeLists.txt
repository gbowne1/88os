cmake_minimum_required (VERSION 2.8.4)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(PROJECT_DIR ~/Projects/os/src)

if (POLICY CMP0054)
  cmake_policy(SET CMP0054 NEW)
endif()

if (POLICY CMP0042)
  # Enable `MACOSX_RPATH` by default.
  cmake_policy(SET CMP0042 NEW)
endif()

if (POLICY CMP0037)
  # Disallow reserved target names
  cmake_policy(SET CMP0037 NEW)
endif()

# Setup project name and used languages
project(kernel CXX ASM)

# Setup project version
set(PROJECT_MAJOR_VERSION 0)
set(PROJECT_MINOR_VERSION 1)
set(PROJECT_PATCH_LEVEL 0)
set(PROJECT_VERSION ${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_LEVEL})
set(LINKER_SCRIPT "${PROJECT_SOURCE_DIR}/arch/${ARCH}/linker.ld")
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffreestanding -Wextra \
-fno-exceptions -fno-pic -fstack-protector-all")
set(CMAKE_C_COMPILER_TARGET i386-elf)
set(CMAKE_C_COMPILER i386-elf-gcc)
set(CMAKE_SYSTEM_PROCESSOR i386)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# This overrides the default flags for the different CMAKE_BUILD_TYPEs
set(CMAKE_USER_MAKE_RULES_OVERRIDE_C
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/c_flags_override.cmake")
set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cxx_flags_override.cmake")
project(KLEE CXX C)

# Setup compilation and linking flags
set(CMAKE_CXX_FLAGS "-std=c++11 -static -mcmodel=kernel -fno-stack-protector -fno-pic -mno-red-zone -fno-use-cxa-atexit -fno-rtti -fno-exceptions -nostdlib -ffreestanding")
set(CMAKE_CXX_FLAGS_RELEASE "-0s")
set(CMAKE_CXX_FLAGS_DEBUG  "-g")
set(CMAKE_ASM_NASM_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_ASM_NASM_FLAGS_RELWITHDEBINFO "-g")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-g -Og")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--build-id=none -n -T ${LINKER_SCRIPT}")

add_compile_options(-Wall)

option(WERROR "Convert warnings into errors" OFF)

if(WERROR)
    add_compile_options(-Werror)
endif()

include_directories(include)
include(CheckFunctionsExists)

# Add subprojects
add_subdirectory(../middlespace middlespace)
add_subdirectory(understructure)
add_subdirectory(services)
add_subdirectory(modules)
add_subdirectory(interface)


# Collect source files
file(GLOB SOURCES_ASM "arch/${ARCH}/*.S") 
file(GLOB SOURCES_CPP "*.cpp") 
    
# Create the kernel binary
add_executable(${BIN} ${SOURCES_ASM} ${SOURCES_CPP})
#target_include_directories(${BIN} .)    

target_link_libraries(${BIN}
    kstd hardware logging memory time filesystem multitasking drivers cpuexceptions ipc # sevices
    fat32 elf64 multiboot2 vga sysinfo # modules
    int80h procfs syscalls # interface
)