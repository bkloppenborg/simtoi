cmake_minimum_required(VERSION 2.8) 
project(simtoi CXX C Fortran)

# OpenGL is required
find_package(OpenGL REQUIRED)

# As is QT4, along with several macros:
find_package(Qt4 COMPONENTS QtCore QtGui QtMain QtOpenGL REQUIRED)
INCLUDE(${QT_USE_FILE})
#ADD_DEFINITIONS(${QT_DEFINITIONS})
file(GLOB simtoi_FORMS *.ui)
file(GLOB simtoi_RESOURCES *.qrc)
QT4_WRAP_CPP(simtoi_HEADERS_MOC main.h CUI.h)
QT4_WRAP_UI(simtoi_FORMS_HEADERS ${simtoi_FORMS})
#QT4_ADD_RESOURCES(simtoi_RESOURCES_RCC ${simtoi_RESOURCES})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

# include directories for the oilib:
include_directories(${OI_INCLUDE_DIRS})

# Enable prototype functions in OpenGL (even though some are no longer prototypes)
set(CMAKE_CXX_FLAGS "-DGL_GLEXT_PROTOTYPES")

# Assemble all of the source to build simtoi.
file(GLOB SOURCE *.cpp)
add_executable(simtoi ${SOURCE} ${simtoi_HEADERS_MOC} ${simtoi_FORMS_HEADERS} ${simtoi_RESOURCES_RCC})
target_link_libraries(simtoi textio oi multinest ${QT_LIBRARIES})
