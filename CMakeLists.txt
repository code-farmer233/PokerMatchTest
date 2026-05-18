cmake_minimum_required(VERSION 3.6)

set(APP_NAME PokerMatchTest)
project(${APP_NAME})

if(NOT DEFINED COCOS2DX_ROOT_PATH)
    message(FATAL_ERROR "Please set COCOS2DX_ROOT_PATH to your cocos2d-x 3.17 root.")
endif()

get_filename_component(COCOS2DX_ROOT_PATH "${COCOS2DX_ROOT_PATH}" ABSOLUTE)
set(CMAKE_MODULE_PATH "${COCOS2DX_ROOT_PATH}/cmake/Modules")

include(CocosBuildSet)
add_subdirectory(${COCOS2DX_ROOT_PATH}/cocos ${ENGINE_BINARY_PATH}/cocos/core)

set(GAME_RES_FOLDER
    "${CMAKE_CURRENT_SOURCE_DIR}/res"
)

if(APPLE OR WINDOWS)
    cocos_mark_multi_resources(common_res_files RES_TO "Resources" FOLDERS ${GAME_RES_FOLDER})
endif()

set(GAME_SOURCE
    Classes/AppDelegate.cpp
    Classes/CardGameController.cpp
    Classes/CardView.cpp
    Classes/PokerScene.cpp
)

set(GAME_HEADER
    Classes/AppDelegate.h
    Classes/CardGameController.h
    Classes/CardModel.h
    Classes/CardView.h
    Classes/PokerScene.h
)

if(WIN32)
    list(APPEND GAME_SOURCE proj.win32/main.cpp)
    list(APPEND GAME_SOURCE ${common_res_files})
endif()

add_executable(${APP_NAME} ${GAME_SOURCE} ${GAME_HEADER})
target_link_libraries(${APP_NAME} cocos2d)
target_include_directories(${APP_NAME}
    PRIVATE Classes
    PRIVATE ${COCOS2DX_ROOT_PATH}/cocos/audio/include/
)

setup_cocos_app_config(${APP_NAME})

if(WINDOWS)
    cocos_copy_target_dll(${APP_NAME})
endif()

if(LINUX OR WINDOWS)
    set(APP_RES_DIR "$<TARGET_FILE_DIR:${APP_NAME}>/Resources")
    cocos_copy_target_res(${APP_NAME} COPY_TO ${APP_RES_DIR} FOLDERS ${GAME_RES_FOLDER})
endif()
