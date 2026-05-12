cmake_minimum_required(VERSION 3.16)

project(data_visualization_app VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets LinguistTools Charts Bluetooth)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools Charts Bluetooth)

set(TS_FILES data_visualization_app_pl_PL.ts)


set(PROJECT_SOURCES
        src/main.cpp
        src/MainWindow.cpp
        inc/MainWindow.h
        ui/MainWindow.ui
        src/RobotDiagram.cpp
        inc/RobotDiagram.h
        ui/RobotDiagram.ui
        src/MotorArrowWidget.cpp
        inc/MotorArrowWidget.h
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(data_visualization_app
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        inc/BluetoothScanner.h src/BluetoothScanner.cpp ui/BluetoothScanner.ui
        inc/BluetoothManager.h src/BluetoothManager.cpp
        inc/BluetoothLogger.h src/BluetoothLogger.cpp ui/BluetoothLogger.ui
        inc/TofChart.h src/TofChart.cpp ui/TofChart.ui
        inc/LineChart.h src/LineChart.cpp ui/LineChart.ui
        inc/RobotPicture.h src/RobotPicture.cpp ui/RobotPicture.ui
        inc/RobotDiagram.h src/RobotDiagram.cpp ui/RobotDiagram.ui
        inc/MotorArrowWidget.h src/MotorArrowWidget.cpp
        inc/TofConeWidget.h src/TofConeWidget.cpp
        inc/RotatedLabel.h
        img/mechanika.png
        pictures.qrc

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET data_visualization_app APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(data_visualization_app SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(data_visualization_app
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(data_visualization_app PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Charts Qt${QT_VERSION_MAJOR}::Bluetooth)

target_include_directories(data_visualization_app PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.data_visualization_app)
endif()
set_target_properties(data_visualization_app PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS data_visualization_app
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

target_include_directories(data_visualization_app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(data_visualization_app)
endif()