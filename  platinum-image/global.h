//////////////////////////////////////////////////////////////////////////
//
//  global constants and macros $Revision$
//
//  $LastChangedBy$
//

// This file is part of the Platinum library.
// Copyright (c) 2007 Uppsala University.
//
//    The Platinum library is free software; you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    The Platinum library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with the Platinum library; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef __global_h__
#define __global_h__
#include "string.h"



#define PLATINUM_MAJOR_VER 0
#define PLATINUM_MINOR_VER 1

#define TESTMODE	//JK test mode --> "dcm_import_button"

#define FLTK_LABEL_SIZE 14
#define FLTK_SMALL_LABEL 11
#define FLTK_SCROLLBAR_SIZE 16

#define BUTTONHEIGHT 25
#define DATALISTINITWIDTH 270

#define DATALISTINITHEIGHT 300

#define LIST_HEADER_HEIGHT 25 //"DATA" and "TOOLS"

#define NOT_FOUND_ID -1
#define NA 0 // not applicable

#define RGB_pixmap_bpp 3 //RGB_pixmap_bytesperpixel
#define RGBA_pixmap_bpp 4 //RGBA_pixmap_bytesperpixel

#define IMGELEMCOMPTYPE unsigned char	//image element computer type...
#define IMGELEMCOMPMAX 255.0
#define RADDR 0
#define GADDR 1
#define BADDR 2
#define AADDR 3

#define FILEPOSTYPE char
#define VOLUMEMAXDIMENSION 3
#define IMGLABELMAX 19
#define ZOOM_CONSTANT 50.0  // physical display size (mm) at zoom 1, in viewport x/horizontal direction... -previously denoted "display_scale"


enum ROImode {ROI_RECT, ROI_CIRCLE};
enum COMBINE_MODE {COMB_ADD, COMB_SUB, COMB_MULT, COMB_DIV, COMB_MAX, COMB_MIN, COMB_MEAN_NON_ZERO, COMB_MAGN}; //and, xor, mask...
enum PROJECTION_MODE {PROJ_MAX, PROJ_MEAN, PROJ_MIN}; 
enum DICOM_LOADER_TYPE {DCM_LOAD_ALL, DCM_LOAD_SERIES_ID_ONLY}; 	//LOAD_ALL is used by dicom_importer to load all selected files (ignoring series_ID)....
enum POINT_TYPE { MAX_GRAD_MAG_X, MAX_GRAD_MAG_Y, MAX_GRAD_MAG_Z, MAX_GRAD_MAG_XY, MAX_GRAD_MAG_XZ, MAX_GRAD_MAG_YZ, MAX_GRAD_MAG_XYZ };
enum GRAD_MAG_TYPE { X, Y, Z, XYZ, XY, XZ, YZ };
enum WEIGHT_TYPE { CHESSBOARD };
enum preset_direction {DEFAULT_DIR, AXIAL, SAGITTAL, CORONAL, Z_DIR, Y_DIR, X_DIR, Z_DIR_NEG, Y_DIR_NEG, X_DIR_NEG, NUM_DIRECTIONS};
enum colors {WHITE, BLACK, RED, GREEN, BLUE, YELLOW, MAGENTA, CYAN};
//enum preset_slice_orientation {AXIAL, SAGITTAL, CORONAL, UNDEFINED};
enum SPACE_TYPE {VOXEL_SPACE, PHYSICAL_SPACE};
enum VIEWPORT_TYPE {PT_MPR, PT_MIP, VTK_EXAMPLE, VTK_MIP, VTK_ISOSURF, PT_CURVE, PT_SPECTRUM};
enum ZERO_HANDLING_TYPE {ZERO_HANDLING_LEAVE, ZERO_HANDLING_SET_ZERO, ZERO_HANDLING_SET_MIN};
enum DCM_IMPORT_WIN_TYPE {DCM_FILES, DCM_SERIES};
enum RENDERER_TYPE {RENDERER_MPR=0, RENDERER_MIP=1, NUM_RENDERER_TYPES, NUM_RENDERER_TYPES_PLUS_END, RENDERER_CURVE = 2, RENDERER_SPECTRUM};
enum ADDITIONAL_TYPE {AT_POINT, AT_LINE, AT_RECT, AT_CIRCLE, AT_GAUSS, AT_STRING, AT_FREEHAND};
enum SPECTRUM_TYPE {SP_REAL, SP_COMPLEX, SP_MAGNITUDE, SP_PHASE, SP_FREQ, SP_TIME};


//Dicom Tag definitions
//More DICOM tag definitions can be found at:  http://medical.nema.org/dicom/2007/07_06pu.pdf
//Note that it is important to use "lower case letters" for the gdcm package...


#define DCM_CREATION_DATE "0008|0012" //20050114	//string
#define DCM_CREATION_TIME "0008|0013" //082633.000	//string
#define DCM_STUDY_DATE "0008|0020"					//string
#define DCM_SERIES_DATE "0008|0021"					//string
#define DCM_SERIES_ID "0020|000e"					//string
#define DCM_MODALITY "0008|0060"					//string
#define DCM_IMAGE_TYPE "0008|0008"					//string
#define DCM_STUDY_DESCRIPTION "0008|1030"			//string
#define DCM_SERIES_DESCRIPTION "0008|103e"			//string

#define DCM_PATIENT_NAME "0010|0010"				//string
#define DCM_PATIENT_ID "0010|0020"					//string
#define DCM_PATIENT_BIRTH_DATE "0010|0030"			//string
#define DCM_PATIENT_SEX "0010|0040"					//string
#define DCM_PATIENT_WEIGHT "0010|1030"				//float

#define DCM_SCANNING_SEQ "0018|0020"				//string (GR)
#define DCM_SEQ_VARIANT "0018|0021"					//string (SS)
#define DCM_SCAN_OPTION "0018|0022"					//string (SP)
#define DCM_MR_ACQ_TYPE "0018|0023"					//string (2D)
#define DCM_TR "0018|0080"							//float
#define DCM_TE "0018|0081"							//float
#define DCM_FLIP "0018|1314"						//float
#define DCM_NSA "0018|0083"							//int
#define DCM_IMAGING_FREQUENCY "0018|0084"			//float (63.9013)
#define DCM_SLICE_THICKNESS "0018|0050"				//float
#define DCM_SLICE_SPACING "0018|0088"				//float
#define DCM_FOV "0018|1100"							//float (Reconstruction diam?)
#define DCM_B0 "0018|0087"							//float
#define DCM_NO_PHASE_ENC "0018|0089"				//int
#define DCM_PERCENT_SAMPLING "0018|0093"			//float
#define DCM_RFOV "0018|0094"						//float
#define DCM_PROTOCOL_NAME "0018|1030"				//string
#define DCM_TRANSMIT_COIL "0018|1251"				//string
#define DCM_RECEIVE_COIL "0018|1250"				//string
#define DCM_PHASE_ENCODING_DIRECTION "0018|1312"	//string (COL)
#define DCM_PATIENT_POSITION "0018|5100"			//string (FFS)
#define DCM_PULSE_SEQUENCE_NAME "0018|9005"			//string

#define DCM_SERIES_NUMBER "0020|0011"				//int (201)
#define DCM_ACQ_NUMBER "0020|0012"					//int (2)
#define DCM_IMAGE_POSITION_PATIENT "0020|0032"		//string (-265.0\-317.646\-614.334)
#define DCM_IMAGE_ORIENTATION_PATIENT "0020|0037"	//string (1.0\0.0\0.0\0.0\1.0\0.0)
#define DCM_TEMP_POS_ID "0020|0100"					//string (4/8)

#define DCM_ROWS "0028|0010"						//int (256)
#define DCM_COLUMNS "0028|0011"						//int (256)
#define DCM_PIXEL_SPACING "0028|0030"				//string (2.07031\2.07031 // dx\dy)
#define DCM_BITS_ALLOCATED "0028|0100"				//int (16)
#define DCM_BITS_STORED "0028|0101"					//int (12)
#define DCM_HIGH_BIT "0028|0102"					//int (11)
#define DCM_WINDOW_CENTER "0028|1050"				//float (396.0)
#define DCM_WINDOW_WIDTH "0028|1051"				//float (792.0)
#define DCM_RESCALE_INTERCEPT "0028|1052"			//float (0.0)
#define DCM_RESCALE_SLOPE "0028|1053"				//float (1.94286)
#define DCM_RESCALE_TYPE "0028|1054"				//string (normalized)

#define DCM_SCALE_INTERCEPT "0029|1052"				//float
#define DCM_SCALE_SLOPE "0029|1053"					//float
#define DCM_SCALE_INTERCEPT2 "2005|100d"			//float (0.0)
#define DCM_SCALE_SLOPE2 "2005|100e"				//float (0.0037668764)

#define DCM_SLICE_ORIENTATION "2001|100b"			//string (TRANSVERSAL/CORONAL/SAGITTAL)
//#define DCM_SLICE_ORIENTATION "2001|100B"			//string (TRANSVERSAL/CORONAL/SAGITTAL)
#define DCM_B_VALUE "2001|1003"						//float diffusion b-value (found in Philips Dicom file)


#define DCM_MAGNITUDE_SUBSTRING "\\M\\"				//Part of the "DCM_IMAGE_TYPE" charateristic for magnitude data...
#define DCM_REAL_SUBSTRING "\\R\\"					//Part of the "DCM_IMAGE_TYPE" charateristic for real data...
#define DCM_IMAGINARY_SUBSTRING "\\I\\"				//Part of the "DCM_IMAGE_TYPE" charateristic for imaginary data...

/*
string FLTK_eventnames[] =
    {
    //array allows event names to be printed to strings and whatnot
    "FL_NO_EVENT",		//0
    "FL_PUSH",			//1
    "FL_RELEASE",		//2
    "FL_ENTER",		    //3
    "FL_LEAVE",		    //4
    "FL_DRAG",			//5
    "FL_FOCUS",		    //6
    "FL_UNFOCUS",		//7
    "FL_KEYDOWN",		//8
    "FL_KEYUP",	    	//9
    "FL_CLOSE",	    	//10
    "FL_MOVE",			//11
    "FL_SHORTCUT",		//12
    "FL_DEACTIVATE",	//13
    "FL_ACTIVATE",		//14
    "FL_HIDE",			//15
    "FL_SHOW",			//16
    "FL_PASTE",		    //17
    "FL_SELECTIONCLEAR",//18
    "FL_MOUSEWHEEL",	//19
    "FL_DND_ENTER", 	//20
    "FL_DND_DRAG",		//21
    "FL_DND_LEAVE", 	//22
    "FL_DND_RELEASE",	//23
    };
*/

#endif