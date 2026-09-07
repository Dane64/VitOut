cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED VITASDK AND DEFINED ENV{VITASDK})
  set(VITASDK "$ENV{VITASDK}")
endif()

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(VITASDK)
    set(CMAKE_TOOLCHAIN_FILE "${VITASDK}/share/vita.toolchain.cmake" CACHE FILEPATH "Vita toolchain")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

set(SHORT_NAME VitOut)
project(${SHORT_NAME} LANGUAGES C)
include("${VITASDK}/share/vita.cmake")

set(VITA_APP_NAME "VitOut")
set(VITA_TITLEID  "VITABREAK")
set(VITA_VERSION "02.00" CACHE STRING "Vita package version (NN.NN)")

add_library(vitout_core STATIC
  src/game.c
  src/level.c
  src/scores.c
)
target_include_directories(vitout_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(vitout_core PUBLIC m)

add_executable(${SHORT_NAME}
  src/main.c
  src/menu.c
  src/input.c
  src/render.c
)

foreach(target vitout_core ${SHORT_NAME})
  set_target_properties(${target} PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS YES
  )
  target_compile_options(${target} PRIVATE
    -Wall -Wextra -Wpedantic -Wstrict-prototypes -Wmissing-prototypes
    -Werror=implicit-function-declaration -fno-common
  )
endforeach()

target_link_libraries(${SHORT_NAME} PRIVATE
  vitout_core
  vita2d
  SceAppMgr_stub
  SceDisplay_stub
  SceGxm_stub
  SceSysmodule_stub
  SceCommonDialog_stub
  SceCtrl_stub
  ScePgf_stub
  freetype
  png
  z
  m
)

file(GLOB bundled_levels CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/levels/*.lvl")
if(NOT bundled_levels)
  message(FATAL_ERROR "No bundled levels found in levels/")
endif()
set(level_files)
foreach(level IN LISTS bundled_levels)
  get_filename_component(level_name "${level}" NAME)
  list(APPEND level_files FILE "${level}" "levels/${level_name}")
endforeach()

vita_create_self(${SHORT_NAME}.self ${SHORT_NAME})
vita_create_vpk(${SHORT_NAME}.vpk ${VITA_TITLEID} ${SHORT_NAME}.self
  VERSION ${VITA_VERSION}
  NAME ${VITA_APP_NAME}
 FILE sce_sys/icon0.png sce_sys/icon0.png
 FILE sce_sys/livearea/contents/bg0.png sce_sys/livearea/contents/bg0.png
 FILE sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
 FILE sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
 ${level_files}
)
