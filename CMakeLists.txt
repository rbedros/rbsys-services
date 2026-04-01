cmake_minimum_required(VERSION 3.28)
project(rbsys-services VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find rbsys framework
# Set RBSYS_DIR to the rbsys install prefix or build directory
if(NOT DEFINED RBSYS_DIR)
    # Default: assume rbsys is a sibling directory
    set(RBSYS_DIR "${CMAKE_SOURCE_DIR}/../rbsys")
endif()

# Include rbsys headers and link against librbsys
if(EXISTS "${RBSYS_DIR}/build-debug/core/librbsys_core.a")
    # Development mode: link against rbsys build directory
    set(RBSYS_INCLUDE_DIRS
        ${RBSYS_DIR}/core/service/include
        ${RBSYS_DIR}/core/include
        ${RBSYS_DIR}/core/logging/include
        ${RBSYS_DIR}/core/metrics/include
        ${RBSYS_DIR}/core/messaging/include
        ${RBSYS_DIR}/core/messaging/interProcessAmqp/include
        ${RBSYS_DIR}/core/messaging/interThread/include
        ${RBSYS_DIR}/features/generated
        ${RBSYS_DIR}/build-debug/_deps/nlohmann_json-src/include
    )
    set(RBSYS_LIBRARIES ${RBSYS_DIR}/build-debug/core/librbsys_core.a)

    # Also need third-party libs
    find_library(RABBITMQ_LIB rabbitmq PATHS ${RBSYS_DIR}/build-debug/_deps/rabbitmq_c-build/librabbitmq)
    find_library(RDKAFKA_LIB rdkafka)
    if(RABBITMQ_LIB)
        list(APPEND RBSYS_LIBRARIES ${RABBITMQ_LIB})
    endif()
    if(RDKAFKA_LIB)
        list(APPEND RBSYS_LIBRARIES ${RDKAFKA_LIB})
    endif()
    list(APPEND RBSYS_LIBRARIES pthread dl)
else()
    # Install mode: use find_package
    find_package(rbsys REQUIRED)
    set(RBSYS_INCLUDE_DIRS ${rbsys_INCLUDE_DIRS})
    set(RBSYS_LIBRARIES rbsys::core)
endif()

# Compiler warnings
function(service_target_warnings target)
    target_compile_options(${target} PRIVATE
        -Wall -Wextra -Wpedantic -Werror
    )
endfunction()

# Build each service as a shared library (.so plugin)
add_subdirectory(services/order)
add_subdirectory(services/inventory)
add_subdirectory(services/notification)
add_subdirectory(services/analytics)
