// $Id$

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

#ifndef __image_general_hxx__
#define __image_general_hxx__

#include "rawimporter.h"
#include "image_scalar.h"

#include "image_binary.h"
#include "image_label.h"
#include "fileutils.h"

#include <iostream>
#include <fstream>

#include "itkImageRegionIterator.h"	//most basic, fastest pixel order...
#include "itkImageRegionIteratorWithIndex.h"

//#include "itkOrientedImage.h" //reads the geomatry information in a different way...

#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"

#include "itkImageSeriesReader.h"
#include "itkImageSeriesWriter.h"
#include "itkDICOMImageIO2.h"
#include "itkDICOMSeriesFileNames.h"
#include "itkNumericSeriesFileNames.h"
#include "itkVTKImageIO.h"
#include "itkRawImageIO.h"
#include "itkImageSliceIteratorWithIndex.h"
#include "itkStatisticsImageFilter.h"

#include "itkDICOMImageIO2Factory.h"
//#include "itkDICOMImageIO2.h"
//#include "itkImageSeriesReader.h"
//#include "itkDICOMSeriesFileNames.h"


using namespace std;

//GCC does not support templated typedefs
//we bow our heads in appreciation and define them with macros instead :P
//#define --> "The define keyword means simple text replacement"
//#typedef --> "The typedef keyword allows you to create a new alias for an existing data type"

//#define theImageType itk::Image<ELEMTYPE,IMAGEDIM>
#define theImageType itk::OrientedImage<ELEMTYPE,IMAGEDIM>
#define theImageType2 itk::Image<ELEMTYPE,IMAGEDIM>
#define theComplexImageType itk::OrientedImage<std::complex<ELEMTYPE>,IMAGEDIM>
#define theComplexImageType2 itk::Image<std::complex<ELEMTYPE>,IMAGEDIM>
#define theImageType2D itk::OrientedImage<ELEMTYPE,2>
#define theOrientedImageType itk::OrientedImage<ELEMTYPE,IMAGEDIM>
#define theImageToOrientedCastFilterType itk::CastImageFilter< theImageType, theOrientedImageType >
#define theImagePointer theImageType::Pointer
#define theComplexImagePointer theComplexImageType::Pointer
#define theIteratorType itk::ImageRegionIterator<theImageType >
#define theSeriesReaderType itk::ImageSeriesReader<theImageType >
#define theSeriesWriterType itk::ImageSeriesWriter<theImageType >
#define theReaderType itk::ImageFileReader<theImageType >
#define theComplexReaderType itk::ImageFileReader<theComplexImageType >
#define	theWriterType itk::ImageFileWriter<theImageType >
#define	theComplexWriterType itk::ImageFileWriter<theComplexImageType >
//#define	theOrientedWriterType itk::ImageFileWriter<theOrientedImageType >
#define theSizeType theImageType::RegionType::SizeType
#define theStatsFilterType itk::StatisticsImageFilter<theImageType >
#define theComplexStatsFilterType itk::StatisticsImageFilter<theComplexImageType >
//#define theStatsFilterPointerType theStatsFilterType::Pointer
//#define theMeanFilterType itk::MeanImageFilter<theImageType,theImageType >
#define theRegionType theImageType::RegionType
#define theIndexType theImageType::IndexType
#define theIteratorWithIndexType itk::ImageRegionIteratorWithIndex<theImageType >

//#define HessianFilterType itk::HessianRecursiveGaussianImageFilter<theImageType >
//#define HessianFilterPointerType HessianFilterType::Pointer
//#define VesselnessMeasureFilterType itk::Hessian3DToVesselnessMeasureImageFilter<theImageType >
//#define VesselnessMeasureFilterPointerType VesselnessMeasureFilterType::Pointer


#include "image_general.h"				//according to the "general-linking-system"
#include "image_storage.hxx"			//according to the "general-linking-system"

#include "image_generalfile.hxx"		//just separate the code in different files
#include "image_general_iterator.hxx"

template <class NEWELEM, class TRYELEM, int DIM, template <class, int> class requestedClass >
requestedClass<NEWELEM, DIM> * try_general (image_base* input) //! Helper function to guaranteed_cast
    {
    requestedClass<NEWELEM, DIM> * output = NULL; //dynamic_cast<requestedClass<NEWELEM, DIM> *> (input);

    image_general <TRYELEM,DIM>* input_general = dynamic_cast<image_general <TRYELEM, DIM> *> (input) ; //! Try to cast into image_general of try-type

    if (input_general != NULL) //! If cast was successful, input had the tried type and input_general can be used in a call to new class' copy constructor
        {
        output = new requestedClass<NEWELEM, DIM> (input_general,true);
        //delete input;
        } 
    return output;
    }

template <template <class, int> class requestedClass, class ELEM, int DIM>
requestedClass<ELEM, DIM>* scalar_copycast (image_base* input)
    {
    requestedClass<ELEM, DIM> * output = NULL; //dynamic_cast<requestedClass<ELEM, DIM > *>(input);

    //Try all possible data types

    if (output == NULL)
        { output = try_general <ELEM,unsigned char,DIM,requestedClass >(input); }

    if (output == NULL)
        { output = try_general <ELEM,signed char,DIM,requestedClass >(input); }

    if (output == NULL)
        { output = try_general <ELEM,unsigned short,DIM,requestedClass >(input); }

    if (output == NULL)
        { output = try_general <ELEM,signed short,DIM,requestedClass >(input); }

    if (output == NULL)
        { output = try_general <ELEM,unsigned long,DIM,requestedClass >(input); }

    if (output == NULL)
        { output = try_general <ELEM,signed long,DIM,requestedClass >(input); }

    if (output == NULL)
        { output = try_general <ELEM,float,DIM,requestedClass >(input); }

    if (output == NULL)
        { output = try_general <ELEM,double,DIM,requestedClass >(input); }

    /*if (output == NULL)
        {throw  (bad_cast());}*/

    return output;
    }

template <class ELEMTYPE, int IMAGEDIM>
template <class sourceType>
void image_general<ELEMTYPE, IMAGEDIM>::set_parameters (image_general<sourceType, IMAGEDIM> * sourceImage)
    {
//	cout<<this->get_id()<<"->set_parameters("<<sourceImage->get_id()<<")"<<endl;
	this->set_image_geometry(sourceImage); //origin, voxel_size, orientation, slice_orientation

    this->set_max(sourceImage->get_max_float_safe());
    this->set_min(sourceImage->get_min_float_safe());
//    this->stats->max(sourceImage->get_max());
//    this->stats->min(sourceImage->get_min());
    this->meta = sourceImage->meta;

//	cout<<"this->get_voxel_size()="<<this->get_voxel_size()<<endl;

    // *ID, from_file, imagename and widget are assigned in image_base constructor
    }


template <class ELEMTYPE, int IMAGEDIM>
image_base * image_general<ELEMTYPE, IMAGEDIM>::alike (imageDataType unit)
    {
    //this alike is called from the template-agnostic image_base class
    image_base * new_image = NULL;

    switch (unit)
        {
        case VOLDATA_BOOL:      new_image = new image_binary<IMAGEDIM>(this,false);             
            break;
        case VOLDATA_CHAR:      new_image = new image_integer<char, IMAGEDIM > (this,false);              break;
        case VOLDATA_UCHAR:     new_image = new image_integer<unsigned char, IMAGEDIM > (this,false);
            break;
        case VOLDATA_SHORT:     new_image = new image_integer<short, IMAGEDIM >(this,false);              break;
        case VOLDATA_USHORT:    new_image = new image_integer<unsigned short, IMAGEDIM >(this,false);     break;
        case VOLDATA_DOUBLE:    new_image = new image_scalar<double, IMAGEDIM >(this,false);             break;
        default :
            {
                pt_error::error("image_general::alike: attempting to create alike as unsupported data type", pt_error::serious);
            }
            break;
        }

    return new_image;
    }

//template <class ELEMTYPE, int IMAGEDIM>
//    template <class newType> 
//image_general<newType> * image_general<ELEMTYPE, IMAGEDIM>::alike ()
//    {
//    image_general<newType, IMAGEDIM> * new_image = NULL;
//
//    new_image = new image_general<newType, IMAGEDIM>();
//
//    new_image->initialize_dataset(datasize[0],datasize[1],datasize[2]);
//
//    ITKimportfilter=NULL;
//
//    new_image->origin          = origin;
//    new_image->orientation       = orientation;
//    new_image->voxel_resize    = voxel_resize;
//
//    new_image->unit_center_     = unit_center;
//    new_image->unit_to_voxel   = unit_to_voxel_;
//
//    return new_image;
//    }

/*template <class ELEMTYPE, int IMAGEDIM>
    template <class inType>
void image_general<ELEMTYPE, IMAGEDIM>::copy_image (image_general<inType, IMAGEDIM> * in)
    {
    if ( same_size (in))
        {  
        typename image_general<inType, IMAGEDIM>::iterator i = in->begin();
        typename image_storage<ELEMTYPE >::iterator o = this->begin();
        
        while (i != in->end() && o != this->end())
            {
            *o = *i;
            
            ++i; ++o;
            }
		this->image_has_changed();
        }
    else
        {     
        //total failure, TODO: throw exception
#ifdef _DEBUG
        cout << "Copying image data: image sizes don't match" << std::endl;
#endif
        }
    }*/

template <class ELEMTYPE, int IMAGEDIM>
    image_general<ELEMTYPE, IMAGEDIM>::image_general():image_storage<ELEMTYPE>()
    {   
        voxel_size.Fill(1);
    }

template <class ELEMTYPE, int IMAGEDIM>
template<class SOURCETYPE>
image_general<ELEMTYPE, IMAGEDIM>::image_general(image_general<SOURCETYPE, IMAGEDIM> * old_image, bool copyData) : image_storage<ELEMTYPE > (old_image) //copy constructor
    {
    initialize_dataset(old_image->get_size_by_dim(0), old_image->get_size_by_dim(1), old_image->get_size_by_dim(2), NULL);

    if (copyData)
        { copy_data (old_image,this); }

    set_parameters(old_image);
    }

template <class ELEMTYPE, int IMAGEDIM>
image_general<ELEMTYPE, IMAGEDIM>::image_general(itk::SmartPointer< itk::OrientedImage<ELEMTYPE, IMAGEDIM > > &i) : image_storage<ELEMTYPE >()
    {
    replicate_itk_to_image(i);
    }

template <class ELEMTYPE, int IMAGEDIM>
image_general<ELEMTYPE, IMAGEDIM>::image_general(int w, int h, int d, ELEMTYPE *ptr) : image_storage<ELEMTYPE >()
    {
    initialize_dataset(w,h,d,ptr);
    }

template <class ELEMTYPE, int IMAGEDIM>
image_general<ELEMTYPE, IMAGEDIM>::image_general(vector<int> sizes, ELEMTYPE *ptr) : image_storage<ELEMTYPE >()
    {
    initialize_dataset(sizes,ptr);
    }

template <class ELEMTYPE, int IMAGEDIM>
image_general<ELEMTYPE, IMAGEDIM>::image_general(const string filepath, const string name)
{
	this->load_file_to_this(filepath);
	this->data_has_changed();
	if(name!=""){
		this->name(name);
	}
}


template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::initialize_dataset(int w, int h, int d)
//this definition catches the virtual initialize_dataset in image_base
    {
    initialize_dataset( w,  h,  d, NULL);
    }

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::initialize_dataset(int w, int h, int d,  ELEMTYPE *ptr)
    {
    datasize[0] = w; datasize[1] = h; datasize[2] = d;

    voxel_size.Fill(1);
	this->set_slice_orientation("undefined");
    
    //dimension-independent loop that may be lifted outside this function
    this->num_elements=1;
    for (unsigned short i=0; i<IMAGEDIM; i++) 
        {
        this->num_elements *= datasize[i];
        }
	
	if(this->imagepointer()!=NULL)
	{
//		cout<<"initialize_dataset--> this->deallocate()... to avoid memory loss..."<<endl;
		this->deallocate();
	}else{
//		cout<<"initialize_dataset--> ... pointer was already NULL..."<<endl;
	}

    this->set_imagepointer( new ELEMTYPE[this->num_elements] );

    if (ptr!=NULL) //memcpy is bad karma! Use copy_data(in, out) whenever you know your (input) datatype!
        {
        memcpy(this->imagepointer(),ptr,sizeof(ELEMTYPE)*this->num_elements);
        
        data_has_changed(true);
        }
        
    set_parameters();
	}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::initialize_dataset(vector<int> sizes, ELEMTYPE *ptr)
{
	for(int i=0;i<IMAGEDIM;i++){
	    datasize[i] = sizes[i];
	}
    voxel_size.Fill(1);
	this->set_slice_orientation("undefined");
    
    //dimension-independent loop that may be lifted outside this function
    this->num_elements=1;
	for(unsigned short i=0; i<IMAGEDIM; i++){
        this->num_elements *= datasize[i];
	}
	
	if(this->imagepointer()!=NULL){
		cout<<"initialize_dataset--> this->deallocate()... to avoid memory loss..."<<endl;
		this->deallocate();
	}else{
		cout<<"initialize_dataset--> ... pointer was already NULL..."<<endl;
	}

    this->set_imagepointer( new ELEMTYPE[this->num_elements] );

	if(ptr!=NULL){ //memcpy is bad karma! Use copy_data(in, out) whenever you know your (input) datatype!
        memcpy(this->imagepointer(),ptr,sizeof(ELEMTYPE)*this->num_elements);
        data_has_changed(true);
	}
        
    set_parameters();
}

       

template <class ELEMTYPE, int IMAGEDIM>
template <class LOADERTYPE>
bool image_general<ELEMTYPE, IMAGEDIM>::try_single_loader(std::string s) //! helper for image_base::load
{
    std::vector<std::string> v;
	v.push_back(s);
	LOADERTYPE loader = LOADERTYPE(&v);
	image_general<ELEMTYPE, IMAGEDIM> *new_image = dynamic_cast< image_general<ELEMTYPE, IMAGEDIM> * >(loader.read());

	if(new_image != NULL){
		this->initialize_dataset(new_image->datasize[0],new_image->datasize[1], new_image->datasize[2]);
		copy_data(new_image,this);
		this->set_parameters(new_image);
		this->from_file(true);

		delete new_image;
		return true;
	}
	delete new_image;
	return false;
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::load_file_to_this(std::string f)	//loads one file to this...
{
	if(file_exists(f)){
		bool success = false;
		success = try_single_loader<vtkloader>(f);
		if(!success){success = try_single_loader<dicomloader>(f);}
		if(!success){success = try_single_loader<analyze_objloader>(f);}
		if(!success){success = try_single_loader<analyze_hdrloader>(f);}
		if(!success){success = try_single_loader<brukerloader>(f);}
		if(!success){success = try_single_loader<niftiloader>(f);}
		//do not pop up a raw_importer window...
	    //rawimporter::create(chosen_files);

		pt_error::error_if_false(success,"image_general-load_file_to_this - FAIL",pt_error::serious);  //Note: if you end up crashing here... you might have entered wrong ELEMTYPE
	}else{
		pt_error::error("image_general-load_file_to_this - file does not exist... \n("+f+")",pt_error::debug);
	}
}


template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::data_has_changed(bool stat_refresh)
    {
    //TODO: some outlandish malfunction in rendererMPR
    //when called with render_thumbnail-generated parameters
    //widget->refresh_thumbnail();

    //data changed, no longer available in a file (not that Mr. Platinum knows of, anyway)
    this->from_file(false);

    //recalculate min/max
    if(stat_refresh)
        {
        this->stats_refresh(true);

        //refresh transfer function
//        this->tfunction->update(); //????
        }

//    cout<<"data_has_changed...clear_itk_porting()"<<endl;
//	clear_itk_porting();	//clear ITK connection
	//previously....
//    ITKimportfilter = NULL;
 //   if (ITKimportimage.IsNotNull())
 //       {ITKimportimage->Delete();}

	this->set_tooltip(this->resolve_tooltip());
    this->redraw();
    }

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::calc_transforms ()
    {
    //NOTE:
    //this currently does nothing, but is useful to keep around for caching
    //e.g. the world to voxel transform matrices
    }

template <class ELEMTYPE, int IMAGEDIM>
Vector3D image_general<ELEMTYPE, IMAGEDIM>::get_phys_pos_of_corner(int corner_id)
{
	Vector3D ret;
	if(corner_id==0){
		ret = get_physical_pos_for_voxel(0,0,0);
	}else if(corner_id==1){
		ret = get_physical_pos_for_voxel(this->datasize[0],0,0);
	}else if(corner_id==2){
		ret = get_physical_pos_for_voxel(this->datasize[0],this->datasize[1],0);
	}else if(corner_id==3){
		ret = get_physical_pos_for_voxel(0,this->datasize[1],0);
	}else if(corner_id==4){
		ret = get_physical_pos_for_voxel(0,0,this->datasize[2]);
	}else if(corner_id==5){
		ret = get_physical_pos_for_voxel(this->datasize[0],0,this->datasize[2]);
	}else if(corner_id==6){
		ret = get_physical_pos_for_voxel(this->datasize[0],this->datasize[1],this->datasize[2]);
	}else if(corner_id==7){
		ret = get_physical_pos_for_voxel(0,this->datasize[1],this->datasize[2]);
	}

	return ret;
}

template <class ELEMTYPE, int IMAGEDIM>
template<class ELEMTYPE2>
void image_general<ELEMTYPE, IMAGEDIM>::combine_using_physical_pos(image_general<ELEMTYPE2, IMAGEDIM> *const image2, COMBINE_MODE mode)
{	
	Vector3D phys;
	Vector3D vox_pos;
	Matrix3D mat1 = this->get_orientation() * this->get_voxel_resize();
//	this->origin + this->get_orientation() * this->get_voxel_resize() * vox_pos;
//	Vector3D vpos = wpos - origin;

//	Matrix3D inv_orientation;
//	inv_orientation = this->get_orientation().GetInverse();
//	Matrix3D inv_voxel_resize;
//	inv_voxel_resize = this->get_voxel_resize().GetInverse();
//	Matrix3D mat2 = inv_voxel_resize * inv_orientation;	// the operations are done from right to left

	Matrix3D mat2 = this->get_orientation().GetInverse() * this->get_voxel_resize().GetInverse();	// the operations are done from right to left
	Vector3D this_voxel;

	Matrix3D mat3 = image2->get_voxel_resize().GetInverse() * image2->get_orientation().GetInverse();	// the operations are done from right to left
	Vector3D other_voxel;


	switch(mode)
	{
    case COMB_ADD:
		cout<<"...ADD";
		for(vox_pos[2]=0;vox_pos[2]<this->nz();vox_pos[2]++){
			cout<<" "<<vox_pos[2];
			for(vox_pos[1]=0;vox_pos[1]<this->ny();vox_pos[1]++){
				for(vox_pos[0]=0;vox_pos[0]<this->nx();vox_pos[0]++){
					phys = this->origin + mat1*vox_pos;
					other_voxel = mat3*(phys - image2->get_origin());
					if(image2->is_voxelpos_within_image_3D(other_voxel)){
//					if(image2->is_physical_pos_within_image_3D(phys)){
						this_voxel = mat2*(phys - this->origin);
						this->set_voxel( this_voxel, this->get_voxel(this_voxel) + image2->get_voxel(other_voxel) );
					}
				}
			}
		}
		cout<<endl;
	break;

	case COMB_MULT:
		cout<<"...MULT";
		for(vox_pos[2]=0;vox_pos[2]<this->nz();vox_pos[2]++){
			cout<<" "<<vox_pos[2];
			for(vox_pos[1]=0;vox_pos[1]<this->ny();vox_pos[1]++){
				for(vox_pos[0]=0;vox_pos[0]<this->nx();vox_pos[0]++){
					phys = this->origin + mat1*vox_pos;
					other_voxel = mat3*(phys - image2->get_origin());
					if(image2->is_voxelpos_within_image_3D(other_voxel)){
//					if(image2->is_physical_pos_within_image_3D(phys)){
						this_voxel = mat2*(phys - this->origin);
						this->set_voxel( this_voxel, this->get_voxel(this_voxel) * image2->get_voxel(other_voxel) );
					}
				}
			}
		}
		cout<<endl;
	break;

	default:
		pt_error::error("image_general<ELEMTYPE, IMAGEDIM>::combine_using_physical_pos --> COMBINE_MODE not recognized",pt_error::debug);
		break;
	}
}


template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::set_parameters()
    {
    calc_transforms();
    }

/*
template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::replicate_itk_to_image()
    {
    replicate_itk_to_image(ITKimportimage);
    }
*/

template <class ELEMTYPE, int IMAGEDIM>
//void image_general<ELEMTYPE, IMAGEDIM>::replicate_itk_to_image(itk::SmartPointer< itk::Image<ELEMTYPE, IMAGEDIM > > &i)
//void image_general<ELEMTYPE, IMAGEDIM>::replicate_itk_to_image(itk::SmartPointer< itk::OrientedImage<ELEMTYPE, IMAGEDIM > > &i)
void image_general<ELEMTYPE, IMAGEDIM>::replicate_itk_to_image(typename itk::OrientedImage<ELEMTYPE, IMAGEDIM >::Pointer i)
{
	pt_error::error_if_true(i.IsNull(),"replicate_itk_to_image -- ITK image is NULL", pt_error::fatal ); 

    typename theSizeType ITKsize = i->GetLargestPossibleRegion().GetSize();
    i->SetBufferedRegion(i->GetLargestPossibleRegion());
    initialize_dataset(ITKsize[0], ITKsize[1], ITKsize[2],i->GetBufferPointer());
    set_parameters(i);
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::replicate_itk_to_image(typename itk::OrientedImage<std::complex<ELEMTYPE>, IMAGEDIM >::Pointer i)
{
	pt_error::error_if_true(i.IsNull(),"replicate_itk_to_image -- ITK image is NULL", pt_error::fatal ); 

    typename theSizeType ITKsize = i->GetLargestPossibleRegion().GetSize();
    i->SetBufferedRegion(i->GetLargestPossibleRegion());
    initialize_dataset(ITKsize[0], ITKsize[1], ITKsize[2],i->GetBufferPointer());
    set_parameters(i);
}

	

/*
template <class ELEMTYPE, int IMAGEDIM>
typename theImagePointer image_general<ELEMTYPE, IMAGEDIM>::itk_image()
    {
	cout<<"itk_image()... ("<<this->name()<<")";
    if (ITKimportimage.IsNull()){
		cout<<" was NULL...";
//        if (ITKimportfilter.IsNull()){
			//make_image_an_itk_reader();
			port_to_itk_format();
//		}

        }
	else{
		cout<<" already exists...";
	}
	cout<<endl;

    return ITKimportimage;
    }
	*/
/*
template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::clear_itk_porting()
{
	cout<<"clear_itk_porting...("<<this->name()<<") ";
//	if(ITKimportfilter.IsNotNull()){
//		ITKimportfilter->Delete();
//	}
	if(ITKimportimage.IsNotNull()){
		cout<<"not null...";
		ITKimportimage->Delete();
		ITKimportimage = NULL;
		if(ITKimportimage.IsNull()){
			cout<<"but NOW...";
		}
		
	}else{
		cout<<"already null...";
	}
	cout<<endl;
}
*/
/*
template <class ELEMTYPE, int IMAGEDIM>
void  image_general<ELEMTYPE, IMAGEDIM>::port_to_itk_format()
    {
	cout<<"port_to_itk_format..."<<endl;

    //ITKimportfilter = itk::ImportImageFilter< ELEMTYPE, IMAGEDIM >::New();
	typedef itk::ImportImageFilter<ELEMTYPE, IMAGEDIM> filterType;
	filterType::Pointer ITKimportfilter = filterType::New();

	ITKimportfilter->SetRegion( get_region_itk() );
	ITKimportfilter->SetOrigin(this->get_origin_itk());
	ITKimportfilter->SetSpacing(this->get_voxel_size_itk());
	ITKimportfilter->SetDirection(this->get_orientation_itk()); //JK - Very important to remember ;-)

    ITKimportfilter->SetImportPointer( this->imagepointer(), this->num_elements, false);
//	ITKimportfilter->Update();

	typedef itk::CastImageFilter<theImageType2, theImageType> castType;
	typename castType::Pointer caster = castType::New();
	caster->SetInput(ITKimportfilter->GetOutput());
	caster->Update();

	cout<<"new Image..."<<endl;
	ITKimportimage = itk::OrientedImage<ELEMTYPE, IMAGEDIM>::New();
	ITKimportimage = caster->GetOutput();
    }
*/

/*
template <class ELEMTYPE, int IMAGEDIM>
typename itk::ImportImageFilter< ELEMTYPE, IMAGEDIM >::Pointer image_general<ELEMTYPE, IMAGEDIM>::itk_import_filter()
    {
    if (ITKimportfilter.IsNull ())
        {
        //make_image_an_itk_reader();
			this->port_to_itk_format();
        }

    return ITKimportfilter;
    }
*/

template <class ELEMTYPE, int IMAGEDIM>
typename itk::OrientedImage<ELEMTYPE, IMAGEDIM >::PointType image_general<ELEMTYPE, IMAGEDIM>::get_origin_itk()
{
	typename itk::OrientedImage<ELEMTYPE, IMAGEDIM >::PointType itk_origin;
	for (unsigned int d=0;d<3;d++){
        itk_origin[d]=this->origin[d];
        }
	return itk_origin;
}

template <class ELEMTYPE, int IMAGEDIM>
typename itk::OrientedImage<ELEMTYPE, IMAGEDIM >::SizeType image_general<ELEMTYPE, IMAGEDIM>::get_size_itk()
{
	typename itk::OrientedImage<ELEMTYPE, IMAGEDIM >::SizeType itk_size;
	for (unsigned int d=0;d<3;d++){
		itk_size[d]=this->datasize[d];
        }
	return itk_size;
}

template <class ELEMTYPE, int IMAGEDIM>
typename itk::OrientedImage<ELEMTYPE, IMAGEDIM >::SpacingType image_general<ELEMTYPE, IMAGEDIM>::get_voxel_size_itk()
{
	typename itk::OrientedImage<ELEMTYPE, IMAGEDIM >::SpacingType itk_spacing;
	for (unsigned int d=0;d<3;d++){
		itk_spacing[d]=this->voxel_size[d];
        }
	return itk_spacing;
}

template <class ELEMTYPE, int IMAGEDIM>
typename itk::OrientedImage<ELEMTYPE, IMAGEDIM >::DirectionType image_general<ELEMTYPE, IMAGEDIM>::get_orientation_itk()
{
	typename itk::OrientedImage<ELEMTYPE, IMAGEDIM >::DirectionType itk_direction;
	for (unsigned int d=0;d<3;d++){
		for (unsigned int c=0;c<3;c++){
			itk_direction[d][c]=this->orientation[d][c];
		}
    }
	return itk_direction;
}

template <class ELEMTYPE, int IMAGEDIM>
typename itk::OrientedImage<ELEMTYPE, IMAGEDIM >::RegionType image_general<ELEMTYPE, IMAGEDIM>::get_region_itk()
{
	typename itk::OrientedImage<ELEMTYPE, IMAGEDIM >::RegionType itk_region;
    typename itk::OrientedImage<ELEMTYPE, IMAGEDIM >::IndexType  itk_start;
    itk_start.Fill( 0 );
    itk_region.SetIndex( itk_start );
	itk_region.SetSize( this->get_size_itk() );
	return itk_region;
}

template <class ELEMTYPE, int IMAGEDIM>
typename itk::OrientedImage<ELEMTYPE, IMAGEDIM >::Pointer image_general<ELEMTYPE, IMAGEDIM>::get_image_as_itk_output()
{
//	cout<<"get_image_as_itk_output..."<<endl;

	typedef itk::ImportImageFilter<ELEMTYPE, IMAGEDIM> filterType;
	typename filterType::Pointer ITKimportfilter = filterType::New();
	ITKimportfilter->SetRegion(this->get_region_itk());
	ITKimportfilter->SetOrigin(this->get_origin_itk());
	ITKimportfilter->SetSpacing(this->get_voxel_size_itk());
	ITKimportfilter->SetDirection(this->get_orientation_itk()); //JK - Very important to remember ;-)

    ITKimportfilter->SetImportPointer(this->imagepointer(), this->num_elements, false);

    typedef itk::CastImageFilter<theImageType2, theImageType> castType;
	typename castType::Pointer caster = castType::New();
	caster->SetInput(ITKimportfilter->GetOutput());
	caster->Update();

	return caster->GetOutput();
}

template <class ELEMTYPE, int IMAGEDIM>
unsigned short image_general<ELEMTYPE, IMAGEDIM>::get_num_voxels()
{
	unsigned short num_voxels=1;
	for (int dim=0; dim<IMAGEDIM; dim++) {
		num_voxels*=datasize[dim];
	}
	return num_voxels;
}

template <class ELEMTYPE, int IMAGEDIM>
unsigned short image_general<ELEMTYPE, IMAGEDIM>::get_size_by_dim(int dim) const
    {
    return datasize[dim];
    }

template <class ELEMTYPE, int IMAGEDIM>
unsigned short image_general<ELEMTYPE, IMAGEDIM>::nx() const
{
	return get_size_by_dim(0);
}

template <class ELEMTYPE, int IMAGEDIM>
unsigned short image_general<ELEMTYPE, IMAGEDIM>::ny() const
{
	return get_size_by_dim(1);
}

template <class ELEMTYPE, int IMAGEDIM>
unsigned short image_general<ELEMTYPE, IMAGEDIM>::nz() const
{
	return get_size_by_dim(2);
}

template <class ELEMTYPE, int IMAGEDIM>
unsigned short image_general<ELEMTYPE, IMAGEDIM>::nu(int dir) const
{
	return get_size_by_dim_and_dir(0,dir);
}

template <class ELEMTYPE, int IMAGEDIM>
unsigned short image_general<ELEMTYPE, IMAGEDIM>::nv(int dir) const
{
	return get_size_by_dim_and_dir(1,dir);
}

template <class ELEMTYPE, int IMAGEDIM>
unsigned short image_general<ELEMTYPE, IMAGEDIM>::nw(int dir) const
{
	return get_size_by_dim_and_dir(2,dir);
}

template <class ELEMTYPE, int IMAGEDIM>
unsigned short image_general<ELEMTYPE, IMAGEDIM>::get_size_by_dim_and_dir(int dim, int direction) const
    {
		return get_size_by_dim((dim+direction+1)%IMAGEDIM);
    }

template <class ELEMTYPE, int IMAGEDIM>
Vector3D image_general<ELEMTYPE, IMAGEDIM>::get_size()
{
	return create_Vector3D(datasize[0],datasize[1],datasize[2]);
}


template <class ELEMTYPE, int IMAGEDIM>
Vector3D image_general<ELEMTYPE, IMAGEDIM>::get_physical_size () const
{
    Vector3D result;
    
    //short smax = 0; //size maxnorm for unit coordinate system, remove for world
    
    for (int d = 0; d < 3; d++)
        {
        result[d] = datasize[d];
        //smax = std::max ((short)result[d],smax);
        }
    
    result = get_voxel_resize() * result;
    
    //remove when world coordinate system is implemented
    //result /= smax;
    
    return result;
}

template <class ELEMTYPE, int IMAGEDIM>
Vector3D image_general<ELEMTYPE, IMAGEDIM>::get_physical_center() const
{
	return this->get_origin() + this->get_orientation()*(get_physical_size()/2.0);
}



template <class ELEMTYPE, int IMAGEDIM>
bool image_general<ELEMTYPE, IMAGEDIM>::same_size (image_base * other)
    {
    for (unsigned int d=0;d < IMAGEDIM; d++)
        {
        if (datasize[d] != other->get_size_by_dim(d))
            {
            return false;
            }
        }
    return true;
    }
/*
template <class ELEMTYPE, int IMAGEDIM>
bool image_general<ELEMTYPE, IMAGEDIM>::is_voxel_pos_within_image(Vector3Dint vox_pos)
{
	if(vox_pos[0]<0) return false;
	if(vox_pos[1]<0) return false;
	if(vox_pos[2]<0) return false;
	if(vox_pos[0]>=datasize[0]) return false;
	if(vox_pos[1]>=datasize[1]) return false;
	if(vox_pos[2]>=datasize[2]) return false;
	return true;
}
*/

template <class ELEMTYPE, int IMAGEDIM>
bool image_general<ELEMTYPE, IMAGEDIM>::same_size (image_base * other, int direction)
	{
		bool ret=false;
		if (datasize[direction] == other->get_size_by_dim(direction))
		{
			ret=true;
		}
	return ret;
	}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::set_voxel_size(float dx, float dy, float dz)
	{
        voxel_size[0] = dx;
        voxel_size[1] = dy;
        voxel_size[2] = dz;
	}


template <class ELEMTYPE, int IMAGEDIM>
bool image_general<ELEMTYPE, IMAGEDIM>::read_voxel_size_from_dicom_file(std::string dcm_file)
{
	bool succeded = false;
	itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();

	if (dicomIO->CanReadFile(dcm_file.c_str()))
	{
		dicomIO->SetFileName(dcm_file.c_str());
		dicomIO->ReadImageInformation();		//get basic DICOM header
		this->voxel_size[0] = dicomIO->GetSpacing(0);
		this->voxel_size[1] = dicomIO->GetSpacing(1);
		this->voxel_size[2] = dicomIO->GetSpacing(2);
		succeded = true;
	}
	return succeded;
}

template <class ELEMTYPE, int IMAGEDIM>
float image_general<ELEMTYPE, IMAGEDIM>::get_voxel_volume_in_mm3()
{
	float res = 1;
	for(int i=0;i<IMAGEDIM;i++){
		res *= voxel_size[i];
	}
	return res;
}

template <class ELEMTYPE, int IMAGEDIM>
float image_general<ELEMTYPE, IMAGEDIM>::get_voxel_volume_in_cm3()
{
	float res = 1;
	for(int i=0;i<IMAGEDIM;i++){
		res *= voxel_size[i]*0.1;
	}
	return res;
}

template <class ELEMTYPE, int IMAGEDIM>
float image_general<ELEMTYPE, IMAGEDIM>::get_voxel_volume_in_dm3()
{
	float res = 1;
	for(int i=0;i<IMAGEDIM;i++){
		res *= voxel_size[i]*0.01;
	}
	return res;
}

template <class ELEMTYPE, int IMAGEDIM>
float image_general<ELEMTYPE, IMAGEDIM>::get_num_voxels_per_dm3()
{
	return 1.0/get_voxel_volume_in_dm3();
}

template <class ELEMTYPE, int IMAGEDIM>
string image_general<ELEMTYPE, IMAGEDIM>::resolve_tooltip()
{
	return resolve_tooltip_image_general();
}

template <class ELEMTYPE, int IMAGEDIM>
string image_general<ELEMTYPE, IMAGEDIM>::resolve_tooltip_image_general()
{
	return this->resolve_tooltip_image_storage() + int2str(IMAGEDIM)+"D --> datasize="+resolve_datasize()+"\n";
}

template <class ELEMTYPE, int IMAGEDIM>
string image_general<ELEMTYPE, IMAGEDIM>::resolve_datasize()
{
	string s=int2str(datasize[0]);
    for (unsigned short i = 1; i < IMAGEDIM; i++){
        s += "," + int2str(datasize[i]);
	}

	return s;
}



template <class ELEMTYPE, int IMAGEDIM>
    Matrix3D image_general<ELEMTYPE, IMAGEDIM>::get_voxel_resize () const
    {
    Matrix3D result;
    
    result.Fill(0);
    
    result[0][0] = voxel_size [0];
    result[1][1] = voxel_size [1];
    result[2][2] = voxel_size [2];
    
    return result;
    }

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::rotate_geometry_around_center_voxel(int fi_x_deg, int fi_y_deg, int fi_z_deg)
{
	cout<<"rotate_geometry_around_center_voxel...TODO"<<endl;
	Vector3D old_phys_center = this->get_physical_center();
	this->rotate_orientation(fi_x_deg,fi_y_deg,fi_z_deg);
	Vector3D new_phys_center = this->get_physical_center();
	this->set_origin( this->get_origin()-(new_phys_center-old_phys_center) );
}

template <class ELEMTYPE, int IMAGEDIM>
image_base* image_general<ELEMTYPE, IMAGEDIM>::expand_borders(unsigned int dx, unsigned int dy, unsigned int dz, ELEMTYPE value)
{
	cout<<"expanding image borders..."<<endl;
	int old_size_x=this->get_size_by_dim(0);
	int old_size_y=this->get_size_by_dim(1);
	int old_size_z=this->get_size_by_dim(2);
	image_general<ELEMTYPE, IMAGEDIM>* res = new image_general<ELEMTYPE, IMAGEDIM>(old_size_x+2*dx, old_size_y+2*dy, old_size_z+2*dz);
	res->fill(value);
	res->set_parameters(this);

	for (int z=0; z<old_size_z; z++){
		for (int y=0; y<old_size_y; y++){
			for (int x=0; x<old_size_x; x++){
				res->set_voxel(x+dx,y+dy,z+dz, this->get_voxel(x,y,z));
			}
		}
	}
	res->set_origin(this->get_physical_pos_for_voxel(-dx,-dy,-dz));
	return res;
}

template <class ELEMTYPE, int IMAGEDIM>
image_base* image_general<ELEMTYPE, IMAGEDIM>::expand_borders2D_by_dir(int dir, unsigned int dr, ELEMTYPE value)
{
	image_general<ELEMTYPE, IMAGEDIM>* res;
	if(dir==2){
		res = dynamic_cast<image_general<ELEMTYPE,IMAGEDIM >*>( expand_borders(dr,dr,0,value) );
	}else if(dir==1){
		res = dynamic_cast<image_general<ELEMTYPE,IMAGEDIM >*>( expand_borders(dr,0,dr,value) );
	}else{
		res = dynamic_cast<image_general<ELEMTYPE,IMAGEDIM >*>( expand_borders(0,dr,dr,value) );
	}
	return res;
}


template <class ELEMTYPE, int IMAGEDIM>
image_base* image_general<ELEMTYPE, IMAGEDIM>::contract_borders(unsigned int dx, unsigned int dy, unsigned int dz)
{
	cout<<"contracting image borders..."<<endl;
	int old_size_x=this->get_size_by_dim(0);
	int old_size_y=this->get_size_by_dim(1);
	int old_size_z=this->get_size_by_dim(2);
	image_general<ELEMTYPE, IMAGEDIM>* res = new image_general<ELEMTYPE, IMAGEDIM>(old_size_x-2*dx, old_size_y-2*dy, old_size_z-2*dz);
	res->set_parameters(this);

	for (int z=0; z<res->nz(); z++){
		for (int y=0; y<res->nz(); y++){
			for (int x=0; x<res->nx(); x++){
				res->set_voxel(x,y,z, this->get_voxel(x+dx,y+dy,z+dz));
			}
		}
	}
	res->set_origin(this->get_physical_pos_for_voxel(0+dx,0+dy,0+dz));
	return res;
}

template <class ELEMTYPE, int IMAGEDIM>
image_base* image_general<ELEMTYPE, IMAGEDIM>::contract_borders2D_by_dir(int dir, unsigned int dr)
{
	image_general<ELEMTYPE, IMAGEDIM>* res;
	if(dir==2){
		res = dynamic_cast<image_general<ELEMTYPE,IMAGEDIM >*>( contract_borders(dr,dr,0) );
	}else if(dir==1){
		res = dynamic_cast<image_general<ELEMTYPE,IMAGEDIM >*>( contract_borders(dr,0,dr) );
	}else{
		res = dynamic_cast<image_general<ELEMTYPE,IMAGEDIM >*>( contract_borders(0,dr,dr) );
	}
	return res;
}


template <class ELEMTYPE, int IMAGEDIM>
unsigned long image_general<ELEMTYPE, IMAGEDIM>::get_number_of_voxels_with_value_in_slice_2D(int slice, int dir, ELEMTYPE value)
{
    int usize=this->get_size_by_dim_and_dir(0,dir);
	int vsize=this->get_size_by_dim_and_dir(1,dir);
	unsigned long res=0;
	for (int u=0; u<usize; u++) {
		for (int v=0; v<vsize; v++) {
			if (this->get_voxel_by_dir(u,v,slice,dir)==value)
				res++;
		}
	}
	return res;
}

template <class ELEMTYPE, int IMAGEDIM>
unsigned long image_general<ELEMTYPE, IMAGEDIM>::get_number_of_voxels_with_value_greater_than_in_slice_2D(int slice, int dir, ELEMTYPE value)
{
    int usize=this->get_size_by_dim_and_dir(0,dir);
	int vsize=this->get_size_by_dim_and_dir(1,dir);
	unsigned long res=0;
	for (int u=0; u<usize; u++) {
		for (int v=0; v<vsize; v++) {
			if (this->get_voxel_by_dir(u,v,slice,dir)>value)
				res++;
		}
	}
	return res;
}


template <class ELEMTYPE, int IMAGEDIM>
unsigned long image_general<ELEMTYPE, IMAGEDIM>::get_number_of_voxels_with_value_in_26_nbh(Vector3Dint pos, ELEMTYPE value)
{
	int z_from = std::max(0,pos[2]-1);
	int y_from = std::max(0,pos[1]-1);
	int x_from = std::max(0,pos[0]-1);
	int z_to = std::min(int(this->nz()-1),pos[2]+1);
	int y_to = std::min(int(this->ny()-1),pos[1]+1);
	int x_to = std::min(int(this->nx()-1),pos[0]+1);
	unsigned long res =0;
	for(int c=z_from; c<=z_to; c++){
		for(int b=y_from; b<=y_to; b++){
			for(int a=x_from; a<=x_to; a++){
				if(this->get_voxel(a,b,c)==value){
					res++;
				}
			}
		}
	}
	return res;
}


template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::get_span_of_values_larger_than_3D(ELEMTYPE val_limit, int &x1, int &y1, int &z1, int &x2, int &y2, int &z2)
{
	x1 = datasize[0];	x2 = 0;
	y1 = datasize[1];	y2 = 0;
    z1 = datasize[2];	z2 = 0;

	for (int z=0; z < datasize[2]; z++){
		for (int y=0; y < datasize[1]; y++){
			for (int x=0; x < datasize[0]; x++){
				if(get_voxel(x,y,z)>val_limit){
					x1 = min(x,x1);
					y1 = min(y,y1);
					z1 = min(z,z1);
					x2 = max(x,x2);
					y2 = max(y,y2);
					z2 = max(z,z2);
				}
			}
		}
	}
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::get_span_of_value_3D(ELEMTYPE val, int &x1, int &y1, int &z1, int &x2, int &y2, int &z2)
{
	x1 = datasize[0];	x2 = 0;
	y1 = datasize[1];	y2 = 0;
    z1 = datasize[2];	z2 = 0;

	for(int z=0; z < datasize[2]; z++){
		for(int y=0; y < datasize[1]; y++){
			for(int x=0; x < datasize[0]; x++){
				if(get_voxel(x,y,z)==val){
					x1 = min(x,x1);
					y1 = min(y,y1);
					z1 = min(z,z1);
					x2 = max(x,x2);
					y2 = max(y,y2);
					z2 = max(z,z2);
				}
			}
		}
	}
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::get_span_of_value_in_subregion_3D(ELEMTYPE val, Vector3Dint sub_from, Vector3Dint sub_to, Vector3Dint &span_from, Vector3Dint &span_to)
{
	span_from[0] = datasize[0];		span_to[0] = 0;
	span_from[1] = datasize[1];		span_to[1] = 0;
	span_from[2] = datasize[2];		span_to[2] = 0;

	for (int z=sub_from[2]; z <= sub_to[2]; z++){
		for (int y=sub_from[1]; y <= sub_to[1]; y++){
			for (int x=sub_from[0]; x <= sub_to[0]; x++){
				if(get_voxel(x,y,z)==val){
					span_from[0] = min(x,span_from[0]);
					span_from[1] = min(y,span_from[1]);
					span_from[2] = min(z,span_from[2]);
					span_to[0] = max(x,span_to[0]);
					span_to[1] = max(y,span_to[1]);
					span_to[2] = max(z,span_to[2]);
				}
			}
		}
	}
}

template <class ELEMTYPE, int IMAGEDIM>
int image_general<ELEMTYPE, IMAGEDIM>::get_span_size_of_value_3D(ELEMTYPE val, int dir)
{
	int x1;
	int y1;
	int z1;
	int x2;
	int y2;
	int z2;
	get_span_of_value_3D(val,x1,y1,z1,x2,y2,z2);

	int ret=-1;

	if(dir==0){
		ret = x2-x1+1;
	}else if(dir==1){
		ret = y2-y1+1;
	}else if(dir==2){
		ret = z2-z1+1;
	}
	return ret;
}


template <class ELEMTYPE, int IMAGEDIM>
image_general<ELEMTYPE, IMAGEDIM>* image_general<ELEMTYPE, IMAGEDIM>::get_subvolume_from_slices_3D(int start_slice, int every_no_slice, int slice_dir)
{
	cout<<"get_subvolume_from_slices_3D...("<<slice_dir<<")"<<endl;

	image_general<ELEMTYPE, IMAGEDIM>* res = new image_general<ELEMTYPE, IMAGEDIM>();

	int nr_slices=0;
	if(slice_dir ==0 || slice_dir ==1 || slice_dir ==2){
		for(int i = start_slice; i<datasize[slice_dir]; i += every_no_slice){
			nr_slices++;
		}
	}else{
		pt_error::error("image_general<ELEMTYPE, IMAGEDIM>::get_subvolume_from_slices_3D -- slice_dir error",pt_error::debug);
	}

	if(slice_dir==2){
		res->initialize_dataset(datasize[0],datasize[1],nr_slices);
	}else if(slice_dir==1){
		res->initialize_dataset(datasize[0],nr_slices,datasize[2]);
	}else if(slice_dir==0){
		res->initialize_dataset(nr_slices,datasize[1],datasize[2]);
	}

	for(int i = start_slice,  j=0; i<datasize[slice_dir]; i += every_no_slice, j++){
		res->copy_slice_from_3D(this, i, j, slice_dir);
	}

	return res;
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::copy_slice_from_3D(image_general<ELEMTYPE, IMAGEDIM> *src, int from_slice_no, int to_slice_no, int slice_dir)
{
//	cout<<"copy_slice_from_3D..("<<slice_dir<<")"<<endl;
	int f = from_slice_no;
	int t = to_slice_no;
	if(slice_dir==2){
		if(f >=0 && f<src->datasize[2] && t>=0 && t<this->datasize[2] && this->same_size(src,0) && this->same_size(src,1)){
//			cout<<"slices OK"<<endl;
			for (int y=0; y < datasize[1]; y++){
				for (int x=0; x < datasize[0]; x++){
					set_voxel(x,y,t, src->get_voxel(x,y,f));
				}
			}
//			this->data_has_changed(true);
		}
	}else if(slice_dir==1){
		if(f >=0 && f<src->datasize[1] && t>=0 && t<this->datasize[1] && same_size(src,0) && same_size(src,2)){
//			cout<<"slices OK"<<endl;
			for (int z=0; z < datasize[2]; z++){
				for (int x=0; x < datasize[0]; x++){
					set_voxel(x,t,z, src->get_voxel(x,f,z));
				}
			}
//			this->data_has_changed(true);
		}
	}else if(slice_dir==0){
		if(f >=0 && f<src->datasize[0] && t>=0 && t<this->datasize[0] && same_size(src,1) && same_size(src,2)){
//			cout<<"slices OK"<<endl;
			for (int z=0; z < datasize[2]; z++){
				for (int y=0; y < datasize[1]; y++){
					set_voxel(t,y,z, src->get_voxel(f,y,z));
				}
			}
//			this->data_has_changed(true);
		}
	}else{
		pt_error::error("image_general<ELEMTYPE, IMAGEDIM>::copy_slice_from_3D -- slice_dir error",pt_error::debug);
	}
}


template <class ELEMTYPE, int IMAGEDIM>
image_general<ELEMTYPE, IMAGEDIM>* image_general<ELEMTYPE, IMAGEDIM>::get_subvolume_from_slice_rotated_3D(int slice, int dir)
{
//	cout<<"get_subvolume_from_slice_rotated_3D..."<<endl;

	int usize=this->get_size_by_dim_and_dir(0,dir);
	int vsize=this->get_size_by_dim_and_dir(1,dir);
	pt_error::error_if_false(slice>=0 && slice<this->get_size_by_dim_and_dir(2,dir)," slice outside image dimensions in image_general<ELEMTYPE, IMAGEDIM>::get_subvolume_from_slice_3D",pt_error::debug);

	image_general<ELEMTYPE, IMAGEDIM>* res = new image_general<ELEMTYPE, IMAGEDIM>(usize, vsize, 1);
	res->set_parameters(this);

	if (dir==0) {
		res->set_origin(this->get_physical_pos_for_voxel(slice,0,0));
	} else if (dir==1) {
		res->set_origin(this->get_physical_pos_for_voxel(0,slice,0));
	} else {
		res->set_origin(this->get_physical_pos_for_voxel(0,0,slice));
	}

	for (int u=0; u<usize; u++){
		for (int v=0; v<vsize; v++){
			res->set_voxel(u,v,0, this->get_voxel_by_dir(u,v,slice,dir));
		}
	}
	return res;
}


template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::add_volume_3D(image_general<ELEMTYPE, IMAGEDIM> *src, int add_dir)
{
//	cout<<"image_general-add_volume_3D..("<<add_dir<<")"<<endl;

//	image_scalar<ELEMTYPE, IMAGEDIM>* res = new image_scalar<ELEMTYPE, IMAGEDIM>(); //cannot instantiate image_general...
	image_general<ELEMTYPE, IMAGEDIM>* res = new image_general<ELEMTYPE, IMAGEDIM>(); //cannot instantiate image_general...
	res->set_parameters(this); //copy rotation and size infor to tmp image first...

	if(add_dir==2){
		res->name(this->name() + "_add_dir2_" + src->name());
		if( same_size(src,0) && same_size(src,1) ){
			res->initialize_dataset(datasize[0],datasize[1],datasize[2]+src->datasize[2]);
			for (int z=0; z < datasize[2]; z++){
				for (int y=0; y < datasize[1]; y++){
					for (int x=0; x < datasize[0]; x++){
						res->set_voxel(x,y,z, get_voxel(x,y,z));
					}
				}
			}
			for (int z=datasize[2]; z < datasize[2]+src->datasize[2]; z++){
				for (int y=0; y < datasize[1]; y++){
					for (int x=0; x < datasize[0]; x++){
						res->set_voxel(x,y,z, src->get_voxel(x,y,z-datasize[2]));
					}
				}
			}
//			this->deallocate(); //deallocate is done in initialize_dataset, if needed...
		    initialize_dataset(res->get_size_by_dim(0), res->get_size_by_dim(1), res->get_size_by_dim(2), NULL);
			copy_data(res,this);
		    set_parameters(res); 
			//res is deleted last in function....
		}


	}else if(add_dir==1){
		res->name(this->name() + "_add_dir1_" + src->name());
		if( same_size(src,0) && same_size(src,2) ){
			res->initialize_dataset(datasize[0],datasize[1]+src->datasize[1],datasize[2]);
			for (int z=0; z < datasize[2]; z++){
				for (int y=0; y < datasize[1]; y++){
					for (int x=0; x < datasize[0]; x++){
						res->set_voxel(x,y,z, get_voxel(x,y,z));
					}
				}
			}

			for (int z=0; z < datasize[2]; z++){
				for (int y=datasize[1]; y < datasize[1]+src->datasize[1]; y++){
					for (int x=0; x < datasize[0]; x++){
						res->set_voxel(x,y,z, src->get_voxel(x,y-datasize[1],z));
					}
				}
			}
//			this->deallocate(); //deallocate is done in initialize_dataset, if needed...
		    initialize_dataset(res->get_size_by_dim(0), res->get_size_by_dim(1), res->get_size_by_dim(2), NULL);
			copy_data(res,this);
		    set_parameters(res);
		}

	}else if(add_dir==0){
		res->name(this->name() + "_add_dir0_" + src->name());
		if( same_size(src,1) && same_size(src,2) ){
			res->initialize_dataset(datasize[0]+src->datasize[0],datasize[1],datasize[2]);
			for (int z=0; z < datasize[2]; z++){
				for (int y=0; y < datasize[1]; y++){
					for (int x=0; x < datasize[0]; x++){
						res->set_voxel(x,y,z, get_voxel(x,y,z));
					}
				}
			}

			for (int z=0; z < datasize[2]; z++){
				for (int y=0; y < datasize[1]; y++){
					for (int x=datasize[0]; x < datasize[0]+src->datasize[0]; x++){
						res->set_voxel(x,y,z, src->get_voxel(x-datasize[0],y,z));
					}
				}
			}
//			this->deallocate(); //deallocate is done in initialize_dataset, if needed...
		    initialize_dataset(res->get_size_by_dim(0), res->get_size_by_dim(1), res->get_size_by_dim(2), NULL);
			copy_data(res,this);
		    set_parameters(res);
		}

	}else{
		pt_error::error("image_general<ELEMTYPE, IMAGEDIM>::copy_slice_from_3D -- add_dir error",pt_error::debug);
	}

	delete res;
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::add_volume_3D(image_label<IMAGEDIM> *src, int add_dir)
{
//	image_scalar<ELEMTYPE,IMAGEDIM> *src2 = scalar_copycast<image_scalar,ELEMTYPE,IMAGEDIM>(src);
	image_general<ELEMTYPE,IMAGEDIM> *src2 = new image_general(src);
//	src2->save_to_file("C:/Joel/TMP/bin_add_volume_3D.vtk");
	src2->scale_by_factor( this->get_max_float()/src2->get_max_float() );	//Often used to create collages of results where binary images benefit from scaling... 
	this->add_volume_3D(src2,add_dir);
	delete src2;
}


template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::add_slice_3D(image_general<ELEMTYPE, IMAGEDIM> *src, int from_slice_no, int slice_dir)
{
//	cout<<"image_general-add_slice_3D..("<<slice_dir<<")"<<endl;

	int f = from_slice_no;
	image_general<ELEMTYPE, IMAGEDIM>* res = new image_general<ELEMTYPE, IMAGEDIM>();

	if(slice_dir==2){
		res->name("res-2..");
		if( f >=0 && f < src->datasize[2] && same_size(src,0) && same_size(src,1) ){
			cout<<"...entered"<<endl;
			res->initialize_dataset(datasize[0],datasize[1],datasize[2]+1);
			for (int z=0; z < datasize[2]; z++){
				for (int y=0; y < datasize[1]; y++){
					for (int x=0; x < datasize[0]; x++){
						res->set_voxel(x,y,z, get_voxel(x,y,z));
					}
				}
			}
			for (int y=0; y < datasize[1]; y++){
				for (int x=0; x < datasize[0]; x++){
					res->set_voxel(x,y,datasize[2], src->get_voxel(x,y,f));
				}
			}
//			this->deallocate(); //deallocate is done in initialize_dataset, if needed...
		    initialize_dataset(res->get_size_by_dim(0), res->get_size_by_dim(1), res->get_size_by_dim(2), NULL);
			copy_data(res,this);
		    set_parameters(res);
		}

	}else if(slice_dir==1){
		res->name("res-1..");
		if( f >=0 && f < src->datasize[1] && same_size(src,0) && same_size(src,2) ){
			cout<<"...entered"<<endl;
			res->initialize_dataset(datasize[0],datasize[1]+1,datasize[2]);
			for (int z=0; z < datasize[2]; z++){
				for (int y=0; y < datasize[1]; y++){
					for (int x=0; x < datasize[0]; x++){
						res->set_voxel(x,y,z, get_voxel(x,y,z));
					}
				}
			}
			for (int z=0; z < datasize[2]; z++){
				for (int x=0; x < datasize[0]; x++){
					res->set_voxel(x,datasize[1],z, src->get_voxel(x,f,z));
				}
			}

//			this->deallocate(); //deallocate is done in initialize_dataset, if needed...
		    initialize_dataset(res->get_size_by_dim(0), res->get_size_by_dim(1), res->get_size_by_dim(2), NULL);
			copy_data(res,this);
		    set_parameters(res);
		}

	}else if(slice_dir==0){
		res->name("res-0..");
		if( f >=0 && f < src->datasize[0] && same_size(src,1) && same_size(src,2) ){
			cout<<"...entered"<<endl;
			res->initialize_dataset(datasize[0]+1,datasize[1],datasize[2]);
			for (int z=0; z < datasize[2]; z++){
				for (int y=0; y < datasize[1]; y++){
					for (int x=0; x < datasize[0]; x++){
						res->set_voxel(x,y,z, get_voxel(x,y,z));
					}
				}
			}
			for (int z=0; z < datasize[2]; z++){
				for (int y=0; y < datasize[1]; y++){
					res->set_voxel(datasize[0],y,z, src->get_voxel(f,y,z));
				}
			}

//			this->deallocate(); //deallocate is done in initialize_dataset, if needed...
		    initialize_dataset(res->get_size_by_dim(0), res->get_size_by_dim(1), res->get_size_by_dim(2), NULL);
			copy_data(res,this);
		    set_parameters(res);
		}

	}else{
		pt_error::error("image_general<ELEMTYPE, IMAGEDIM>::copy_slice_from_3D -- slice_dir error",pt_error::debug);
	}

	delete res;		// ->deallocate();		//÷÷÷ - JK WARNING MEMORY LEAK???
}



template <class ELEMTYPE, int IMAGEDIM>
image_general<ELEMTYPE, IMAGEDIM>* image_general<ELEMTYPE, IMAGEDIM>::get_collage2D_from3D_volume(int num_cols, int num_rows)
{
	image_general<ELEMTYPE, IMAGEDIM> *res = new image_general<ELEMTYPE, IMAGEDIM>(num_cols*this->nx(),num_rows*this->ny(),1);
	res->fill(0);
	int slice=0;
	Vector3Dint from_size = create_Vector3Dint(this->nx(),this->ny(),1);
	for(int j=0;j<num_rows;j++){
		for(int i=0;i<num_cols;i++){
			//fill_region_3D_with_subvolume_image(Vector3Dint to_pos, image_general<ELEMTYPE, IMAGEDIM> *im, Vector3Dint from_pos, Vector3Dint from_size); //based on given voxel coords
			Vector3Dint to_pos = create_Vector3Dint(i*this->nx(), j*this->ny(),0);
			Vector3Dint from_pos = create_Vector3Dint(0,0,slice);
			res->fill_region_3D_with_subvolume_image(to_pos, this, from_pos, from_size);
			slice++;
		}
	}
	return res;	
}
 



template <class ELEMTYPE, int IMAGEDIM>
image_general<ELEMTYPE, IMAGEDIM>* image_general<ELEMTYPE, IMAGEDIM>::rotate_voxeldata_3D(int rot_axis, int pos_neg_dir)
{ 
//	cout<<"rotate_voxeldata_3D("+int2str(rot_axis)+", "+int2str(pos_neg_dir)+")"<<endl;
	
	image_general<ELEMTYPE, 3>* res = new image_general<ELEMTYPE, 3>(); 
	res->set_parameters(this);
	
	int sx = this->get_size_by_dim(0);
	int sy = this->get_size_by_dim(1);
	int sz = this->get_size_by_dim(2);
	Vector3D old_voxel_size = this->get_voxel_size();
	Vector3D new_voxel_size;
	Vector3D old_origin = this->get_origin();
	Vector3D new_origin; 
	Matrix3D old_orientation = this->get_orientation();
	Matrix3D new_orientation;
	
	if(rot_axis>=0 && rot_axis<=2 && (pos_neg_dir==+1 || pos_neg_dir==-1)) {
		
		if(rot_axis == 0){
			res->initialize_dataset(sx,sz,sy);
			new_voxel_size = create_Vector3D(old_voxel_size[0], old_voxel_size[2], old_voxel_size[1]);
			if (pos_neg_dir == +1)
				new_origin = create_Vector3D(old_origin[0], sz-1-old_origin[2], old_origin[1]);
			else if (pos_neg_dir == -1)
				new_origin = create_Vector3D(old_origin[0], old_origin[2], sy-1-old_origin[1]);
			new_orientation[0][0] = old_orientation[0][0];
			new_orientation[0][1] = -pos_neg_dir*old_orientation[0][2];
			new_orientation[0][2] = pos_neg_dir*old_orientation[0][1];
			new_orientation[1][0] = -pos_neg_dir*old_orientation[2][0];
			new_orientation[1][1] = old_orientation[2][2];
			new_orientation[1][2] = -old_orientation[2][1];
			new_orientation[2][0] = pos_neg_dir*old_orientation[1][0];
			new_orientation[2][1] = -old_orientation[1][2];
			new_orientation[2][2] = old_orientation[1][1];
			for(int x=0; x<sx; x++){
				for(int y=0; y<sy; y++){
					for(int z=0; z<sz; z++){
						if (pos_neg_dir == +1)
							res->set_voxel(x,sz-1-z,y,this->get_voxel(x,y,z));
						else if (pos_neg_dir == -1)
							res->set_voxel(x,z,sy-1-y,this->get_voxel(x,y,z));
					}
				}
			}
		}
		else if(rot_axis == 1){
			res->initialize_dataset(sz,sy,sx);
			new_voxel_size = create_Vector3D(old_voxel_size[2], old_voxel_size[1], old_voxel_size[0]);
			if (pos_neg_dir == +1)
				new_origin = create_Vector3D(old_origin[2], old_origin[1], sx-1-old_origin[0]);
			else if (pos_neg_dir == -1)
				new_origin = create_Vector3D(sz-1-old_origin[2], old_origin[1], old_origin[0]);
			new_orientation[0][0] = old_orientation[2][2];
			new_orientation[0][1] = pos_neg_dir*old_orientation[2][1];
			new_orientation[0][2] = -old_orientation[2][0];
			new_orientation[1][0] = pos_neg_dir*old_orientation[1][2];
			new_orientation[1][1] = old_orientation[1][1];
			new_orientation[1][2] = -pos_neg_dir*old_orientation[1][0];
			new_orientation[2][0] = -old_orientation[0][2];
			new_orientation[2][1] = -pos_neg_dir*old_orientation[0][1];
			new_orientation[2][2] = old_orientation[0][0];
			for(int x=0; x<sx; x++){
				for(int y=0; y<sy; y++){
					for(int z=0; z<sz; z++){
						if (pos_neg_dir == +1)
							res->set_voxel(z,y,sx-1-x,this->get_voxel(x,y,z));
						else if (pos_neg_dir == -1)
							res->set_voxel(sz-1-z,y,x,this->get_voxel(x,y,z));
					}
				}
			}
		}
		else if(rot_axis == 2){
			res->initialize_dataset(sy,sx,sz);
			new_voxel_size = create_Vector3D(old_voxel_size[1], old_voxel_size[0], old_voxel_size[2]);
			if (pos_neg_dir == +1)
				new_origin = create_Vector3D(sy-1-old_origin[1], old_origin[0], old_origin[2]);
			else if (pos_neg_dir == -1)
				new_origin = create_Vector3D(old_origin[1], sx-1-old_origin[0], old_origin[2]);
			new_orientation[0][0] = old_orientation[1][1];
			new_orientation[0][1] = -old_orientation[1][0];
			new_orientation[0][2] = -pos_neg_dir*old_orientation[1][2];
			new_orientation[1][0] = -old_orientation[0][1];
			new_orientation[1][1] = old_orientation[0][0];
			new_orientation[1][2] = pos_neg_dir*old_orientation[0][2];
			new_orientation[2][0] = -pos_neg_dir*old_orientation[2][1];
			new_orientation[2][1] = pos_neg_dir*old_orientation[2][0];
			new_orientation[2][2] = old_orientation[2][2];
			for(int x=0; x<sx; x++){
				for(int y=0; y<sy; y++){
					for(int z=0; z<sz; z++){
						if (pos_neg_dir == +1)
							res->set_voxel(sy-1-y,x,z,this->get_voxel(x,y,z));
						else if (pos_neg_dir == -1)
							res->set_voxel(y,sx-1-x,z,this->get_voxel(x,y,z));
					}
				}
			}
			
		}
	}
	else
		pt_error::error("rotate_voxeldata_3D-->parameters...("+int2str(rot_axis)+", "+int2str(pos_neg_dir)+")",pt_error::debug);
	
	res->set_voxel_size(new_voxel_size);
	res->set_origin(new_origin);
	res->set_orientation(new_orientation);
	//TODO:JK figure out how to handle shift slice orientation 
	
	return res;
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::rotate_voxeldata_3D_in_this(int rot_axis, int pos_neg_dir)
{
//	cout<<"rotate_voxeldata_3D_in_this..."<<endl;
	image_general<ELEMTYPE, IMAGEDIM> *tmp = rotate_voxeldata_3D(rot_axis, pos_neg_dir);
//	tmp->save_to_file("C:/joel/TMP/rot.vtk");
	int sx = tmp->get_size_by_dim(0);
	int sy = tmp->get_size_by_dim(1);
	int sz = tmp->get_size_by_dim(2);

	this->initialize_dataset(sx,sy,sz);
	copy_data(tmp,this);
	this->set_parameters(tmp);

	delete tmp;
}


template <class ELEMTYPE, int IMAGEDIM>
const Vector3D image_general<ELEMTYPE, IMAGEDIM>::get_voxel_size () const
{
    return voxel_size;
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::set_voxel_size(const Vector3D v)
{
    voxel_size = v;
}

template <class ELEMTYPE, int IMAGEDIM>
bool image_general<ELEMTYPE, IMAGEDIM>::is_voxelpos_within_image_3D(int vp_x, int vp_y, int vp_z)
	{
	return ( vp_x>=0 && vp_y>=0 && vp_z>=0 && vp_x<datasize[0] && vp_y<datasize[1] && vp_z<datasize[2] );
	}

template <class ELEMTYPE, int IMAGEDIM>
bool image_general<ELEMTYPE, IMAGEDIM>::is_voxelpos_within_image_3D( Vector3Dint vp )
	{
	return is_voxelpos_within_image_3D(vp[0],vp[1],vp[2]);
	}

template <class ELEMTYPE, int IMAGEDIM>
bool image_general<ELEMTYPE, IMAGEDIM>::is_voxelpos_inside_image_border_3D(int vp_x, int vp_y, int vp_z, int dist)
	{
	return (vp_x>=dist && vp_y>=dist && vp_z>=dist && vp_x<datasize[0]-dist && vp_y<datasize[1]-dist && vp_z<datasize[2]-dist);
	}

template <class ELEMTYPE, int IMAGEDIM>
bool image_general<ELEMTYPE, IMAGEDIM>::is_voxelpos_inside_image_border_3D(Vector3D vp, int dist)
	{
	return is_voxelpos_inside_image_border_3D(vp[0],vp[1],vp[2], dist);
	}



template <class ELEMTYPE, int IMAGEDIM>
bool image_general<ELEMTYPE, IMAGEDIM>::is_physical_pos_within_image_3D(Vector3D phys_pos)
	{
	//cout<<" *** is_physical_pos_within_image_3D(Vector3D phys_pos)... *** "<<std::endl;
	//ev. phys_pos = origin + direction*voxel_resize*voxel_pos
	//figure out if (direction*X = phys_pos_of_first_corner) --> are then all X-components positive? and...
	//figure out if (direction*Y = phys_pos_of_last_corner) --> are then all Y-components positive?
	Vector3D phys_pos2 = phys_pos - this->origin; //now we only need to find out the scalar product of...
	Matrix3D dir_inv;
	dir_inv = this->orientation.GetInverse();
	Vector3D X = dir_inv*phys_pos2;
//	cout<<"X="<<X<<std::endl;
	bool ret=false;
	if(X[0]>=0 && X[1]>=0 && X[2]>=0)
	{
		phys_pos2 = phys_pos - get_physical_pos_for_voxel(datasize[0]-1,datasize[1]-1,datasize[2]-1);
		X = dir_inv*phys_pos2;
//		cout<<"X2="<<X<<std::endl;
		if(X[0]<=0 && X[1]<=0 && X[2]<=0)
		{
			ret = true;
		}
	}
	
	return ret;
	}

template <class ELEMTYPE, int IMAGEDIM>
Vector3Dint image_general<ELEMTYPE, IMAGEDIM>::get_voxelpos_integers_from_physical_pos_3D(Vector3D phys_pos)
	{
	Vector3D vox = this->world_to_voxel(phys_pos);
	Vector3Dint vox2;
	vox2[0] = round<float>(vox[0]);
	vox2[1] = round<float>(vox[1]);
	vox2[2] = round<float>(vox[2]);
	return vox2;
	}

template <class ELEMTYPE, int IMAGEDIM>
plane3D image_general<ELEMTYPE, IMAGEDIM>::get_plane_spanning_volume3D(int plane_id)
{
	plane3D p;
	if(plane_id==0 || plane_id==1 || plane_id==2){
		p = plane3D(this->get_origin(),-this->get_phys_dir_from_axis_dir(plane_id));
	}else{
		p = plane3D(this->get_phys_pos_of_corner(6),this->get_phys_dir_from_axis_dir(plane_id-3));
	}

	return p;
}

template <class ELEMTYPE, int IMAGEDIM>
Vector3Dint image_general<ELEMTYPE, IMAGEDIM>::get_line_intersection_with_plane_spanning_volume3D(line3D l, int plane_id)
{
	Vector3Dint res = create_Vector3Dint(-1,-1,-1);

	plane3D plane = this->get_plane_spanning_volume3D(plane_id);
	Vector3D intersect = plane.get_point_of_intersection(l);
//	cout<<"intersect="<<intersect<<endl;
	if(is_defined(intersect)){
//		cout<<"defined...."<<endl;
		Vector3D corner1 = this->get_origin();
		Vector3D corner2 = this->get_phys_pos_of_corner(6);
		Vector3D dir_1;
		Vector3D dir_2;
		if(plane_id==0||plane_id==3){
			dir_1 = this->get_phys_dir_from_axis_dir(1);
			dir_2 = this->get_phys_dir_from_axis_dir(2);
		}
		else if(plane_id==1||plane_id==4){
			dir_1 = this->get_phys_dir_from_axis_dir(0);
			dir_2 = this->get_phys_dir_from_axis_dir(2);
		}
		else{//(plane_id==2||plane_id==5){
			dir_1 = this->get_phys_dir_from_axis_dir(0);
			dir_2 = this->get_phys_dir_from_axis_dir(1);
		}
		
		float a = (intersect-corner1)*dir_1;
		float b = (intersect-corner1)*dir_2;
		float c = (intersect-corner2)*(-dir_1);
		float d = (intersect-corner2)*(-dir_2);
//		cout<<"a="<<a<<endl;
//		cout<<"b="<<b<<endl;
//		cout<<"c="<<c<<endl;
//		cout<<"d="<<d<<endl;
		if(a>0 && b>0 && c>0 && d>0){
//			this->print_geometry();
			res = this->world_to_voxel(intersect);
		}
	}
	return res;
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::get_line_intersection_voxels(line3D l, Vector3Dint &v1, Vector3Dint &v2)
{
	Vector3Dint vox;
	bool v1_set=false;
	bool v2_set=false;

	for(int i=0;i<6;i++){
		if(!v2_set){
			vox = this->get_line_intersection_with_plane_spanning_volume3D(l,i);
			if(vox[0]>=0){
				if(!v1_set){
					v1 = vox;
					v1_set=true;
				}else{ // We already know that v2_set is not set "here"!
					v2 = vox;
					v2_set=true;
				}
			}
		}
	}
}

template <class ELEMTYPE, int IMAGEDIM>
vector<plane3D> image_general<ELEMTYPE, IMAGEDIM>::get_planes_spanning_volume3D()
{
	vector<plane3D> v;
	Matrix3D m = this->get_orientation();
//	plane3D p = plane3D(this->get_origin(),this->get_orientation()* //JK TODO
	return v;
}



template <class ELEMTYPE, int IMAGEDIM>
Vector3D image_general<ELEMTYPE, IMAGEDIM>::get_phys_pos_of_max_intensity_between(Vector3Dint from_vox, Vector3Dint to_vox, unsigned int radius)
{
	Vector3Dint diff = (to_vox - from_vox);
	float n = max( max(diff[0],diff[1]), diff[2] );
	Vector3D delta;
	delta[0] = float(diff[0])/(n+1.0);
	delta[1] = float(diff[1])/(n+1.0);
	delta[2] = float(diff[2])/(n+1.0);

	Vector3D max_pos;
	ELEMTYPE max_intensity = std::numeric_limits<ELEMTYPE>::min();
	ELEMTYPE tmp;

	if(radius==0){

		for(float i=0;i<n+1;i++){
			tmp = this->get_voxel( from_vox[0]+delta[0]*i, from_vox[1]+delta[1]*i, from_vox[2]+delta[2]*i);
			if(tmp>max_intensity){
				max_intensity = tmp;
				max_pos = this->voxel_to_world( create_Vector3D(from_vox[0]+delta[0]*i, from_vox[1]+delta[1]*i, from_vox[2]+delta[2]*i) );
			}
		}

	}else{

		for(float i=0;i<n+1;i++){
			tmp = this->get_max_in_region(from_vox[0]+delta[0]*i, from_vox[1]+delta[1]*i, from_vox[2]+delta[2]*i, radius);
			if(tmp>max_intensity){
				max_intensity = tmp;
				max_pos = this->voxel_to_world( create_Vector3D(from_vox[0]+delta[0]*i, from_vox[1]+delta[1]*i, from_vox[2]+delta[2]*i) );
			}
		}

	}

	return max_pos;
}

template <class ELEMTYPE, int IMAGEDIM>
Vector3D image_general<ELEMTYPE, IMAGEDIM>::get_phys_pos_of_max_intensity_along(line3D line, unsigned int radius)
{
	Vector3D pos;

	Vector3Dint v1;
	Vector3Dint v2;
	this->get_line_intersection_voxels(line, v1, v2);
	cout<<"v1="<<v1<<endl;
	cout<<"v2="<<v2<<endl;
	if(v2[0]>=0){
		pos = this->get_phys_pos_of_max_intensity_between(v1,v2, radius);
	}

	return pos;
}

template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_general<ELEMTYPE, IMAGEDIM>::get_max_in_region(int x,int y,int z, unsigned int radius)
{
	ELEMTYPE max_val = std::numeric_limits<ELEMTYPE>::min();
	ELEMTYPE val;

	for(int w=std::max(0,int(z-radius)); w<=std::min(int(this->nz()-1),int(z+radius)); w++){
		for(int v=std::max(0,int(y-radius)); v<=std::min(int(this->ny()-1),int(y+radius)); v++){
			for(int u=std::max(0,int(x-radius)); u<=std::min(int(this->nx()-1),int(x+radius)); u++){
				val = this->get_voxel(u,v,w);
				if(val>max_val){
					max_val = val;
				}
			}
		}
	}

	return max_val;
}

template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_general<ELEMTYPE, IMAGEDIM>::get_min_in_region(int x,int y,int z, unsigned int radius)
{
	ELEMTYPE min_val = std::numeric_limits<ELEMTYPE>::max();
	ELEMTYPE val;

	for(int w=std::max(0,int(z-radius)); w<=std::min(int(this->nz()-1),int(z+radius)); w++){
		for(int v=std::max(0,int(y-radius)); v<=std::min(int(this->ny()-1),int(y+radius)); v++){
			for(int u=std::max(0,int(x-radius)); u<=std::min(int(this->nx()-1),int(x+radius)); u++){
				val = this->get_voxel(u,v,w);
				if(val<min_val){
					min_val = val;
				}
			}
		}
	}

	return min_val;
}


template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_general<ELEMTYPE, IMAGEDIM>::get_voxel(int x, int y, int z) const
    {
    return this->dataptr[x + datasize[0]*y + datasize[0]*datasize[1]*z];
    }

template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_general<ELEMTYPE, IMAGEDIM>::get_voxel(Vector3Dint vox_pos) const
{
    return this->dataptr[vox_pos[0] + datasize[0]*vox_pos[1] + datasize[0]*datasize[1]*vox_pos[2]];
}

template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_general<ELEMTYPE, IMAGEDIM>::get_voxel(int vox_pos[3]) const
{
    return this->dataptr[vox_pos[0] + datasize[0]*vox_pos[1] + datasize[0]*datasize[1]*vox_pos[2]];
}

template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE* image_general<ELEMTYPE, IMAGEDIM>::get_voxel_pointer(int x, int y, int z)
{
	unsigned long steps = x + datasize[0]*y + datasize[0]*datasize[1]*z;
	return (this->imagepointer() + steps);
}

template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE* image_general<ELEMTYPE, IMAGEDIM>::get_voxel_pointer(Vector3Dint vox_pos)
{
	unsigned long steps = vox_pos[0] + datasize[0]*vox_pos[1] + datasize[0]*datasize[1]*vox_pos[2];
	return (this->imagepointer() + steps);
}



template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_general<ELEMTYPE, IMAGEDIM>::get_voxel_in_physical_pos(Vector3D phys_pos)
    {
	Vector3D v = this->world_to_voxel(phys_pos);
	return (is_voxelpos_within_image_3D(v[0],v[1],v[2])) ? get_voxel(v[0],v[1],v[2]) : 0;
    }

template <class ELEMTYPE, int IMAGEDIM> //JK - No boundary checking...
ELEMTYPE image_general<ELEMTYPE, IMAGEDIM>::get_voxel_in_physical_pos_mean_3D_interp26(Vector3D phys_pos)
    {
	Vector3D v = this->world_to_voxel(phys_pos);
	Vector3D cv;
	cv[0]=int(v[0]); cv[1]=int(v[1]); cv[2]=int(v[2]);	//center voxel to interpolate around...

	ELEMTYPE sum=0;
	Vector3D tmp;
	for(int i=-1;i<=1;i++){
		for(int j=-1;j<=1;j++){
			for(int k=-1;k<=1;k++){
				tmp[0]=cv[0]+i;
				tmp[1]=cv[1]+j;
				tmp[2]=cv[2]+k;
				if(is_voxelpos_within_image_3D(tmp[0],tmp[1],tmp[2]))
					{
					sum += get_voxel(tmp[0],tmp[1],tmp[2]);
					}
				}
			}
		}
	ELEMTYPE res = sum/26.0;
	//std::cout<<"v="<<v<<" cv="<<cv<<" res="<<res<<std::endl;

    return res;
    }

template <class ELEMTYPE, int IMAGEDIM> //JK
ELEMTYPE image_general<ELEMTYPE, IMAGEDIM>::get_voxel_in_physical_pos_26NB_weighted(Vector3D phys_pos, float w1_center, float w2_side, float w3_edge, float w4_vertex)
    {
	Vector3D v = this->world_to_voxel(phys_pos);
	Vector3D cv;
	cv[0]=int(v[0]); cv[1]=int(v[1]); cv[2]=int(v[2]);	//center voxel to interpolate around...

	ELEMTYPE sum=0;
	Vector3D tmp;
	ELEMTYPE res = 0;
	
	if(is_voxelpos_within_image_3D(cv[0],cv[1],cv[2])){

		//Center Pixel
		sum += w1_center*get_voxel(cv[0],cv[1],cv[2]);

		//Side Neighbours
		sum += w2_side*get_voxel(cv[0]-1,cv[1],cv[2]);	//left
		sum += w2_side*get_voxel(cv[0]+1,cv[1],cv[2]);	//right
		sum += w2_side*get_voxel(cv[0],cv[1]-1,cv[2]);	//top
		sum += w2_side*get_voxel(cv[0],cv[1]+1,cv[2]);	//bottom
		sum += w2_side*get_voxel(cv[0],cv[1],cv[2]-1);	//under
		sum += w2_side*get_voxel(cv[0],cv[1],cv[2]+1);	//over

		//Edge Neighbours
		for(int k=-1;k<=1;k=k+2){	//"under" and "over" layers...
			sum += w3_edge*get_voxel(cv[0]-1,cv[1],cv[2]+k);
			sum += w3_edge*get_voxel(cv[0]+1,cv[1],cv[2]+k);
			sum += w3_edge*get_voxel(cv[0],cv[1]-1,cv[2]+k);
			sum += w3_edge*get_voxel(cv[0],cv[1]+1,cv[2]+k);
		}
		//"center" layer...
		sum += w3_edge*get_voxel(cv[0]-1,cv[1]-1,cv[2]);
		sum += w3_edge*get_voxel(cv[0]-1,cv[1]+1,cv[2]);
		sum += w3_edge*get_voxel(cv[0]+1,cv[1]-1,cv[2]);
		sum += w3_edge*get_voxel(cv[0]+1,cv[1]+1,cv[2]);


		//Corner Neighbours
		for(int i=-1;i<=1;i=i+2){
			for(int j=-1;j<=1;j=j+2){
				for(int k=-1;k<=1;k=k+2){
					tmp[0]=cv[0]+i;	tmp[1]=cv[1]+j;	tmp[2]=cv[2]+k;
					if(is_voxelpos_within_image_3D(tmp[0],tmp[1],tmp[2])){
						sum += w4_vertex*get_voxel(tmp[0],tmp[1],tmp[2]);
					}
				}
			}
		}
	}
	res = sum / (this->w1 + 6.0*this->w2 + 12*this->w3 + 8*this->w4);
	//std::cout<<"v="<<v<<" cv="<<cv<<" res="<<res<<std::endl;

    return res;
    }

template <class ELEMTYPE, int IMAGEDIM>
float image_general<ELEMTYPE, IMAGEDIM>::get_number_voxel(int x, int y, int z) const
{
//    return static_cast<float>(get_number_voxel(x, y, z)); //JK4
	return 0; //JK4
}


template <class ELEMTYPE, int IMAGEDIM>
Vector3D image_general<ELEMTYPE, IMAGEDIM>::get_physical_pos_for_voxel(int x, int y, int z)
{
	Vector3D vox_pos = create_Vector3D( x, y, z );
	Vector3D phys_pos = this->voxel_to_world( vox_pos );
	return phys_pos;
}

template <class ELEMTYPE, int IMAGEDIM>
Vector3D image_general<ELEMTYPE, IMAGEDIM>::get_physical_pos_or_voxel_pos(int x, int y, int z, SPACE_TYPE st)
{
	Vector3D res;
	if(st == VOXEL_SPACE){
		res[0]=x; 
		res[1]=y; 
		res[2]=z; 
	}else{ //PHYSICAL_SPACE
		res = this->get_physical_pos_for_voxel(x,y,z);
	}
	return res;
}


template <class ELEMTYPE, int IMAGEDIM>
float image_general<ELEMTYPE, IMAGEDIM>::get_physical_distance_between_voxels(int x1, int y1, int z1, int x2, int y2, int z2)
{
	Vector3D distance = get_physical_pos_for_voxel(x1, y1, z1) - get_physical_pos_for_voxel(x2, y2, z2);
	return sqrt(pow(distance[0],2)+pow(distance[1],2)+pow(distance[2],2));
}

template <class ELEMTYPE, int IMAGEDIM>
float image_general<ELEMTYPE, IMAGEDIM>::get_phys_span_in_dir(Vector3D dir)
{
//	cout<<"get_phys_span_in_dir("<<dir<<")"<<endl;
	float min_dist=std::numeric_limits<float>::max();
//	float max_dist=std::numeric_limits<float>::min(); //JK MEGA WARNING this gives approx 0 e-32
	float max_dist = -std::numeric_limits<float>::max();
//	cout<<"start-min_dist="<<min_dist<<endl;
//	cout<<"start-max_dist="<<max_dist<<endl;

	//normalize dir...
//	dir.Normalize();	//not needed...

	float a=0;
	for(int i=0;i<8;i++){
		a = dir*this->get_phys_pos_of_corner(i);
//		cout<<"i="<<i<<" "<<a<<" "<<this->get_phys_pos_of_corner(i)<<" "<<min_dist<<" "<<max_dist<<endl;
		min_dist = std::min(min_dist, a);
		max_dist = std::max(max_dist, a);
	}
//	cout<<"min_dist="<<min_dist<<endl;
//	cout<<"max_dist="<<max_dist<<endl;
	return max_dist-min_dist;
}

template <class ELEMTYPE, int IMAGEDIM>
Vector3D image_general<ELEMTYPE, IMAGEDIM>::get_phys_dir_from_axis_dir(int dir)
{
	Vector3D phys;
	if(dir==0){
		phys = this->get_orientation()*create_Vector3D(1,0,0);
	}else if(dir==1){
		phys = this->get_orientation()*create_Vector3D(0,1,0);
	}else if(dir==2){
		phys = this->get_orientation()*create_Vector3D(0,0,1);
	}
	return phys;
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::set_voxel(int x, int y, int z, ELEMTYPE voxelvalue)
    {
	//JK - uncomment these rows to detect writing outside allocated memory...
	if(x<0||x>=datasize[0] || y<0||y>=datasize[1] || z<0||z>=datasize[2])
		{cout<<"set_voxel--> strange index... x="<<x<<" y="<<y<<" z="<<z<<"... datasize=("<<datasize[0]<<","<<datasize[1]<<","<<datasize[2]<<")"<<endl;}

    this->dataptr[x + datasize[0]*y + datasize[0]*datasize[1]*z] = voxelvalue;
    }

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::set_voxel(Vector3D coord_pos, ELEMTYPE voxelvalue)
    {
		this->set_voxel(coord_pos[0],coord_pos[1],coord_pos[2],voxelvalue);
    }

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::set_voxel(int pos[3], ELEMTYPE voxelvalue)
    {
	//JK - uncomment these rows to detect writing outside allocated memory...
	if(pos[0]<0||pos[0]>=datasize[0] || pos[1]<0||pos[1]>=datasize[1] || pos[2]<0||pos[2]>=datasize[2])
		{cout<<"set_voxel--> strange index... x="<<pos[0]<<" y="<<pos[1]<<" z="<<pos[2]<<"... datasize=("<<datasize[0]<<","<<datasize[1]<<","<<datasize[2]<<")"<<endl;}

    this->dataptr[pos[0] + datasize[0]*pos[1] + datasize[0]*datasize[1]*pos[2]] = voxelvalue;
    }

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::set_voxel(int x, int y, int z, int w, ELEMTYPE voxelvalue)
{
	if(x<0||x>=datasize[0] || y<0||y>=datasize[1] || z<0||z>=datasize[2])
		{cout<<"set_voxel--> strange index... x="<<x<<" y=.... datasize=("<<datasize[0]<<","<<datasize[1]<<","<<datasize[2]<<","<<datasize[3]<<")"<<endl;}

    this->dataptr[x + datasize[0]*y + datasize[0]*datasize[1]*z + datasize[0]*datasize[1]*datasize[2]*w] = voxelvalue;
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::set_voxels(vector<Vector3D> coords, ELEMTYPE voxelvalue)
    {
		for(int i=0;i<coords.size();i++){
			this->set_voxel(coords[i][0],coords[i][1],coords[i][2],voxelvalue);
		}
    }

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::set_voxel_in_physical_pos(Vector3D phys_pos, ELEMTYPE voxelvalue)
{
	Vector3Dint pos = this->get_voxelpos_integers_from_physical_pos_3D(phys_pos);
	this->set_voxel(pos[0],pos[1],pos[2],voxelvalue);
}


template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::add_value_to_voxel(int x, int y, int z, ELEMTYPE value)
{
	//JK - uncomment these rows to detect writing outside allocated memory...
	if(x<0||x>=datasize[0] || y<0||y>=datasize[1] || z<0||z>=datasize[2])
	{cout<<"set_voxel--> strange index... x="<<x<<" y="<<y<<" z="<<z<<"... datasize=("<<datasize[0]<<","<<datasize[1]<<","<<datasize[2]<<")"<<endl;}
	
    this->dataptr[x + datasize[0]*y + datasize[0]*datasize[1]*z] += value;
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::add_value_to_voxel(Vector3D coord_pos, ELEMTYPE value)
{
	this->add_value_to_voxel(coord_pos[0],coord_pos[1],coord_pos[2],value);
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::add_value_to_voxels(vector<Vector3D> coords, ELEMTYPE value)
{
	for(int i=0;i<coords.size();i++){
		this->add_value_to_voxel(coords[i][0],coords[i][1],coords[i][2],value);
	}
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::get_display_voxel(RGBvalue &val,int x, int y, int z) const
    {
//    this->tfunction->get(get_voxel(x, y, z),val); //????
    //val.set_mono(255*(get_voxel (x, y, z)-minvalue)/(maxvalue-minvalue));
    }

template <class ELEMTYPE, int IMAGEDIM>
float image_general<ELEMTYPE, IMAGEDIM>::get_max_float() const
{
	return 0; //JK4 - Function is virtual and result will be delivered by sub-classes
}

template <class ELEMTYPE, int IMAGEDIM>
float image_general<ELEMTYPE, IMAGEDIM>::get_min_float() const
{
	return 0; //JK4 - Function is virtual and result will be delivered by sub-classes
}

template <class ELEMTYPE, int IMAGEDIM>
float image_general<ELEMTYPE, IMAGEDIM>::get_display_min_float() const
{
//	RGBvalue val;
//   this->tfunction->get(this->get_min_float(),val);
//	return float(val.mono());
	return 0;
}

template <class ELEMTYPE, int IMAGEDIM>
float image_general<ELEMTYPE, IMAGEDIM>::get_display_max_float() const
{
//	RGBvalue val;
  //  this->tfunction->get(this->get_max_float(),val);
	//return float(val.mono());
	return 0;
}


template <class ELEMTYPE, int IMAGEDIM>
histogram_1D<ELEMTYPE>* image_general<ELEMTYPE, IMAGEDIM>::get_histogram_from_masked_region_3D(image_binary<3>* mask, int num_buckets)
{
//	cout<<"get_histogram_from_masked_region_3D..."<<endl;
	image_storage<unsigned char>* tmp = dynamic_cast<image_storage<unsigned char>*>(mask);

	histogram_1D<ELEMTYPE> *stats_masked = new histogram_1D<ELEMTYPE>(this, tmp, num_buckets);
	return stats_masked;
}

template <class ELEMTYPE, int IMAGEDIM>
histogram_1D<ELEMTYPE>* image_general<ELEMTYPE, IMAGEDIM>::get_histogram_with_num_buckets(int num_buckets)
{
	histogram_1D<ELEMTYPE> *stats2 = new histogram_1D<ELEMTYPE>(this, num_buckets);
	return stats2;
}


template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_general<ELEMTYPE, IMAGEDIM>::get_voxel_by_dir(int u, int v, int w, int direction)
{	
	if(direction==0)//Loop over x
		return get_voxel(w,u,v);
	if(direction==1)//Loop over y
		return get_voxel(v,w,u);
	return get_voxel(u,v,w); //Loop over z	
}

template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE* image_general<ELEMTYPE, IMAGEDIM>::get_voxel_pointer_by_dir(int u, int v, int w, int direction)
{
	if(direction==0)//Loop over x
		return get_voxel_pointer(w,u,v);
	if(direction==1)//Loop over y
		return get_voxel_pointer(v,w,u);
	return get_voxel_pointer(u,v,w); //Loop over z		
}


template <class ELEMTYPE, int IMAGEDIM>
Vector3Dint image_general<ELEMTYPE, IMAGEDIM>::get_voxel_coord_by_dir(int u, int v, int w, int direction)
{
	if(direction==0)
		return create_Vector3Dint(w,u,v);
	if(direction==1)
		return create_Vector3Dint(v,w,u);
	return create_Vector3Dint(u,v,w);
}


template <class ELEMTYPE, int IMAGEDIM>
double image_general<ELEMTYPE, IMAGEDIM>::get_num_diff_1storder_central_diff_3D(int x, int y, int z, int direction)
{	
	if(direction==0)
		return 0.5*double(get_voxel(x+1,y,z)-get_voxel(x-1,y,z));
	if(direction==1)
		return 0.5*double(get_voxel(x,y+1,z)-get_voxel(x,y-1,z));
	return 0.5*double(get_voxel(x,y,z+1)-get_voxel(x,y,z-1));
}

template <class ELEMTYPE, int IMAGEDIM>
double image_general<ELEMTYPE, IMAGEDIM>::get_num_diff_2ndorder_central_diff_3D(int x, int y, int z, int direction1, int direction2)
{	
	if(direction2==0)
		return 0.5*(get_num_diff_1storder_central_diff_3D(x+1,y,z,direction1)-get_num_diff_1storder_central_diff_3D(x-1,y,z,direction1));
	if(direction2==1)
		return 0.5*(get_num_diff_1storder_central_diff_3D(x,y+1,z,direction1)-get_num_diff_1storder_central_diff_3D(x,y-1,z,direction1));
	return 0.5*(get_num_diff_1storder_central_diff_3D(x,y,z+1,direction1)-get_num_diff_1storder_central_diff_3D(x,y,z-1,direction1));
}

template <class ELEMTYPE, int IMAGEDIM>
double image_general<ELEMTYPE, IMAGEDIM>::get_num_diff_3rdorder_central_diff_3D(int x, int y, int z, int direction1, int direction2, int direction3)
{	
	if(direction3==0)
		return 0.5*(get_num_diff_2ndorder_central_diff_3D(x+1,y,z,direction1,direction2)-get_num_diff_2ndorder_central_diff_3D(x-1,y,z,direction1,direction2));
	if(direction3==1)
		return 0.5*(get_num_diff_2ndorder_central_diff_3D(x,y+1,z,direction1,direction2)-get_num_diff_2ndorder_central_diff_3D(x,y-1,z,direction1,direction2));
	return 0.5*(get_num_diff_2ndorder_central_diff_3D(x,y,z+1,direction1,direction2)-get_num_diff_2ndorder_central_diff_3D(x,y,z-1,direction1,direction2));
}


template <class ELEMTYPE, int IMAGEDIM>
vector<Vector3Dint> image_general<ELEMTYPE, IMAGEDIM>::get_neighbour_voxel_vector_6nbh(int x, int y, int z)
{
	vector<Vector3Dint> neighbours;
	if (x<this->get_size_by_dim(0)-1) neighbours.push_back(create_Vector3Dint(x+1,y,z));
	if (x>0) neighbours.push_back(create_Vector3Dint(x-1,y,z));
	if (y<this->get_size_by_dim(1)-1) neighbours.push_back(create_Vector3Dint(x,y+1,z));
	if (y>0) neighbours.push_back(create_Vector3Dint(x,y-1,z));
	if (z<this->get_size_by_dim(2)-1) neighbours.push_back(create_Vector3Dint(x,y,z+1));
	if (z>0) neighbours.push_back(create_Vector3Dint(x,y,z-1));	
	return neighbours;
}

template <class ELEMTYPE, int IMAGEDIM>
vector<int[3]> image_general<ELEMTYPE, IMAGEDIM>::get_neighbour_voxel_vector_6nbh2(int x, int y, int z)
{
	vector<int[3]> neighbours;
	if (x<this->get_size_by_dim(0)-1){
		int a[3]; a[0]=x+1; a[1]=y; a[2]=z;	neighbours.push_back(a);
	}
	if (x>0){
		int a[3]; a[0]=x-1; a[1]=y; a[2]=z;	neighbours.push_back(a);
	}
	if (y<this->get_size_by_dim(1)-1){
		int a[3]; a[0]=x; a[1]=y+1; a[2]=z;	neighbours.push_back(a);
	}
	if (y>0){
		int a[3]; a[0]=x; a[1]=y-1; a[2]=z;	neighbours.push_back(a);
	}
	if (z<this->get_size_by_dim(2)-1){
		int a[3]; a[0]=x; a[1]=y; a[2]=z+1;	neighbours.push_back(a);
	}
	if (z>0){
		int a[3]; a[0]=x; a[1]=y; a[2]=z-1;	neighbours.push_back(a);
	}
	return neighbours;
}



template <class ELEMTYPE, int IMAGEDIM>
vector<Vector3Dint> image_general<ELEMTYPE, IMAGEDIM>::get_neighbour_voxel_vector_4nbh(int u, int v, int w, int direction)
{
	vector<Vector3Dint> neighbours;
	if (u<this->get_size_by_dim_and_dir(0,direction)-1) neighbours.push_back(this->get_voxel_coord_by_dir(u+1,v,w,direction));
	if (u>0) neighbours.push_back(this->get_voxel_coord_by_dir(u-1,v,w,direction));
	if (v<this->get_size_by_dim_and_dir(1,direction)-1) neighbours.push_back(this->get_voxel_coord_by_dir(u,v+1,w,direction));
	if (v>0) neighbours.push_back(this->get_voxel_coord_by_dir(u,v-1,w,direction));
	return neighbours;
}




template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::set_voxel_by_dir(int u, int v, int w, ELEMTYPE value, int direction)
{
	if(direction==0)//Loop over x
		set_voxel(w,u,v,value);
	else if(direction==1)//Loop over y
		set_voxel(v,w,u,value);
	else
		set_voxel(u,v,w,value);//Loop over z
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::fill_region_3D(int x, int y, int z, int dx, int dy, int dz, ELEMTYPE value)
{
	for (int k=z; k < z+dz; k++){
		for (int j=y; j < y+dy; j++){
			for (int i=x; i < x+dx; i++){
                set_voxel(i,j,k,value);
			}
		}
	}
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::fill_region_3D(Vector3Dint vox_pos, Vector3Dint vox_size, ELEMTYPE value)
{
	this->fill_region_3D(vox_pos[0],vox_pos[1],vox_pos[2],vox_size[0],vox_size[1],vox_size[2], value);
}

//template <class ELEMTYPE, int IMAGEDIM>
//void image_general<ELEMTYPE, IMAGEDIM>::fill_region_3D_with_subvolume_image(image_general<ELEMTYPE, IMAGEDIM> *subvolume)
//{
//	int nx = subvolume->nx();
//	int ny = subvolume->ny();
//	int nz = subvolume->nz();
//	Vector3D phys_pos;
//	for(int z=0; z<nz; z++){
//		for(int y=0; y<ny; y++){
//			for(int x=0; x<nx; x++){
//				phys_pos = subvolume->get_physical_pos_for_voxel(x,y,z);
//				if( this->is_physical_pos_within_image_3D(phys_pos) ){
//					this->set_voxel_in_physical_pos(phys_pos, subvolume->get_voxel(x,y,z));
//				}
//			}
//		}
//	}
//}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::fill_region_3D_with_subvolume_image(image_general<ELEMTYPE, IMAGEDIM> *subvolume)
{
	int nx = subvolume->nx();
	int ny = subvolume->ny();
	int nz = subvolume->nz();
	Vector3D this_begin_phys = this->get_physical_pos_for_voxel(0,0,0);
	Vector3D this_end_phys = this->get_physical_pos_for_voxel(this->nx(),this->ny(),this->nz());
	Vector3Dint this_begin = subvolume->get_voxelpos_integers_from_physical_pos_3D(this_begin_phys);
	Vector3D subvolume_begin_phys = subvolume->get_physical_pos_for_voxel(0,0,0);
	Vector3D subvolume_end_phys = subvolume->get_physical_pos_for_voxel(nx,ny,nz);
	Vector3D begin_phys = create_Vector3D(max(this_begin_phys[0], subvolume_begin_phys[0]), max(this_begin_phys[1], subvolume_begin_phys[1]), max(this_begin_phys[2], subvolume_begin_phys[2]));
	Vector3D end_phys = create_Vector3D(min(this_end_phys[0], subvolume_end_phys[0]), min(this_end_phys[1], subvolume_end_phys[1]), min(this_end_phys[2], subvolume_end_phys[2]));
	Vector3Dint begin = subvolume->get_voxelpos_integers_from_physical_pos_3D(begin_phys);
	Vector3Dint end = subvolume->get_voxelpos_integers_from_physical_pos_3D(end_phys);
	Vector3D phys_pos;
	for(int z=begin[2]; z<end[2]; z++){
		for(int y=begin[1]; y<end[1]; y++){
			for(int x=begin[0]; x<end[0]; x++){
				//phys_pos = subvolume->get_physical_pos_for_voxel(x,y,z);
				//this->set_voxel_in_physical_pos(phys_pos, subvolume->get_voxel(x,y,z));
				this->set_voxel(x-this_begin[0],y-this_begin[1],z-this_begin[2], subvolume->get_voxel(x,y,z));
			}
		}
	}
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::fill_region_3D_with_subvolume_image(Vector3Dint to_pos, image_general<ELEMTYPE, IMAGEDIM> *im, Vector3Dint from_pos, Vector3Dint from_size, ELEMTYPE empty_value)
{
	//this->print_geometry();
	int xx;
	int yy;
	int zz = to_pos[2];
	for(int z=from_pos[2]; z< (from_pos[2] + from_size[2]); z++){
		yy = to_pos[1];
		for(int y=from_pos[1]; y< (from_pos[1] + from_size[1]); y++){
			xx = to_pos[0];
			for(int x=from_pos[0]; x< (from_pos[0] + from_size[0]); x++){
				if(this->is_voxelpos_within_image_3D(xx,yy,zz)){
					if(im->is_voxelpos_within_image_3D(x,y,z)){
						this->set_voxel( xx,yy,zz, im->get_voxel(x,y,z) );
					}else{
						this->set_voxel( xx,yy,zz, empty_value );
					}
				}
				xx++;
			}
			yy++;
		}
		zz++;
	}
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::fill_slice_wise_grid(float grid_res_mm, ELEMTYPE value, int dir)
{
//	this->get_phys_span_in_dir(
	for(int z=0; z<this->get_size_by_dim_and_dir(2,dir); z++){

	}
}


template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::fill_region_3D(int dir, int start_index, int end_index, ELEMTYPE value)
{
	switch(dir){
	case 0:
		if(start_index<0 || start_index>=this->get_size_by_dim(0) || end_index<0 || end_index>=this->get_size_by_dim(0))
			{pt_error::error("fill_region_3D--> erroneous index...",pt_error::debug);}
		
		for(int x=start_index; x<=end_index; x++){
			for(int z=0; z<this->datasize[2]; z++){
				for(int y=0; y<this->datasize[1]; y++){
					this->set_voxel(x,y,z,value);
				}
			}
		}
		break;

	case 1:
		if(start_index<0 || start_index>=this->get_size_by_dim(1) || end_index<0 || end_index>=this->get_size_by_dim(1))
			{pt_error::error("fill_region_3D--> erroneous index...",pt_error::debug);}
		
		for(int y=start_index; y<=end_index; y++){
			for(int z=0; z<this->datasize[2]; z++){
				for(int x=0; x<this->datasize[0]; x++){
					this->set_voxel(x,y,z,value);
				}
			}
		}
		break;

	case 2:
		if(start_index<0 || start_index>=this->get_size_by_dim(2) || end_index<0 || end_index>=this->get_size_by_dim(2))
			{pt_error::error("fill_region_3D--> erroneous index...",pt_error::debug);}
		
		for(int z=start_index; z<=end_index; z++){
			for(int y=0; y<this->datasize[1]; y++){
				for(int x=0; x<this->datasize[0]; x++){
					this->set_voxel(x,y,z,value);
				}
			}
		}
		break;

	default:
		break;
	}
}
		


template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::fill_region_of_mask_3D(image_binary<IMAGEDIM> *mask, ELEMTYPE value)
{
	if(this->same_size(mask)){
		for (int k=0; k<this->datasize[2]; k++){
			for (int j=0; j<this->datasize[1]; j++){
				for (int i=0; i<this->datasize[0]; i++){
					if(mask->get_voxel(i,j,k)>0){
						this->set_voxel(i,j,k,value);
					}
				}
			}
		}
	}else{
		pt_error::error("image_general<ELEMTYPE, IMAGEDIM>::fill_region_of_mask_3D - images do not have same size", pt_error::debug);
	}
}


template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::fill_region_3D_around_voxels(vector<Vector3D> coords, int rx, int ry, int rz, ELEMTYPE val)
{
	int x1, y1, z1, dx, dy, dz;
	for(int i=0;i<coords.size();i++){
		if(coords[i][2]>0){
			x1 = max(0,int(coords[i][0])-rx);
			y1 = max(0,int(coords[i][1])-ry);
			z1 = max(0,int(coords[i][2])-rz);
			dx = min(int(this->nx()),int(2*rx+1));
			dy = min(int(this->ny()),int(2*ry+1));
			dz = min(int(this->nz()),int(2*rz+1));
			this->fill_region_3D(x1,y1,z1,dx,dy,dz,val);
		}
	}
}



template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::combine_with_offset(image_general<ELEMTYPE, IMAGEDIM> *const image2, COMBINE_MODE mode, Vector3Dint to_pos){

	//enum COMBINE_MODE {COMB_ADD, COMB_SUB, COMB_MULT, COMB_DIV, COMB_MAX, COMB_MIN, COMB_MEAN_NON_ZERO, COMB_MAGN}; //and, xor, mask...

	int xx; //destination voxel pos....
	int yy;

	int zz = to_pos[2];
	for(int z=0; z<image2->nz(); z++){
		yy = to_pos[1];
		for(int y=0; y<image2->ny(); y++){
			xx = to_pos[0];
			for(int x=0; x<image2->nx(); x++){
				if(this->is_voxelpos_within_image_3D(xx,yy,zz)){
					if(mode==COMB_ADD){
						this->set_voxel( xx,yy,zz, this->get_voxel(xx,yy,zz) + image2->get_voxel(x,y,z) );
					}//TODO more COMB-options...
					else if(mode==COMB_SUB){
						this->set_voxel( xx,yy,zz, this->get_voxel(xx,yy,zz) - image2->get_voxel(x,y,z) );
					}
					else if(mode==COMB_MULT){
						this->set_voxel( xx,yy,zz, this->get_voxel(xx,yy,zz) * image2->get_voxel(x,y,z) );
					}
					else if(mode==COMB_DIV){
						this->set_voxel( xx,yy,zz, this->get_voxel(xx,yy,zz) / image2->get_voxel(x,y,z) );
					}
					//MAX
					//MIN
					else if(mode==COMB_MEAN_NON_ZERO){
						if( this->get_voxel(xx,yy,zz) == 0){
							this->set_voxel( xx,yy,zz, image2->get_voxel(x,y,z) );
						}
						else if( image2->get_voxel(x,y,z) != 0){	//this voxel in NOt zero... nor is image2´s voxel...
							this->set_voxel( xx,yy,zz, (this->get_voxel(xx,yy,zz) + image2->get_voxel(x,y,z))/2.0 );
						}
						//else keep this voxel unchanged...
					}
					//MAGN
					
				}
				xx++;
			}
			yy++;
		}
		zz++;
	}
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::fill_image_border_3D(ELEMTYPE value, int border_thickness)
{
	for(int dim=0;dim<=2;dim++){
		this->fill_region_3D(dim,0,border_thickness-1, value);								//low x/y/z-values
		this->fill_region_3D(dim,datasize[dim]-border_thickness,datasize[dim]-1, value);	//high x/y/z-values
	}
}
template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::translate_subvolume_3D(Vector3Dint pos, Vector3Dint size, Vector3Dint T, ELEMTYPE empty_value)
{
	image_general<ELEMTYPE, IMAGEDIM> *tmp = new image_general<ELEMTYPE, IMAGEDIM>(this);
	tmp->fill_region_3D(pos, size, empty_value);

	tmp->fill_region_3D_with_subvolume_image(pos+T, this, pos, size, empty_value); //based on given voxel coords
/*	int xx,yy,zz;
	for(int z=pos[2]; z<pos[2]+size[2]; z++){
		for(int y=pos[1]; y<pos[1]+size[1]; y++){
			for(int x=pos[0]; x<pos[0]+size[0]; x++){
				if(this->is_voxelpos_within_image_3D(x,y,z)){
					xx=x+T[0];
					yy=y+T[1];
					zz=z+T[2];
					if(tmp->is_voxelpos_within_image_3D(xx,yy,zz)){
						tmp->set_voxel(xx,yy,zz, this->get_voxel(x,y,z));
					}

				}

			}
		}
	}
*/
	copy_data(tmp,this);
	delete tmp;
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::translate_slice_3D(int dir, int slice, int du, int dv, ELEMTYPE empty_value)
{
	if(dir==0){
		this->translate_subvolume_3D( create_Vector3Dint(slice,0,0), create_Vector3Dint(1,this->ny(),this->nz()), create_Vector3Dint(0,du,dv));
	}else if(dir==1){
		this->translate_subvolume_3D( create_Vector3Dint(0,slice,0), create_Vector3Dint(this->nx(),1,this->nz()), create_Vector3Dint(du,0,dv));
	}else{
		this->translate_subvolume_3D( create_Vector3Dint(0,0,slice), create_Vector3Dint(this->nx(),this->ny(),1), create_Vector3Dint(du,dv,0));
	}
}


template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::testpattern()
    {
    //gradient test pattern
    for (int z=0; z < datasize[2]; z++)
        for (int y=0; y < datasize[1]; y++)
            for (int x=0; x < datasize[0]; x++)
                {
                set_voxel(x,y,z, int(float(x+y+z)*255.0/float(datasize[2]+datasize[1]+datasize[0])));
                }

	this->data_has_changed(true);
    }

template <class ELEMTYPE, int IMAGEDIM>
template<class TARGETTYPE> 
void image_general<ELEMTYPE, IMAGEDIM>::resample_into_this_image_NN(image_general<TARGETTYPE, 3> * new_image)
{
	//std::cout<<"image_general::resample_into_this_image_NN"<<std::endl;
//	this->print_geometry();
//	new_image->print_geometry();

	Vector3D vox_pos;
	Vector3D phys_pos;
	float res;
    for (int z=0; z < new_image->datasize[2]; z++)
        {
        for (int y=0; y < new_image->datasize[1]; y++)
            {
            for (int x=0; x < new_image->datasize[0]; x++)
                {  
				vox_pos[0] = x;	vox_pos[1] = y;	vox_pos[2] = z;
				phys_pos = new_image->origin + new_image->get_voxel_resize()*new_image->orientation*vox_pos;
//				cout<<"phys_pos="<<phys_pos<<std::endl;
//				res = get_voxel_in_physical_pos_26NB_weighted(phys_pos,10,2,0,0);
//				res = get_voxel_in_physical_pos_mean_3D_interp26(phys_pos);
				res = get_voxel_in_physical_pos(phys_pos);
//				cout<<"res="<<res<<std::endl;
				new_image->set_voxel(x,y,z,res);
                }
            }
        }
}

/*
template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::set_parameters(itk::SmartPointer< itk::OrientedImage<ELEMTYPE, IMAGEDIM > > &i)
{
    typename itk::OrientedImage<ELEMTYPE,IMAGEDIM>::SpacingType		itk_vox_size = i->GetSpacing(); 
    typename itk::OrientedImage<ELEMTYPE,IMAGEDIM>::PointType       itk_origin = i->GetOrigin();
    typename itk::OrientedImage<ELEMTYPE,IMAGEDIM>::DirectionType   itk_orientation = i->GetDirection();

    for(unsigned int d=0;d<IMAGEDIM;d++){
        if(itk_vox_size[d] > 0){
			voxel_size[d]=itk_vox_size[d];
		}

		this->origin[d]=itk_origin[d];

        for(unsigned int c=0;c<3;c++){
			this->orientation[d][c]=itk_orientation[d][c];
		}
	}
    
    if(voxel_size[0] * voxel_size[1] * voxel_size[2] == 0){
		voxel_size.Fill(1); 
	}

//	this->print_geometry(); //JK
    calc_transforms();

	typename theStatsFilterType::Pointer statsFilter = theStatsFilterType::New();
    statsFilter->SetInput(i);
    statsFilter->Update();
    this->set_max(statsFilter->GetMaximum());
    this->set_min(statsFilter->GetMinimum());
//    this->stats->min(statsFilter->GetMinimum());
//    this->stats->max(statsFilter->GetMaximum());

//    ELEMTYPE new_max  = statsFilter->GetMaximum();
    //we don't want to lose pixel-data correspondence by scaling chars,
    //don't alter max/min in that case
    //note: scaling is for display only

    //we *could* scale dynamically narrow 8-bit images as well,
    //but it's a question if what the viewer expects
    //images with just a few classes (20) starting with 0
    //(eg. binary) still get their scaling

//    if (new_max - this->get_min() > 255 || (this->get_min() ==0 && new_max - this->get_min() < 20) ){
//        //this->maxvalue=new_max;
//		this->stats->max(new_max);
//	}else{
        //this->maxvalue = this->minvalue + 255;
//		this->stats->max(this->stats->min() + 255);
//	}

}
*/
/*
template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::set_parameters(itk::SmartPointer< itk::OrientedImage<std::complex<ELEMTYPE>, IMAGEDIM > > &i)
{
    typename itk::OrientedImage<std::complex<ELEMTYPE>,IMAGEDIM>::SpacingType		itk_vox_size = i->GetSpacing(); 
    typename itk::OrientedImage<std::complex<ELEMTYPE>,IMAGEDIM>::PointType       itk_origin = i->GetOrigin();
    typename itk::OrientedImage<std::complex<ELEMTYPE>,IMAGEDIM>::DirectionType   itk_orientation = i->GetDirection();

    for(unsigned int d=0;d<IMAGEDIM;d++){
        if(itk_vox_size[d] > 0){
			voxel_size[d]=itk_vox_size[d];
		}

		this->origin[d]=itk_origin[d];

        for(unsigned int c=0;c<3;c++){
			this->orientation[d][c]=itk_orientation[d][c];
		}
	}
    
    if(voxel_size[0] * voxel_size[1] * voxel_size[2] == 0){
		voxel_size.Fill(1); 
	}

//	this->print_geometry(); //JK
    calc_transforms();

	typename theComplexStatsFilterType::Pointer statsFilter = theComplexStatsFilterType::New();
    statsFilter->SetInput(i);
    statsFilter->Update();
    this->set_max(statsFilter->GetMaximum());
    this->set_min(statsFilter->GetMinimum());
}
*/

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::set_image_geometry(float ox,float oy,float oz,float dx,float dy,float dz,float fi_x,float fi_y,float fi_z)
{
	this->origin[0]=ox;
	this->origin[1]=oy;
	this->origin[2]=oz;
	this->set_voxel_size(dx,dy,dz);
	this->orientation.SetIdentity();
	this->rotate_orientation(fi_z,fi_y,fi_x);
}

template <class ELEMTYPE, int IMAGEDIM>
template <class sourceType>
void image_general<ELEMTYPE, IMAGEDIM>::set_image_geometry (image_general<sourceType, IMAGEDIM> *ref_im)
{
	this->set_origin(ref_im->get_origin());
	this->set_voxel_size(ref_im->get_voxel_size());
	this->set_orientation(ref_im->get_orientation());
	this->set_slice_orientation(ref_im->get_slice_orientation());
}

template <class ELEMTYPE, int IMAGEDIM>
template <class sourceType>
void image_general<ELEMTYPE, IMAGEDIM>::set_image_geometry_with_different_resolution(image_general<sourceType, IMAGEDIM> *ref_im, Vector3D new_voxel_size)
{
	cout<<"set_image_geometry_with_different_resolution..."<<endl;

	this->set_image_geometry(ref_im);
	cout<<"old_voxel_size="<<this->get_voxel_size()<<endl;
	cout<<"new_voxel_size="<<new_voxel_size<<endl;

	this->initialize_dataset( this->nx()*this->get_voxel_size()[0]/new_voxel_size[0], this->ny()*this->get_voxel_size()[1]/new_voxel_size[1], this->nz()*this->get_voxel_size()[2]/new_voxel_size[2] );
	this->fill(50);
	this->set_voxel_size(new_voxel_size);
}



template <class ELEMTYPE, int IMAGEDIM>
bool image_general<ELEMTYPE, IMAGEDIM>::read_geometry_from_dicom_file(std::string dcm_file)
{
	//std::cout<<"read_geometry_from_dicom_file"<<std::endl;
	bool b1 = this->read_origin_from_dicom_file(dcm_file);
	bool b2 = this->read_orientation_from_dicom_file(dcm_file);
	bool b3 = this->read_voxel_size_from_dicom_file(dcm_file);
	return b1&&b2&&b3;
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::print_geometry()
{
	std::cout<<"*************************************"<<std::endl;
	std::cout<< this->name()<<"\n->print_geometry() datasize: ("<<datasize[0]<<","<<datasize[1]<<","<<datasize[2]<<")"<<endl;
	std::cout<<"num_elements:"<<this->num_elements<<std::endl;
	std::cout<<"origin:"<<this->origin<<std::endl;
	std::cout<<"voxel_size:"<<this->get_voxel_size()<<std::endl;
	std::cout<<"voxel_resize:"<<std::endl<<this->get_voxel_resize()<<std::endl;
	std::cout<<"slice_orientation:"<<std::endl<<this->get_slice_orientation()<<std::endl;
	std::cout<<"orientation:"<<std::endl;
	std::cout<<this->orientation[0][0]<<" "<<this->orientation[1][0]<<" "<<this->orientation[2][0]<<std::endl;
	std::cout<<this->orientation[0][1]<<" "<<this->orientation[1][1]<<" "<<this->orientation[2][1]<<std::endl;
	std::cout<<this->orientation[0][2]<<" "<<this->orientation[1][2]<<" "<<this->orientation[2][2]<<std::endl;
	std::cout<<"physical_center:"<<this->get_physical_center()<<std::endl;
	std::cout<<"*************************************"<<std::endl;
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::print_physical_corner_coords()
{
	std::cout<<"***************************************"<<std::endl;
	std::cout<<this->name()<<"->print_physical_corner_coords()"<<std::endl;
	std::cout<<		get_physical_pos_for_voxel(0,				0,				datasize[2]-1);
	std::cout<<" "<<	get_physical_pos_for_voxel(datasize[0]-1,	0,				datasize[2]-1);
	std::cout<<std::endl;
	std::cout<<		get_physical_pos_for_voxel(0,				datasize[1]-1,	datasize[2]-1);
	std::cout<<" "<<	get_physical_pos_for_voxel(datasize[0]-1,	datasize[1]-1,	datasize[2]-1);
	std::cout<<std::endl;
	std::cout<<std::endl;
	std::cout<<		get_physical_pos_for_voxel(0,				0,				0);
	std::cout<<" "<<	get_physical_pos_for_voxel(datasize[0]-1,	0,				0);
	std::cout<<std::endl;
	std::cout<<		get_physical_pos_for_voxel(0,				datasize[1]-1,	0);
	std::cout<<" "<<	get_physical_pos_for_voxel(datasize[0]-1,	datasize[1]-1,	0);
	std::cout<<std::endl;
}


#endif