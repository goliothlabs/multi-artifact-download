cmake_minimum_required(VERSION 3.5)
set(projname "multi_artifact_download")
project(${projname} C)

set(CMAKE_BUILD_TYPE Debug)

set(sdk_root ../golioth-firmware-sdk)

set(srcs
    main.c
)

get_filename_component(user_config_file "golioth_user_config.h" ABSOLUTE)
add_definitions(-DCONFIG_GOLIOTH_USER_CONFIG_INCLUDE="${user_config_file}")

add_subdirectory(${sdk_root}/port/linux/golioth_sdk build)
add_executable(${projname} ${srcs})
target_link_libraries(${projname} golioth_sdk)
