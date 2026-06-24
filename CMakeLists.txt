cmake_minimum_required(VERSION 3.16)

project(Restaurant VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Test)

set(PROJECT_SOURCES
    src/main.cpp

    # UI
    src/ui/mainwindow.cpp src/ui/mainwindow.h src/ui/mainwindow.ui
    src/ui/creationacc.cpp src/ui/creationacc.h src/ui/creationacc.ui
    src/ui/userwindow.cpp src/ui/userwindow.h src/ui/userwindow.ui
    src/ui/menuwindow.cpp src/ui/menuwindow.h src/ui/menuwindow.ui
    src/ui/myorders.cpp src/ui/myorders.h src/ui/myorders.ui
    src/ui/employeewindow.cpp src/ui/employeewindow.h src/ui/employeewindow.ui
    src/ui/adminwindow.cpp src/ui/adminwindow.h src/ui/adminwindow.ui

    # Backend
    src/backend/server_login_creation.cpp src/backend/server_login_creation.h
    src/backend/server_user.cpp src/backend/server_user.h
    src/backend/server_employee.cpp src/backend/server_employee.h
    src/backend/server_admin.cpp src/backend/server_admin.h

    # Core
    src/core/dbconnection.cpp src/core/dbconnection.h
    src/core/generic_utils.h
)

file(GLOB EXAMPLES "src/core/dbconnection_example.cpp")
add_custom_target(Examples SOURCES ${EXAMPLES})

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Retaurant
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Retaurant APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Retaurant SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Retaurant
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(Retaurant PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/backend
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
)

target_link_libraries(Retaurant PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::Test)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Retaurant)
endif()
set_target_properties(Retaurant PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Retaurant
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Retaurant)
endif()
