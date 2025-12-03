cmake_minimum_required(VERSION 3.15)
project(OCRDSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# Find required packages
find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)

# Find Tesseract
pkg_check_modules(TESSERACT REQUIRED tesseract)

# Find Leptonica
pkg_check_modules(LEPTONICA REQUIRED lept)

# Protobuf
set(PROTO_FILES
    proto/ocr.proto
)

# Generate gRPC files
set(PROTO_SRCS)
set(PROTO_HDRS)
set(GRPC_SRCS)
set(GRPC_HDRS)

foreach(PROTO_FILE ${PROTO_FILES})
    get_filename_component(PROTO_DIR ${PROTO_FILE} DIRECTORY)
    get_filename_component(PROTO_NAME ${PROTO_FILE} NAME_WE)
    
    set(PROTO_SRC "${CMAKE_CURRENT_BINARY_DIR}/${PROTO_NAME}.pb.cc")
    set(PROTO_HDR "${CMAKE_CURRENT_BINARY_DIR}/${PROTO_NAME}.pb.h")
    set(GRPC_SRC "${CMAKE_CURRENT_BINARY_DIR}/${PROTO_NAME}.grpc.pb.cc")
    set(GRPC_HDR "${CMAKE_CURRENT_BINARY_DIR}/${PROTO_NAME}.grpc.pb.h")
    
    list(APPEND PROTO_SRCS ${PROTO_SRC})
    list(APPEND PROTO_HDRS ${PROTO_HDR})
    list(APPEND GRPC_SRCS ${GRPC_SRC})
    list(APPEND GRPC_HDRS ${GRPC_HDR})
    
    add_custom_command(
        OUTPUT "${PROTO_SRC}" "${PROTO_HDR}" "${GRPC_SRC}" "${GRPC_HDR}"
        COMMAND protobuf::protoc
        ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
             --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
             -I "${CMAKE_CURRENT_SOURCE_DIR}/proto"
             --plugin=protoc-gen-grpc="${gRPC_CPP_PLUGIN_EXECUTABLE}"
             "${CMAKE_CURRENT_SOURCE_DIR}/${PROTO_FILE}"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${PROTO_FILE}"
    )
endforeach()

# Server executable
add_executable(ocr_server
    server/main.cpp
    ${PROTO_SRCS}
    ${PROTO_HDRS}
    ${GRPC_SRCS}
    ${GRPC_HDRS}
)

target_include_directories(ocr_server PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${TESSERACT_INCLUDE_DIRS}
    ${LEPTONICA_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(ocr_server PRIVATE
    gRPC::grpc++
    protobuf::libprotobuf
    ${OpenCV_LIBS}
    ${TESSERACT_LIBRARIES}
    ${LEPTONICA_LIBRARIES}
)

target_compile_options(ocr_server PRIVATE
    ${TESSERACT_CFLAGS_OTHER}
    ${LEPTONICA_CFLAGS_OTHER}
)

# Client executable
set(CLIENT_SOURCES
    client/main.cpp
    client/mainwindow.cpp
    client/mainwindow.h
    client/ocr_client.cpp
    client/ocr_client.h
    ${PROTO_SRCS}
    ${PROTO_HDRS}
    ${GRPC_SRCS}
    ${GRPC_HDRS}
)

add_executable(ocr_client
    ${CLIENT_SOURCES}
)

target_include_directories(ocr_client PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(ocr_client PRIVATE
    Qt6::Core
    Qt6::Widgets
    gRPC::grpc++
    protobuf::libprotobuf
)

