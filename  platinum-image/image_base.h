//////////////////////////////////////////////////////////////////////////
//
//  Image_base $Revision $
//
//  Untemplated abstract base class for image storage, including images
//  contains 3D position and orientation information
//
//  $LastChangedBy $
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

#ifndef __image_base__
#define __image_base__

#include "data_base.h"

#include "global.h"
#include "color.h"

#include "itkVTKImageIO.h"
//#include "bruker.h"

#include "itkMetaDataDictionary.h"	//used in dicomloader::
#include "itkMetaDataObject.h"		//used in dicomloader::
#include "itkGDCMImageIO.h"			//used in dicomloader::
#include "itkDICOMImageIO2.h"		//JK compresstion test...
#include "itkNiftiImageIO.h"
#include "itkAnalyzeImageIO.h"
#include "itkGE5ImageIO.h"

#include "gdcmFileHelper.h"			//used in dicomloader::

enum imageDataType
    {
    VOLDATA_UNDEFINED,
    VOLDATA_DOUBLE      = -16,
    VOLDATA_FLOAT       = -8,
    VOLDATA_LONG        = -4,
    VOLDATA_SHORT       = -2,
    VOLDATA_CHAR        = -1,
    VOLDATA_BOOL        =  0,
    VOLDATA_UCHAR       =  1,
    VOLDATA_USHORT      =  2,
    VOLDATA_ULONG       =  4
    };


//Take note: the purpose of the base class is to allow access to instances in a
//template-unaware fashion. Hence the base class cannot be templated.

class image_base : public data_base
    {

    private:
        void set_parameters ();

    protected:
        image_base();
        image_base(image_base* const s);

        void redraw();

        // *** cached data ***

        //image parameters
        //TODO: obtain from functions instead of storing
        
        Vector3D origin;
        Matrix3D orientation;
		//Note: Vector3D voxel_size is defined in image_general...
		string slice_orientation; // undefined/axial/coronal/sagittal (lower case letters...)



    public:
		static vector<RENDERER_TYPE> get_supported_renderers();
        virtual image_base * alike (imageDataType unit) = 0;
       
        virtual void initialize_dataset(int w, int h, int d) = 0; // create empty dataset

        template <class LOADERTYPE>
            static void try_loader (std::vector<std::string> * f); //! helper for image_base::load
        static void load(const std::vector<std::string> files);  //load files in supported formats
                                                    //as selected in "files" vector

        virtual void transfer_function(std::string functionName)=0; //! replace transfer function using string identifier
                    
        virtual void get_display_voxel(RGBvalue &val,int x, int y, int z=0) const = 0; //uses transfer function to map its data to rgb
        virtual float get_number_voxel(int x, int y, int z) const = 0;//get value as float for onscreen display in numbers (e.g. complex values--> scalars)
                                                                    //when other kinds than 3D images are implemented,
                                                                    //one might want to make these dimensionality-independent 
                                                                    //like get_size_by_dim(int dim)
        
        virtual float get_min_float() const = 0; //return max/min values in type-independent form     
        virtual float get_max_float() const = 0;
        virtual float get_display_min_float() const = 0;
        virtual float get_display_max_float() const = 0;
            
        virtual void testpattern() = 0 ;

//        virtual void port_to_itk_format() = 0; // previously "make_image_an_itk_reader"

        virtual unsigned short get_size_by_dim(int dim) const = 0;  //return voxel dimensions
        virtual Vector3D get_physical_size () const = 0; //return size in world coordinates
        virtual Vector3D get_physical_center() const = 0; //return center in world coordinates
        virtual bool same_size (image_base *) = 0;
        virtual bool is_voxelpos_within_image_3D(Vector3Dint vox_pos)=0;
//        virtual bool is_voxelpos_within_image_3D(Vector3Dint vox_pos) = 0;
        
        bool read_origin_from_dicom_file(std::string dcm_file);
		bool read_orientation_from_dicom_file(std::string dcm_file);
		bool read_slice_orientation_from_dicom_file(std::string dcm_file);
		//void rotate(...); //replaced by the 2 functions below...
		void rotate_orientation(int fi_x_deg, int fi_y_deg, int fi_z_deg);
		void rotate_orientation(float fi_x_rad, float fi_y_rad, float fi_z_rad);
//		void rotate_geometry_around_center_voxel(int fi_z_deg, int fi_y_deg, int fi_x_deg); //implemented in image_general
		void rotate_origin(int rot_axis, int pos_neg_dir=+1); //assumes orientation and voxel_size are correct
		void rotate_voxel_size(int rot_axis, int pos_neg_dir=+1);

		string get_slice_orientation();
		void set_slice_orientation(string s);

	//	virtual void load_dataset_from_VTK_file(std::string file_path){};


        Vector3D world_to_voxel( const Vector3D & wpos ) const;
		Vector3D voxel_to_world( const Vector3D & vpos ) const;
		virtual string resolve_value_world(Vector3D worldPos);

        
        virtual void data_has_changed(bool stats_refresh = true) = 0;
        // *** access methods ***

        virtual Matrix3D get_voxel_resize () const = 0;
        virtual const Vector3D get_voxel_size () const = 0;
        virtual void set_voxel_size(const Vector3D v) = 0;
        
        Matrix3D get_orientation () const;
		string get_orientation_as_dcm_string(); //print first two columns (cosines for x and y direction)
        Vector3D get_slice_direction();
        Vector3D get_voxel_x_dir_in_phys();
        Vector3D get_voxel_y_dir_in_phys();
        Vector3D get_voxel_z_dir_in_phys();
		Matrix3D get_dir_rendering_matrix(preset_direction direction);
		void set_orientation(const Matrix3D m);
		void set_orientation_to_unity();
        Vector3D get_origin() const;
        void set_origin(const Vector3D v); 
        void translate_origin(const Vector3D t); 

		virtual float get_phys_span_in_dir(Vector3D dir) = 0; //needed for example when setting upp view_ports "center3d_and_fit"

		virtual void save_histogram_to_txt_file(const std::string filename, const std::string separator=";");

		string resolve_tooltip();
		string resolve_tooltip_image_base();
		virtual void helper_data_to_binary_image(vector<int> vec) = 0;

    };


class imageloader
{
protected:
    std::vector<std::string> *files;
public:
    imageloader(std::vector<std::string> *f);
    ~imageloader();
	std::string read_file;
};


class dicomloader: public imageloader
{
private:
	DICOM_LOADER_TYPE this_load_type; //DCM_LOAD_ALL / DCM_LOAD_SERIES_ID_ONLY
    itk::GDCMImageIO::Pointer dicomIO;
	std::vector<std::string> loaded_TEs; 
	std::vector<std::string> loaded_series; //! UIDs of the DICOM series loaded during this call
                         //! to prevent multiple selected frames from loading the same series multiple times
	bool is_file_already_loaded(string file_path);
	void clear_files_vector_from_already_loaded();

public:
//    dicomloader(std::vector<std::string> *f);
    dicomloader(std::vector<std::string> *f, DICOM_LOADER_TYPE type= DCM_LOAD_SERIES_ID_ONLY);
    image_base * read();



// Remember that modifying the header entries of a DICOM file involves
// very serious risks for patients and therefore must be done with extreme caution.

//JK-Warning... These functions changes the series id. (0020,000e)...
//It is also problematic to pass on the orientation (when saving a file) using the gdcm package...

//	static void anonymize_single_dcm_file(string load_path, string save_path); 
//	static void anonymize_all_dcm_files_in_folder(string load_folder_path, string save_folder_path);
//	static void anonymize_all_dcm_files_in_folder2(string load_folder_path, string save_folder_path);
};


class vtkloader: public imageloader
{
private:
    itk::VTKImageIO::Pointer vtkIO;
    
public:
    vtkloader(std::vector<std::string> *f);
    image_base* read();
};

class analyze_hdrloader_itk: public imageloader
{
private:
    itk::AnalyzeImageIO::Pointer hdrIO;
    
public:
    analyze_hdrloader_itk(std::vector<std::string> *f);
    image_base* read();
};

class ximgloader : public imageloader
{
	private:
		itk::GE5ImageIO::Pointer ximgIO;
		
	public:
		ximgloader(std::vector<std::string> * f);
		image_base * read();
};

class analyze_objloader: public imageloader 
{
public:
    analyze_objloader(std::vector<std::string> *f);
    image_base* read();
};

class analyze_hdrloader: public imageloader 
{
private:
	int buf2int(unsigned char* buf);
	short buf2short(unsigned char* buf);
public:
    analyze_hdrloader(std::vector<std::string> *f);
    image_base* read();
};

class niftiloader: public imageloader
{
private:
    itk::NiftiImageIO::Pointer niftiIO;
public:
    niftiloader(std::vector<std::string> *f);
    image_base *read();
};


#endif