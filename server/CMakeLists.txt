cmake_minimum_required(VERSION 3.16)

project(StudentTopicSystemServer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 查找Qt6组件
find_package(Qt6 REQUIRED COMPONENTS Core Network Sql)

qt_standard_project_setup()

# 添加源文件
set(SOURCES
    main.cpp
    databasehandler.cpp
    # tcpserver.cpp
    # authmanager.cpp
)

set(HEADERS
    databasehandler.h
    # tcpserver.h
    # authmanager.h
    # datamodel.h
)

# 创建可执行文件
qt_add_executable(StudentTopicSystemServer
    ${SOURCES}
    ${HEADERS}
)

# 链接Qt库
target_link_libraries(StudentTopicSystemServer PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Sql
)

# 编译选项
if(MSVC)
    target_compile_options(StudentTopicSystemServer PRIVATE /W4 /Zc:__cplusplus /permissive-)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus /permissive-")
else()
    target_compile_options(StudentTopicSystemServer PRIVATE -Wall -Wextra -Wpedantic)
endif()

# 安装设置
install(TARGETS StudentTopicSystemServer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 部署设置（Windows）
if(WIN32)
    get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(QT_WINDEPLOYQT_EXECUTABLE
        "${QT_QMAKE_EXECUTABLE}/../windeployqt.exe" ABSOLUTE)
    
    add_custom_command(TARGET StudentTopicSystemServer POST_BUILD
        COMMAND ${QT_WINDEPLOYQT_EXECUTABLE}
        --verbose 0
        --no-compiler-runtime
        "$<TARGET_FILE:StudentTopicSystemServer>"
    )
endif()