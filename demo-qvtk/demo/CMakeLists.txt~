
cmake_minimum_required(VERSION 2.8)
 
PROJECT(demo-qvtk)
 
find_package(VTK REQUIRED NO_MODULE)
include(${VTK_USE_FILE})

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB UI_FILES *.ui)
file(GLOB QT_WRAP *.h)
file(GLOB CXX_FILES *.cxx)

QT4_WRAP_UI(UISrcs ${UI_FILES})
QT4_WRAP_CPP(MOCSrcs ${QT_WRAP})
add_executable(demo MACOSX_BUNDLE ${CXX_FILES} ${UISrcs} ${MOCSrcs})

FIND_LIBRARY(AAVE_LIBRARY libaave.a "/home/ccg/acousticave/libaave")
FIND_LIBRARY(ISENSE_LIBRARY libisense64.so "/home/ccg/acousticave/demo-qvtk/isense")
 
if(VTK_LIBRARIES)
  if(${VTK_VERSION} VERSION_LESS "6")
    target_link_libraries(demo ${VTK_LIBRARIES} asound ${AAVE_LIBRARY} ${ISENSE_LIBRARY} QVTK)
  else()
    target_link_libraries(demo ${VTK_LIBRARIES} asound ${ISENSE_LIBRARY} ${AAVE_LIBRARY})
  endif()
else()
  target_link_libraries(demo vtkHybrid QVTK vtkViews asound ${ISENSE_LIBRARY} ${AAVE_LIBRARY} ${QT_LIBRARIES})
endif()
