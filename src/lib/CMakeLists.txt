find_package(Qt6 REQUIRED COMPONENTS Core Quick Gui Qml QuickControls2)

# qt_add_library(gststudio SHARED gstinspectparser.cpp gstinspectparser.h
# gstelementbrowser.h gstelementbrowser.cpp gstpropertymodel.h
# gstpropertymodel.cpp gstpadmodel.h gstpadmodel.cpp gstelement.h )

qt_add_qml_module(
  gststudio
  URI
  GstInspect
  VERSION
  1.0
  SOURCES
  gstinspectparser.cpp
  gstinspectparser.h
  gstelementbrowser.h
  gstelementbrowser.cpp
  gstpropertymodel.h
  gstpropertymodel.cpp
  gstpadmodel.h
  gstpadmodel.cpp
  gstelement.h)

target_link_libraries(gststudio PRIVATE Qt6::Core Qt6::Quick Qt6::Gui Qt6::Qml
                                        Qt6::QuickControls2)

include(GNUInstallDirs)
install(
  TARGETS gststudio
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
