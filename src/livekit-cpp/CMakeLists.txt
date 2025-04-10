cmake_minimum_required(VERSION 3.15)
project(livekit-cpp)

add_library(livekit-cpp)

target_compile_features(livekit-cpp PUBLIC cxx_std_17)

# Add source files
set(SOURCES
    src/ffi_client.cpp
    src/generated/livekit_agent_dispatch.pb.cc
    src/generated/livekit_agent_dispatch.pb.h
    src/generated/livekit_agent.pb.cc
    src/generated/livekit_agent.pb.h
    src/generated/livekit_egress.pb.cc
    src/generated/livekit_egress.pb.h
    src/generated/livekit_ingress.pb.cc
    src/generated/livekit_ingress.pb.h
    src/generated/livekit_metrics.pb.cc
    src/generated/livekit_metrics.pb.h
    src/generated/livekit_models.pb.cc
    src/generated/livekit_models.pb.h
    src/generated/livekit_room.pb.cc
    src/generated/livekit_room.pb.h
    src/generated/livekit_rtc.pb.cc
    src/generated/livekit_rtc.pb.h
    src/generated/livekit_webhook.pb.cc
    src/generated/livekit_webhook.pb.h
)

# Add header files 
set(HEADERS
    include/ffi_client.hpp
)

target_sources(livekit-cpp
    PRIVATE ${SOURCES}
    PUBLIC ${HEADERS}
)

#target_compile_definitions(livekit-cpp
#    PUBLIC
#    PROTOBUF_USE_DLLS
#)

# Set include directories
target_include_directories(livekit-cpp
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${EXTERNAL_INSTALL_LOCATION}/protobuf/include
)

target_link_directories(livekit-cpp
    PRIVATE ${EXTERNAL_INSTALL_LOCATION}/protobuf/lib
)

target_link_libraries(livekit-cpp
    PRIVATE 
    $<$<CONFIG:Debug>:libprotobufd>$<$<NOT:$<CONFIG:Debug>>:libprotobuf>
    absl_decode_rust_punycode
    absl_demangle_rust
    absl_flags_commandlineflag
    absl_flags_commandlineflag_internal
    absl_flags_config
    absl_flags_internal
    absl_flags_marshalling
    absl_flags_parse
    absl_flags_private_handle_accessor
    absl_flags_program_name
    absl_flags_reflection
    absl_flags_usage
    absl_flags_usage_internal
    absl_log_flags
    absl_log_internal_structured_proto
    absl_poison
    absl_tracing_internal
    absl_utf8_for_code_point
    libutf8_range
    libutf8_validity
)

# Set runtime library to MDd (Dynamic Debug) instead of MTd (Static Debug)
#if(MSVC)
#    set_property(TARGET livekit-cpp PROPERTY
#        MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL"
#    )
#endif()

set_target_properties(livekit-cpp PROPERTIES OUTPUT_NAME livekit)

# Install library and headers
install(TARGETS livekit-cpp
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION lib  # For Windows DLL
)

install(FILES ${HEADERS}
    DESTINATION include
)
