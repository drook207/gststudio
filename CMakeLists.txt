cmake_minimum_required(VERSION 3.16)

project(
    GstStudio
    VERSION 0.1
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Gui Qml QuickControls2)

qt_standard_project_setup(REQUIRES 6.8)

add_subdirectory(src)

qt_add_executable(appGstStudio main.cpp)

qt_add_qml_module(
    appGstStudio
    URI
    GstStudio
    VERSION
    1.0
    QML_FILES
    Main.qml)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1. If you are developing for
# iOS or macOS you should consider setting an explicit, fixed bundle identifier manually though.
set_target_properties(
    appGstStudio
    PROPERTIES # MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appGstStudio
               MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
               MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
               MACOSX_BUNDLE TRUE
               WIN32_EXECUTABLE TRUE)

target_link_libraries(appGstStudio PRIVATE Qt6::Quick Qt6::QuickControls2 Qt6::Gui Qt6::Qml
                                           gststudio)

# Documentation target
find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
    add_custom_target(
        doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)

    # Optional: Add a custom target to clean documentation
    add_custom_target(
        doc-clean
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_SOURCE_DIR}/docs
        COMMENT "Removing generated documentation"
        VERBATIM)

    message(STATUS "Doxygen found: ${DOXYGEN_EXECUTABLE}")
    message(STATUS "Documentation target 'doc' available")
else()
    message(WARNING "Doxygen not found. Documentation target 'doc' not available")
endif()

include(GNUInstallDirs)
install(
    TARGETS appGstStudio
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
