//////////////////////////////////////////////////////////////////////////
//
//  Image_label $Revision$
//
//  Image type with integer values signifying distinct classes or "labels"
//  (i.e. not continuous values). Can be described as multiple binary
//  images rolled into one. 0 (zero) carries special meaning as
//  empty/background.
//
//  $LastChangedBy$

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

#ifndef __image_label__
#define __image_label__

#include "image_integer.h"

//forward declaration
template<int IMAGEDIM>
class image_binary;

template<int IMAGEDIM = 3>
class image_label : public image_integer <IMGLABELTYPE, IMAGEDIM>
{
public:
	image_label(int w, int h, int d, IMGLABELTYPE *ptr = NULL);
    image_label(itk::SmartPointer< itk::OrientedImage<IMGLABELTYPE, IMAGEDIM > > &i);

	template<class SOURCETYPE>
	image_label(image_general<SOURCETYPE, IMAGEDIM> * old_image, bool copyData = true): image_integer<IMGLABELTYPE, IMAGEDIM>(old_image, copyData)
	{
	transfer_function();
	} //copy constructor

//	template<class SOURCETYPE>
//		image_label(image_general<SOURCETYPE, IMAGEDIM> *old_image, bool copyData = true);

	image_label(IMGLABELTYPE * inData, unsigned long inDataNumElems, long width, long height, Vector3D voxelSize);		//pre-loaded raw constructor
	image_label(std::vector<std::string> files, long width, long height, bool bigEndian = false, long headerSize = 0, Vector3D voxelSize = Vector3D (1,1,4), unsigned int startFile = 1,unsigned int increment = 1);
	image_label(const string filepath, const string name="");
	image_label(vector< image_binary<IMAGEDIM>* > images, const string name="");
//JK: TMP removal due to Histo2D - compile error!
	image_label(image_binary<IMAGEDIM>* image1, image_binary<IMAGEDIM>* image2, image_binary<IMAGEDIM>* image3, image_binary<IMAGEDIM>* image4=NULL, image_binary<IMAGEDIM>* image5=NULL, const string name="");

    virtual void transfer_function(transfer_scalar_base<IMGLABELTYPE> * t = NULL); //NOTE: must be called by all constructors in this class!

	string resolve_tooltip();		//combines tooltip data of this class with data from other classes
	string resolve_tooltip_image_label(); //resolves tooltip data typical for this class

	image_label<IMAGEDIM>* get_subvolume_from_slice_3D(int slice, int dir=2);
	image_label<IMAGEDIM>* get_subvolume_from_region_3D(Vector3Dint vox_pos, Vector3Dint vox_size);
	image_label<IMAGEDIM>* get_subvolume_from_region_3D(int x1, int y1, int z1, int x2, int y2, int z2);

	int get_label_with_largest_span_in_dir_3D(int dir);

};

template <int IMAGEDIM>
image_label<IMAGEDIM>* label_copycast (image_base* const input);

//template<class ELEMTYPE, int IMAGEDIM = 3>
//image_label<IMAGEDIM>* creat_image_label_from_highest_intensities(vector< image_scalar<ELEMTYPE,IMAGEDIM>* > v);

#endif