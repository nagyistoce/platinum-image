//////////////////////////////////////////////////////////////////////////
//
//  Image_complex $Revision$
//
//  Image class with complex voxels
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

#ifndef __image_complex__
#define __image_complex__

//#include "image_multi.h"
#include "image_general.h"

#include "transfer.h"

#include <complex>
#include <typeinfo> //typeid


template <class ELEMTYPE, int IMAGEDIM = 3>
class image_complex : public image_general<complex<ELEMTYPE> , IMAGEDIM>
//class image_complex : public image_general<ELEMTYPE , IMAGEDIM>
//class image_complex : public image_multi<ELEMTYPE, IMAGEDIM>
{
	friend class image_storage<ELEMTYPE>;

private:
	complex<ELEMTYPE> min_complex;
	complex<ELEMTYPE> max_complex;

protected:
	void set_complex_parameters();
   	histogram_1D<ELEMTYPE> *stats;
	void set_stats_histogram(histogram_1D<ELEMTYPE > * h);
	virtual void stats_refresh(bool min_max_refresh=false);

	transfer_complex<ELEMTYPE,IMAGEDIM> *tfunction;
    virtual void transfer_function(transfer_complex<ELEMTYPE,IMAGEDIM> * t = NULL); //NOTE: must be called by all constructors in this class!
	virtual void transfer_function(std::string functionName){}; //! replace transfer function using string identifier

	void set_parameters(itk::SmartPointer< itk::OrientedImage<std::complex<ELEMTYPE>, IMAGEDIM > > &i);   //set parameters from ITK metadata

//	virtual histogram_1D<ELEMTYPE>* get_stats(){return stats;} //called by image_storage
	virtual void set_max(float m){if(stats!=NULL) stats->max(m);}
	virtual void set_min(float m){if(stats!=NULL) stats->min(m);}

public:
    image_complex();//:image_general<complex<ELEMTYPE>, IMAGEDIM>();
	image_complex(int w, int h, int d, complex<ELEMTYPE> *ptr = NULL);//:image_general<complex<ELEMTYPE>, IMAGEDIM>(w,h,d,ptr);
	image_complex(string path_image_re, string path_image_im, float unsigned_offset=2047, string name="");
    
//    template<class SOURCETYPE>
//    image_complex(image_multi<SOURCETYPE, IMAGEDIM> * old_image, bool copyData = true): image_multi<complex<float> , IMAGEDIM>(old_image, copyData)
//    {} //copy constructor
    
//    image_complex (std::vector<std::string> files, long width, long height, bool bigEndian = false, long headerSize = 0, Vector3D voxelSize = Vector3D (1,1,4), unsigned int startFile = 1,unsigned int increment = 1): image_multi<complex<float> , IMAGEDIM> (files, width, height, bigEndian, headerSize, voxelSize, startFile,increment) {}

	virtual void get_display_voxel(RGBvalue &val,int x, int y, int z=0) const;
	virtual string resolve_value_world(Vector3D worldPos);

	void silly_test();
	complex<ELEMTYPE> get_max() const;
	complex<ELEMTYPE> get_min() const;

	float get_number_voxel(int x, int y, int z) const;
    virtual float get_max_float() const;
    virtual float get_min_float() const;
	float get_max_float_safe() const;
	float get_min_float_safe() const;


	ELEMTYPE get_voxel_re(int x, int y, int z) const;
	ELEMTYPE get_voxel_im(int x, int y, int z) const;
	ELEMTYPE get_voxel_magn(int x, int y, int z) const;
	float get_voxel_phase(int x, int y, int z) const;  //returns phase in radians [-PI, PI]

	image_scalar<ELEMTYPE, IMAGEDIM>* get_magnitude_image();


	typename itk::OrientedImage<std::complex<ELEMTYPE>, IMAGEDIM >::Pointer	get_complex_image_as_itk_output();

// -------------- Scalar Load/Save functions ---------------
	void load_dataset_from_VTK_file(std::string file_path);
	void load_dataset_from_meta_file(std::string file_path);

	void load_complex_dataset_from_these_DICOM_files(vector<string> filenames);
	void save_to_VTK_file(const std::string file_path, const bool useCompression = true);
    void save_to_DCM_file(const std::string file_path, const bool useCompression = false, const bool anonymize = true);



};


//-------------------------------- operators -----------------------------------------

/*
template <class T>
bool operator<(complex<T> &c1, complex<T> &c2)
{
	if(abs(c1)<abs(c2))
		return true;

	return false;
}

template <class T>
bool operator>(complex<T> &c1, complex<T> &c2)
{
	if(abs(c1)>abs(c2))
		return true;

	return false;
}

template <class T>
bool operator<=(complex<T> &c1, complex<T> &c2)
{
	if(abs(c1)<=abs(c2))
		return true;

	return false;
}

template <class T>
bool operator>=(complex<T> &c1, complex<T> &c2)
{
	if(abs(c1)=>abs(c2))
		return true;

	return false;
}
*/
//------------------------------------------------------------------------------------

#endif
