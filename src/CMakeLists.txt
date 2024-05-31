set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# PNG
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)

# the source files
file(GLOB SOURCE_FILES
        "*.h"
        "*.cpp"
        "lib/*.h"
        "lib/*.cpp"
        "lib/network/*.cpp"
        "lib/network/*.h"
        "overview/*.cpp"
        "overview/*.h"
        "models/*.cpp"
        "models/*.h"
        )

# Find Qt. Use a CMake definition to point it to your Qt installation
# location (if necessary), e.g: -DCMAKE_PREFIX_PATH=/home/user/Qt/5.15.2/gcc_64
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui Network DBus Svg Xml Quick QuickWidgets Qml QuickControls2 QuickCompiler)

qtquick_compiler_add_resources(RESOURCES assets.qrc)

set(EXECUTABLE_FLAG)

add_executable(maemo_kodi_remote ${EXECUTABLE_FLAG} main.cpp
        ${SOURCE_FILES}
        ${RESOURCES}
        )

set_property(TARGET maemo_kodi_remote PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

target_include_directories(maemo_kodi_remote PUBLIC ${Qt5Gui_PRIVATE_INCLUDE_DIRS})

target_include_directories(maemo_kodi_remote PUBLIC
        ${CMAKE_SOURCE_DIR}/src
        ${X11_INCLUDE_DIR}
        ${OPENSSL_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${Qt5Core_INCLUDE_DIRS}
        ${Qt5Widgets_INCLUDE_DIRS}
        ${Qt5Gui_INCLUDE_DIRS}
        ${Qt5Network_INCLUDE_DIRS}
        ${Qt5Svg_INCLUDE_DIRS}
        ${Qt5Xml_INCLUDE_DIRS}
        ${Qt5WebSockets_INCLUDE_DIRS}
        )

add_definitions(${QT_DEFINITIONS})
if(NOT "${CMAKE_BUILD_TYPE}" EQUAL "Debug")
    add_definitions(-DQT_NO_DEBUG)
endif()

target_compile_definitions(maemo_kodi_remote
        PUBLIC
        ${Qt5Core_DEFINITIONS}
        ${Qt5Widgets_DEFINITIONS}
        ${Qt5Gui_DEFINITIONS}
        ${Qt5Network_DEFINITIONS}
        ${Qt5Svg_DEFINITIONS}
        ${Qt5Xml_DEFINITIONS}
        ${Qt5WebSockets_DEFINITIONS}
        )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

target_link_libraries(maemo_kodi_remote PUBLIC
        ${OPENSSL_LIBRARIES}
        ${CMAKE_DL_LIBS}
        Qt5::Core
        Qt5::Widgets
        Qt5::Gui
        Qt5::Network
        Qt5::Svg
        Qt5::QSvgPlugin
        Qt5::QSvgIconPlugin
        Qt5::Xml
        Qt5::Quick
        Qt5::QuickWidgets
        Qt5::Qml
        Qt5::QuickControls2
        Threads::Threads
)

target_link_libraries(maemo_kodi_remote PUBLIC
        Qt5::QSvgIconPlugin
        Qt5::QSvgPlugin
        )

if(X11_FOUND)
    target_link_libraries(maemo_kodi_remote PUBLIC ${X11_LIBRARIES})
endif()

install(TARGETS maemo_kodi_remote
        DESTINATION ${CMAKE_INSTALL_PREFIX}
        )
