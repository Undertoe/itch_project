file(GLOB_RECURSE ITCH_LIB_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/itch/*.cc")

add_library(itch STATIC ${ITCH_LIB_SOURCES})

add_library(nasdaq_itch::itch ALIAS itch)

target_include_directories(itch
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_compile_features(itch PUBLIC cxx_std_23)
