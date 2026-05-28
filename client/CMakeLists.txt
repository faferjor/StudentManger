cmake_minimum_required(VERSION 3.16)

project(StudentTopicSystemClient VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 查找Qt6组件
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network Sql Charts)

qt_standard_project_setup()

# 添加源文件
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    loginwindow.cpp
    loginwindow.h
    loginwindow.ui
    networkmanager.cpp
    networkmanager.h
    databaseconnection.h
    usermanagementwindow.cpp
    usermanagementwindow.h
    topicmanagementwindow.cpp
    topicmanagementwindow.h
    topicbrowsingwindow.cpp
    topicbrowsingwindow.h
    applicationreviewwindow.cpp
    applicationreviewwindow.h
    statisticswindow.cpp
    statisticswindow.h
)

# 创建可执行文件
qt_add_executable(StudentTopicSystemClient
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

# 链接Qt库
target_link_libraries(StudentTopicSystemClient PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    Qt6::Sql
    Qt6::Charts
)

# 编译选项
if(MSVC)
    target_compile_options(StudentTopicSystemClient PRIVATE /W4 /Zc:__cplusplus /permissive-)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus /permissive-")
else()
    target_compile_options(StudentTopicSystemClient PRIVATE -Wall -Wextra -Wpedantic)
endif()

# 目标属性设置
set_target_properties(StudentTopicSystemClient PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.studenttopicsystem
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# 安装设置
install(TARGETS StudentTopicSystemClient
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 部署设置（Windows）
if(WIN32)
    get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(QT_WINDEPLOYQT_EXECUTABLE
        "${QT_QMAKE_EXECUTABLE}/../windeployqt.exe" ABSOLUTE)
    
    add_custom_command(TARGET StudentTopicSystemClient POST_BUILD
        COMMAND ${QT_WINDEPLOYQT_EXECUTABLE}
        --verbose 0
        --no-compiler-runtime
        "$<TARGET_FILE:StudentTopicSystemClient>"
    )
endif()

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(StudentTopicSystemClient)
endif()
