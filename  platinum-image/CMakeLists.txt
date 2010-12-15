#platinumlib CMakelists $Revision$

PROJECT(Platinum CXX)
CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# This file is part of the Platinum library.
# Copyright (c) 2007 Uppsala University.
#
#
#    The Platinum library is free software; you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    The Platinum library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with the Platinum library; if not, write to the Free Software
#    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

INCLUDE_REGULAR_EXPRESSION(".*[.](cc|h|cxx|cp)$")

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Wunused")

FIND_PACKAGE( VTK )
IF ( VTK_FOUND )
  INCLUDE( ${USE_VTK_FILE} )
ENDIF( VTK_FOUND )
INCLUDE_DIRECTORIES( ${VTK_INCLUDE_DIR} )

FIND_PACKAGE( ITK )
IF ( ITK_FOUND )
  INCLUDE( ${USE_ITK_FILE} )
ENDIF( ITK_FOUND )
INCLUDE_DIRECTORIES( ${ITK_INCLUDE_DIR} )

FIND_PACKAGE ( FLTK )
IF (   USE_FLTK_FILE   )
  INCLUDE(  ${USE_FLTK_FILE}  )
ENDIF(   USE_FLTK_FILE   )
INCLUDE_DIRECTORIES( ${FLTK_INCLUDE_DIR}  )

LINK_DIRECTORIES(${ITK_LIBRARY_DIR} ${FLTK_LIBRARY_DIR})
# ${VTK_LIBRARY_DIR}

#SET(UTILITIES_VXL Utilities/vxl/contrib)
#INCLUDE_DIRECTORIES( ${UTILITIES_VXL} )
INCLUDE_DIRECTORIES ( Utilities/vxl/contrib )


#SET(UTILITIES_FLTK Utilities/fltk)
#INCLUDE_DIRECTORIES( ${UTILITIES_FLTK} )					#Location of: Fl_Table.H/cxx Fl_Table_Row.H/cxx
INCLUDE_DIRECTORIES( Utilities/fltk )
INCLUDE_DIRECTORIES( Apps/Histo2D)							#Modify this line to include application in this project
#INCLUDE_DIRECTORIES( Apps/My_App)							#Modify this line to include application in this project
INCLUDE_DIRECTORIES( Apps/T1Mapping)						#Modify this line to include application in this project

IF(WIN32)
  #dirent.h has to be hidden from UNIX compilers
  INCLUDE_DIRECTORIES( Utilities/dirent-win32 )
  SET(WIN32_DIRENT_SRC Utilities/dirent-win32/dirent.cc)
  SET(WIN32_DIRENT_HEAD Utilities/dirent-win32/dirent.h)
ENDIF(WIN32)

OPTION(BUILD_SHARED "Build Platinum as a shared library" OFF)


#SET (PLATINUM_SOURCE_FILES bruker.h color.h colormap.h cursor3D.h data_base.h ${WIN32_DIRENT_HEAD} datamanager.h datawidget.h error.h event.h fileutils.h FLTKuserIOparameters.h FLTKutilities.h FLTKviewport.h global.h histo2D_tool.h histogram.h image_base.h image_binary.h image_complex.h image_general.h image_integer.h image_label.h image_multi.h image_scalar.h image_storage.h landmark_tool.h listedfactory.h metadata.h Utilities/vxl/contrib/mil3d_trilin_interp_3d.h platinumprogram.h point_collection.h points_seq_func1D.h point.h rawimporter.h dicom_importer.h renderer_base.h rendererVTK.h rendererMIP.h rendererMPR.h rendergeometry.h rendercombination.h rendermanager.h ptmath.h statusarea.h stringmatrix.h threshold.h transfer.h transferfactory.h transferfactory.hxx userIO.h userIOmanager.h viewport.h viewporttool.h viewmanager.h bruker.cc color.cc colormap.cc cursor3D.cc data_base.cc datamanager.cc datawidget.cc error.cc fileutils.cc Utilities/fltk/Fl_Table.cxx Utilities/fltk/Fl_Table_Row.cxx FLTKuserIOparameters.cc FLTKutilities.cc FLTKviewport.cc histo2D_tool.cc histogram.cc event.cc histogram.hxx image_base.cc image_binary.hxx image_binaryprocess.hxx image_complex.hxx image_general.hxx image_generalfile.hxx image_storage_iterator.hxx image_integer.hxx image_label.hxx image_multi.hxx image_scalar.hxx image_scalarprocess.hxx image_storage.hxx landmark_tool.cc mainwindow.cc metadata.cc point.cc point_collection.cc points_seq_func1D.hxx rawimporter.cc dicom_importer.cc renderer_base.cc rendererMIP.cc rendererMPR.cc rendererVTK.cc rendergeometry.cc rendercombination.cc rendermanager.cc ptmath.cc statusarea.cc stringmatrix.cc threshold.cc transfer.hxx transfer_interpolated.hxx userIO.cc userIOmanager.cc transferfactory.cc viewport.cc viewporttool.cc viewmanager.cc ${WIN32_DIRENT_SRC} CMakeLists.txt)

SET (PLATINUM_SOURCE_FILES 
additional_data.h
bias_base.h
bias_field.h
brainops.h
bruker.h 
color.h 
colormap.h 
cursor3D.h 
curve.h 
curve_tool.h
data_base.h 
${WIN32_DIRENT_HEAD} 
datamanager.h 
datawidget.h 
dicom_importer.h 
draw_tool.h
error.h 
event.h 
fcm.h 
fileutils.h 
filters.h
FLTKuserIOparameters.h 
FLTKutilities.h 
FLTKviewport.h 
global.h 
graph.hxx
halfedge.h
halfedge_ops.h
histo2D_tool.h 
histogram.h 
image_base.h 
image_binary.h 
image_complex.h 
image_general.h 
image_integer.h 
image_label.h 
image_multi.h 
image_scalar.h 
image_storage.h 
landmark_tool.h 
listedfactory.h 
Mesh.h
meta_tool.h 
metadata.h 
off_parser.h
optimization.h
platinumprogram.h 
point_collection.h 
points_seq_func1D.h 
point.h 
pt_vector.h
ptconfig.h 
rawimporter.h 
rendercombination.h 
renderer_base.h 
renderer_curve.h
renderer_spectrum.h
rendererMIP.h 
rendererMPR.h 
rendererVTK.h 
rendergeometry.h 
rendermanager.h 
ptmath.h 
shape_calc.h
spectrum_importer.h
statusarea.h 
stringmatrix.h 
threshold.h 
transfer.h 
transferfactory.h 
transferfactory.hxx
ultra1dops.h 
ultrasound_importer.h
userIO.h 
userIOmanager.h 
viewport.h 
viewporttool.h 
viewmanager.h 
voxel_tools.h 
Utilities/vxl/contrib/mil3d_trilin_interp_3d.h 
Utilities/configfile/configfile.h
Utilities/vtkFl/vtkFlRenderWindowInteractor.h

additional_data.cc
bias_base.cc
bias_field.hxx
brainops.cc
bruker.cc 
color.cc 
colormap.cc 
cursor3D.cc 
curve.hxx
curve.cc
curve_tool.cc
data_base.cc 
datamanager.cc 
datawidget.cc 
dicom_importer.cc 
draw_tool.cc
error.cc 
event.cc 
fcm.cc
fileutils.cc 
filters.cc
FLTKuserIOparameters.cc 
FLTKutilities.cc 
FLTKviewport.cc 
halfedge.cc
halfedge_ops.cc
histo2D_tool.cc 
histogram.cc 
histogram.hxx 
image_base.cc 
image_binary.hxx 
image_binaryprocess.hxx 
image_complex.hxx 
image_general.hxx 
image_general_iterator.hxx 
image_generalfile.hxx 
image_storage_iterator.hxx 
image_integer.hxx 
image_label.hxx 
image_multi.hxx 
image_scalar.hxx 
image_scalarprocess.hxx 
image_storage.hxx 
landmark_tool.cc 
mainwindow.cc
Mesh.cc 
meta_tool.cc
metadata.cc 
off_parser.cc
optimization.cc
point.cc 
point_collection.cc 
points_seq_func1D.hxx 
ptconfig.cc
pt_vector.cc
rawimporter.cc 
renderer_base.cc 
renderer_curve.cc
renderer_spectrum.cc
rendererMIP.cc 
rendererMPR.cc 
rendererVTK.cc 
rendergeometry.cc 
rendercombination.cc 
rendermanager.cc 
ptmath.cc 
shape_calc.cc
spectrum_importer.cc
statusarea.cc 
stringmatrix.cc 
threshold.cc 
transfer.hxx 
transfer_interpolated.hxx 
transferfactory.cc 
ultra1dops.cc
ultrasound_importer.cc
userIO.cc 
userIOmanager.cc 
viewmanager.cc 
viewport.cc 
viewporttool.cc 
voxel_tools.hxx
${WIN32_DIRENT_SRC} 
CMakeLists.txt
Utilities/fltk/Fl_Table.cxx 
Utilities/fltk/Fl_Table_Row.cxx 
Utilities/configfile/configfile.cc
Utilities/vtkFl/vtkFlRenderWindowInteractor.cxx
)
			

IF(BUILD_SHARED)
  ADD_LIBRARY(platinumlib SHARED ${PLATINUM_SOURCE_FILES})
ELSE(BUILD_SHARED)
  ADD_LIBRARY(platinumlib STATIC ${PLATINUM_SOURCE_FILES})
ENDIF(BUILD_SHARED)

LINK_DIRECTORIES (${Platinum_BINARY_DIR})

#IF(APPLE)
#  INSTALL(DIRECTORY Resources/Icons DESTINATION ${EXECUTABLE_OUTPUT_PATH}/PlatinumTest.app/Contents/Resources/Graphics )
#ELSE (APPLE)
#  INSTALL(DIRECTORY Resources/Icons DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Resources/Graphics )
#ENDIF(APPLE)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

# _DEBUG flag will be defined when in debug mode
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

# Relax syntax requirements for GCC, since MSVC cannot shape up 
#IF(NOT MSVC)
#  ADD_DEFINITIONS(-fpermissive)
#ENDIF(NOT MSVC)

IF(WIN32)
  OPTION(FLTK_HAS_DEPENDENT_NAMES "FLtk debug lib is named fltkd" OFF)
  MARK_AS_ADVANCED(FLTK_HAS_DEPENDENT_NAMES)
  TARGET_LINK_LIBRARIES (platinumlib wsock32 comctl32)
  IF (FLTK_HAS_DEPENDENT_NAMES)
    TARGET_LINK_LIBRARIES (platinumlib debug fltkgld optimized fltkgl debug fltkd optimized fltk)
  ELSE(FLTK_HAS_DEPENDENT_NAMES)
    TARGET_LINK_LIBRARIES (platinumlib fltk_gl fltk )
  ENDIF(FLTK_HAS_DEPENDENT_NAMES)
ENDIF(WIN32)

IF(UNIX)
  LINK_DIRECTORIES(/usr/local/lib )
  IF(APPLE)
  ELSE(APPLE)
    LINK_DIRECTORIES (/usr/X11R6/lib)
  ENDIF (APPLE)
  TARGET_LINK_LIBRARIES (platinumlib fltk_gl fltk )
  #TARGET_LINK_LIBRARIES(platinumlib GLU GL m  Xext X11  supc++)
ENDIF(UNIX)

TARGET_LINK_LIBRARIES(platinumlib ITKCommon ITKIO ITKBasicFilters ${FLTK_LIBRARIES})		#ITKBasicFilters --> ITK --> VTK filters...
TARGET_LINK_LIBRARIES(platinumlib vtkRendering vtkVolumeRendering vtkIO)

#test program for quick compile cycles
#MACOSX_BUNDLE only affects OS X builds, and is required for GUI to function
ADD_EXECUTABLE(PlatinumTest MACOSX_BUNDLE test_app.cc)
ADD_EXECUTABLE(Histo2D MACOSX_BUNDLE Apps/Histo2D/Histo2D.cc)								#Modify this line to include application in this project
ADD_EXECUTABLE(HighResUS MACOSX_BUNDLE Apps/HighResUS/HighResUS.cc)
#ADD_EXECUTABLE(My_App MACOSX_BUNDLE Apps/My_App/My_App.cc)									#Modify this line to include application in this project
ADD_EXECUTABLE(T1Mapping MACOSX_BUNDLE Apps/T1Mapping/T1Mapping.cc)							#Modify this line to include application in this project



IF(APPLE)
   FIND_PROGRAM(ITK_APPLE_RESOURCE Rez /Developer/Tools)
   IF(ITK_APPLE_RESOURCE)
      ADD_CUSTOM_COMMAND(
        TARGET PlatinumTest.app/Contents/Resources/PlatinumTest.rsrc POST_BUILD
        TARGET Histo2D.app/Contents/Resources/Histo2D.rsrc POST_BUILD						#Modify this line to include application in this project
        TARGET HighResUS.app/Contents/Resources/HighResUS.rsrc POST_BUILD
        #TARGET My_App.app/Contents/Resources/My_App.rsrc POST_BUILD						#Modify this line to include application in this project
        TARGET T1Mapping.app/Contents/Resources/T1Mapping.rsrc POST_BUILD					#Modify this line to include application in this project
  
    
        COMMAND ${ITK_APPLE_RESOURCE} ARGS -useDF ${FLTK_INCLUDE_DIR}/mac.r
        WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
         )
   ENDIF(ITK_APPLE_RESOURCE)
ENDIF(APPLE)

TARGET_LINK_LIBRARIES(PlatinumTest platinumlib)
TARGET_LINK_LIBRARIES(Histo2D platinumlib)							#Modify this line to include application in this project
TARGET_LINK_LIBRARIES(HighResUS platinumlib)
#TARGET_LINK_LIBRARIES(My_App platinumlib)							#Modify this line to include application in this project
TARGET_LINK_LIBRARIES(T1Mapping platinumlib)						#Modify this line to include application in this project

ADD_DEPENDENCIES(PlatinumTest platinumlib)
ADD_DEPENDENCIES(Histo2D platinumlib)								#Modify this line to include application in this project
ADD_DEPENDENCIES(HighResUS platinumlib)
#ADD_DEPENDENCIES(My_App platinumlib)								#Modify this line to include application in this project
ADD_DEPENDENCIES(T1Mapping platinumlib)								#Modify this line to include application in this project


#SOURCE_GROUP(Headers REGULAR_EXPRESSION ".*[.]h$")
#SOURCE_GROUP(Headers FILES global.h platinumprogram.h ptmath.h)

SOURCE_GROUP(ImageVarieties FILES 
image_binary.h 
image_complex.h 
image_integer.h 
image_label.h 
image_multi.h 
image_scalar.h 
image_binary.hxx 
image_complex.hxx 
image_integer.hxx 
image_label.hxx 
image_multi.hxx 
image_scalar.hxx)

SOURCE_GROUP(Data FILES 
additional_data.h
curve.h 
datamanager.h 
data_base.h 
halfedge.h
image_base.h 
image_general.h 
image_storage.h 
Mesh.h
metadata.h 
off_parser.h
optimization.h
point.h 
point_collection.h 
points_seq_func1D.h 
rawimporter.h 
spectrum_importer.h
dicom_importer.h 
ultrasound_importer.h
voxel_tools.h

additional_data.cc
curve.hxx
curve.cc
datamanager.cc 
data_base.cc 
halfedge.cc
image_base.cc 
image_general.hxx 
image_general_iterator.hxx 
image_generalfile.hxx 
image_storage_iterator.hxx 
image_storage.hxx 
Mesh.cc
metadata.cc 
off_parser.cc
optimization.cc
point_collection.cc 
point.cc 
points_seq_func1D.hxx 
rawimporter.cc 
spectrum_importer.cc
dicom_importer.cc
ultrasound_importer.cc
voxel_tools.hxx)

SOURCE_GROUP(Processing FILES 
image_binaryprocess.hxx 
image_scalarprocess.hxx)

SOURCE_GROUP(View FILES 
viewport.h 
FLTKviewport.h 
viewmanager.h 
renderer_base.h 
renderer_curve.h
renderer_spectrum.h
rendererVTK.h 
rendererMIP.h 
rendererMPR.h 
rendercombination.h 
rendergeometry.h 
rendermanager.h 
transfer.h 
viewport.cc 
FLTKviewport.cc 
mainwindow.cc 
viewmanager.cc 
renderer_base.cc 
renderer_curve.cc
renderer_spectrum.cc
rendererVTK.cc 
rendererMIP.cc 
rendererMPR.cc 
rendercombination.cc 
rendergeometry.cc 
rendermanager.cc 
transfer.hxx 
transfer_interpolated.hxx)

SOURCE_GROUP(Interaction FILES 
curve_tool.h
datawidget.h 
draw_tool.h
event.h 
userIO.h 
FLTKuserIOparameters.h 
histo2D_tool.h 
landmark_tool.h 
meta_tool.h 
transferfactory.h 
transferfactory.hxx 
userIOmanager.h 
viewporttool.h 
curve_tool.cc
datawidget.cc 
draw_tool.cc
event.cc 
userIO.cc 
FLTKuserIOparameters.cc 
histo2D_tool.cc 
landmark_tool.cc 
meta_tool.cc
statusarea.h 
transferfactory.cc 
userIOmanager.cc 
viewporttool.cc 
statusarea.cc) 
#test_app.cc)

SOURCE_GROUP(Helpers FILES 
bias_base.h
bias_field.h
brainops.h
bruker.h 
color.h 
colormap.h 
cursor3D.h 
error.h 
fcm.h
fileutils.h 
filters.h
FLTKutilities.h 
halfedge_ops.h
histogram.h 
listedfactory.h 
pt_vector.h
ptconfig.h
shape_calc.h
threshold.h 
ultra1dops.h

bias_base.cc
bias_field.hxx
brainops.cc
bruker.cc 
color.cc 
colormap.cc 
cursor3D.cc 
${WIN32_DIRENT_SRC} 
error.cc 
fcm.cc
fileutils.cc 
filters.cc
FLTKutilities.cc 
graph.hxx
halfedge_ops.cc
histogram.cc 
histogram.hxx 
pt_vector.cc
ptconfig.cc
ptmath.cc 
shape_calc.cc
stringmatrix.cc 
threshold.cc
ultra1dops.cc)

SOURCE_GROUP(Utilities FILES 
Utilities/fltk/Fl_Table.cxx 
Utilities/fltk/Fl_Table_Row.cxx 
Utilities/vxl/contrib/mil3d_trilin_interp_3d.h
Utilities/configfile/configfile.h
Utilities/configfile/configfile.cc
Utilities/vtkFl/vtkFlRenderWindowInteractor.h
Utilities/vtkFl/vtkFlRenderWindowInteractor.cxx
)

#add CMakelists for inclusion in version control
SOURCE_GROUP(Scripts CMakeLists.txt)


#Write PlatinumConfig.cmake so client programs can be built with Platinum

#CMAKE_EXPORT_BUILD_SETTINGS(${Platinum_BINARY_DIR}/PlatinumBuildSettings.cmake)
SET(Platinum_LIBRARY_DEPENDS_FILE ${Platinum_BINARY_DIR}/PlatinumLibraryDepends.cmake)
EXPORT_LIBRARY_DEPENDENCIES(${Platinum_BINARY_DIR}/PlatinumLibraryDepends.cmake)
CONFIGURE_FILE("${Platinum_SOURCE_DIR}/CMake/PlatinumConfig.cmake.in"
  "${Platinum_BINARY_DIR}/PlatinumConfig.cmake" @ONLY IMMEDIATE)



#----------------------------------------------------------------------
#          Reading file that sets up private applications
#----------------------------------------------------------------------
IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../PlatinumPrivateApps/CMakeLists_PrivateApps.txt")
	INCLUDE("../PlatinumPrivateApps/CMakeLists_PrivateApps.txt")
ENDIF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../PlatinumPrivateApps/CMakeLists_PrivateApps.txt")
