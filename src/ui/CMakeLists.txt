cmake_minimum_required(VERSION 2.8) 
project(simtoi_ui CXX)

# OpenGL is required
if(NOT OpenGL_FOUND)
	find_package(OpenGL REQUIRED)
endif(NOT OpenGL_FOUND)

# As is QT4, along with several macros:
find_package(Qt4 COMPONENTS QtCore QtGui QtMain QtOpenGL REQUIRED)
INCLUDE(${QT_USE_FILE})
#ADD_DEFINITIONS(${QT_DEFINITIONS})
file(GLOB simtoi_FORMS *.ui)
file(GLOB simtoi_RESOURCES *.qrc)
QT4_WRAP_CPP(simtoi_HEADERS_MOC CGLWidget.h CUI.h)
QT4_WRAP_UI(simtoi_FORMS_HEADERS ${simtoi_FORMS})
#QT4_ADD_RESOURCES(simtoi_RESOURCES_RCC ${simtoi_RESOURCES})

INCLUDE_DIRECTORIES(. ${CMAKE_CURRENT_BINARY_DIR})
file(GLOB SOURCE *.cpp)

add_library(simtoi_ui ${SOURCE} ${simtoi_HEADERS_MOC} ${simtoi_FORMS_HEADERS} ${simtoi_RESOURCES_RCC})
target_link_libraries(simtoi_ui ${QT_LIBRARIES})
