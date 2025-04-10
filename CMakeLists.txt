cmake_minimum_required(VERSION 3.15)
project(livekit-unreal)

include(ExternalProject)

cmake_policy(SET CMP0135 NEW) # Files extracted from the zip file will have their timestamps set to the current date. Used by ExternalProject_Add when using the URL method.

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}" CACHE PATH "Default install path" FORCE)
endif()

# Create a directory for external projects
set(EXTERNAL_INSTALL_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/ext)

# Build protobuf from source, which provides the headers and libraries for us to compile and link against when referencing the generated LiveKit protocol files.
ExternalProject_Add(
    protobuf
    GIT_REPOSITORY https://github.com/protocolbuffers/protobuf.git
    GIT_TAG 0d815c5b74281f081c1ee4b431a4d5bbb1615c97 # v30.1
    UPDATE_COMMAND ""
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}/protobuf
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_CXX_STANDARD=17
        -Dprotobuf_BUILD_TESTS=OFF
        -Dprotobuf_BUILD_LIBUPB=OFF
        -Dprotobuf_BUILD_PROTOC_BINARIES=OFF # Don't build protoc, we'll use the pre-compiled version.
        #-Dprotobuf_BUILD_SHARED_LIBS=ON # Shared libraries are disabled for the sample project, but will be needed for UE5.
        -Dprotobuf_USE_UNITY_BUILD=ON
)

ExternalProject_Add(
    livekit-protocol
    GIT_REPOSITORY https://github.com/livekit/protocol.git
    GIT_TAG f517a07061e19b6ce4b98b6818426411f0147d48 # @livekit/protocol@1.35.0
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_BINARY_DIR}/livekit-protocol-prefix/src/livekit-protocol/protobufs ${EXTERNAL_INSTALL_LOCATION}/livekit-protocol/protobufs
)

# Download the protoc binary, which is used to compile the protocol files provided by livekit-protocol.
# We could build protoc from source, but this guarantees we have a release optimised version of the compiler, which is vastly faster (>20s in debug to <1s in release).
ExternalProject_Add(
    protoc
    URL https://github.com/protocolbuffers/protobuf/releases/download/v30.1/protoc-30.1-win64.zip
    SOURCE_DIR ${EXTERNAL_INSTALL_LOCATION}/protoc
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

ExternalProject_Add(
    livekit-rust-sdks
    URL https://github.com/livekit/rust-sdks/releases/download/rust-sdks%2Flivekit-ffi%400.12.17/ffi-windows-x86_64.zip
    SOURCE_DIR ${EXTERNAL_INSTALL_LOCATION}/livekit-rust-sdks/windows-x86_64
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

set(PROTOBUFS_FILES_PATH ${EXTERNAL_INSTALL_LOCATION}/livekit-protocol/protobufs)
set(PROTOC_BINARY ${EXTERNAL_INSTALL_LOCATION}/protoc/bin/protoc.exe)
set(GENERATED_PROTO_SRCS_DIR ${CMAKE_SOURCE_DIR}/src/livekit-cpp/src/generated)

file(MAKE_DIRECTORY ${GENERATED_PROTO_SRCS_DIR})

set(PROTO_FILES
    ${PROTOBUFS_FILES_PATH}/livekit_agent.proto
    ${PROTOBUFS_FILES_PATH}/livekit_agent_dispatch.proto
    ${PROTOBUFS_FILES_PATH}/livekit_egress.proto
    ${PROTOBUFS_FILES_PATH}/livekit_ingress.proto
    ${PROTOBUFS_FILES_PATH}/livekit_rtc.proto
    ${PROTOBUFS_FILES_PATH}/livekit_room.proto
    ${PROTOBUFS_FILES_PATH}/livekit_webhook.proto
    ${PROTOBUFS_FILES_PATH}/livekit_metrics.proto
    ${PROTOBUFS_FILES_PATH}/livekit_models.proto
)

set(GENERATED_PROTO_SRCS)
set(GENERATED_PROTO_HDRS)

foreach(PROTO_FILE ${PROTO_FILES})
    get_filename_component(PROTO_NAME ${PROTO_FILE} NAME_WE)
    list(APPEND GENERATED_PROTO_SRCS "${GENERATED_PROTO_SRCS_DIR}/${PROTO_NAME}.pb.cc")
    list(APPEND GENERATED_PROTO_HDRS "${GENERATED_PROTO_SRCS_DIR}/${PROTO_NAME}.pb.h")

    add_custom_command(
        OUTPUT "${GENERATED_PROTO_SRCS_DIR}/${PROTO_NAME}.pb.cc" "${GENERATED_PROTO_SRCS_DIR}/${PROTO_NAME}.pb.h"
        COMMAND ${PROTOC_BINARY} -I=${PROTOBUFS_FILES_PATH} --cpp_out=${GENERATED_PROTO_SRCS_DIR} ${PROTO_FILE}
        DEPENDS ${PROTO_FILE} ${PROTOC_BINARY}
        COMMENT "Generating ${PROTO_NAME}.pb.cc and .pb.h"
        VERBATIM
    )
endforeach()

# Custom target for all generated proto sources
add_custom_target(compile_protocol_files ALL
    DEPENDS ${GENERATED_PROTO_SRCS} ${GENERATED_PROTO_HDRS}
)

ExternalProject_Add(livekit-cpp
    DEPENDS compile_protocol_files
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/livekit-cpp
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DEXTERNAL_INSTALL_LOCATION=${EXTERNAL_INSTALL_LOCATION}
    BUILD_ALWAYS ON
)

ExternalProject_Add(example
    DEPENDS livekit-cpp
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/example
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DEXTERNAL_INSTALL_LOCATION=${EXTERNAL_INSTALL_LOCATION}
)