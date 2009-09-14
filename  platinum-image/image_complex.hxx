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

#ifndef __image_complex_hxx__
#define __image_complex_hxx__

#include "image_complex.h"
#include "image_general.hxx"
//#include "image_multi.hxx"

#include "transfer.hxx"


template <class ELEMTYPE, int IMAGEDIM>
image_complex<ELEMTYPE, IMAGEDIM>::image_complex():image_general<complex<ELEMTYPE>, IMAGEDIM>()
//image_complex<ELEMTYPE, IMAGEDIM>::image_complex():image_general<ELEMTYPE, IMAGEDIM>()
{   
	this->set_complex_parameters();
}

template <class ELEMTYPE, int IMAGEDIM>
image_complex<ELEMTYPE, IMAGEDIM>::image_complex(int w, int h, int d, complex<ELEMTYPE> *ptr):image_general<complex<ELEMTYPE>, IMAGEDIM>(w,h,d,ptr)
//image_complex<ELEMTYPE, IMAGEDIM>::image_complex(int w, int h, int d, ELEMTYPE *ptr):image_general<ELEMTYPE, IMAGEDIM>(w,h,d,ptr)
{   
	this->set_complex_parameters();
}

template <class ELEMTYPE, int IMAGEDIM>
image_complex<ELEMTYPE, IMAGEDIM>::image_complex(string path_image_re, string path_image_im,float unsigned_offset, string name)
{
	image_scalar<ELEMTYPE,IMAGEDIM> *re = new image_scalar<ELEMTYPE,IMAGEDIM>(path_image_re);
	image_scalar<ELEMTYPE,IMAGEDIM> *im = new image_scalar<ELEMTYPE,IMAGEDIM>(path_image_im);
	this->initialize_dataset(re->nx(),re->ny(),re->nz(), NULL);
	complex<ELEMTYPE> c;
	for(int z=0; z<re->nz(); z++){				
		for(int y=0; y<re->ny(); y++){				
			for(int x=0; x<re->nx(); x++){				
				c = complex<ELEMTYPE>( re->get_voxel(x,y,z)-unsigned_offset, im->get_voxel(x,y,z)-unsigned_offset );
				this->set_voxel(x,y,z, c);
			}
		}
	}

	this->set_complex_parameters();
	image_general<complex<ELEMTYPE>, IMAGEDIM>::set_parameters(re); //The "superclass" needs to be specified, at least for Visual Studio 2003...
	if(name!=""){
		this->name(name);
	}
	this->stats_refresh(true);

	delete re;
	delete im;
	//datamanagement.add(re,"re");
	//datamanagement.add(im,"im");
}


template <class ELEMTYPE, int IMAGEDIM>
void image_complex<ELEMTYPE, IMAGEDIM>::set_complex_parameters()
    {
	image_scalar<ELEMTYPE, IMAGEDIM> *magn = this->get_magnitude_image();

	magn->name("image_complex_tmp_image");//this->get_magnitude_image();
	stats = NULL;
	set_stats_histogram( new histogram_1D<ELEMTYPE>(magn) );  //hist1D constructor calls resize()... and calculate()

    tfunction = NULL;
	transfer_function();

	min_complex = complex<ELEMTYPE>(0,1);	//JK ????
	max_complex = complex<ELEMTYPE>(11,12); //JK ????
    }

template <class ELEMTYPE, int IMAGEDIM>
void image_complex<ELEMTYPE, IMAGEDIM>::set_stats_histogram(histogram_1D<ELEMTYPE > * h)
    {
    if (stats != NULL)
        {
        delete stats;
        }

    stats = h;
    }

template <class ELEMTYPE, int IMAGEDIM>
void image_complex<ELEMTYPE, IMAGEDIM>::stats_refresh(bool min_max_refresh)
    {
//	if(min_max_refresh){
//		this->min_max_refresh();
//	}
    stats->calculate_from_image_complex(this); //when called without argument (=0) the histogram "resolution" is kept..

    }


template <class ELEMTYPE, int IMAGEDIM>
void image_complex<ELEMTYPE, IMAGEDIM>::transfer_function(transfer_complex<ELEMTYPE,IMAGEDIM > * const t)
{
    if (this->tfunction != NULL)
        {delete this->tfunction;}

    if (t == NULL)
        this->tfunction = new transfer_complex<ELEMTYPE,IMAGEDIM>(this);
    else
        this->tfunction = t;
}


template <class ELEMTYPE, int IMAGEDIM>
void image_complex<ELEMTYPE, IMAGEDIM>::set_parameters(itk::SmartPointer< itk::OrientedImage<std::complex<ELEMTYPE>, IMAGEDIM > > &i)
{
	
    typename itk::OrientedImage<std::complex<ELEMTYPE>,IMAGEDIM>::SpacingType		itk_vox_size = i->GetSpacing(); 
    typename itk::OrientedImage<std::complex<ELEMTYPE>,IMAGEDIM>::PointType       itk_origin = i->GetOrigin();
    typename itk::OrientedImage<std::complex<ELEMTYPE>,IMAGEDIM>::DirectionType   itk_orientation = i->GetDirection();

    for(unsigned int d=0;d<IMAGEDIM;d++){
        if(itk_vox_size[d] > 0){
			this->voxel_size[d]=itk_vox_size[d];
		}

		this->origin[d]=itk_origin[d];

        for(unsigned int c=0;c<3;c++){
			this->orientation[d][c]=itk_orientation[d][c];
		}
	}
    
    if(this->voxel_size[0] * this->voxel_size[1] * this->voxel_size[2] == 0){
		this->voxel_size.Fill(1); 
	}

//	this->print_geometry(); //JK
    this->calc_transforms();
/*
//	typename theComplexStatsFilterType::Pointer statsFilter = theComplexStatsFilterType::New(); //????
//	statsFilter->SetInput(i);
//    statsFilter->Update();
//    this->set_max(statsFilter->GetMaximum());
//    this->set_min(statsFilter->GetMinimum());
*/
}





template <class ELEMTYPE, int IMAGEDIM>
void image_complex<ELEMTYPE, IMAGEDIM>::get_display_voxel(RGBvalue &val,int x, int y, int z) const
{
//	val.r( (IMGELEMCOMPTYPE)(100) );
//	val.g( (IMGELEMCOMPTYPE)(0) );
//	val.b( (IMGELEMCOMPTYPE)(0) );

	this->tfunction->get(this->get_voxel(x, y, z),val);

//		val.r( (IMGELEMCOMPTYPE)(image_vector[0]->get_voxel(x, y, z)) ); //JK4 - involve transfer functions later....
//    this->tfunction->get(get_voxel(x, y, z),val);
    //val.set_mono(255*(get_voxel (x, y, z)-minvalue)/(maxvalue-minvalue));
}

template <class ELEMTYPE, int IMAGEDIM>
string image_complex<ELEMTYPE, IMAGEDIM>::resolve_value_world(Vector3D worldPos)
{
	string s="";
	Vector3D vPos = this->world_to_voxel(worldPos);
	if( this->is_voxelpos_within_image_3D(create_Vector3Dint(vPos[0],vPos[1],vPos[2])) ){
		s += "r="+float2str(this->get_voxel_re(vPos[0],vPos[1],vPos[2]));
		s += " i="+float2str(this->get_voxel_im(vPos[0],vPos[1],vPos[2]));
		s += " i="+float2str(this->get_voxel_magn(vPos[0],vPos[1],vPos[2]));
		s += " ph="+float2str(this->get_voxel_phase(vPos[0],vPos[1],vPos[2]));
	}
	return s;
}

template <class ELEMTYPE, int IMAGEDIM>
void image_complex<ELEMTYPE, IMAGEDIM>::silly_test()
{
	cout<<"* This is a silly test , JK"<<endl;
}


template <class ELEMTYPE, int IMAGEDIM>
complex<ELEMTYPE> image_complex<ELEMTYPE, IMAGEDIM>::get_max() const
{
    return max_complex;
}

template <class ELEMTYPE, int IMAGEDIM>
complex<ELEMTYPE> image_complex<ELEMTYPE, IMAGEDIM>::get_min() const
{
    return min_complex;
}


template <class ELEMTYPE, int IMAGEDIM>
float image_complex<ELEMTYPE, IMAGEDIM>::get_number_voxel(int x, int y, int z) const
{
    return get_voxel_magn(x,y,z);
}

//JK - I have not managed to specialize this function for "complex<ELEMTYPE>" - I think the whole class needs to be rewritten for "complex<ELEMTYPE>"
template <class ELEMTYPE, int IMAGEDIM>
float image_complex<ELEMTYPE, IMAGEDIM>::get_max_float() const
{
	return abs(stats->max());
}

template <class ELEMTYPE, int IMAGEDIM>
float image_complex<ELEMTYPE, IMAGEDIM>::get_max_float_safe() const
{
	if(stats!=NULL){
		return abs(stats->max()); //JK4
	}
	return 1000;
}
template <class ELEMTYPE, int IMAGEDIM>
float image_complex<ELEMTYPE, IMAGEDIM>::get_min_float() const
{
	return abs(stats->min());
}

template <class ELEMTYPE, int IMAGEDIM>
float image_complex<ELEMTYPE, IMAGEDIM>::get_min_float_safe() const
{
	if(stats!=NULL){
		return abs(stats->min()); //JK4
	}
	return 0;
}


template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_complex<ELEMTYPE, IMAGEDIM>::get_voxel_re(int x, int y, int z) const
{
	return real(this->get_voxel(x,y,z));
}

template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_complex<ELEMTYPE, IMAGEDIM>::get_voxel_im(int x, int y, int z) const
{
	return imag(this->get_voxel(x,y,z));
}

template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_complex<ELEMTYPE, IMAGEDIM>::get_voxel_magn(int x, int y, int z) const
{
	return abs(this->get_voxel(x,y,z));
}

template <class ELEMTYPE, int IMAGEDIM>
float image_complex<ELEMTYPE, IMAGEDIM>::get_voxel_phase(int x, int y, int z) const
{
	return arg(this->get_voxel(x,y,z));
}

template <class ELEMTYPE, int IMAGEDIM>
image_scalar<ELEMTYPE, IMAGEDIM>* image_complex<ELEMTYPE, IMAGEDIM>::get_magnitude_image()
{
	image_scalar<ELEMTYPE,IMAGEDIM> *m = new image_scalar<ELEMTYPE,IMAGEDIM>(this->nx(),this->ny(),this->nz());
	for(int z=0; z<this->nz(); z++){				
		for(int y=0; y<this->ny(); y++){				
			for(int x=0; x<this->nx(); x++){				
				m->set_voxel(x,y,z, this->get_voxel_magn(x,y,z) );
			}
		}
	}

//	m->set_parameters(this);
	return m;
}


//-------------------------------------------------------------------
//-------------------------------------------------------------------

template <class ELEMTYPE, int IMAGEDIM>
typename itk::OrientedImage<std::complex<ELEMTYPE>, IMAGEDIM >::Pointer image_complex<ELEMTYPE, IMAGEDIM>::get_complex_image_as_itk_output()
{
//	cout<<"get_image_as_itk_output..."<<endl;

	typedef itk::ImportImageFilter<std::complex<ELEMTYPE>, IMAGEDIM> filterType;
	typename filterType::Pointer ITKimportfilter = filterType::New();
	ITKimportfilter->SetRegion(this->get_region_itk());
	ITKimportfilter->SetOrigin(this->get_origin_itk());
	ITKimportfilter->SetSpacing(this->get_voxel_size_itk());
	ITKimportfilter->SetDirection(this->get_orientation_itk()); //JK - Very important to remember ;-)

    ITKimportfilter->SetImportPointer(this->imagepointer(), this->num_elements, false);

    typedef itk::CastImageFilter<theComplexImageType2, theComplexImageType> castType;
	typename castType::Pointer caster = castType::New();
	caster->SetInput(ITKimportfilter->GetOutput());
	caster->Update();

	return caster->GetOutput();
}

template <class ELEMTYPE, int IMAGEDIM>
void image_complex<ELEMTYPE, IMAGEDIM>::load_dataset_from_VTK_file(string file_path)
{
	if(file_exists(file_path)){
		typename theComplexReaderType::Pointer r = theComplexReaderType::New();
		r->SetFileName(file_path.c_str());
		itk::VTKImageIO::Pointer VTKIO = itk::VTKImageIO::New();
		r->SetImageIO( VTKIO );

		typename theComplexImagePointer image = theComplexImageType::New();
		image = r->GetOutput();
		r->Update();
//		typename theSizeType s = image->GetBufferedRegion().GetSize();
		replicate_itk_to_image(image);
		this->name_from_path(file_path);
	}else{
		pt_error::error("image_complex::load_dataset_from_VTK_file()--> file does not exist...",pt_error::debug);
	}
}


template <class ELEMTYPE, int IMAGEDIM>
void image_complex<ELEMTYPE, IMAGEDIM>::load_complex_dataset_from_these_DICOM_files(vector<string> filenames)
{
	cout<<"load_complex_dataset_from_these_DICOM_files...("<<filenames.size()<<")"<<endl;

}

template <class ELEMTYPE, int IMAGEDIM>
void image_complex<ELEMTYPE, IMAGEDIM>::save_to_VTK_file(const std::string file_path, const bool useCompression)
{
	cout<<"save_to_VTK_file..."<<endl;
    typename theComplexWriterType::Pointer writer = theComplexWriterType::New();   //default file type is VTK
    writer->SetFileName( file_path.c_str() );
	writer->SetInput(get_complex_image_as_itk_output());

	//itk::ImageIOBase::IOPixelType pixelType=dicomIO->GetPixelType();
	//VTKIO->SetPixelTypeInfo(itk::VTKImageIO::COMPLEX);
	itk::VTKImageIO::Pointer VTKIO = itk::VTKImageIO::New();
	VTKIO->SetPixelType(itk::ImageIOBase::COMPLEX);			//does not seem to be written to the file....
	writer->SetImageIO( VTKIO );

	if(useCompression){
		writer->UseCompressionOn();
	}

    try{
        writer->Update();
        }
    catch (itk::ExceptionObject &ex){
        pt_error::error("Exception thrown saving file (" +file_path + ")", pt_error::warning);
		std::cout<<ex<<std::endl;
        }
}

template <class ELEMTYPE, int IMAGEDIM>
void image_complex<ELEMTYPE, IMAGEDIM>::save_to_DCM_file(const std::string file_path, const bool useCompression, const bool anonymize)
{
//	cout<<"save_to_DCM_file..."<<endl;    //port image to ITK image and save it as DCM file

	//--------------------------------------------------------
	//--- If dicom image has tag "DCM_IMAGE_ORIENTATION_PATIENT" a/b/c/d/e/f
	//--- This is read as 
	//		a d j
	//		b e	k
	//		c f l
	//--- GDCMImageIO saves the direcional conines transposed compared to the ITK use....
	//--- Idea - Transpose rotation matrix before saving... and again after...

	//--------------------------------------------------------
	this->orientation = this->orientation.GetTranspose();
	//--------------------------------------------------------

    typename theComplexWriterType::Pointer writer = theComplexWriterType::New();
	typename itk::OrientedImage<std::complex<ELEMTYPE>, IMAGEDIM >::Pointer image = get_complex_image_as_itk_output();
    writer->SetFileName( file_path.c_str() );
    writer->SetInput(image);
	if(useCompression){
		writer->UseCompressionOn();
	}

	if(anonymize){
		typedef itk::GDCMImageIO ImageIOType;
		ImageIOType::Pointer gdcmImageIO = ImageIOType::New();
		writer->SetImageIO( gdcmImageIO );
		typedef itk::MetaDataDictionary   DictionaryType;
		DictionaryType & dictionary = image->GetMetaDataDictionary();

		//TODO - make sure all interesting data in "metadata-object" is saves....
		itk::EncapsulateMetaData<std::string>( dictionary, DCM_PATIENT_NAME, "Anonymized" );
		itk::EncapsulateMetaData<std::string>( dictionary, DCM_PATIENT_ID, "Anonymized" );
		itk::EncapsulateMetaData<std::string>( dictionary, DCM_PATIENT_BIRTH_DATE, "Anonymized" );
		// The two lines below don't work... 
		// The brutal rotation of the orientation matrix was used before and after saving instead...
		//itk::EncapsulateMetaData<std::string>( dictionary, DCM_IMAGE_POSITION_PATIENT, "1\\2\\3" );
		//itk::EncapsulateMetaData<std::string>( dictionary, DCM_IMAGE_ORIENTATION_PATIENT, this->get_orientation_as_dcm_string() );
	}

    try{
        writer->Update();
    }catch (itk::ExceptionObject &ex){
        pt_error::error("Exception thrown saving file (" +file_path + ")", pt_error::warning);
		std::cout<<ex<<std::endl;
    }

	//--------------------------------------------------------
	this->orientation = this->orientation.GetTranspose(); //transpose again  "=back"
	//--------------------------------------------------------
}


#endif