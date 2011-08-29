# CMake file for Arvid Rudling's 2D histogram
# master thesis project

PROJECT(Histo2D)
CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

INCLUDE_REGULAR_EXPRESSION(".*[.](cc|h|cxx|cp)$")

FIND_PACKAGE (Platinum)
INCLUDE_DIRECTORIES( ${Platinum_INCLUDE_DIR} )

FIND_PACKAGE( ITK )
IF ( ITK_FOUND )
  INCLUDE( ${USE_ITK_FILE} )
ENDIF( ITK_FOUND )

INCLUDE_DIRECTORIES( ${ITK_INCLUDE_DIR} )

FIND_PACKAGE ( FLTK )
IF (   USE_FLTK_FILE   )
  INCLUDE(  ${USE_FLTK_FILE}  )
ENDIF(   USE_FLTK_FILE   )

INCLUDE_DIRECTORIES( ${FLTK_INCLUDE_DIR} )

LINK_DIRECTORIES(${ITK_LIBRARY_DIR} ${FLTK_LIBRARY_DIR} ${Platinum_LIBRARY_DIR})

#MACOSX_BUNDLE only affects OS X builds, and is required for GUI to function
ADD_EXECUTABLE(Histo2D MACOSX_BUNDLE Histo2D.cc)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

# _DEBUG flag will be defined when in debug mode
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

IF(APPLE)
   FIND_PROGRAM(ITK_APPLE_RESOURCE Rez /Developer/Tools)
   IF(ITK_APPLE_RESOURCE)
      ADD_CUSTOM_COMMAND(
         TARGET Histo2D.app/Contents/Resources/Histo2D.rsrc POST_BUILD
         COMMAND ${ITK_APPLE_RESOURCE} ARGS -useDF ${FLTK_INCLUDE_DIR}/mac.r
         WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
         )
   ENDIF(ITK_APPLE_RESOURCE)
ENDIF(APPLE)

TARGET_LINK_LIBRARIES(Histo2D platinumlib)

#add header files to project
SOURCE_GROUP(Headers REGULAR_EXPRESSION ".*[.]h$")

#add CMakelists for inclusion in integrated version control
SOURCE_GROUP(Scripts CMakeLists.txt)