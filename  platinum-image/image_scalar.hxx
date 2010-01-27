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

#ifndef __image_scalar_hxx__
#define __image_scalar_hxx__

#include "image_scalar.h"
#include "image_general.hxx"

#include "userIOmanager.h"
#include "datamanager.h"

extern userIOmanager userIOmanagement;
extern datamanager datamanagement;

#define theScalarImageType itk::OrientedImage<ELEMTYPE,IMAGEDIM>
#define theScalarReaderType itk::ImageFileReader<theScalarImageType >

template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_scalar<ELEMTYPE, IMAGEDIM>::get_max() const
{
    return stats->max();
}

template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_scalar<ELEMTYPE, IMAGEDIM>::get_min() const
{
    return stats->min();
}
/*
template <class ELEMTYPE, int IMAGEDIM>
float image_scalar<ELEMTYPE, IMAGEDIM>::get_mean_intensity()
{
    return stats->get_mean_intensity(); //does not do what we want it too.... yet...
}
*/

template <class ELEMTYPE, int IMAGEDIM>
template <class sourceType>
void image_scalar<ELEMTYPE, IMAGEDIM>::set_parameters(image_general<sourceType, IMAGEDIM> *sourceImage)
{
	image_general<ELEMTYPE, IMAGEDIM>::set_parameters(sourceImage);
}


template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_scalar<ELEMTYPE, IMAGEDIM>::get_num_values()
{ 
	return stats->num_values(); 
}

template <class ELEMTYPE, int IMAGEDIM>
histogram_1D<ELEMTYPE> * image_scalar<ELEMTYPE, IMAGEDIM>::get_histogram()
{
	pt_error::error_if_null(stats,"Trying to get_histogram() which is NULL");
	return stats;
}

template <class ELEMTYPE, int IMAGEDIM>
histogram_1D<ELEMTYPE> *image_scalar<ELEMTYPE, IMAGEDIM>::get_histogram_new_with_same_num_buckets_as_intensities()
{
	ELEMTYPE minimum;
	ELEMTYPE maximum;
	this->get_min_max_values(minimum, maximum);
	return new histogram_1D<ELEMTYPE>(this,int(maximum-minimum));
}

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::stats_refresh(bool min_max_refresh)
    {
	if(min_max_refresh){
		this->min_max_refresh();
	}
    stats->calculate_from_image_data(); //when called without argument (=0) the histogram "resolution" is kept..

    }

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::min_max_refresh()
{
    ELEMTYPE minimum, maximum;
	get_min_max_values(minimum, maximum);
   
    //don't change if values don't make sense - 
    //that would be an empty/zero image
    if (minimum < maximum)
        {
        this->stats->min(minimum);
        this->stats->max(maximum);
        }
}


//JK - I have not managed to specialize this function for "complex<ELEMTYPE>" - I think the whole class needs to be rewritten for "complex<ELEMTYPE>"
template <class ELEMTYPE, int IMAGEDIM>
float image_scalar<ELEMTYPE, IMAGEDIM>::get_number_voxel(int x, int y, int z) const
{
    return static_cast<float>(this->get_voxel(x, y, z)); //JK4
}

template <class ELEMTYPE, int IMAGEDIM>
float image_scalar<ELEMTYPE, IMAGEDIM>::get_max_float() const
{
	return float(stats->max()); //JK4
}

template <class ELEMTYPE, int IMAGEDIM>
float image_scalar<ELEMTYPE, IMAGEDIM>::get_max_float_safe() const
{
	if(stats!=NULL){
		return float(stats->max()); //JK4
	}
	return 1000;
}


template <class ELEMTYPE, int IMAGEDIM>
float image_scalar<ELEMTYPE, IMAGEDIM>::get_min_float() const
{
	return float(stats->min()); //JK4
}

template <class ELEMTYPE, int IMAGEDIM>
float image_scalar<ELEMTYPE, IMAGEDIM>::get_min_float_safe() const
{
	if(stats!=NULL){
		return float(stats->min()); //JK4
	}
	return 0;
}

template <class ELEMTYPE, int IMAGEDIM>
float image_scalar<ELEMTYPE, IMAGEDIM>::get_display_min_float() const
{
	RGBvalue val;
    this->tfunction->get(this->get_min_float(),val);
	return float(val.mono());
}

template <class ELEMTYPE, int IMAGEDIM>
float image_scalar<ELEMTYPE, IMAGEDIM>::get_display_max_float() const
{
	RGBvalue val;
    this->tfunction->get(this->get_max_float(),val);
	return float(val.mono());
}

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::get_display_voxel(RGBvalue &val,int x, int y, int z) const
{
	if(this->tfunction!=NULL){
		this->tfunction->get(this->get_voxel(x, y, z),val);
	}else{
		cout<<"*this->tfunction==NULL*";
	}
}


template <class ELEMTYPE, int IMAGEDIM>
string image_scalar<ELEMTYPE, IMAGEDIM>::resolve_tooltip()
{
	return "image_scalar\n" + resolve_tooltip_image_scalar();
}

template <class ELEMTYPE, int IMAGEDIM>
string image_scalar<ELEMTYPE, IMAGEDIM>::resolve_tooltip_image_scalar()
{
	return this->resolve_tooltip_image_general() + "\n";
}

template <class ELEMTYPE, int IMAGEDIM>
image_scalar<ELEMTYPE, IMAGEDIM>::~image_scalar()
    {
    if (tfunction != NULL)
	    delete tfunction; //????

    if (stats != NULL)
        { delete stats; }
    }

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::set_scalar_parameters()
    {
	stats = NULL;
	set_stats_histogram(new histogram_1D<ELEMTYPE >(this));  //hist1D constructor calls resize()... and calculate()
	
	ELEMTYPE mi = stats->min();
	ELEMTYPE ma = stats->max();
//    stats->min(std::numeric_limits<ELEMTYPE>::min());
//    stats->max(std::numeric_limits<ELEMTYPE>::max()); //JK ????

    tfunction = NULL;

	transfer_function();
    }

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::set_stats_histogram(histogram_1D<ELEMTYPE > * h)
    {
    if (stats != NULL)
        {
        delete stats;
        }

    stats = h;
    }

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::transfer_function(transfer_scalar_base<ELEMTYPE> * t)
    {
    if (t == NULL) //default
        { 
        if (tfunction == NULL)
            {tfunction = new TFUNCTIONTEST<ELEMTYPE >(this); }
        //else, do nothing since a transfer function was assigned elsewhere
        }
    else
        { 
        if (tfunction != NULL)
            {delete tfunction;}

        tfunction = t;
        }
    }

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::transfer_function(std::string functionName)
    {
		transfer_scalar_base<ELEMTYPE > *t = transfer_manufactured::factory.Create<ELEMTYPE> (functionName,this);
		this->transfer_function(t); //JK TODO ???? quick fix...
    }

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::set_parameters(itk::SmartPointer< itk::OrientedImage<ELEMTYPE, IMAGEDIM > > &i)
{
    typename itk::OrientedImage<ELEMTYPE,IMAGEDIM>::SpacingType		itk_vox_size = i->GetSpacing(); 
    typename itk::OrientedImage<ELEMTYPE,IMAGEDIM>::PointType       itk_origin = i->GetOrigin();
    typename itk::OrientedImage<ELEMTYPE,IMAGEDIM>::DirectionType   itk_orientation = i->GetDirection();

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

	typename theStatsFilterType::Pointer statsFilter = theStatsFilterType::New();
    statsFilter->SetInput(i);
    statsFilter->Update();
    this->set_max(statsFilter->GetMaximum());
    this->set_min(statsFilter->GetMinimum());
	
}


/*
template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::getVTKImagePointer()
{
	typename theImageType::Pointer image = get_image_as_itk_output();
	cout<<"dir="<<endl<<image->GetDirection()<<endl;

	//------------------------------
	//------------------------------

	try {
//    typedef itk::RGBPixel< unsigned char > PixelType;
    typedef itk::Image< unsigned char, 3 > ImageType;
    typedef itk::ImageFileReader< ImageType > ReaderType;

    ReaderType::Pointer reader  = ReaderType::New();
	reader->SetFileName( "D:/Joel/TMP/brain8bit.vtk" );
    reader->Update();

    typedef itk::VTKImageExport< ImageType > ExportFilterType;
    ExportFilterType::Pointer itkExporter = ExportFilterType::New();

    itkExporter->SetInput( reader->GetOutput() );

    // Create the vtkImageImport and connect it to the
    // itk::VTKImageExport instance.
    vtkImageImport* vtkImporter = vtkImageImport::New();  
    ConnectPipelines(itkExporter, vtkImporter);
    

    // Just for double checking export it from VTK back into ITK 
    // and save it into a file.
    typedef itk::VTKImageImport< ImageType > ImportFilterType;
    ImportFilterType::Pointer itkImporter = ImportFilterType::New();


    vtkImageExport* vtkExporter = vtkImageExport::New();  
    ConnectPipelines(vtkExporter, itkImporter);
    
    vtkExporter->SetInput( vtkImporter->GetOutput() );
    
    typedef itk::ImageFileWriter< ImageType > WriterType;
    WriterType::Pointer itkWriter = WriterType::New();
    itkWriter->SetInput( itkImporter->GetOutput() );
    
     std::cout << "Writing file... " << std::endl;
	 itkWriter->SetFileName( "D:/Joel/TMP/brain8bit2.vtk" );
     itkWriter->Update();

    //------------------------------------------------------------------------
    // VTK pipeline.
    //------------------------------------------------------------------------
    
    // Create a vtkImageActor to help render the image.  Connect it to
    // the vtkImporter instance.
    vtkImageActor* actor = vtkImageActor::New();
    actor->SetInput(vtkImporter->GetOutput());
    
    vtkInteractorStyleImage * interactorStyle = vtkInteractorStyleImage::New();

    // Create a renderer, render window, and render window interactor to
    // display the results.
    vtkRenderer* renderer = vtkRenderer::New();
    vtkRenderWindow* renWin = vtkRenderWindow::New();
    vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();
    
    renWin->SetSize(500, 500);
    renWin->AddRenderer(renderer);
    iren->SetRenderWindow(renWin);
    iren->SetInteractorStyle( interactorStyle );
    
    // Add the vtkImageActor to the renderer for display.
    renderer->AddActor(actor);
    renderer->SetBackground(0.4392, 0.5020, 0.5647);

    // Bring up the render window and begin interaction.
    renWin->Render();
    iren->Start();

    // Release all VTK components
    actor->Delete();
    interactorStyle->Delete(); 
    vtkImporter->Delete();
    vtkExporter->Delete();
    renWin->Delete();
    renderer->Delete();
    iren->Delete();

    }
  catch( itk::ExceptionObject & e )
    {
    std::cerr << "Exception catched !! " << e << std::endl;
    }

}

*/

template <class ELEMTYPE, int IMAGEDIM>
vtkImageData* image_scalar<ELEMTYPE, IMAGEDIM>::getVTKImagePointer()
{
	typename theImageType::Pointer image = this->get_image_as_itk_output();
	cout<<"dir="<<endl<<image->GetDirection()<<endl;

	//------------------------------
	//------------------------------
    // Create the vtkImageImport and connect it to the itk::VTKImageExport instance.
    vtkImageImport* vtkImporter = vtkImageImport::New();  

//	try {

    typedef itk::VTKImageExport< theImageType > ExportFilterType;
    typename ExportFilterType::Pointer itkExporter = ExportFilterType::New();
    itkExporter->SetInput( image );

//    ConnectPipelines(itkExporter, vtkImporter);
	vtkImporter->SetUpdateInformationCallback(itkExporter->GetUpdateInformationCallback());
	vtkImporter->SetPipelineModifiedCallback(itkExporter->GetPipelineModifiedCallback());
	vtkImporter->SetWholeExtentCallback(itkExporter->GetWholeExtentCallback());
	vtkImporter->SetSpacingCallback(itkExporter->GetSpacingCallback());
	vtkImporter->SetOriginCallback(itkExporter->GetOriginCallback());
	vtkImporter->SetScalarTypeCallback(itkExporter->GetScalarTypeCallback());
	vtkImporter->SetNumberOfComponentsCallback(itkExporter->GetNumberOfComponentsCallback());
	vtkImporter->SetPropagateUpdateExtentCallback(itkExporter->GetPropagateUpdateExtentCallback());
	vtkImporter->SetUpdateDataCallback(itkExporter->GetUpdateDataCallback());
	vtkImporter->SetDataExtentCallback(itkExporter->GetDataExtentCallback());
	vtkImporter->SetBufferPointerCallback(itkExporter->GetBufferPointerCallback());
	vtkImporter->SetCallbackUserData(itkExporter->GetCallbackUserData());


/*    // Just for double checking export it from VTK back into ITK 
    // and save it into a file.
    typedef itk::VTKImageImport< theImageType > ImportFilterType;
    ImportFilterType::Pointer itkImporter = ImportFilterType::New();

    vtkImageExport* vtkExporter = vtkImageExport::New();  
    ConnectPipelines(vtkExporter, itkImporter);
    
    vtkExporter->SetInput( vtkImporter->GetOutput() );
    
    typedef itk::ImageFileWriter< theImageType > WriterType;
    WriterType::Pointer itkWriter = WriterType::New();
    itkWriter->SetInput( itkImporter->GetOutput() );
    
     std::cout << "Writing file... " << std::endl;
	 itkWriter->SetFileName( "D:/Joel/TMP/brain8bit5.vtk" );
     itkWriter->Update();

    //------------------------------------------------------------------------
    // VTK pipeline.
    //------------------------------------------------------------------------
    
    // Create a vtkImageActor to help render the image.  Connect it to
    // the vtkImporter instance.
    vtkImageActor* actor = vtkImageActor::New();
    actor->SetInput(vtkImporter->GetOutput());
    
    vtkInteractorStyleImage * interactorStyle = vtkInteractorStyleImage::New();

    // Create a renderer, render window, and render window interactor to
    // display the results.
    vtkRenderer* renderer = vtkRenderer::New();
    vtkRenderWindow* renWin = vtkRenderWindow::New();
    vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();
    
    renWin->SetSize(500, 500);
    renWin->AddRenderer(renderer);
    iren->SetRenderWindow(renWin);
    iren->SetInteractorStyle( interactorStyle );
    
    // Add the vtkImageActor to the renderer for display.
    renderer->AddActor(actor);
    renderer->SetBackground(0.4392, 0.5020, 0.5647);

    // Bring up the render window and begin interaction.
    renWin->Render();
    iren->Start();

    // Release all VTK components
    actor->Delete();
    interactorStyle->Delete(); 
    vtkImporter->Delete();
    vtkExporter->Delete();
    renWin->Delete();
    renderer->Delete();
    iren->Delete();
*/



//    }catch( itk::ExceptionObject & e ){
//    std::cerr << "Exception catched !! " << e << std::endl;
//    }



//		vtkImporter->Delete();
//		vtkExporter->Delete();

	vtkImporter->Update();

//---------------------------------------------

//---------------------------------------------

//	this->writeVTKImagePointer(vtkImporter->GetOutput());
//	vtkImporter->GetOutputPort(
	return vtkImporter->GetOutput();
}


template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::writeVTKImagePointer(vtkImageData *im)
{	
			vtkImageExport* vtkExporter = vtkImageExport::New();  
//			vtkExporter->SetInput( vtkImporter->GetOutput() );
//			vtkExporter->SetInput( f->getVTKImagePointer() );
//			vtkImageData *im = f->getVTKImagePointer();
			vtkExporter->SetInput( im );

	
			typedef itk::VTKImageImport< itk::OrientedImage<unsigned char,3> > ImportFilterType;
			ImportFilterType::Pointer itkImporter = ImportFilterType::New();

//			ConnectPipelines(vtkExporter, itkImporter);
			itkImporter->SetUpdateInformationCallback(vtkExporter->GetUpdateInformationCallback());
			itkImporter->SetPipelineModifiedCallback(vtkExporter->GetPipelineModifiedCallback());
			itkImporter->SetWholeExtentCallback(vtkExporter->GetWholeExtentCallback());
			itkImporter->SetSpacingCallback(vtkExporter->GetSpacingCallback());
			itkImporter->SetOriginCallback(vtkExporter->GetOriginCallback());
			itkImporter->SetScalarTypeCallback(vtkExporter->GetScalarTypeCallback());
			itkImporter->SetNumberOfComponentsCallback(vtkExporter->GetNumberOfComponentsCallback());
			itkImporter->SetPropagateUpdateExtentCallback(vtkExporter->GetPropagateUpdateExtentCallback());
			itkImporter->SetUpdateDataCallback(vtkExporter->GetUpdateDataCallback());
			itkImporter->SetDataExtentCallback(vtkExporter->GetDataExtentCallback());
			itkImporter->SetBufferPointerCallback(vtkExporter->GetBufferPointerCallback());
			itkImporter->SetCallbackUserData(vtkExporter->GetCallbackUserData());

				    
			typedef itk::ImageFileWriter< itk::OrientedImage<unsigned char,3> > WriterType;
			WriterType::Pointer itkWriter = WriterType::New();
			itkWriter->SetInput( itkImporter->GetOutput() );
			std::cout << "Writing file... " << std::endl;
			itkWriter->SetFileName( "D:/Joel/TMP/brain8bit55.vtk" );
			itkWriter->Update();
}

template <class ELEMTYPE, int IMAGEDIM>
vtkAlgorithmOutput* image_scalar<ELEMTYPE, IMAGEDIM>::getvtkStructuredPoints()
{	
	this->save_to_file("tmp_getvtkStructuredPoints.vtk");
	vtkStructuredPointsReader *reader = vtkStructuredPointsReader::New();
	reader->SetFileName("tmp_getvtkStructuredPoints.vtk");
	reader->Update();

	vtkImageCast *imageCast = vtkImageCast::New();
	//imageCast->SetInput(reader->GetOutput());  
	imageCast->SetInputConnection(reader->GetOutputPort()); //SO
	imageCast->SetOutputScalarTypeToUnsignedShort();

	//return reader->GetOutputPort();
	return imageCast->GetOutputPort();
}



template <class ELEMTYPE, int IMAGEDIM>
image_scalar<double,3>* image_scalar<ELEMTYPE, IMAGEDIM>::get_num_diff_image_1storder_central_diff_3D(int direction)
{	
	image_scalar<double,3> *res = new image_scalar<double,3>(this);
	if(direction==0){
		for (int k=0; k < this->datasize[2]; k++){
			for (int j=0; j < this->datasize[1]; j++){
				res->set_voxel(0,j,k, double(this->get_voxel(1,j,k)-this->get_voxel(0,j,k)));				//forward diff
				for (int i=1; i < this->datasize[0]-1; i++){
					res->set_voxel(i,j,k,0.5* double(this->get_voxel(i+1,j,k)-this->get_voxel(i-1,j,k)));	//central diff
				}
				res->set_voxel(this->datasize[0]-1,j,k, double(get_voxel(this->datasize[0]-1,j,k)-get_voxel(this->datasize[0]-2,j,k)));//backward diff
			}
		}

	}else if(direction==1){
		for (int k=0; k < this->datasize[2]; k++){
			for (int i=0; i < this->datasize[0]; i++){
				res->set_voxel(i,0,k, double(this->get_voxel(i,1,k)-this->get_voxel(i,0,k)));				//forward diff
				for (int j=1; j < this->datasize[1]-1; j++){
					res->set_voxel(i,j,k,0.5* double(this->get_voxel(i,j+1,k)-this->get_voxel(i,j-1,k)));	//central diff
				}
				res->set_voxel(i,this->datasize[1]-1,k, double(get_voxel(i,this->datasize[1]-1,k)-get_voxel(i,this->datasize[1]-2,k)));//backward diff
			}
		}
	}else{  // direction==2
		for (int j=0; j < this->datasize[1]; j++){
			for (int i=0; i < this->datasize[0]; i++){
				res->set_voxel(i,j,0, double(this->get_voxel(i,j,1)-this->get_voxel(i,j,0)));				//forward diff
				for (int k=1; k < this->datasize[2]-1; k++){
					res->set_voxel(i,j,k,0.5* double(this->get_voxel(i,j,k+1)-this->get_voxel(i,j,k-1)));	//central diff
				}
				res->set_voxel(i,j,this->datasize[2]-1, double(get_voxel(i,j,this->datasize[2]-1)-this->get_voxel(i,j,this->datasize[2]-2)));//backward diff
			}
		}
	}

	return res;
}


template <class ELEMTYPE, int IMAGEDIM>
image_scalar<double,3>* image_scalar<ELEMTYPE, IMAGEDIM>::get_num_diff_image_2ndorder_central_diff_3D(image_scalar<double,3>*df_dir1, int direction2)
{	
	image_scalar<double,3> *res = new image_scalar<double,3>(this);
	if(direction2==0){
		for (int k=0; k < this->datasize[2]; k++){
			for (int j=0; j < this->datasize[1]; j++){
				res->set_voxel(0,j,k, double(df_dir1->get_voxel(1,j,k)-df_dir1->get_voxel(0,j,k)));				//forward diff
				for (int i=1; i < this->datasize[0]-1; i++){
					res->set_voxel(i,j,k,0.5* double(df_dir1->get_voxel(i+1,j,k)-df_dir1->get_voxel(i-1,j,k)));	//central diff
				}
				res->set_voxel(this->datasize[0]-1,j,k, double(df_dir1->get_voxel(this->datasize[0]-1,j,k)-df_dir1->get_voxel(this->datasize[0]-2,j,k)));//backward diff
			}
		}
	}else if(direction2==1){
		for (int k=0; k < this->datasize[2]; k++){
			for (int i=0; i < this->datasize[0]; i++){
				res->set_voxel(i,0,k, double(df_dir1->get_voxel(i,1,k)-df_dir1->get_voxel(i,0,k)));				//forward diff
				for (int j=1; j < this->datasize[1]-1; j++){
					res->set_voxel(i,j,k,0.5* double(df_dir1->get_voxel(i,j+1,k)-df_dir1->get_voxel(i,j-1,k)));	//central diff
				}
				res->set_voxel(i,this->datasize[1]-1,k, double(df_dir1->get_voxel(i,this->datasize[1]-1,k)-df_dir1->get_voxel(i,this->datasize[1]-2,k)));//backward diff
			}
		}
	}else{  // direction==2
		for (int j=0; j < this->datasize[1]; j++){
			for (int i=0; i < this->datasize[0]; i++){
				res->set_voxel(i,j,0, double(df_dir1->get_voxel(i,j,1)-df_dir1->get_voxel(i,j,0)));				//forward diff
				for (int k=1; k < this->datasize[2]-1; k++){
					res->set_voxel(i,j,k,0.5* double(df_dir1->get_voxel(i,j,k+1)-df_dir1->get_voxel(i,j,k-1)));	//central diff
				}
				res->set_voxel(i,j,this->datasize[2]-1, double(df_dir1->get_voxel(i,j,this->datasize[2]-1)-df_dir1->get_voxel(i,j,this->datasize[2]-2)));//backward diff
			}
		}
	}

	return res;
}


template <class ELEMTYPE, int IMAGEDIM >
template<class ELEMTYPE2>
void image_scalar<ELEMTYPE, IMAGEDIM >::interpolate_spline_ITK_3D(image_scalar<ELEMTYPE2, IMAGEDIM > *ref_im, int spline_order)
{
	cout<<"interpolate_spline_ITK_3D..."<<endl;
	typedef itk::ResampleImageFilter<theImageType, theImageType>  FilterType;
	typename FilterType::Pointer filter = FilterType::New();

//	typedef itk::AffineTransform< double, IMAGEDIM >  TransformType;
//	typedef itk::Rigid3DTransform< double > TransformType;
	typedef itk::MatrixOffsetTransformBase<double,3,3> TransformType;
	TransformType::Pointer transform = TransformType::New();

	TransformType::MatrixType m;

	typename itk::OrientedImage<ELEMTYPE2, IMAGEDIM >::DirectionType d = ref_im->get_orientation_itk();
//	cout<<"m="<<endl;
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
			m[i][j] = d[i][j];
//			cout<<m[i][j]<<" ";
		}
//		cout<<endl;
	}
//	transform->SetRotationMatrix(m);
//	transform->SetMatrix(m);


	typedef itk::BSplineInterpolateImageFunction<theOrientedImageType, double >  InterpolatorType;
	typename InterpolatorType::Pointer interpolator = InterpolatorType::New(); //3-rd order is default.....
	
	interpolator->SetSplineOrder(spline_order);
	filter->SetInput(this->get_image_as_itk_output());
	filter->SetTransform( transform );
	filter->SetInterpolator( interpolator );//3-rd order is default.....
	filter->SetDefaultPixelValue( 0 );
	filter->SetOutputOrigin( ref_im->get_origin_itk() );
	filter->SetOutputSpacing( ref_im->get_voxel_size_itk() );
	filter->SetSize( ref_im->get_size_itk() );
	filter->SetOutputDirection(m);


	try{
		cout<<"filter update..."<<endl;
		filter->Update();
	}catch( itk::ExceptionObject & excep ){
		std::cerr << "Exception catched !" << std::endl;
		std::cerr << excep << std::endl;
	}
	this->replicate_itk_to_image(filter->GetOutput());
}



template <class ELEMTYPE, int IMAGEDIM >
void image_scalar<ELEMTYPE, IMAGEDIM >::interpolate_trilinear_3D_vxl(image_scalar<ELEMTYPE, IMAGEDIM > *src_im)
{	
	//cout<<"image_scalar<ELEMTYPE, IMAGEDIM >::interpolate_trilinear_3D_vxl_speed..."<<endl;
	Vector3D rvoxpos;	//integer voxel location in reference image
	Matrix3D rdir2;		//precalculated reference direction data
	//		Vector3D phys;		//physical location in target image
	Matrix3D sdir2;
	Vector3D a;
	Matrix3D b;
	Vector3D svox;		//non-truncated voxel location in source image
	int nx = src_im->datasize[0];
	int ny = src_im->datasize[1];
	int nz = src_im->datasize[2];
	int xstep = 1;
	int ystep = nx;
	int zstep = nx*ny;

	Vector3D s = this->get_physical_size();
	float dx = s[0];
	float dy = s[1];
	float dz = s[2];
	float norm_dxdydz = sqrt(dx*dx + dy*dy + dz*dz);
	//		double val=0;
	//cout<<"nx/ny/nz/ xstep/ystep/zstep"<<nx<<"/"<<ny<<"/"<<nz<<"/ "<<xstep<<"/"<<ystep<<"/"<<zstep<<endl;

	//pre calculations for speedup...
	rdir2 = this->orientation*this->get_voxel_resize();
	sdir2 = src_im->orientation*src_im->get_voxel_resize();
	sdir2 = sdir2.GetInverse();
	a = sdir2*(this->origin - src_im->origin);
	b = sdir2*rdir2;

	for (int k=0; k < this->datasize[2]; k++)
	{
		userIOmanagement.progress_update(k,"Trilinear interp. progress...",this->datasize[2]);
		//cout<<".";
		for (int j=0; j < this->datasize[1]; j++)
		{
			for (int i=0; i < this->datasize[0]; i++)
			{
				//--------------------------------
				//phys coordinates need to be transfomed (origin/rotation/scaling) into "local" physical source coordinates
				//A version not optimized for speed would look something like:
				//phys = this->get_physical_pos_for_voxel(i,j,k);
				//vox = src_im->get_voxelpos_from_physical_pos_3D(phys);
				//--------------------------------

				//phys = this->origin + rdir2*rvoxpos;
				//svox = sdir2*(phys - src_im->origin);
				//svox = sdir2*(this->origin + rdir2*rvoxpos - src_im->origin);
				//svox = sdir2*(this->origin - src_im->origin) + sdir2*rdir2*rvoxpos 
				//svox =                 a                     +           b*rvoxpos

				rvoxpos[0]=i;	rvoxpos[1]=j;	rvoxpos[2]=k;
				svox = a + b*rvoxpos;
				this->set_voxel(i,j,k,mil3d_safe_trilin_interp_3d<ELEMTYPE>(svox[0], svox[1], svox[2], src_im->imagepointer(), nx, ny, nz, xstep, ystep, zstep,dx,dy,dz,norm_dxdydz));
			}
		}
	}
}

template <class ELEMTYPE, int IMAGEDIM >
void image_scalar<ELEMTYPE, IMAGEDIM >::resample_slice_wise_with_spline_interpolation_3D(float factor, int spline_order)
{
	int newxsize=this->datasize[0]*factor;
	int newysize=this->datasize[1]*factor;
	int newzsize=this->datasize[2];

	image_scalar<ELEMTYPE, IMAGEDIM> *ref_im = new image_scalar<ELEMTYPE, IMAGEDIM>(newxsize, newysize, newzsize); // sen template
		
	ref_im->set_parameters(this);

	Vector3D v=ref_im->get_voxel_size();
	v.SetElement(0, v.GetElement(0)/factor);
	v.SetElement(1, v.GetElement(1)/factor);
	ref_im->set_voxel_size(v);
	
	this->interpolate_spline_ITK_3D(ref_im, spline_order);
	delete ref_im;
}


template <class ELEMTYPE, int IMAGEDIM >
void image_scalar<ELEMTYPE, IMAGEDIM >::resample_with_spline_interpolation_3D(int newxsize, int newysize, int newzsize, int spline_order)
{
	int oldxsize=this->datasize[0];
	int oldysize=this->datasize[1];
	int oldzsize=this->datasize[2];

	float xfactor=(float)newxsize/(float)oldxsize;
	float yfactor=(float)newysize/(float)oldysize;
	float zfactor=(float)newzsize/(float)oldzsize;

	image_scalar<ELEMTYPE, IMAGEDIM> *ref_im = new image_scalar<ELEMTYPE, IMAGEDIM>(newxsize, newysize, newzsize); // sen template
		
	ref_im->set_parameters(this);

	Vector3D v=ref_im->get_voxel_size();
	v.SetElement(0, v.GetElement(0)/xfactor);
	v.SetElement(1, v.GetElement(1)/yfactor);
	v.SetElement(2, v.GetElement(2)/zfactor);
	ref_im->set_voxel_size(v);
	
	this->interpolate_spline_ITK_3D(ref_im, spline_order);
	delete ref_im;
}

template <class ELEMTYPE, int IMAGEDIM >
void image_scalar<ELEMTYPE, IMAGEDIM >::set_a_coeff2(double a[64], double f[8], double dfdx[8], double dfdy[8], double dfdz[8], double d2fdxdy[8], double d2fdxdz[8], double d2fdydz[8], double d3fdxdydz[8]) 
{
	// Tricubic interpolation using method described in:
	// F. Lekien, J.E. Marsden
	// Tricubic Interpolation in Three Dimensions
	// International Journal for Numerical Methods in Engineering, 63 (3), 455-471, 2005
	// ...used in the function: image_scalar.interpolate_tricubic_3D...

	for (int i=0;i<64;i++) {
		a[i] = 0.0;
		for (int j=0;j<8;j++) {
			a[i] += A_tricubic[i][0+j]*f[j];
			a[i] += A_tricubic[i][8+j]*dfdx[j];
			a[i] += A_tricubic[i][16+j]*dfdy[j];
			a[i] += A_tricubic[i][24+j]*dfdz[j];
			a[i] += A_tricubic[i][32+j]*d2fdxdy[j];
			a[i] += A_tricubic[i][40+j]*d2fdxdz[j];
			a[i] += A_tricubic[i][48+j]*d2fdydz[j];
			a[i] += A_tricubic[i][56+j]*d3fdxdydz[j];
		}
	}
}


template <class ELEMTYPE, int IMAGEDIM >
double image_scalar<ELEMTYPE, IMAGEDIM >::tricubic_eval(double a[64], double x, double y, double z) 
{
	double ret = 0.0;
	for (int i=0;i<4;i++) {
		for (int j=0;j<4;j++) {
			for (int k=0;k<4;k++) {
				ret += a[i+4*j+16*k]*pow(x,i)*pow(y,j)*pow(z,k);
			}
		}
	}
	return(ret);
}


template <class ELEMTYPE, int IMAGEDIM >
void image_scalar<ELEMTYPE, IMAGEDIM >::interpolate_tricubic_3D(image_scalar<ELEMTYPE, IMAGEDIM > *src_im)
{	
	//cout<<"image_scalar<ELEMTYPE, IMAGEDIM >::interpolate_tricubic_3D..."<<endl;
	Vector3D rvoxpos;	//integer voxel location in reference image
	Matrix3D rdir2;		//precalculated reference direction data
	Matrix3D sdir2;
	Vector3D a;
	Matrix3D b;
	Vector3D svox;		//non-truncated voxel location in source image

	ELEMTYPE val;

	//		int nx = src_im->datasize[0];
	//		int ny = src_im->datasize[1];
	//		int nz = src_im->datasize[2];
	//		int xstep = 1;
	//		int ystep = nx;
	//		int zstep = nx*ny;

	Vector3D s = this->get_voxel_size();
	double dx = double(s[0]);
	double dy = double(s[1]);
	double dz = double(s[2]);

	//pre calculations for speedup...
	rdir2 = this->orientation*this->get_voxel_resize();
	sdir2 = src_im->orientation*src_im->get_voxel_resize();
	sdir2 = sdir2.GetInverse();
	a = sdir2*(this->origin - src_im->origin);
	b = sdir2*rdir2;

	double fval[8];//={1.2, 2.3, 3.4, 4.5, 5.6, 6.7, 7.8, 8.9};
	double dfdxval[8];//={1.2, 2.3, 3.4, 4.5, 5.6, 6.7, 7.8, 8.9};
	double dfdyval[8];//={1.2, 2.3, 3.4, 4.5, 5.6, 6.7, 7.8, 8.9};
	double dfdzval[8];//={1.2, 2.3, 3.4, 4.5, 5.6, 6.7, 7.8, 8.9};
	double d2fdxdyval[8];//={1.2, 2.3, 3.4, 4.5, 5.6, 6.7, 7.8, 8.9};
	double d2fdxdzval[8];//={1.2, 2.3, 3.4, 4.5, 5.6, 6.7, 7.8, 8.9};
	double d2fdydzval[8];//={1.2, 2.3, 3.4, 4.5, 5.6, 6.7, 7.8, 8.9};
	double d3fdxdydzval[8];//={1.2, 2.3, 3.4, 4.5, 5.6, 6.7, 7.8, 8.9};

	double coeff[64];

	int id=0;
	int x =0;
	int y =0;
	int z =0;
	int xpos=0;
	int ypos=0;
	int zpos=0;

	userIOmanagement.progress_update(1,"Precalculating derivatives...",7);
	image_scalar<double,3> *fx = get_num_diff_image_1storder_central_diff_3D(0);
	userIOmanagement.progress_update(2,"Precalculating derivatives...",7);
	image_scalar<double,3> *fy = get_num_diff_image_1storder_central_diff_3D(1);
	userIOmanagement.progress_update(3,"Precalculating derivatives...",7);
	image_scalar<double,3> *fz = get_num_diff_image_1storder_central_diff_3D(2);
	userIOmanagement.progress_update(4,"Precalculating derivatives...",7);
	image_scalar<double,3> *fxy = get_num_diff_image_2ndorder_central_diff_3D(fx,1);
	userIOmanagement.progress_update(5,"Precalculating derivatives...",7);
	image_scalar<double,3> *fxz = get_num_diff_image_2ndorder_central_diff_3D(fx,2);
	userIOmanagement.progress_update(6,"Precalculating derivatives...",7);
	image_scalar<double,3> *fyz = get_num_diff_image_2ndorder_central_diff_3D(fy,2);
	userIOmanagement.progress_update(7,"Precalculating derivatives...",7);
	image_scalar<double,3> *fxyz = get_num_diff_image_2ndorder_central_diff_3D(fxy,2);

	for (int k=0; k < this->datasize[2]; k++)
	{
		userIOmanagement.progress_update(k,"interpolate_tricubic_3D - progress...",this->datasize[2]);
		//cout<<".";
		for (int j=0; j < this->datasize[1]; j++)
		{
			for (int i=0; i < this->datasize[0]; i++)
			{
				rvoxpos[0]=i;	rvoxpos[1]=j;	rvoxpos[2]=k;
				svox = a + b*rvoxpos;
				val = 0;
				if(src_im->is_voxelpos_inside_image_border_3D(svox[0],svox[1],svox[2],1))
				{
					for (id=0;id<8;id++) {
						switch(id){
								case 0:	x=0; y=0; z=0; break;
								case 1: x=1; y=0; z=0; break;
								case 2: x=0; y=1; z=0; break;
								case 3: x=1; y=1; z=0; break;
								case 4: x=0; y=0; z=1; break;
								case 5: x=1; y=0; z=1; break;
								case 6: x=0; y=1; z=1; break;
								case 7: x=1; y=1; z=1; break;
								default:x=0; y=0; z=0;
						}
						xpos = svox[0]+x;
						ypos = svox[1]+y;
						zpos = svox[2]+z;

						//fval[id]		= src_im->get_voxel(							xpos,ypos,zpos);
						//dfdxval[id]		= src_im->get_num_diff_1storder_central_diff_3D(xpos,ypos,zpos, 0);
						//dfdyval[id]		= src_im->get_num_diff_1storder_central_diff_3D(xpos,ypos,zpos, 1);
						//dfdzval[id]		= src_im->get_num_diff_1storder_central_diff_3D(xpos,ypos,zpos, 2);
						//d2fdxdyval[id]	= src_im->get_num_diff_2ndorder_central_diff_3D(xpos,ypos,zpos, 0, 1);
						//d2fdxdzval[id]	= src_im->get_num_diff_2ndorder_central_diff_3D(xpos,ypos,zpos, 0, 2);
						//d2fdydzval[id]	= src_im->get_num_diff_2ndorder_central_diff_3D(xpos,ypos,zpos, 1, 2);
						//d3fdxdydzval[id]= src_im->get_num_diff_3rdorder_central_diff_3D(xpos,ypos,zpos, 1, 2, 3);

						fval[id]		= src_im->get_voxel(xpos,ypos,zpos);
						dfdxval[id]		= fx->get_voxel(	xpos,ypos,zpos);
						dfdyval[id]		= fy->get_voxel(	xpos,ypos,zpos);
						dfdzval[id]		= fz->get_voxel(	xpos,ypos,zpos);
						d2fdxdyval[id]	= fxy->get_voxel(	xpos,ypos,zpos);
						d2fdxdzval[id]	= fxz->get_voxel(	xpos,ypos,zpos);
						d2fdydzval[id]	= fyz->get_voxel(	xpos,ypos,zpos);
						d3fdxdydzval[id]= fxyz->get_voxel(	xpos,ypos,zpos);
					}

					//Set the a-coefficients....				
					//					set_a_coeff(coeff,fval,dfdxval,dfdyval,dfdzval,d2fdxdyval,d2fdxdzval,d2fdydzval,d3fdxdydzval);
					set_a_coeff2(coeff,fval,dfdxval,dfdyval,dfdzval,d2fdxdyval,d2fdxdzval,d2fdydzval,d3fdxdydzval);
					val = tricubic_eval(coeff,double(svox[0]-int(svox[0]))/dx,double(svox[1]-int(svox[1]))/dy,double(svox[2]-int(svox[2]))/dz);
				}
				this->set_voxel(i,j,k,val);
			}
		}
	}
	userIOmanagement.progress_update(0,"interpolate_tricubic_3D - progress...Done",this->datasize[2]);

	delete fx;
	delete fy;
	delete fz;
	delete fxy;
	delete fxz;
	delete fyz;
	delete fxyz;
}

template <class ELEMTYPE, int IMAGEDIM>
image_scalar<ELEMTYPE, IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::get_subvolume_from_slice_3D(int slice, int dir)
{
	cout<<"get_subvolume_from_region_3D..."<<endl;

	int usize=this->get_size_by_dim_and_dir(0,dir);
	int vsize=this->get_size_by_dim_and_dir(1,dir);
	pt_error::error_if_false(slice>=0 && slice<this->get_size_by_dim_and_dir(2,dir)," slice outside image dimensions in image_scalar<ELEMTYPE, IMAGEDIM>::get_subvolume_from_slice_3D",pt_error::debug);

	image_scalar<ELEMTYPE, IMAGEDIM>* res;
	if (dir==0) {
		res = new image_scalar<ELEMTYPE, IMAGEDIM>(1, usize, vsize);
		res->set_parameters(this);
		res->set_origin(this->get_physical_pos_for_voxel(slice,0,0));
	} else if (dir==1) {
		res = new image_scalar<ELEMTYPE, IMAGEDIM>(vsize, 1, usize);
		res->set_parameters(this);
		res->set_origin(this->get_physical_pos_for_voxel(0,slice,0));
	} else {
		res = new image_scalar<ELEMTYPE, IMAGEDIM>(usize, vsize, 1);
		res->set_parameters(this);
		res->set_origin(this->get_physical_pos_for_voxel(0,0,slice));
	}

	for (int u=0; u<usize; u++){
		for (int v=0; v<vsize; v++){
			res->set_voxel_by_dir(u,v,0, this->get_voxel_by_dir(u,v,slice,dir), dir);
		}
	}
	return res;
}

template <class ELEMTYPE, int IMAGEDIM>
image_scalar<ELEMTYPE, IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::get_subvolume_from_slice_rotated_3D(int slice, int dir)
{
//	cout<<"get_subvolume_from_slice_rotated_3D..."<<endl;

	int usize=this->get_size_by_dim_and_dir(0,dir);
	int vsize=this->get_size_by_dim_and_dir(1,dir);
	pt_error::error_if_false(slice>=0 && slice<this->get_size_by_dim_and_dir(2,dir)," slice outside image dimensions in image_scalar<ELEMTYPE, IMAGEDIM>::get_subvolume_from_slice_3D",pt_error::debug);

	image_scalar<ELEMTYPE, IMAGEDIM>* res = new image_scalar<ELEMTYPE, IMAGEDIM>(usize, vsize, 1);
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
image_scalar<ELEMTYPE, IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::get_subvolume_from_region_3D(Vector3Dint vox_pos, Vector3Dint vox_size)
{
	return get_subvolume_from_region_3D(vox_pos[0],vox_pos[1],vox_pos[2],vox_pos[0]+vox_size[0]-1,vox_pos[1]+vox_size[1]-1,vox_pos[2]+vox_size[2]-1);
}


template <class ELEMTYPE, int IMAGEDIM>
image_scalar<ELEMTYPE, IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::get_subvolume_from_region_3D(int x1, int y1, int z1, int x2, int y2, int z2)
{
	//cout<<"image_scalar-get_subvolume_from_region_3D..."<<endl;

	int nx=this->nx();
	int ny=this->ny();
	int nz=this->nz();
	x1 = max(x1,0);	x2 = min(x2,nx-1);
	y1 = max(y1,0);	y2 = min(y2,ny-1);
	z1 = max(z1,0);	z2 = min(z2,nz-1);

	image_scalar<ELEMTYPE, IMAGEDIM>* res = new image_scalar<ELEMTYPE, IMAGEDIM>(x2-x1+1, y2-y1+1, z2-z1+1);
	res->set_parameters(this);
	res->set_origin(this->get_physical_pos_for_voxel(x1,y1,z1));

	for (int z=z1, res_z=0; z<=z2; z++, res_z++){
		for (int y=y1, res_y=0; y<=y2; y++,res_y++){
			for (int x=x1, res_x=0; x<=x2; x++,res_x++){
				res->set_voxel(res_x,res_y,res_z, this->get_voxel(x,y,z));
			}
		}
	}
	res->set_origin(this->get_physical_pos_for_voxel(x1,y1,z1));
	return res;
}
/*
template <class ELEMTYPE, int IMAGEDIM>
image_scalar<ELEMTYPE, IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::get_subvolume_from_region_3D(int dir, int from_slice, int to_slice)
{

}
*/
template <class ELEMTYPE, int IMAGEDIM>
image_scalar<ELEMTYPE, IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::get_subvolume_from_region_3D(image_binary<3> *mask, IMGBINARYTYPE object_value)
{
	int x1,y1,z1,x2,y2,z2;
	mask->get_span_of_value_3D(object_value,x1,y1,z1,x2,y2,z2);
	return this->get_subvolume_from_region_3D(x1,y1,z1,x2,y2,z2);
}

template <class ELEMTYPE, int IMAGEDIM>
image_scalar<ELEMTYPE, IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::get_subvolume_from_thresholded_region_3D(ELEMTYPE from_val, ELEMTYPE to_val)
{
	image_binary<3>* tmp_mask = this->threshold(from_val,to_val);
	tmp_mask->data_has_changed();
	image_scalar<ELEMTYPE, IMAGEDIM> *im = this->get_subvolume_from_region_3D(tmp_mask);
	delete tmp_mask;
	return im;
}


template <class ELEMTYPE, int IMAGEDIM>
image_scalar<ELEMTYPE, IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::expand_borders(unsigned int dx, unsigned int dy, unsigned int dz, IMGBINARYTYPE value)
{
	cout<<"expanding image borders (image_scalar)..."<<endl;
	image_scalar<ELEMTYPE, IMAGEDIM>* res = new image_scalar<ELEMTYPE, IMAGEDIM>(this->nx()+2*dx, this->ny()+2*dy, this->nz()+2*dz);
	res->fill(value);
	res->set_parameters(this);
	res->fill_region_3D_with_subvolume_image( create_Vector3Dint(dx,dy,dz),this,create_Vector3Dint(0,0,0),create_Vector3Dint(this->nx(),this->ny(),this->nz()) ); //based on given voxel coords
	res->set_origin(this->get_physical_pos_for_voxel(-dx,-dy,-dz));
	return res;
}

template <class ELEMTYPE, int IMAGEDIM>
image_scalar<ELEMTYPE, IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::expand_borders2D_by_dir(int dir, unsigned int dr, IMGBINARYTYPE value)
{
	image_scalar<ELEMTYPE, IMAGEDIM>* res;
	if(dir==2){
		res = expand_borders(dr,dr,0,value);
	}else if(dir==1){
		res = expand_borders(dr,0,dr,value);
	}else{
		res = expand_borders(0,dr,dr,value);
	}
	return res;
}

template <class ELEMTYPE, int IMAGEDIM>
image_scalar<ELEMTYPE, IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::contract_borders(unsigned int dx, unsigned int dy, unsigned int dz)
{
	cout<<"contracting image borders (image_binary)..."<<endl;
	int old_size_x=this->get_size_by_dim(0);
	int old_size_y=this->get_size_by_dim(1);
	int old_size_z=this->get_size_by_dim(2);
	image_scalar<ELEMTYPE, IMAGEDIM>* res = new image_scalar<ELEMTYPE, IMAGEDIM>(old_size_x-2*dx, old_size_y-2*dy, old_size_z-2*dz);
	res->set_parameters(this);

	for (int z=0; z<res->nz(); z++){
		for (int y=0; y<res->ny(); y++){
			for (int x=0; x<res->nx(); x++){
				res->set_voxel(x,y,z, this->get_voxel(x+dx,y+dy,z+dz));
			}
		}
	}
	res->set_origin(this->get_physical_pos_for_voxel(0+dx,0+dy,0+dz));
	return res;
}

template <class ELEMTYPE, int IMAGEDIM>
image_scalar<ELEMTYPE, IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::contract_borders2D_by_dir(int dir, unsigned int dr)
{
	image_scalar<ELEMTYPE, IMAGEDIM>* res;
	if(dir==2){
		res = contract_borders(dr,dr,0);
	}else if(dir==1){
		res = contract_borders(dr,0,dr);
	}else{
		res = contract_borders(0,dr,dr);
	}
	return res;
}




template <class ELEMTYPE, int IMAGEDIM>
image_scalar<ELEMTYPE, IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::crop_and_return_3D(image_binary<3> *mask)
{
	image_scalar<ELEMTYPE, IMAGEDIM>* res;
	if(this->same_size(mask)){
		//jk-????... implement here... //if a return value is given in a similar function... also implement in all image_classes...
		res = this->get_subvolume_from_region_3D(mask);
	}else{
		pt_error::error("crop_and_return_3D(image_binary<3> *mask)--> NOT same size...",pt_error::debug);
	}

	return res;
}

template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_scalar<ELEMTYPE, IMAGEDIM>::get_max_in_slice3D(int slice, int dir)
{
	ELEMTYPE max = std::numeric_limits<ELEMTYPE>::min();

	if(dir==2){			
		for(int y=0; y<this->ny(); y++){
			for(int x=0; x<this->nx(); x++){
				if(this->get_voxel(x,y,slice)>max){
					max = this->get_voxel(x,y,slice);
				}
			}
		}
	}else if(dir ==1){
		for(int z=0; z<this->nz(); z++){
			for(int x=0; x<this->nx(); x++){
				if(this->get_voxel(x,slice,z)>max){
					max = this->get_voxel(x,slice,z);
				}
			}
		}
	}else{
		for(int z=0; z<this->nz(); z++){
			for(int y=0; y<this->ny(); y++){
				if(this->get_voxel(slice,y,z)>max){
					max = this->get_voxel(slice,y,z);
				}
			}
		}
	}
	 return max;
}


template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::crop_3D(image_binary<3> *mask)
{
	if(this->same_size(mask)){
		//jk-????... implement here... if a return value is given in a similar function... also implement in all image_classes...
		image_scalar<ELEMTYPE, IMAGEDIM>* res = this->crop_and_return_3D(mask);
	    this->initialize_dataset(res->nx(), res->ny(), res->nz(), NULL); //deallocate is done in initialize_dataset, if needed...
		copy_data(res,this);
		this->set_parameters(res);
		delete res;
	}else{
		pt_error::error("crop_3D(image_binary<3> *mask)--> NOT same size...",pt_error::debug);
	}
}


template <class ELEMTYPE, int IMAGEDIM>
image_binary<IMAGEDIM> * image_scalar<ELEMTYPE, IMAGEDIM>::threshold(ELEMTYPE low, ELEMTYPE high, IMGBINARYTYPE true_inside_threshold)
	{
    image_binary<IMAGEDIM> * output = new image_binary<IMAGEDIM> (this,false);
        
	typename image_storage<ELEMTYPE>::iterator i = this->begin();
	typename image_binary<IMAGEDIM>::iterator o = output->begin();

    while (i != this->end()) //images are same size and should necessarily end at the same time
        {
        if(*i>=low && *i<=high)
            {*o=true_inside_threshold;}
        else
            {*o=!true_inside_threshold;}

        ++i; ++o;
        }

	output->set_parameters(this);
    return output;
	}

template <class ELEMTYPE, int IMAGEDIM>
image_binary<IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::threshold_slice_wise_with_slice_max_offsets(ELEMTYPE thres_min, ELEMTYPE max_offset, int dir, IMGBINARYTYPE true_inside_threshold, ELEMTYPE high)
{
    image_binary<IMAGEDIM> *output = new image_binary<IMAGEDIM>(this,false);
	ELEMTYPE val;
	ELEMTYPE slice_max;
	ELEMTYPE slice_thres;
	for(int w=0; w<this->get_size_by_dim_and_dir(2,dir); w++){
		slice_max = this->get_max_in_slice3D(w,dir);
		slice_thres = std::max<ELEMTYPE>(ELEMTYPE(slice_max+max_offset),thres_min);

		for(int v=0; v<this->get_size_by_dim_and_dir(1,dir); v++){
			for(int u=0; u<this->get_size_by_dim_and_dir(0,dir); u++){
				val = this->get_voxel_by_dir(u,v,w,dir);
				if( (val>=slice_thres) && (val<=high) ){
					output->set_voxel_by_dir(u, v, w, true_inside_threshold, dir);
				}else{
					output->set_voxel_by_dir(u, v, w, !true_inside_threshold, dir);
				}
			}
		}
	}
	return output;
}


template <class ELEMTYPE, int IMAGEDIM>	
void image_scalar<ELEMTYPE, IMAGEDIM>::draw_line_2D(int x0, int y0, int x1, int y1, int z, ELEMTYPE value, int direction)
	{
	bool steep=abs(y1-y0)>abs(x1-x0);
	int temp;
	if(steep)
		{
		temp=x0;
		x0=y0;
		y0=temp;
		temp=x1;
		x1=y1;
		y1=temp;
		}
	if(x0>x1)
		{
		temp=x0;
		x0=x1;
		x1=temp;
		temp=y0;
		y0=y1;
		y1=temp;
		}
	int deltax=x1-x0;
	int deltay=abs(y1-y0);
	int error= -deltax/2;
	int ystep;
	int y=y0;
	if(y0<y1)
		ystep=1;
	else
		ystep=-1;
	int x;
	for(x=x0; x<=x1; x++)
		{
		if(steep)
			{
			this->set_voxel_by_dir(y,x,z,value,direction);
			}
		else
			{
			this->set_voxel_by_dir(x,y,z,value,direction);
			}
		error+=deltay;
		if(error>0)
			{
			y+=ystep;
			error-=deltax;
			}
		}
	//this->image_has_changed();
	}


template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::draw_line_3D(Vector3Dint from_vox, Vector3Dint to_vox, ELEMTYPE value)
{
	Vector3Dint diff = (to_vox - from_vox);
	float n = max( max(abs(diff[0]),abs(diff[1])), abs(diff[2]) );
	Vector3D delta;
	delta[0] = float(diff[0])/(n+1.0);
	delta[1] = float(diff[1])/(n+1.0);
	delta[2] = float(diff[2])/(n+1.0);
	for(float i=0;i<n+1;i++){
		this->set_voxel( from_vox[0]+delta[0]*i, from_vox[1]+delta[1]*i, from_vox[2]+delta[2]*i, value);
	}
}

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::convex_hull_line_filling_3D(int dir, ELEMTYPE object_value)
{
	int first;
	int last;
	int sx = this->datasize[0];
	int sy = this->datasize[1];
	int sz = this->datasize[2];

	switch(dir){
	case 0:
		for(int z=0; z<sz; z++){		
			for(int y=0; y<sy; y++){
				first=-1;
				last=-1;
				for(int x=0; x<sx; x++){
					if(this->get_voxel(x,y,z) == object_value){
						if(first==-1){
							first = x;
						}else{
							last = x;
						}
					}
				}
				if(last>-1){
					this->draw_line_2D(first,y,last,y,z,object_value);
				}
			}
		}
		break;
	case 1:
		for(int z=0; z<sz; z++){		
			for(int x=0; x<sx; x++){
				first=-1;
				last=-1;
				for(int y=0; y<sy; y++){
					if(this->get_voxel(x,y,z) == object_value){
						if(first==-1){
							first = y;
						}else{
							last = y;
						}
					}
				}
				if(last>-1){
					this->draw_line_2D(x,first,x,last,z,object_value);
				}
			}
		}
		break;
	case 2:
		for(int y=0; y<sy; y++){
			for(int x=0; x<sx; x++){
				first=-1;
				last=-1;
				for(int z=0; z<sz; z++){		
					if(this->get_voxel(x,y,z) == object_value){
						if(first==-1){
							first = z;
						}else{
							last = z;
						}
					}
				}
				if(last>-1){
					for(int z=first; z<=last; z++){	
						this->set_voxel(x,y,z,object_value);
					}
				}
			}
		}
		break;
	default:
		pt_error::error("convex_hull_line_filling_3D, erroneous direction...",pt_error::debug);
		break;
	}
}

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::convex_hull_line_filling_in_two_dirs_3D(int dir1, int dir2, ELEMTYPE object_value)
{
	this->convex_hull_line_filling_3D(dir1,object_value);
	this->convex_hull_line_filling_3D(dir2,object_value);
}

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::convex_hull_line_filling_inplane_3D(int dir, ELEMTYPE object_value)
{
	if(dir==0){
		this->convex_hull_line_filling_in_two_dirs_3D(1,2,object_value);
	}else if(dir==1){
		this->convex_hull_line_filling_in_two_dirs_3D(0,2,object_value);
	}else if(dir==2){
		this->convex_hull_line_filling_in_two_dirs_3D(0,1,object_value);
	}
}

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::draw_line_3D(line3D line, ELEMTYPE value)
{
	Vector3Dint v1;
	Vector3Dint v2;
	this->get_line_intersection_voxels(line, v1, v2);
	cout<<"v1="<<v1<<endl;
	cout<<"v2="<<v2<<endl;
	if(v2[0]>=0){
		this->draw_line_3D(v1,v2,value);
	}
}


template <class ELEMTYPE, int IMAGEDIM>
bool image_scalar<ELEMTYPE, IMAGEDIM>::row_sum_threshold(int* res, ELEMTYPE low_thr, ELEMTYPE high_thr, int row_direction, int z_direction, int first_slice, int last_slice)
	{
	double totdiff=0;
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim_and_dir(0,z_direction);
	max_v=this->get_size_by_dim_and_dir(1,z_direction);
	max_w=this->get_size_by_dim_and_dir(2,z_direction);
	
	memset(res,0,sizeof(int)*max_w);
	ELEMTYPE p;//pixel value
	double sum=0;
	int optthr;
	if(first_slice<0)
		first_slice=0;
	if(last_slice<0)
		last_slice=max_w-1;
 	if(row_direction==(z_direction+1)%3)
		{
		double* hist=new double[max_v];
		for(w=first_slice; w<=last_slice; w++)
			{
			for(v=0; v<max_v; v++)
				{
				int sum=0;
				for(u=0; u<max_u; u++)
					{
					p=this->get_voxel_by_dir(u,v,w,z_direction);
					if(p>=low_thr && p<=high_thr)
						{
						sum++;
						}
					}
				hist[v]=sum;
				}

			//Test all threshold values
			double sum1=0;
			double sum2=0;
			double n1=0;
			double n2=0;
			double cumSum=0;
			double cumSum_2=0;
			double cumN=0;

			int j;
			for(j=0; j<max_v; j++)
				{
				cumSum+=j*hist[j];
				cumSum_2+=j*j*hist[j];
				cumN+=hist[j];
				}
			double maxvar=0;
			optthr=0;
			double optdiff=0;
			for(j=0; j<max_v; j++)
				{
				//Compute mean and std
				sum1+=j*hist[j];
				n1+=hist[j];
				sum2=cumSum-sum1;
				n2=cumN-n1;		

				if(n1>0 && n2>0)
					{
					double mean1=sum1/n1;
					double mean2=sum2/n2;
					double betweenvar=n1*n2*(mean1-mean2)*(mean1-mean2)/(n1+n2);
					if(betweenvar>maxvar)
						{
						maxvar=betweenvar;
						optthr=j;
						optdiff=sum1-sum2;
						}
					}
				}
			totdiff+=optdiff;
			//double cumSumU=0;
			//double cumSumU_2=0;
			//double cumSumV=0;
			//double cumSumV_2=0;
			//cumN=0;
			//for(v=0; v<optthr; v++)
			//	{
			//	int sum=0;
			//	for(u=0; u<max_u; u++)
			//		{
			//		p=this->get_voxel_by_dir(u,v,w,z_direction);
			//		if(p>=low_thr && p<=high_thr)
			//			{
			//			cumSumV+=v;
			//			cumSumV_2+=v*v;
			//			cumSumU+=u;
			//			cumSumU_2+=u*u;
			//			cumN++;
			//			}
			//		}
			//	}
			//double stdU=sqrt((cumSumU_2-cumSumU*cumSumU/cumN)/(cumN-1));
			//double stdV=sqrt((cumSumV_2-cumSumV*cumSumV/cumN)/(cumN-1));
			//double widthU=(cumN*stdU/stdV);
			//double widthV=(cumN*stdV/stdU);
			//out << "Slice: " << w << " Thr: " << optthr << " WidthU: " << widthU << " WidthV: " << widthV << " StdU: " << stdU << " StdV: " << stdV << " Sum: " << cumN<< endl;
			res[w]=optthr;
			}
		delete[] hist;
        //std::cout << "Slice: " << w-1 << " Thr: " << optthr << std::endl;
		}
	else
		{
		//ofstream out("stduv.txt");
		double* hist=new double[max_u];
		for(w=first_slice; w<=last_slice; w++)
			{
			for(u=0; u<max_u; u++)
				{
				int sum=0;
				for(v=0; v<max_v; v++)
					{
					p=this->get_voxel_by_dir(u,v,w,z_direction);
					if(p>=low_thr && p<=high_thr)
						{
						sum++;
						}
					}
				hist[u]=sum;
				}

			//Test all threshold values
			double sum1=0;
			double sum2=0;
			double n1=0;
			double n2=0;
			double cumSum=0;
			double cumSum_2=0;
			double cumN=0;

			int j;
			for(j=0; j<max_u; j++)
				{
				cumSum+=j*hist[j];
				cumSum_2+=j*j*hist[j];
				cumN+=hist[j];
				}
			double maxvar=0;
			optthr=0;
			double optdiff=0;
			for(j=0; j<max_u; j++)
				{
				//Compute mean and std
				sum1+=j*hist[j];
				n1+=hist[j];
				sum2=cumSum-sum1;
				n2=cumN-n1;		

				if(n1>0 && n2>0)
					{
					double mean1=sum1/n1;
					double mean2=sum2/n2;
					double betweenvar=n1*n2*(mean1-mean2)*(mean1-mean2)/(n1+n2);
					if(betweenvar>maxvar)
						{
						maxvar=betweenvar;
						optthr=j;
						optdiff=sum1-sum2;
						}
					}
				}
			totdiff+=optdiff;
			//double cumSumU=0;
			//double cumSumU_2=0;
			//double cumSumV=0;
			//double cumSumV_2=0;
			//cumN=0;
			//for(u=0; u<optthr; u++)
			//	{
			//	int sum=0;
			//	for(v=0; v<max_v; v++)
			//		{
			//		p=this->get_voxel_by_dir(u,v,w,z_direction);
			//		if(p>=low_thr && p<=high_thr)
			//			{
			//			cumSumV+=v;
			//			cumSumV_2+=v*v;
			//			cumSumU+=u;
			//			cumSumU_2+=u*u;
			//			cumN++;
			//			}
			//		}
			//	}
			//double stdU=sqrt((cumSumU_2-cumSumU*cumSumU/cumN)/(cumN-1));
			//double stdV=sqrt((cumSumV_2-cumSumV*cumSumV/cumN)/(cumN-1));
			//double widthU=sqrt(cumN*stdU/stdV);
			//double widthV=sqrt(cumN*stdV/stdU);
			//cout << "Slice: " << w << " Thr: " << optthr << " WidthU: " << widthU << " WidthV: " << widthV << " StdU: " << stdU << " StdV: " << stdV << " Sum: " << cumN<< endl;
			res[w]=optthr;
			}
		//out.close();
		delete[] hist;
        //std::cout << "Slice: " << w-1 << " Thr: " << optthr << std::endl;
		}
	return totdiff>0;
	}


template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::mask_out(image_binary<IMAGEDIM> *mask, IMGBINARYTYPE object_value, ELEMTYPE blank)
    {
    typename image_storage<ELEMTYPE>::iterator i = this->begin();
    image_storage<IMGBINARYTYPE >::iterator m = mask->begin();
    while (i != this->end()) //images are same size and should necessarily end at the same time
        {
        if(*m != object_value)
            {*i=blank;}
        ++i; ++m;
        }
	//this->min_max_refresh();
    }

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::mask_out(int low_x, int high_x, int low_y, int high_y, int low_z, int high_z, ELEMTYPE blank)
    {
	int x,y,z;
	int max_x, max_y, max_z;
	max_x=this->get_size_by_dim(0);
	max_y=this->get_size_by_dim(1);
	max_z=this->get_size_by_dim(2);
	if(low_x<0)
		low_x=0;
	if(low_y<0)
		low_y=0;
	if(low_z<0)
		low_z=0;
	if(high_x<0 || high_x>max_x)
		high_x=max_x;
	if(high_y<0 || high_y>max_y)
		high_y=max_y;
	if(high_z<0 || high_z>max_z)
		high_z=max_z;
	for(z=low_z; z<high_z; z++)
		for(y=low_y; y<high_y; y++)
			for(x=low_x; x<high_x; x++)
				this->set_voxel(x,y,z,blank);
    }
	
template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::mask_out_from_planes_3D(vector<plane3D> planes, ELEMTYPE blank, bool outside_all_planes_needed, SPACE_TYPE st)
{
	//JK TODO //???? make sure this function uses the SPACE_TYPE information
	int num_planes=planes.size();
	vector<float> d(num_planes);
	for (int i=0; i<num_planes; i++) {
		d[i]= planes[i].get_point()*planes[i].get_normal();
	}
	if(outside_all_planes_needed){
		bool outside_all;
		for (int z=0; z<this->get_size_by_dim(2); z++) {
			for (int y=0; y<this->get_size_by_dim(1); y++) {
				for (int x=0; x<this->get_size_by_dim(0); x++) {
					outside_all=true;
					for (int i=0; i<num_planes; i++) {
						if (d[i] <= this->get_physical_pos_or_voxel_pos(x,y,z,st)*planes[i].get_normal()) {
							outside_all=false;
							break;
						}	
					}
					if(outside_all){
						this->set_voxel(x,y,z,blank);
					}
				}
			}
		}
	}else{
		for (int z=0; z<this->get_size_by_dim(2); z++) {
			for (int y=0; y<this->get_size_by_dim(1); y++) {
				for (int x=0; x<this->get_size_by_dim(0); x++) {
					for (int i=0; i<num_planes; i++) {
						if (d[i] > this->get_physical_pos_or_voxel_pos(x,y,z,st)*planes[i].get_normal()) {
							this->set_voxel(x,y,z,blank);
							break;
						}	
					}
				}
			}
		}
	}
}

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::mask_out_from_planes_3D(plane3D plane1, plane3D plane2, plane3D plane3, plane3D plane4, plane3D plane5, ELEMTYPE blank, SPACE_TYPE st)
{
	int num_planes=5;
	if (!plane2.is_defined()) {num_planes=1;}
	else if (!plane3.is_defined()) {num_planes=2;}
	else if (!plane4.is_defined()) {num_planes=3;}
	else if (!plane5.is_defined()) {num_planes=4;}
	
	vector<plane3D> planes(num_planes);
    planes[0]=plane1;
	if (plane2.is_defined()) {planes[1]=plane2;}
	if (plane3.is_defined()) {planes[2]=plane3;}
	if (plane4.is_defined()) {planes[3]=plane4;}
	if (plane5.is_defined()) {planes[4]=plane5;}
	
	mask_out_from_planes_3D(planes, blank, st);
}


template <class ELEMTYPE, int IMAGEDIM>
std::vector<double> image_scalar<ELEMTYPE, IMAGEDIM>::get_slice_sum(int direction)
{
		int u,v,w;
		int max_u, max_v, max_w;
		max_u=this->get_size_by_dim_and_dir(0,direction);
		max_v=this->get_size_by_dim_and_dir(1,direction);
		max_w=this->get_size_by_dim_and_dir(2,direction);
		ELEMTYPE p;//pixel value
		double sum=0;
		std::vector<double> res;
		for(w=0; w<max_w; w++)
		{
			double sum=0;
			for(v=0; v<max_v; v++)
			{
				for(u=0; u<max_u; u++)
				{
					p=this->get_voxel_by_dir(u,v,w,direction);
					sum+=p;
				}
			}
			res.push_back(sum);
		}

		return res;
	}


template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::flip_voxel_data_3D(int direction)
	{
		//loop over floor(half) of the volume and copy that half to the other...
		ELEMTYPE tmp1;
		ELEMTYPE tmp2;
		int pos2;

		switch(direction){
		case 0:
			for(int z=0; z<this->datasize[2]; z++){		
				for(int y=0; y<this->datasize[1]; y++){
					for(int x=0; x<this->datasize[0]/2; x++){
						pos2 = this->datasize[0]-1-x;
						tmp1 = this->get_voxel(x,y,z);
						tmp2 = this->get_voxel(pos2,y,z);
						this->set_voxel(x,y,z,tmp2);
						this->set_voxel(pos2,y,z,tmp1);
					}
				}
			}
			break;
		case 1: 
			for(int z=0; z<this->datasize[2]; z++){		
				for(int y=0; y<this->datasize[1]/2; y++){
					for(int x=0; x<this->datasize[0]; x++){
						pos2 = this->datasize[1]-1-y;
						tmp1 = this->get_voxel(x,y,z);
						tmp2 = this->get_voxel(x,pos2,z);
						this->set_voxel(x,y,z,tmp2);
						this->set_voxel(x,pos2,z,tmp1);
					}
				}
			}
			break;
		case 2: 
			for(int z=0; z<this->datasize[2]/2; z++){
				for(int y=0; y<this->datasize[1]; y++){
					for(int x=0; x<this->datasize[0]; x++){
						pos2 = this->datasize[2]-1-z;
						tmp1 = this->get_voxel(x,y,z);
						tmp2 = this->get_voxel(x,y,pos2);
						this->set_voxel(x,y,z,tmp2);
						this->set_voxel(x,y,pos2,tmp1);
					}
				}
			}
			break;
		default:
			pt_error::error("image_scalar<>-flip_voxel_data_3D(), erroneous direction",pt_error::debug);
			break;
		}
	}

template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_scalar<ELEMTYPE, IMAGEDIM>::get_intensity_at_lower_percentile(float percentile, bool ignore_zero_intensity)
{
	return this->stats->get_intensity_at_histogram_lower_percentile(percentile, ignore_zero_intensity);
}

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::save_histogram_to_txt_file(const std::string filename, const std::string separator)
{
	this->stats->save_histogram_to_txt_file(filename,0,false,separator);
}
template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::make_histogram_curve(){
	datamanagement.add(this->stats->get_curve_for_rendering(),name() + " hist");
}


template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::save_histogram_to_txt_file(const std::string filename, gaussian *g, bool reload_hist_from_image, const std::string separator)
{
//	cout<<"save_histogram_to_txt_file..."<<endl;
	pt_error::error_if_null(this->stats,"image_scalar<ELEMTYPE, IMAGEDIM>::save_histogram_to_txt_file - stats==NULL",pt_error::debug);
	this->stats->save_histogram_to_txt_file(filename, g, reload_hist_from_image, separator);
}

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::save_histogram_to_txt_file(const std::string filename, image_binary<3> *mask_im, int num_buckets) {
	histogram_1D<ELEMTYPE> *tmpHist = this->get_histogram_from_masked_region_3D(mask_im, num_buckets);
	tmpHist->save_histogram_to_txt_file(filename);
}


template <class ELEMTYPE, int IMAGEDIM>
image_scalar<unsigned short, 3>* image_scalar<ELEMTYPE, IMAGEDIM>::create2Dhistogram_3D(image_scalar<ELEMTYPE, IMAGEDIM> *second_image, bool remove_zero_intensity, int num_buckets_a, int num_buckets_b, image_binary<IMAGEDIM>* mask)
{
	if(num_buckets_a<=0){	num_buckets_a = this->get_max()-this->get_min()+1; }
	if(num_buckets_b<=0){	num_buckets_b = second_image->get_max()-second_image->get_min()+1; }

	image_scalar<unsigned short, 3> *hist = new image_scalar<unsigned short, 3>(num_buckets_a,num_buckets_b,1);
	hist->fill(0);

	if(!this->same_size(second_image)){
		pt_error::error("image_scalar<ELEMTYPE, IMAGEDIM>::create2Dhistogram - image size not equal",pt_error::warning);

	}else{
		int this_x;
		int this_y;
		float this_min=this->get_min();
		float this_max=this->get_max();
		float second_min=second_image->get_min();
		float second_max=second_image->get_max();
		float scale_a = float(this->get_max()-this_min)*1.000001/float(num_buckets_a);
		float scale_b = float(second_image->get_max()-second_min)*1.000001/float(num_buckets_b);
		for(int z=0; z<this->datasize[2]; z++){
			for(int y=0; y<this->datasize[1]; y++){
				for(int x=0; x<this->datasize[0]; x++){
					if ( mask==NULL || mask->get_voxel(x,y,z)!=0 ) {
						this_x = (this->get_voxel(x,y,z)-this_min)/scale_a;
						this_y = (second_image->get_voxel(x,y,z)-second_min)/scale_b;
						hist->set_voxel(this_x,this_y,0,hist->get_voxel(this_x,this_y,0)+1);
					}
				}
			}
		}
		if (remove_zero_intensity) {hist->set_voxel((0-this_min)/scale_a, (0-second_min)/scale_b, 0, 0);}
	}
	hist->data_has_changed(true);
	return hist;
}

template <class ELEMTYPE, int IMAGEDIM>
image_scalar<ELEMTYPE, IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::create_slicewise_2Dhistograms_3D(image_scalar<ELEMTYPE, IMAGEDIM> *second_image, int hist_slc_dir, bool remove_zero_intensity, int scale_a, int scale_b){

	if(scale_a<=0){	scale_a = this->get_max(); }
	if(scale_b<=0){	scale_b = second_image->get_max(); }

	image_scalar<ELEMTYPE, IMAGEDIM> *hist;

	if(!this->same_size(second_image)){
		pt_error::error("image_scalar<ELEMTYPE, IMAGEDIM>::create2Dhistogram - image size not equal",pt_error::warning);
		return NULL;
	}else{
		int this_a;
		int this_b;

		if(hist_slc_dir==2){
			hist = new image_scalar<ELEMTYPE, IMAGEDIM>(scale_a,scale_b,this->datasize[2]);
			hist->fill(0);
			for(int z=0; z<this->datasize[2]; z++){
				for(int y=0; y<this->datasize[1]; y++){
					for(int x=0; x<this->datasize[0]; x++){
						this_a = this->get_voxel(x,y,z)*float(scale_a-1)/this->get_max_float();
						this_b = second_image->get_voxel(x,y,z)*float(scale_b-1)/second_image->get_max_float();
						hist->set_voxel(this_a,this_b,z, hist->get_voxel(this_a,this_b,z)+1);
					}
				}
			}
		}else if(hist_slc_dir==1){
			hist = new image_scalar<ELEMTYPE, IMAGEDIM>(scale_a,scale_b,this->datasize[1]);
			hist->fill(0);
			for(int z=0; z<this->datasize[2]; z++){
				for(int y=0; y<this->datasize[1]; y++){
					for(int x=0; x<this->datasize[0]; x++){
						this_a = this->get_voxel(x,y,z)*float(scale_a-1)/this->get_max_float();
						this_b = second_image->get_voxel(x,y,z)*float(scale_b-1)/second_image->get_max_float();
						hist->set_voxel(this_a,this_b,y, hist->get_voxel(this_a,this_b,y)+1);
					}
				}
			}
		}else if(hist_slc_dir==0){
			hist = new image_scalar<ELEMTYPE, IMAGEDIM>(scale_a,scale_b,this->datasize[0]);
			hist->fill(0);
			for(int z=0; z<this->datasize[2]; z++){
				for(int y=0; y<this->datasize[1]; y++){
					for(int x=0; x<this->datasize[0]; x++){
						this_a = this->get_voxel(x,y,z)*float(scale_a-1)/this->get_max_float();
						this_b = second_image->get_voxel(x,y,z)*float(scale_b-1)/second_image->get_max_float();
						hist->set_voxel(this_a,this_b,x, hist->get_voxel(this_a,this_b,x)+1);
					}
				}
			}

		}else{
			pt_error::error("image_scalar<ELEMTYPE, IMAGEDIM>::create2Dhistogram - image directions nor recognized",pt_error::warning);	
		}

		if(remove_zero_intensity){
			for(int z=0; z<hist->datasize[2]; z++){
				hist->set_voxel(0,0,z,0);
			}
		}

	hist->data_has_changed(true);
	return hist;
	}
}

template <class ELEMTYPE, int IMAGEDIM>
image_scalar<unsigned short, 3>* image_scalar<ELEMTYPE, IMAGEDIM>::create2Dhistogram_from_slices(int dir, int slice1, int slice2, bool remove_zero_intensity, int num_buckets_a, int num_buckets_b)
{
	float max1 = this->get_max_in_slice3D(slice1,dir);
	float max2 = this->get_max_in_slice3D(slice2,dir);
	float max = std::max(max1,max2);
	if(num_buckets_a<=0){ num_buckets_a = max1; }
	if(num_buckets_b<=0){ num_buckets_b = max2; }

	image_scalar<unsigned short, 3> *hist_im = new image_scalar<unsigned short, 3>(num_buckets_a,num_buckets_b,1);
	hist_im->fill(0);
	float val_1;
	float val_2;
	int pos1;
	int pos2;
	vector<double> sum_int(num_buckets_a);
	vector<float> num_voxels(num_buckets_a);

	for(int i=0; i<num_buckets_a; i++){
		sum_int[i] =0;
		num_voxels[i] =0;
	}


	for(int v=0; v<this->get_size_by_dim(1); v++){
		for(int u=0; u<this->get_size_by_dim(0); u++){
			val_1 = this->get_voxel_by_dir(u,v,slice1,dir);
			val_2 = this->get_voxel_by_dir(u,v,slice2,dir);
			pos1 = val_1/max*num_buckets_a;
			pos2 = val_2/max*num_buckets_b;
			if(pos1>=num_buckets_a){
				pos1 = num_buckets_a-1;
			}
			if(pos2>=num_buckets_b){
				pos2 = num_buckets_b-1;
			}
			sum_int[pos1] += pos2; 
			num_voxels[pos1] += 1.0;
		}
	}

	cout<<"***********"<<endl;
	for(int i=0; i<num_buckets_a; i++){
		cout<<i<<"\t"<<sum_int[i]/num_voxels[i]<<endl;
	}
	cout<<"***********"<<endl;

	return hist_im;
}

/*
template <class ELEMTYPE, int IMAGEDIM>
image_scalar<unsigned short, 3>* image_scalar<ELEMTYPE, IMAGEDIM>::create2Dhistogram_from_two_outermost_slices(int dir, int num_buckets=200){

	if(scale_a<=0){	scale_a = this->get_max(); }
	if(scale_b<=0){	scale_b = second_image->get_max(); }

	image_scalar<ELEMTYPE, IMAGEDIM> *hist;
}
*/



// The "calculate_T1Map_3D" function is based on following publication... (Chen2006)
// Rapid High-Resolution T1 Mapping by Variable Flip Angles: Accurate and Precise Measurements in the Presence of Radiofrequency Field Inhomogeneity
// Hai-Ling Margaret Cheng and Graham A Wright, Magnetic Resonance in Medicine 55:566ñ574 (2006)

// The "FLASH" signal equation is use din linear form.
// least square based linear regression is used to determine the T1-values..
// Add all volumes to the vector v for simple implementation...

template <class ELEMTYPE, int IMAGEDIM>
image_scalar<ELEMTYPE, IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::calculate_T1Map_3D(vector<image_scalar<ELEMTYPE, IMAGEDIM > *> v, float body_thres, float t1_min, float t1_max)
{
	cout<<"calculate_T1Map_3D..."<<endl;

	int N = v.size();
//	int N = 3;
	if(N<2){
		cout<<"...to few volumes loaded!"<<endl;
		return NULL;
	}else{

	image_scalar<ELEMTYPE, IMAGEDIM>* t1map = new image_scalar<ELEMTYPE, IMAGEDIM>(this);
	t1map->fill(0);

	vnl_vector<float> alpha(N);		//flip angles...
	for( int i=0;i<N;i++){
		alpha(i) = v[i]->meta.get_flip()*pt_PI/180.0;
	}
	cout<<"alpha="<<alpha<<endl;

//	vnl_vector<float> S(N);	 //MR signal... 3*1, printed as 1*3 echo times for the N echoes...
	vnl_vector<float> y(N);
	vnl_vector<float> x(N);
	float TR = v[0]->meta.get_data_float(DCM_TR);
	cout<<"TR="<<TR<<endl;
	float y_mean=0;
	float x_mean=0;
	float Sxx=0;
	float Sxy=0;
	float t1=0;

	for(int k=0; k<t1map->datasize[2]; k++){
		cout<<".";
		for(int j=0; j<t1map->datasize[1]; j++){
			for(int i=0; i<t1map->datasize[0]; i++){

				t1=0;
				if( v[0]->get_voxel(i,j,k)>body_thres ){		//intensity in first image
					for(int l=0; l<N; l++){
						//S(l) = v[l]->get_voxel(i,j,k);
						//y(l) = S(l)/sin(alpha(l));
						//x(l) = S(l)/tan(alpha(l));
						y(l) = v[l]->get_voxel(i,j,k)/sin(alpha(l));
						x(l) = v[l]->get_voxel(i,j,k)/tan(alpha(l));
					}

					y_mean = y.sum()/float(N);
					x_mean = x.sum()/float(N);
					Sxx=0;
					Sxy=0;

					for(int l=0; l<N; l++){
						Sxx += (x(l)-x_mean)*(x(l)-x_mean);
						Sxy += (x(l)-x_mean)*(y(l)-y_mean);
					}
					t1 = -TR/log(Sxy/Sxx);

				}
				if(t1<t1_min){t1=t1_min;}
				if(t1>t1_max){t1=t1_max;}

				t1map->set_voxel(i,j,k,t1);
			}
		}
	}

	cout<<"Done"<<endl;

	t1map->data_has_changed(true);	//true is needed since intensity min/max needs to be recalculated...
	return t1map;
	}
}


 //cannot instantiate abstract class
template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::translate_slices_to_align_coordinates_3D(vector<Vector3D> coords, int dir, ELEMTYPE empty_value, bool unalign)
{
	image_scalar<ELEMTYPE, IMAGEDIM> *tmp = new image_scalar<ELEMTYPE, IMAGEDIM>(this,0);
	tmp->fill(empty_value);

	//uses the first coordinate in vector as reference and aligns all the other coordinates...		
	Vector3D mean_coord = get_mean_Vector3D(coords);
	cout<<"mean_coord="<<mean_coord<<endl;

	mean_coord[0] = int(mean_coord[0]);
	mean_coord[1] = int(mean_coord[1]);
	mean_coord[2] = int(mean_coord[2]);

	int du;
	int dv;
	int slice;

	if(dir==0){
		for(int i=0;i<coords.size();i++){
			du = coords[i][1]-mean_coord[1];
			dv = coords[i][2]-mean_coord[2];
			if(unalign){
				du = -du;
				dv = -dv;
			}
			slice = coords[i][0];
			tmp->fill_region_3D_with_subvolume_image( create_Vector3Dint(slice,0-du,0-dv), this, create_Vector3Dint(slice,0,0), create_Vector3Dint(1,this->ny(),this->nz()), empty_value );
		}
	}else if(dir==1){
		for(int i=0;i<coords.size();i++){
			du = coords[i][0]-mean_coord[0];
			dv = coords[i][2]-mean_coord[2];
			if(unalign){
				du = -du;
				dv = -dv;
			}
			slice = coords[i][1];
			tmp->fill_region_3D_with_subvolume_image( create_Vector3Dint(0-du,slice,0-dv), this, create_Vector3Dint(0,slice,0), create_Vector3Dint(this->nx(),1,this->nz()), empty_value );
		}
	}else{
		for(int i=0;i<coords.size();i++){
			du = coords[i][0]-mean_coord[0];
			dv = coords[i][1]-mean_coord[1];
			if(unalign){
				du = -du;
				dv = -dv;
			}
			slice = coords[i][2];
			tmp->fill_region_3D_with_subvolume_image( create_Vector3Dint(0-du,0-dv,slice), this, create_Vector3Dint(0,0,slice), create_Vector3Dint(this->nx(),this->ny(),1), empty_value );
		}
	}

	copy_data(tmp,this);
	delete tmp;
}




template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::smooth_ITK(Vector3D radius)
{
	typename itk::MeanImageFilter<theImageType,theImageType>::Pointer filter = itk::MeanImageFilter<theImageType,theImageType>::New();

	typename theSizeType r;
	//	r[0] = 1; // radius along x
	//	r[1] = 1; // radius along y
	//	r[2] = 1; // radius along z
	r[0] = radius[0];
	r[1] = radius[1];
	r[2] = radius[2];

	filter->SetRadius(r);
	cout<<"before -->ITK..."<<clock()<<endl;
	filter->SetInput(this->get_image_as_itk_output());
	cout<<"after..."<<clock()<<endl;
	cout<<"before Update()"<<clock()<<endl;
	filter->Update();
	cout<<"after..."<<clock()<<endl;
	cout<<"before --> Pt"<<clock()<<endl;
	this->replicate_itk_to_image(filter->GetOutput());
	cout<<"after..."<<clock()<<endl;
}



template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::smooth_3D(Vector3D r)
{
	cout<<"smooth_3D..."<<endl;
	image_scalar<ELEMTYPE, IMAGEDIM> *res = new image_scalar<ELEMTYPE, IMAGEDIM>(this,0);

	res->fill(0); //not very time efficient... the outer borders would be enough...

	if(r[0]>=0 && r[1]>=0 && r[2]>=0)
	{
		float num_neighbours = (1+2*r[0])*(1+2*r[1])*(1+2*r[2]);
		float sum = 0;
		float mean1 = 0;
		float tmp = 0;
		float limit = 100;
		float num_counted = 0;

//		cout<<"k=";
		for(int k=r[2]; k < this->datasize[2]-r[2]; k++){
//			cout<<k<<",";
			for(int j=r[1]; j < this->datasize[1]-r[1]; j++){
				for(int i=r[0]; i < this->datasize[0]-r[0]; i++){

					sum = 0;
					for(int n=k-r[2]; n<=k+r[2]; n++){
						for(int m=j-r[1]; m<=j+r[1]; m++){
							for(int l=i-r[0]; l<=i+r[0]; l++){
								sum += float(this->get_voxel(l,m,n));
							}
						}
					}
					mean1 = sum/num_neighbours;
					res->set_voxel(i,j,k,mean1);
/*
					sum = 0;
					num_counted = 0;
					for(int n=k-r[2]; n<=k+r[2]; n++){
						for(int m=j-r[1]; m<=j+r[1]; m++){
							for(int l=i-r[0]; l<=i+r[0]; l++){
								tmp = float(this->get_voxel(l,m,n));
								if(abs(tmp-mean1)<limit)
								{
									sum += tmp;
									num_counted++;
								}
							}
						}
					}

					res->set_voxel(i,j,k,sum/num_counted);	//mean2...
*/
				}
			}
		}
	}

	copy_data(res,this);
	delete res;
}

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::smooth_using_filter3D(int std_x, int std_y, int std_z)
{
	filter_gaussian* gauss_x=new filter_gaussian(std_x*2+1,0,std_x);
	filter_gaussian* gauss_y=new filter_gaussian(std_y*2+1,1,std_y);
	filter_gaussian* gauss_z=new filter_gaussian(std_z*2+1,2,std_z);

	cout << "Gauss smoothing in x-direction" << endl;
	this->filter_3D(gauss_x, 1);

	cout << "Gauss smoothing in y-direction" << endl;
	this->filter_3D(gauss_y, 1);

	cout << "Gauss smoothing in z-direction" << endl;
	this->filter_3D(gauss_z, 1);

	delete gauss_x;
	delete gauss_y;
	delete gauss_z;
}


template <class ELEMTYPE, int IMAGEDIM>
image_binary<IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::region_grow_corners_3D(ELEMTYPE min_intensity, ELEMTYPE max_intensity)
{
	queue<Vector3D> s;

	s.push(create_Vector3D(0,					0,						0));
	s.push(create_Vector3D(this->datasize[0]-1,	0,						0));
	s.push(create_Vector3D(0,					this->datasize[1]-1,	0));
	s.push(create_Vector3D(this->datasize[0]-1,	this->datasize[1]-1,	0));

	s.push(create_Vector3D(0,					0,						this->datasize[2]-1));
	s.push(create_Vector3D(this->datasize[0]-1,	0,						this->datasize[2]-1));
	s.push(create_Vector3D(0,					this->datasize[1]-1,	this->datasize[2]-1));
	s.push(create_Vector3D(this->datasize[0]-1,	this->datasize[1]-1,	this->datasize[2]-1));

	return region_grow_3D(s, min_intensity, max_intensity);
}


template <class ELEMTYPE, int IMAGEDIM>
image_binary<IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::region_grow_3D(Vector3D seed, ELEMTYPE min_intensity, ELEMTYPE max_intensity)
{
	queue<Vector3D> s;
	s.push(seed);
	return region_grow_3D(s, min_intensity, max_intensity);
}


template <class ELEMTYPE, int IMAGEDIM>
image_binary<IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::region_grow_3D(image_binary<IMAGEDIM> *seed_image, ELEMTYPE min_intensity, ELEMTYPE max_intensity)
{
	queue<Vector3D> s;

	for(int x=0; x<this->datasize[0]; x++){
		for(int y=0; y<this->datasize[1]; y++){
			for(int z=0; z<this->datasize[2]; z++){
				if(seed_image->get_voxel(x,y,z)){
					s.push(create_Vector3D(x,y,z));
				}
			}
		}
	}
	return region_grow_3D(s, min_intensity, max_intensity);
}

template <class ELEMTYPE, int IMAGEDIM>
image_binary<IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::region_grow_3D(queue<Vector3D> seed_queue, ELEMTYPE min_intensity, ELEMTYPE max_intensity)
{
	image_binary<IMAGEDIM> *res = new image_binary<IMAGEDIM>(this,0);
	res->fill(false);
	//for seed-queue... //res->set_voxel(seed[0],seed[1],seed[2],true);
	int sx = this->datasize[0];
	int sy = this->datasize[1];
	int sz = this->datasize[2];
//	cout<<sx<<" "<<sy<<" "<<sz<<endl;

	Vector3D pos;
	Vector3D pos2;

	ELEMTYPE val;

	while(seed_queue.size()>0){
		pos = seed_queue.front();
		seed_queue.pop();

		for(int x=std::max(0,int(pos[0]-1)); x<=std::min(int(pos[0]+1),sx-1); x++){
			for(int y=std::max(0,int(pos[1]-1)); y<=std::min(int(pos[1]+1),sy-1); y++){
				for(int z=std::max(0,int(pos[2]-1)); z<=std::min(int(pos[2]+1),sz-1); z++){
					val = this->get_voxel(x,y,z);

					if(val>=min_intensity && val<=max_intensity && res->get_voxel(x,y,z)==false){
						pos2[0]=x; pos2[1]=y; pos2[2]=z;
						seed_queue.push(pos2);
						res->set_voxel(x,y,z,true);
					}
				}
			}
		}
	}
	cout<<"region_grow_3D --> Done...(seed_queue.size()="<<seed_queue.size()<<")"<<endl;
	return res;
}

template <class ELEMTYPE, int IMAGEDIM>
image_binary<IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::region_grow_3D_if_equal_or_lower_intensity(queue<Vector3D> seed_queue, ELEMTYPE min_intensity)
{
	image_binary<IMAGEDIM> *res = new image_binary<IMAGEDIM>(this,0);
	res->fill(false);
	int sx = this->datasize[0];
	int sy = this->datasize[1];
	int sz = this->datasize[2];
//	cout<<sx<<" "<<sy<<" "<<sz<<endl;

	Vector3D pos;
	Vector3D pos2;

	ELEMTYPE current_val;
	ELEMTYPE val;

	while(seed_queue.size()>0){
		pos = seed_queue.front();
		seed_queue.pop();

		current_val = this->get_voxel(pos[0],pos[1],pos[2]);
		for(int x=std::max(0,int(pos[0]-1)); x<=std::min(int(pos[0]+1),sx-1); x++){
			for(int y=std::max(0,int(pos[1]-1)); y<=std::min(int(pos[1]+1),sy-1); y++){
				for(int z=std::max(0,int(pos[2]-1)); z<=std::min(int(pos[2]+1),sz-1); z++){
					val = this->get_voxel(x,y,z);

					if(val<=current_val && val>=min_intensity && res->get_voxel(x,y,z)==false){
						pos2[0]=x; pos2[1]=y; pos2[2]=z;
						seed_queue.push(pos2);
						res->set_voxel(x,y,z,true);
					}
				}
			}
		}
	}

	cout<<"region_grow_3D_if_equal_or_lower_intensity --> Done...(seed_queue.size()="<<seed_queue.size()<<")"<<endl;
	return res;
}


template <class ELEMTYPE, int IMAGEDIM>
image_binary<IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::region_grow_3D_if_lower_intensity(image_binary<IMAGEDIM> *seed_image, ELEMTYPE min_intensity)
{
	queue<Vector3D> s;

	for(int x=0; x<this->datasize[0]; x++){
		for(int y=0; y<this->datasize[1]; y++){
			for(int z=0; z<this->datasize[2]; z++){
				if(seed_image->get_voxel(x,y,z)){
					s.push(create_Vector3D(x,y,z));
				}
			}
		}
	}
	return region_grow_3D_if_lower_intensity(s, min_intensity);
}


template <class ELEMTYPE, int IMAGEDIM>
image_binary<IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::region_grow_3D_if_lower_intensity(queue<Vector3D> seed_queue, ELEMTYPE min_intensity)
{
	image_binary<IMAGEDIM> *res = new image_binary<IMAGEDIM>(this,0);
	res->fill(false);
	int sx = this->datasize[0];
	int sy = this->datasize[1];
	int sz = this->datasize[2];
//	cout<<sx<<" "<<sy<<" "<<sz<<endl;

	Vector3D pos;
	Vector3D pos2;

	ELEMTYPE current_val;
	ELEMTYPE val;

	while(seed_queue.size()>0){
		pos = seed_queue.front();
		seed_queue.pop();

		current_val = this->get_voxel(pos[0],pos[1],pos[2]);
		for(int x=std::max(0,int(pos[0]-1)); x<=std::min(int(pos[0]+1),sx-1); x++){
			for(int y=std::max(0,int(pos[1]-1)); y<=std::min(int(pos[1]+1),sy-1); y++){
				for(int z=std::max(0,int(pos[2]-1)); z<=std::min(int(pos[2]+1),sz-1); z++){
					val = this->get_voxel(x,y,z);

					if(val<current_val && val>=min_intensity && res->get_voxel(x,y,z)==false){
						pos2[0]=x; pos2[1]=y; pos2[2]=z;
						seed_queue.push(pos2);
						res->set_voxel(x,y,z,true);
					}
				}
			}
		}
	}

	cout<<"region_grow_3D_if_lower_intensity --> Done...(seed_queue.size()="<<seed_queue.size()<<")"<<endl;
	return res;
}



template <class ELEMTYPE, int IMAGEDIM>
image_binary<3>* image_scalar<ELEMTYPE, IMAGEDIM>::region_grow_3D_using_object_labeling(Vector3D seed, ELEMTYPE min_intensity, ELEMTYPE max_intensity)
{
	queue<Vector3D> s;
	s.push(seed);
	return region_grow_3D_using_object_labeling(s, min_intensity, max_intensity);
}


template <class ELEMTYPE, int IMAGEDIM>
image_binary<3>* image_scalar<ELEMTYPE, IMAGEDIM>::region_grow_3D_using_object_labeling(image_binary<IMAGEDIM> *seed_image, ELEMTYPE min_intensity, ELEMTYPE max_intensity)
{
	queue<Vector3D> s;

	for(int x=0; x<this->datasize[0]; x++){
		for(int y=0; y<this->datasize[1]; y++){
			for(int z=0; z<this->datasize[2]; z++){
				if(seed_image->get_voxel(x,y,z)){
					s.push(create_Vector3D(x,y,z));
				}
			}
		}
	}
	return region_grow_3D_using_object_labeling(s, min_intensity, max_intensity);
}

template <class ELEMTYPE, int IMAGEDIM>
image_binary<3>* image_scalar<ELEMTYPE, IMAGEDIM>::region_grow_3D_using_object_labeling(queue<Vector3D> seed_queue, ELEMTYPE min_intensity, ELEMTYPE max_intensity)
{
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim(0);
	max_v=this->get_size_by_dim(1);
	max_w=this->get_size_by_dim(2);
		
	ELEMTYPE p;//pixel value
	int label,above,up,left;//neighbour labels 
    image_integer<int,IMAGEDIM> * label_image = new image_integer<int,IMAGEDIM> (this);
	int new_label;
	//init labels
	int number_of_objects=1;
	vector<int> changes;
	changes.push_back(0);
	for(w=0; w<max_w; w++) {
		for(v=0; v<max_v; v++) {
			for(u=0; u<max_u; u++) {
				p=this->get_voxel(u,v,w);
				if(p>=min_intensity && p<=max_intensity) {
					above=(w>0)? label_image->get_voxel(u,v,w-1) : 0;
					up=(v>0)? label_image->get_voxel(u,v-1,w) : 0;
					left=(u>0)? label_image->get_voxel(u-1,v,w) : 0;
					while(above!=changes[above])
						above=changes[above];
					while(up!=changes[up])
						up=changes[up];
					while(left!=changes[left])
						left=changes[left];
					new_label=number_of_objects;
					if(above>0 && above<new_label)
						new_label=above;
					if(up>0 && up<new_label)
						new_label=up;
					if(left>0 && left<new_label)
						new_label=left;
					if(above>new_label)
						changes[above]=new_label;
					if(up>new_label)
						changes[up]=new_label;
					if(left>new_label)
						changes[left]=new_label;
					if(new_label==number_of_objects) {
						changes.push_back(number_of_objects);
						number_of_objects++;	
					}
					label_image->set_voxel(u,v,w,new_label);
				}
				else
					label_image->set_voxel(u,v,w,0);
			}
		}
	}
	
	// calculate look-up-table
	int* lut=new int[number_of_objects];
	memset(lut, 0, sizeof(int)*number_of_objects);
	Vector3D pos;
	while(seed_queue.size()>0){
		pos = seed_queue.front();
		seed_queue.pop();
		label=label_image->get_voxel(pos[0], pos[1], pos[2]);
		while(label!=changes[label])
			label=changes[label];
		changes[label]=0;
	}
	for(int i=1; i<number_of_objects; i++) {
		label=i;
		while(label!=changes[label])
			label=changes[label];
		if(label==0)
			lut[i]=1;
		else
			lut[i]=0;
	}
	
    // set new labels from look-up table
	image_binary<IMAGEDIM> * region_image = new image_binary<IMAGEDIM> (label_image);
	for(w=0; w<max_w; w++) {
		for(v=0; v<max_v; v++) {
			for(u=0; u<max_u; u++) {
				label=label_image->get_voxel(u,v,w);
				if (label>0)
					region_image->set_voxel(u,v,w,lut[label]);
			}
		}
	}
	delete[] lut;
	delete label_image;
	return region_image;
}

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::region_grow_robust_in_slice_3D(image_binary<3>* result, Vector2Dint seed, ELEMTYPE min_intensity, ELEMTYPE max_intensity, int nr_accepted_neighbours, int radius, int slice, int dir)
{
	stack<Vector2Dint> s;
	s.push(seed);
	return region_grow_robust_in_slice_3D(result, s, min_intensity, max_intensity, nr_accepted_neighbours, radius, slice, dir);
}

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::region_grow_robust_in_slice_3D(image_binary<3>* result, image_binary<3>* seed_image, ELEMTYPE min_intensity, ELEMTYPE max_intensity, int nr_accepted_neighbours, int radius, int slice, int dir)
{
	stack<Vector2Dint> s;

	for(int u=0; u<seed_image->get_size_by_dim_and_dir(0,dir); u++){
		for(int v=0; v<seed_image->get_size_by_dim_and_dir(1,dir); v++){
			for(int w=0; w<seed_image->get_size_by_dim_and_dir(2,dir); w++){
				if(seed_image->get_voxel_by_dir(u,v,w,dir)){
					s.push(create_Vector2Dint(u,v));
				}
			}
		}
	}
	return region_grow_robust_in_slice_3D(result, s, min_intensity, max_intensity, nr_accepted_neighbours, radius, slice, dir);
}

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::region_grow_robust_in_slice_3D(image_binary<3>* result, stack<Vector2Dint> seeds, ELEMTYPE min_intensity, ELEMTYPE max_intensity, int nr_accepted_neighbours, int radius, int slice, int dir)
{
	pt_error::error_if_false(this->same_size(result), "res must be same size as this in region_grow_robust_in_slice_3D", pt_error::debug);
	cout<<"region_grow_robust_in_slice_3D...slice " << slice << "...";
	
	int su = this->get_size_by_dim_and_dir(0,dir);
	int sv = this->get_size_by_dim_and_dir(1,dir);
	
	ELEMTYPE val;
	image_scalar<unsigned short, 3> *neighb = new image_scalar<unsigned short, 3>(su,sv,1);
	neighb->fill(0);
	for (int u=0; u<su; u++) {
		for (int v=0; v<sv; v++) {
			val = this->get_voxel_by_dir(u,v,slice,dir);
			if (val>=min_intensity && val<=max_intensity) {
				for(int a=std::max(0,int(u-radius)); a<=std::min(int(u+radius),su-1); a++){
					for(int b=std::max(0,int(v-radius)); b<=std::min(int(v+radius),sv-1); b++){
						neighb->set_voxel(a,b,0, neighb->get_voxel(a,b,0)+1);
					}
				}
				neighb->set_voxel(u,v,0,neighb->get_voxel(u,v)-1);
			}
		}
	}

	result->fill_region_3D(dir,slice,slice,false);
	stack<Vector2Dint> s;
	Vector2Dint pos;
	Vector2Dint pos2;
	while (seeds.size()>0) {
		pos = seeds.top();
		seeds.pop();
		val = this->get_voxel_by_dir(pos[0],pos[1],slice,dir);
		if (val>=min_intensity && val<=max_intensity && neighb->get_voxel(pos[0],pos[1])>=nr_accepted_neighbours) {
			result->set_voxel_by_dir(pos[0],pos[1],slice,true,dir); 
			s.push(pos);
		}
	}

	while(s.size()>0){
		pos = s.top();
		s.pop();
		for(int u=std::max(0,pos[0]-1); u<=std::min(pos[0]+1,su-1); u++){
			for(int v=std::max(0,pos[1]-1); v<=std::min(pos[1]+1,sv-1); v++){
				val = this->get_voxel_by_dir(u,v,slice,dir);
				if(result->get_voxel_by_dir(u,v,slice,dir)==false && val>=min_intensity && val<=max_intensity && neighb->get_voxel(u,v)>=nr_accepted_neighbours){
					pos2[0]=u; pos2[1]=v;
					s.push(pos2);
					result->set_voxel_by_dir(u,v,slice,true,dir);	
				}
			}//v
		}//u
	}//end while

	cout<<"DONE"<<endl;
	delete neighb;
}





template <class ELEMTYPE, int IMAGEDIM>
image_binary<IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::region_grow_robust_3D(stack<Vector3Dint> seeds, ELEMTYPE min_intensity, ELEMTYPE max_intensity, int nr_accepted_neighbours, int radius)
{
	image_binary<3> *res = new image_binary<3>(this,0); 
	res->fill(false);

	stack<Vector3Dint> s;
	Vector3Dint pos;
	Vector3Dint pos2;

	while(seeds.size()>0){
		pos = seeds.top();
		seeds.pop();
		res->set_voxel(pos[0],pos[1],pos[2],true);
		s.push(pos);
	}

	int num_acc_nb=0;
	ELEMTYPE val;

	while(s.size()>0){
		pos = s.top();
		s.pop();
		for(int z=std::max(0,pos[2]-1); z<=std::min(pos[2]+1,this->nz()-1); z++){
//			cout<<z<<"/"<<min(pos[2]+1,this->nz()-1)<<" ";
			for(int y=std::max(0,pos[1]-1); y<=std::min(pos[1]+1,this->ny()-1); y++){
				for(int x=std::max(0,pos[0]-1); x<=std::min(pos[0]+1,this->nx()-1); x++){
					val = this->get_voxel(x,y,z);
					if(res->get_voxel(x,y,z)==false && val>=min_intensity && val<=max_intensity){

						num_acc_nb=0;
						for(int c=std::max(0,int(z-radius)); c<=std::min(int(z+radius),this->nz()-1); c++){
							for(int b=std::max(0,int(y-radius)); b<=std::min(int(y+radius),this->ny()-1); b++){
								for(int a=std::max(0,int(x-radius)); a<=std::min(int(x+radius),this->nx()-1); a++){
									val = this->get_voxel(a,b,c);
									if(val>=min_intensity && val<=max_intensity){
										num_acc_nb++;
									}
								}
							}
						}
						if(num_acc_nb >= nr_accepted_neighbours){
							pos2[0]=x; pos2[1]=y; pos2[2]=z;
							s.push(pos2);
							res->set_voxel(x,y,z,true);	
						}
					}
				}//z
			}//y
		}//x
	}//end while

	cout<<"DONE"<<endl;
	return res;
}

template <class ELEMTYPE, int IMAGEDIM>
image_binary<IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::region_grow_robust_3D(Vector3Dint seed, ELEMTYPE min_intensity, ELEMTYPE max_intensity, int nr_accepted_neighbours, int radius)
{
	cout<<"region_grow_robust_3D...";
	stack<Vector3Dint> s;
	s.push(seed);

	return region_grow_robust_3D(s, min_intensity, max_intensity, nr_accepted_neighbours, radius);
}

template <class ELEMTYPE, int IMAGEDIM>
image_binary<IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::region_grow_robust_3D(image_binary<3>* seed_im, ELEMTYPE min_intensity, ELEMTYPE max_intensity, int nr_accepted_neighbours, int radius)
{
	cout<<"region_grow_robust_3D...";
	stack<Vector3Dint> s = seed_im->get_voxel_positions_from_values_greater_than_3D_as_stack(0);

	return region_grow_robust_3D(s, min_intensity, max_intensity, nr_accepted_neighbours, radius);
}



template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::region_grow_declining_in_slice_3D(image_binary<3>* result, image_binary<3>* seed_image, ELEMTYPE min_intensity, int slice, int dir)
{
	stack<Vector2Dint> s;

	for(int u=0; u<seed_image->get_size_by_dim_and_dir(0,dir); u++){
		for(int v=0; v<seed_image->get_size_by_dim_and_dir(1,dir); v++){
			if(seed_image->get_voxel_by_dir(u,v,slice,dir)){
				s.push(create_Vector2Dint(u,v));
			}
		}
	}
	return region_grow_declining_in_slice_3D(result, s, min_intensity, slice, dir);
}


template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::region_grow_declining_in_slice_3D(image_binary<3>* result, stack<Vector2Dint> seeds, ELEMTYPE min_intensity, int slice, int dir)
{
	pt_error::error_if_false(this->same_size(result), "res must be same size as this in region_grow_declining_in_slice_3D", pt_error::debug);
//	cout<<"region_grow_declining_in_slice_3D...slice " << slice << "...";
	
	int su = this->get_size_by_dim_and_dir(0,dir);
	int sv = this->get_size_by_dim_and_dir(1,dir);
	
	ELEMTYPE val;
	ELEMTYPE current_val;

	result->fill_region_3D(dir,slice,slice,false);
	stack<Vector2Dint> seeds2;
	Vector2Dint pos;
	Vector2Dint pos2;
	while(seeds.size()>0){
		pos = seeds.top();
		seeds.pop();
		result->set_voxel_by_dir(pos[0],pos[1],slice,true,dir); 
		seeds2.push(pos);
	}

	while(seeds2.size()>0){
		pos = seeds2.top();
		seeds2.pop();
		current_val = this->get_voxel_by_dir(pos[0],pos[1],slice,dir);
		for(int u=std::max(0,pos[0]-1); u<=std::min(pos[0]+1,su-1); u++){
			for(int v=std::max(0,pos[1]-1); v<=std::min(pos[1]+1,sv-1); v++){
				val = this->get_voxel_by_dir(u,v,slice,dir);
				if(result->get_voxel_by_dir(u,v,slice,dir)==false && val>=min_intensity && val<current_val){
					pos2[0]=u; pos2[1]=v;
					seeds2.push(pos2);
					result->set_voxel_by_dir(u,v,slice,true,dir);	
				}
			}//v
		}//u
	}//end while

//	cout<<"DONE"<<endl;
}

template <class ELEMTYPE, int IMAGEDIM>
float image_scalar<ELEMTYPE, IMAGEDIM>::get_moment(int k, int l, int m, ELEMTYPE lower_int_limit, ELEMTYPE upper_int_limit,  SPACE_TYPE type)
{
//	Vector3D res = create_Vector3D(0,0,0);
	float res = 0;
//	float num_voxels=0;

	ELEMTYPE val;
	Vector3D pos;
	for(int z=0;z<this->nz();z++){
		for(int y=0;y<this->ny();y++){
			for(int x=0;x<this->nx();x++){
				val = this->get_voxel(x,y,z);
				if( val >= lower_int_limit && val <= upper_int_limit){
					pos = this->get_physical_pos_or_voxel_pos(x,y,z,type);
					res += pow(pos[0],k)*pow(pos[1],l)*pow(pos[2],m)*val;
				}
			}
		}
	}
	return res;
}

template <class ELEMTYPE, int IMAGEDIM>
float image_scalar<ELEMTYPE, IMAGEDIM>::get_moment_central(int k, int l, int m, ELEMTYPE lower_int_limit, ELEMTYPE upper_int_limit,  SPACE_TYPE type)
{
//	Vector3D res = create_Vector3D(0,0,0);
	float res=0;
	Vector3D cg = this->get_center_of_gravity_binary_from_range(lower_int_limit, upper_int_limit,  type);


	ELEMTYPE val;
	Vector3D pos;
	for(int z=0;z<this->nz();z++){
		for(int y=0;y<this->ny();y++){
			for(int x=0;x<this->nx();x++){
				val = this->get_voxel(x,y,z);
				if( val >= float(lower_int_limit) && val <= float(upper_int_limit)){
					pos = this->get_physical_pos_or_voxel_pos(x,y,z,type);
					res += pow(pos[0]-cg[0],k)*pow(pos[1]-cg[1],l)*pow(pos[2]-cg[2],m)*val;
				}
			}
		}
	}
	return res;
}

template <class ELEMTYPE, int IMAGEDIM>
Vector3D image_scalar<ELEMTYPE, IMAGEDIM>::get_center_of_gravity(SPACE_TYPE type)
{
	return this->get_center_of_gravity_from_range(std::numeric_limits<ELEMTYPE>::min(),std::numeric_limits<ELEMTYPE>::max(),type);
}

template <class ELEMTYPE, int IMAGEDIM>
Vector3D image_scalar<ELEMTYPE, IMAGEDIM>::get_center_of_gravity_from_range(ELEMTYPE lower_int_limit, ELEMTYPE upper_int_limit,  SPACE_TYPE type)
{
	Vector3D res = create_Vector3D(-1,-1,-1);
	Vector3D cg = create_Vector3D(0,0,0);
	double sum=0;

	ELEMTYPE val;
	for(int z=0;z<this->nz();z++){
		for(int y=0;y<this->ny();y++){
			for(int x=0;x<this->nx();x++){
				val = this->get_voxel(x,y,z);
				if(val >= lower_int_limit && val <= upper_int_limit){
					cg[0] += val*x;
					cg[1] += val*y;
					cg[2] += val*z;
					sum += val;
				}
			}
		}
	}
	if(sum>0){
		res = cg / sum;

		if(type == PHYSICAL_SPACE){
			res = this->get_physical_pos_for_voxel(res[0],res[1],res[2]);
		}
	}
	return res;
}

template <class ELEMTYPE, int IMAGEDIM>
Vector3D image_scalar<ELEMTYPE, IMAGEDIM>::get_center_of_gravity_binary_from_range(ELEMTYPE lower_int_limit, ELEMTYPE upper_int_limit,  SPACE_TYPE type)
{
	Vector3D res = create_Vector3D(-1,-1,-1);
	Vector3D cg = create_Vector3D(0,0,0);
	float num_voxels=0;

	for(int z=0;z<this->nz();z++){
		for(int y=0;y<this->ny();y++){
			for(int x=0;x<this->nx();x++){
				if(this->get_voxel(x,y,z) >= lower_int_limit && this->get_voxel(x,y,z) <= upper_int_limit){
					cg[0] += x;
					cg[1] += y;
					cg[2] += z;
					num_voxels++;
				}
			}
		}
	}
	if(num_voxels>0){
		res = cg / num_voxels;

		if(type == PHYSICAL_SPACE){
			res = this->get_physical_pos_for_voxel(res[0],res[1],res[2]);
		}
	}
	return res;
}

template <class ELEMTYPE, int IMAGEDIM>
Vector3D image_scalar<ELEMTYPE, IMAGEDIM>::get_in_slice_center_of_gravity_in_dir(int dir, int slice, ELEMTYPE lower_int_limit, ELEMTYPE upper_int_limit,  SPACE_TYPE type)
{
	Vector3D res = create_Vector3D(-1,-1,-1);
	Vector3D cg = create_Vector3D(0,0,0);
	float num_voxels=0;

	if(type == VOXEL_SPACE){
		if(dir==0){
			for(int z=0;z<this->nz();z++){
				for(int y=0;y<this->ny();y++){
					if(this->get_voxel(slice,y,z) >= lower_int_limit && this->get_voxel(slice,y,z) <= upper_int_limit){
						cg[1] += y;
						cg[2] += z;
						num_voxels++;
					}
				}
			}
			if(num_voxels>0){
				res = cg / num_voxels;
				res[0]=slice;
			}

		}else if(dir==1){
			for(int z=0;z<this->nz();z++){
				for(int x=0;x<this->nx();x++){
					if(this->get_voxel(x,slice,z) >= lower_int_limit && this->get_voxel(x,slice,z) <= upper_int_limit){
						cg[0] += x;
						cg[2] += z;
						num_voxels++;
					}
				}
			}
			if(num_voxels>0){
				res = cg / num_voxels;
				res[1]=slice;
			}
		}else{ //dir == 2
			for(int y=0;y<this->ny();y++){
				for(int x=0;x<this->nx();x++){
					if(this->get_voxel(x,y,slice) >= lower_int_limit && this->get_voxel(x,y,slice) <= upper_int_limit){
						cg[0] += x;
						cg[1] += y;
						num_voxels++;
					}
				}
			}
			if(num_voxels>0){
				res = cg / num_voxels;
				res[2]=slice;
			}
		}

	// ----------------------------------- PHYSICAL_SPACE --------------------------------------
	}else if(type == PHYSICAL_SPACE){
		if(dir==0){
			for(int z=0;z<this->nz();z++){
				for(int y=0;y<this->ny();y++){
					if(this->get_voxel(slice,y,z) >= lower_int_limit && this->get_voxel(slice,y,z) <= upper_int_limit){
						cg += this->get_physical_pos_for_voxel(slice,y,z);
						num_voxels++;
					}
				}
			}
			if(num_voxels>0){
				res = cg / num_voxels;
			}
		}else if(dir==1){
			for(int z=0;z<this->nz();z++){
				for(int x=0;x<this->nx();x++){
					if(this->get_voxel(x,slice,z) >= lower_int_limit && this->get_voxel(x,slice,z) <= upper_int_limit){
						cg += this->get_physical_pos_for_voxel(x,slice,z);
						num_voxels++;
					}
				}
			}
			if(num_voxels>0){
				res = cg / num_voxels;
			}
		}else{ //dir==2
			for(int y=0;y<this->ny();y++){
				for(int x=0;x<this->nx();x++){
					if(this->get_voxel(x,y,slice) >= lower_int_limit && this->get_voxel(x,y,slice) <= upper_int_limit){
						cg += this->get_physical_pos_for_voxel(x,y,slice);
						num_voxels++;
					}
				}
			}
			if(num_voxels>0){
				res = cg / num_voxels;
			}
		}

	}	//SPACE_TYPE

	return res;
}

template <class ELEMTYPE, int IMAGEDIM>
vector<Vector3D> image_scalar<ELEMTYPE, IMAGEDIM>::get_in_slice_center_of_gravities_in_dir(int dir, ELEMTYPE lower_int_limit, ELEMTYPE upper_int_limit, SPACE_TYPE type)
{
	vector<Vector3D> cg_points;
	Vector3D tmp;
	Vector3D tmp2 = create_Vector3D(-1,-1,-1);

	for(int s=0;s<this->get_size_by_dim(dir);s++){
		tmp = this->get_in_slice_center_of_gravity_in_dir(dir, s, lower_int_limit, upper_int_limit, type);
		if(tmp != tmp2){
			cg_points.push_back(tmp);
		}
	}

	return cg_points;
}

template <class ELEMTYPE, int IMAGEDIM>
vector<Vector3D> image_scalar<ELEMTYPE, IMAGEDIM>::get_positions_of_voxels_with_value_between(ELEMTYPE from_val, ELEMTYPE to_val, SPACE_TYPE st)
{
	vector<Vector3D> points;
	ELEMTYPE val;

	if(st == VOXEL_SPACE){
		for(int z=0;z<this->nz();z++){
			for(int y=0;y<this->ny();y++){
				for(int x=0;x<this->nx();x++){
					val = this->get_voxel(x,y,z);
					if( val >= from_val && val <= to_val){
						points.push_back(create_Vector3D(x,y,z));
					}
				}
			}
		}
	}else{
		for(int z=0;z<this->nz();z++){
			for(int y=0;y<this->ny();y++){
				for(int x=0;x<this->nx();x++){
					val = this->get_voxel(x,y,z);
					if( val >= from_val && val <= to_val){
						points.push_back(this->get_physical_pos_for_voxel(x,y,z)); //????
					}
				}
			}
		}
	}

	return points;
}


template <class ELEMTYPE, int IMAGEDIM>
Vector3D image_scalar<ELEMTYPE, IMAGEDIM>::get_pos_of_highest_value_in_region(int x1, int y1, int z1, int x2, int y2, int z2, ELEMTYPE upper_limit)
{
	ELEMTYPE max = std::numeric_limits<ELEMTYPE>::min();
	Vector3D max_pos = create_Vector3D(-1,-1,-1);
	ELEMTYPE val;

	for(int z=z1;z<=z2;z++){
		for(int y=y1;y<=y2;y++){
			for(int x=x1;x<=x2;x++){
				val = this->get_voxel(x,y,z);
				if(val<upper_limit && val>max){
					max = val; 
					max_pos = create_Vector3D(x,y,z);
				}
			}
		}
	}

	return max_pos;
}

template <class ELEMTYPE, int IMAGEDIM>
Vector3D image_scalar<ELEMTYPE, IMAGEDIM>::get_pos_of_highest_value_in_region(Vector3D from_pos, Vector3D to_pos, ELEMTYPE upper_limit)
{
	return get_pos_of_highest_value_in_region(from_pos[0],from_pos[1],from_pos[2],to_pos[0],to_pos[1],to_pos[2], upper_limit);
}

template <class ELEMTYPE, int IMAGEDIM>
Vector3D image_scalar<ELEMTYPE, IMAGEDIM>::get_pos_of_highest_value(ELEMTYPE upper_limit)
{
	return get_pos_of_highest_value_in_region(0,0,0,this->nx()-1,this->ny()-1,this->nz()-1, upper_limit);
}

template <class ELEMTYPE, int IMAGEDIM>
Vector3D image_scalar<ELEMTYPE, IMAGEDIM>::get_pos_of_lowest_value_in_region(int x1, int y1, int z1, int x2, int y2, int z2, ELEMTYPE lower_limit)
{
	ELEMTYPE min = std::numeric_limits<ELEMTYPE>::max();
	Vector3D min_pos = create_Vector3D(-1,-1,-1);
	ELEMTYPE val;

	for(int z=z1;z<=z2;z++){
		for(int y=y1;y<=y2;y++){
			for(int x=x1;x<=x2;x++){
				val = this->get_voxel(x,y,z);
				if(val>lower_limit && val<min){
					min = val; 
					min_pos = create_Vector3D(x,y,z);
				}
			}
		}
	}

	return min_pos;
}

template <class ELEMTYPE, int IMAGEDIM>
Vector3D image_scalar<ELEMTYPE, IMAGEDIM>::get_pos_of_lowest_value_in_region(Vector3D pos, Vector3D to_pos, ELEMTYPE lower_limit)
{
	return get_pos_of_lowest_value_in_region(pos[0],pos[1],pos[2],to_pos[0],to_pos[1],to_pos[2],lower_limit);
}

template <class ELEMTYPE, int IMAGEDIM>
Vector3D image_scalar<ELEMTYPE, IMAGEDIM>::get_pos_of_lowest_value(ELEMTYPE lower_limit)
{
	return get_pos_of_lowest_value_in_region(0,0,0,this->nx()-1,this->ny()-1,this->nz()-1,lower_limit);
}

template <class ELEMTYPE, int IMAGEDIM>
vector<Vector3Dint> image_scalar<ELEMTYPE, IMAGEDIM>::get_voxel_positions_from_values_greater_than_3D(ELEMTYPE val)
{
	vector<Vector3Dint> v;

	for(int x=0; x<this->nx(); x++){
		for(int y=0; y<this->ny(); y++){
			for(int z=0; z<this->nz(); z++){
				if(this->get_voxel(x,y,z)>val){
					v.push_back(create_Vector3Dint(x,y,z));
				}
			}
		}
	}

	return v;
}

template <class ELEMTYPE, int IMAGEDIM>
stack<Vector3Dint> image_scalar<ELEMTYPE, IMAGEDIM>::get_voxel_positions_from_values_greater_than_3D_as_stack(ELEMTYPE val)
{
	stack<Vector3Dint> s;
	for(int x=0; x<this->nx(); x++){
		for(int y=0; y<this->ny(); y++){
			for(int z=0; z<this->nz(); z++){
				if(this->get_voxel(x,y,z)>val){
					s.push(create_Vector3Dint(x,y,z));
				}
			}
		}
	}
	cout<<"s.size()="<<s.size()<<endl;
	return s;
}

template <class ELEMTYPE, int IMAGEDIM>
vector<Vector3Dint> image_scalar<ELEMTYPE, IMAGEDIM>::get_voxel_positions_in_slice_from_values_greater_than_3D(int slice, int dir, ELEMTYPE val)
{
	vector<Vector3Dint> v;
	if(dir==2){
		for(int y=0; y<this->ny(); y++){
			for(int x=0; x<this->nx(); x++){
				if(this->get_voxel(x,y,slice)>val){
					v.push_back(create_Vector3Dint(x,y,slice));
				}
			}
		}
	}else if(dir==1){
		for(int z=0; z<this->nz(); z++){
			for(int x=0; x<this->nx(); x++){
				if(this->get_voxel(x,slice,z)>val){
					v.push_back(create_Vector3Dint(x,slice,z));
				}
			}
		}
	}else{
		for(int z=0; z<this->nz(); z++){
			for(int y=0; y<this->ny(); y++){
				if(this->get_voxel(slice,y,z)>val){
					v.push_back(create_Vector3Dint(slice,y,z));
				}
			}
		}
	}

	return v;
}


template <class ELEMTYPE, int IMAGEDIM>
image_scalar<ELEMTYPE, IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::correct_inclined_object_slicewise_after_cg_line(int dir, line3D cg_line, SPACE_TYPE type)
{
	image_scalar<unsigned short,3> *res = new image_scalar<unsigned short,3>(this,0);
	res->fill(0);
	int this_da=0;
	int a_new=0;

	if(type == VOXEL_SPACE && dir ==0){
		for(int y=0;y<this->ny();y++){
			this_da = int( cg_line.get_direction()[0]*(y-this->ny()/2) );
//			cout<<"y="<<y<<" this_da="<<this_da<<endl;
			for(int x=0;x<this->nx();x++){
				a_new = x-this_da;
				if( a_new >=0 && a_new<this->nx()){
					for(int z=0;z<this->nz();z++){
						res->set_voxel(a_new,y,z,this->get_voxel(x,y,z));
					}
				}
			}
		}
	}else{
		pt_error::error("correct_inclined_object_slicewise_after_cg_line... for these arguments... not implemented yet...",pt_error::debug);
	}
	return res;
}

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::fit_gaussian_2D_to_this_image_2D(gaussian_2d* g) // Will only converge with initial parameters of gaussian_2d close to optimal
{
/*
	fit_gaussian_2d_cost_function<ELEMTYPE,IMAGEDIM> f = fit_gaussian_2d_cost_function<ELEMTYPE,IMAGEDIM>(this,g);
	vnl_levenberg_marquardt vlm = vnl_levenberg_marquardt(f);
	
	vnl_vector<double> x(6);
	x[0]=g->amplitude;
	x[1]=g->center_x;
	x[2]=g->center_y;
	x[3]=g->sigma_u;
	x[4]=g->sigma_v;
	x[5]=g->phi;
	cout<<"Initial x="<<x<<endl;
	vlm.minimize_without_gradient(x);
	cout<<"Final x="<<x<<endl;
	*/
}


template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::gradientFilter2D(int dir)
{
	filter_central_difference_magn_2d* diff_2d = new filter_central_difference_magn_2d(dir);
	this->filter_3D(diff_2d);
}


template <class ELEMTYPE, int IMAGEDIM>
image_scalar<ELEMTYPE, 3>* image_scalar<ELEMTYPE, IMAGEDIM>::create_projection_3D(int dir, PROJECTION_MODE PROJ)
{ //enum PROJECTION_MODE {PROJ_MEAN, PROJ_MAX, PROJ_MIN}; 

	image_scalar<ELEMTYPE, 3>* res = new image_scalar<ELEMTYPE, 3>(); 
	res->set_parameters(this); //copy rotation and size infor to tmp image first...

	//JK change this to 2D later, when there is time for 3D-->2D verification...

	int dx = this->get_size_by_dim(0);
	int dy = this->get_size_by_dim(1);
	int dz = this->get_size_by_dim(2);

	float line_max=0;
	float line_sum=0;
	float line_min=0;


	switch(dir){
	case 0:
		res->initialize_dataset(dz,dy,1);

		for(int z=0; z<this->datasize[2]; z++){		
			for(int y=0; y<this->datasize[1]; y++){
				if(PROJ==PROJ_MAX){
					line_max=std::numeric_limits<ELEMTYPE>::min();
					for(int x=0; x<this->datasize[0]; x++){
						line_max = std::max(line_max,float(this->get_voxel(x,y,z)));
					}
					res->set_voxel(z,y,0,line_max);
				}
				else if(PROJ==PROJ_MEAN){
					line_sum=0;
					for(int x=0; x<this->datasize[0]; x++){
						line_sum += float( this->get_voxel(x,y,z) );
					}
					res->set_voxel(z,y,0,line_sum /float(this->datasize[0]));
				}
				else if(PROJ==PROJ_MIN){
					line_min=std::numeric_limits<ELEMTYPE>::max();
					for(int x=0; x<this->datasize[0]; x++){
						line_min = std::min(line_min,float(this->get_voxel(x,y,z)));
					}
					res->set_voxel(z,y,0,line_min);
				}
			}
		}
		break;
	case 1: 
		res->initialize_dataset(dx,dz,1);

		for(int z=0; z<this->datasize[2]; z++){		
			for(int x=0; x<this->datasize[0]; x++){
				if(PROJ==PROJ_MAX){
					line_max=std::numeric_limits<ELEMTYPE>::min();
					for(int y=0; y<this->datasize[1]; y++){
						line_max = std::max(line_max,float(this->get_voxel(x,y,z)));
					}
					res->set_voxel(x,z,0,line_max);
				}
				else if(PROJ==PROJ_MEAN){
					line_sum=0;
					for(int y=0; y<this->datasize[1]; y++){
						line_sum += float( this->get_voxel(x,y,z) );
					}
					res->set_voxel(x,z,0,line_sum /float(this->datasize[1]));
				}
				if(PROJ==PROJ_MIN){
					line_min=std::numeric_limits<ELEMTYPE>::max();
					for(int y=0; y<this->datasize[1]; y++){
						line_min = std::min(line_min,float(this->get_voxel(x,y,z)));
					}
					res->set_voxel(x,z,0,line_min);
				}
			}
		}
		break;
	case 2: 
		res->initialize_dataset(dx,dy,1);

		for(int y=0; y<this->datasize[1]; y++){		
			for(int x=0; x<this->datasize[0]; x++){
				if(PROJ==PROJ_MAX){
					line_max=std::numeric_limits<ELEMTYPE>::min();
					for(int z=0; z<this->datasize[2]; z++){
						line_max = std::max(line_max,float(this->get_voxel(x,y,z)));
					}
					res->set_voxel(x,y,0,line_max);
				}
				else if(PROJ==PROJ_MEAN){
					line_sum=0;
					for(int z=0; z<this->datasize[2]; z++){
						line_sum += float( this->get_voxel(x,y,z) );
					}
					res->set_voxel(x,y,0,line_sum /float(this->datasize[2]));
				}
				if(PROJ==PROJ_MIN){
					line_min=std::numeric_limits<ELEMTYPE>::max();
					for(int z=0; z<this->datasize[2]; z++){
						line_min = std::min(line_min,float(this->get_voxel(x,y,z)));
					}
					res->set_voxel(x,y,0,line_min);
				}
			}
		}
		break;
	default:
		pt_error::error("create_projection-->strange direction...",pt_error::debug);
		break;
	}
	return res;
}


template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::calculate_TP_FP_Udupa_3D(float &tp, float &fp, float &udupa, image_scalar<ELEMTYPE, IMAGEDIM>* ground_truth, ELEMTYPE gt_obj_val, ELEMTYPE this_obj_val)
{
	pt_error::error_if_false(this->same_size(ground_truth)," calculate_TP_FP_Udupa-->NOT same size...",pt_error::debug);

	float any_object=0;
	float hit=0;
	float over=0;
	float under=0;
	float nr_in_gt=0;

	bool val;
	bool gt;

	for(int z=0; z<this->datasize[2]; z++){
		for(int y=0; y<this->datasize[1]; y++){
			for(int x=0; x<this->datasize[0]; x++){
				val = this->get_voxel(x,y,z)>0?true:false;
				gt = ground_truth->get_voxel(x,y,z)>0?true:false;

				if(val&&gt){
					hit++;
					any_object++;
					nr_in_gt++;
				}else if(val){
					over++;
					any_object++;
				}else if(gt){
					under++;
					any_object++;
					nr_in_gt++;
				}
			}
		}
	}

	tp = hit/nr_in_gt;
	fp = over/nr_in_gt;
	udupa = hit/any_object;
}

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::calculate_FP_FN_images(image_scalar<ELEMTYPE, IMAGEDIM>* ground_truth, image_binary<IMAGEDIM>* fp_im, image_binary<IMAGEDIM>* fn_im, ELEMTYPE gt_obj_val, ELEMTYPE this_obj_val)
{
	pt_error::error_if_false(this->same_size(ground_truth)&&this->same_size(fp_im)&&this->same_size(fn_im)," calculate_FP_FN_images-->NOT same size...",pt_error::debug);

	bool val;
	bool gt;

	for(int z=0; z<this->datasize[2]; z++){
		for(int y=0; y<this->datasize[1]; y++){
			for(int x=0; x<this->datasize[0]; x++){
				val = this->get_voxel(x,y,z)==gt_obj_val?true:false;
				gt = ground_truth->get_voxel(x,y,z)==this_obj_val?true:false;
				if( val&&(!gt) ){
					fp_im->set_voxel(x,y,z,1);
				}else if( (!val)&&gt ){
					fn_im->set_voxel(x,y,z,1);
				}
			}
		}
	}
}

template <class ELEMTYPE, int IMAGEDIM>
Vector3D image_scalar<ELEMTYPE, IMAGEDIM>::get_pos_of_max_grad_mag_in_region_voxel( Vector3D center, Vector3D radius, GRAD_MAG_TYPE type )
{
	if ( ! this->is_voxelpos_within_image_3D( center ) )
	{	// the point is not within the image
		pt_error::error("get_pos_of_type_in_region_voxel--> point is not within image",pt_error::debug);		
		return center; //is this useful?
	}

	image_scalar<ELEMTYPE, IMAGEDIM> * res = new image_scalar<ELEMTYPE, IMAGEDIM>( this, 0 );
	res->fill( 0 ); //full image size... for simplicity...

	float center_grad_mag = grad_mag_voxel( center, type );
	
	
	Vector3Dint start = center - radius;
	Vector3Dint end = center + radius;
	
	for ( int i = 0; i < 3; i++ )
	{	// set start and end to be within the image
		start[i] = start[i] < 0 ? 0 : start[i];
		end[i] = end[i] >= this->datasize[i] ? ( this->datasize[i] -  1 ) : end[i];
	}

	Vector3D pos = center;	// return center position if no other is found

	for ( int z = start[2]; z <= end[2]; z++ )
	{
		for ( int y = start[1]; y <= end[1]; y++ )
		{
			for ( int x = start[0]; x <= end[0]; x++ )
			{
				Vector3Dint current = create_Vector3Dint( x, y, z );
				Vector3Dint center_as_int = center;

				float weight = weight_of_type( center_as_int, current, CHESSBOARD );

				float current_grad_mag = grad_mag_voxel( x, y, z, type );
				
				float current_grad_mag_weighted = 0;
				
				if ( weight != 0 )
					{ current_grad_mag_weighted = current_grad_mag / weight; }
				else
					{ current_grad_mag_weighted = current_grad_mag; }	// weight == 0 means the center of the region
				
				if ( current_grad_mag_weighted > center_grad_mag )
				{ 
					pos = create_Vector3Dint( x, y, z );
				}
				
				res->set_voxel( x, y, z, floor( current_grad_mag_weighted ) );
			
			}
		}
	}
	
	//res->set_voxel( pos[0], pos[1], pos[2], 511 );
	datamanagement.add( res );

	// tag bort !!
	//pos[2] = center[2];

	return pos;
}

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::fill_image_with_bias_field_data3D(bias_poly<3> b)
{
	for(int z=0; z<this->datasize[2]; z++){
		for(int y=0; y<this->datasize[1]; y++){
			for(int x=0; x<this->datasize[0]; x++){
				this->set_voxel(x,y,z,b.eval3D(x,y,z));
			}
		}
	}

}

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::fill_image_with_gaussian_values_centered_2D(int dir, gaussian g)
{
	Vector3Dint center;
	for(int i=0; i<3; i++){
		center[i] = this->get_size_by_dim_and_dir(i,dir)/2;
	}

	Vector3Dint diff;
	diff[2]=0;

	for(int w=0; w<this->get_size_by_dim_and_dir(2,dir); w++){
		for(int v=0; v<this->get_size_by_dim_and_dir(1,dir); v++){
			diff[1]=center[1]-v;
			for(int u=0; u<this->get_size_by_dim_and_dir(0,dir); u++){
				diff[0]=center[0]-u;
				this->set_voxel_by_dir(u,v,w,g.evaluate_at(magnitude(diff)),dir);
			}
		}
	}
}

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::fill_image_with_gaussian_values_centered_2D(int dir, float ampl, float sigma_in_voxels)
{
	this->fill_image_with_gaussian_values_centered_2D(dir, gaussian(ampl,0,sigma_in_voxels));
}

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::fill_image_with_gaussian_values_2D(int dir, float ampl, float sigma_in_voxels, int center_u, int center_v)
{
	Vector3Dint diff;
	diff[2]=0;
	gaussian g = gaussian(ampl,0,sigma_in_voxels);
	for(int w=0; w<this->get_size_by_dim_and_dir(2,dir); w++){
		for(int v=0; v<this->get_size_by_dim_and_dir(1,dir); v++){
			diff[1]=center_v-v;
			for(int u=0; u<this->get_size_by_dim_and_dir(0,dir); u++){
				diff[0]=center_u-u;
				this->set_voxel_by_dir(u,v,w,g.evaluate_at(magnitude(diff)),dir);
			}
		}
	}
}


template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::load_dataset_from_VTK_file(string file_path)
{
//	cout<<"Warning... no file loaded...."<<endl;
//	cout<<"ELEMTYPE=("<<string(typeid(ELEMTYPE).name())<<")"<<endl;
	
	if(file_exists(file_path)){
		typename theReaderType::Pointer r = theReaderType::New();
//		typename theReaderType22::Pointer r = theReaderType22::New();
//		typename theScalarReaderType::Pointer r = theScalarReaderType::New();

		itk::VTKImageIO::Pointer VTKIO = itk::VTKImageIO::New();
		r->SetFileName(file_path.c_str());
		r->SetImageIO( VTKIO );

		typename theImagePointer image = theImageType::New();
		image = r->GetOutput();
		r->Update();
		typename theSizeType s = image->GetBufferedRegion().GetSize();
		replicate_itk_to_image(image);
		this->name_from_path(file_path);

	}else{
		pt_error::error("image_scalar::load_dataset_from_VTK_file()--> file does not exist...",pt_error::debug);
	}
}


template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::save_to_TIF_file_series_3D(const std::string file_path_base, int dir, int from_slice, int to_slice)
{
	char buf[10];
	int s_start=0;
	int s_end;
	
	if(from_slice>=0){
		s_start = from_slice;
	}
	if(to_slice>=0){
		s_end = to_slice;
	}else{
		s_end = this->get_size_by_dim(dir)-1;
	}
		
	image_scalar<ELEMTYPE, IMAGEDIM> *slc;
//	image_general<unsigned char,3> *slc2;
	image_scalar<unsigned char,3> *slc2;
	for(int s=s_start; s<=s_end; s++){
		slc = this->get_subvolume_from_slice_rotated_3D(s,dir);
		slc->scale(); //0...256
		slc->data_has_changed(true);

		sprintf(buf,"%04i",s);
		slc2 = new image_scalar<unsigned char,3>(slc);
		slc2->save_uchar2D_to_TIF_file(file_path_base, string(buf));
//		datamanagement.add(slc,"slc");
//		datamanagement.add(slc2,"slc2");
		delete slc;
		delete slc2;
	}
}

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::save_uchar2D_to_TIF_file(const std::string file_path_base, const std::string slice)
{
	typedef itk::ImageFileWriter<itk::OrientedImage<unsigned char,3> >	theTifWriterType;

	string s = file_path_base+"_"+slice+".tif";
	theTifWriterType::Pointer writer = theTifWriterType::New();
	writer->SetFileName(s.c_str());
	writer->SetInput(this->get_image_as_itk_output());
	try{
		writer->Update();
	}catch (itk::ExceptionObject &ex){
		cout<<"Exception thrown saving file.....("<<s<<")"<<ex;
	}

//	this->clear_itk_porting();
}

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::save_projection_to_8bit_tif_file(const std::string file_path_base, int dir, PROJECTION_MODE PROJ)
{
//	cout<<"save_projection_to_8bit_tif_file..."<<endl;
	image_scalar<ELEMTYPE,3> *mip = this->create_projection_3D(dir, PROJ); //PROJ_MAX, PROJ_MEAN, PROJ_MIN 
	mip->save_to_TIF_file_series_3D(file_path_base,2,0,0);
	delete mip;
}



template <class ELEMTYPE, int IMAGEDIM>
Vector3D image_scalar<ELEMTYPE, IMAGEDIM>::get_pos_of_type_in_region_voxel( Vector3D center, Vector3D radius, POINT_TYPE type )
{	
	if ( ! this->is_voxelpos_within_image_3D( center ) )
	{	// the point is not within the image
		pt_error::error("get_pos_of_type_in_region_voxel--> point is not within image",pt_error::debug);
		return center;
	}

	switch ( type )
	{
		case MAX_GRAD_MAG_X:
			return get_pos_of_max_grad_mag_in_region_voxel( center, radius, X );
		case MAX_GRAD_MAG_Y:
			return get_pos_of_max_grad_mag_in_region_voxel( center, radius, Y );
		case MAX_GRAD_MAG_Z:
			return get_pos_of_max_grad_mag_in_region_voxel( center, radius, Z );
		case MAX_GRAD_MAG_XY:
			return get_pos_of_max_grad_mag_in_region_voxel( center, radius, XY );
		case MAX_GRAD_MAG_XZ:
			return get_pos_of_max_grad_mag_in_region_voxel( center, radius, XZ );
		case MAX_GRAD_MAG_YZ:
			return get_pos_of_max_grad_mag_in_region_voxel( center, radius, YZ );
		case MAX_GRAD_MAG_XYZ:
			return get_pos_of_max_grad_mag_in_region_voxel( center, radius, XYZ );
		default:
			return center;
	}
}


template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::scale_slice_by_factor_3d(int dir, float factor, int slice) 
{
	if (dir<0 || dir>2) {
		pt_error::error("Direction dir must be between 0 and 2 in scale_slice_by_factor_3d", pt_error::debug);
	}
	if(slice<0 || slice>=this->get_size_by_dim(dir)){
		pt_error::error("Slice out of bounds in scale_slice_by_factor_3d",pt_error::debug); 
	}
	if (dir==0)	{
		for(int j=0; j < this->get_size_by_dim(1); j++){
			for(int k=0; k < this->get_size_by_dim(2); k++){
				this->set_voxel(slice,j,k, ELEMTYPE(float(this->get_voxel(slice,j,k))*factor));
			}
		}
	}
	else if (dir==1) {
		for(int i=0; i < this->get_size_by_dim(0); i++){
			for(int k=0; k < this->get_size_by_dim(2); k++){
				this->set_voxel(i,slice,k, ELEMTYPE(float(this->get_voxel(i,slice,k))*factor));
			}
		}
	}
	else {
		for(int i=0; i < this->get_size_by_dim(0); i++){
			for(int j=0; j < this->get_size_by_dim(1); j++){
				this->set_voxel(i,j,slice, ELEMTYPE(float(this->get_voxel(i,j,slice))*factor));
			}
		}
	}
}

template <class ELEMTYPE, int IMAGEDIM>
float image_scalar<ELEMTYPE, IMAGEDIM>::scale_slice_average_to_3d(int dir, ELEMTYPE new_average, int slice, image_binary<IMAGEDIM>* mask) 
{
	if(dir<0 || dir>2) {
		pt_error::error("Direction dir must be between 0 and 2 in scale_slice_by_factor_3d", pt_error::debug);
	}
	if(slice<0 || slice>=this->get_size_by_dim(dir)){
		pt_error::error("Slice out of bounds in scale_slice_average_to_3d",pt_error::debug); 
	}

	ELEMTYPE mean_local = this->get_mean_from_slice_3d(dir,slice, mask);
	float factor = 0;

	if(mean_local!=0){
		factor = float(new_average)/float(mean_local);
		cout<<"slice="<<slice<<" (mean_local="<<mean_local<<" factor="<<factor<<")\t"<<endl;
		for(int v=0; v<this->get_size_by_dim_and_dir(1,dir); v++){
			for(int u=0; u<this->get_size_by_dim_and_dir(0,dir); u++){
				this->set_voxel_by_dir(u,v,slice, ELEMTYPE(float(this->get_voxel_by_dir(u,v,slice,dir)*factor)),dir);
			}
		}
	}

	return factor;
}


template <class ELEMTYPE, int IMAGEDIM>
float image_scalar<ELEMTYPE, IMAGEDIM>::get_mean_from_slice_3d(int dir, int slice, image_binary<IMAGEDIM>* mask) 
{
	pt_error::error_if_false(dir>=0 && dir<=2, "Direction dir must be between 0 and 2 in get_mean_from_slice_3d", pt_error::debug);
	pt_error::error_if_false(slice>=0 && slice<this->get_size_by_dim(dir), "Slice out of bounds in get_mean_from_slice_3d",pt_error::debug); 
	pt_error::error_if_true( (mask!=NULL)&&(!this->same_size(mask)) , "Image size does not match mask size in get_mean_from_slice_3d", pt_error::debug);

	float sum=0;
	float num_voxels=0;

	for(int v=0; v<this->get_size_by_dim_and_dir(1,dir); v++){
		for(int u=0; u<this->get_size_by_dim_and_dir(0,dir); u++){
			if( mask==NULL || mask->get_voxel_by_dir(u,v,slice,dir) ) { //????
				sum += this->get_voxel_by_dir(u,v,slice,dir);
				num_voxels++;
			}
		}
	}
	return sum/num_voxels;
}
/*
template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_scalar<ELEMTYPE, IMAGEDIM>::get_mean_from_5nbh(int x, int y, int z, int dir)
{
//this->get_voxel_by_dir(
}
*/
template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::logarithm_3d(int zero_handling)
{
	// zero_handling=0: non-positive voxels are left unchanged
	// zero_handling=1: non-positive voxels are set to zero
	// zero_handling=2: non-positive voxels are set to std::numeric_limits<ELEMTYPE>::min()
	if (zero_handling<0 || zero_handling>2) {zero_handling=0;}
	for (int z=0; z<this->get_size_by_dim(2); z++) {
		for (int y=0; y<this->get_size_by_dim(1); y++) {
			for (int x=0; x<this->get_size_by_dim(0); x++) {
				if (this->get_voxel(x,y,z)>0) {this->set_voxel(x,y,z,log(this->get_voxel(x,y,z)));	}
				else if (zero_handling==1) {this->set_voxel(x,y,z,0);}
				else if (zero_handling==2) {this->set_voxel(x,y,z,std::numeric_limits<ELEMTYPE>::min());}
			}
		}
	}
}

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::abs_3d()
{
	for (int z=0; z<this->get_size_by_dim(2); z++) {
		for (int y=0; y<this->get_size_by_dim(1); y++) {
			for (int x=0; x<this->get_size_by_dim(0); x++) {
				this->set_voxel( x,y,z,abs(this->get_voxel(x,y,z)) );
			}
		}
	}
}


template <class ELEMTYPE, int IMAGEDIM>
double image_scalar<ELEMTYPE, IMAGEDIM>::calculate_entropy_2d() 
{
	image_scalar<ELEMTYPE,IMAGEDIM> *copy = new image_scalar<ELEMTYPE,IMAGEDIM>(this);
	copy->set_sum_of_voxels_to_value(1);
	double entropy=0;
	for (int i=0; i<this->get_size_by_dim(0); i++) {
		for (int j=0; j<this->get_size_by_dim(1); j++) {
			ELEMTYPE val=copy->get_voxel(i,j);
			if (val!=0) {
				entropy -= this->get_voxel(i,j) * log(copy->get_voxel(i,j));
			}
		}
	}
	delete copy;
	return entropy=entropy/this->get_sum_of_voxels();
}

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::filter_3D(filter_base* filter, int borderflag, image_binary<IMAGEDIM>* mask, int maskflag)
{
	// maskflag=0: Perform filtering for mask voxels only
	// maskflag=1: Perform filtering for mask voxels only, and handle non-mask voxels in neighbourhood as voxels outside image border (works only with borderflags 0 and 1)
	// borderflag=0: Ignore voxels outside image border
	// borderflag=1: Ignore voxels outside image border and renormalize filter weight (sum of weights=1) for border voxels (use for linear filters only)
	// borderflag=2: Treat voxels outside image border as nearest border voxel
	// borderflag=3: Calculate values of voxels outside image border as value of voxel "reflected" in image border
	if (borderflag<0 || borderflag>3) {borderflag=0;}
	if (maskflag<0 || maskflag>1) {maskflag=0;}

	image_scalar<ELEMTYPE,IMAGEDIM> *copy = new image_scalar<ELEMTYPE,IMAGEDIM>(this);
	int xsize=this->datasize[0];
	int ysize=this->datasize[1];
	int zsize=this->datasize[2];
	int x_offs_min = max ((int)(-filter->data.get_column(0).min_value()), 0);
	int x_offs_max = min ((int)(xsize-filter->data.get_column(0).max_value()), xsize); 
	int y_offs_min = max ((int)(-filter->data.get_column(1).min_value()), 0);
	int y_offs_max = min ((int)(ysize-filter->data.get_column(1).max_value()), ysize); 
	int z_offs_min = max ((int)(-filter->data.get_column(2).min_value()), 0);
	int z_offs_max = min ((int)(zsize-filter->data.get_column(2).max_value()), zsize); 
	
	if (mask!=NULL && maskflag==0) {
		// voxels where the filter neighbourhood includes voxels outside image borders are handled by image_scalar::filter_3d_border_voxel
		for (int y=0; y<ysize; y++) {
			for (int x=0; x<xsize; x++) {
				for (int z=0; z<z_offs_min; z++) {
					if (mask->get_voxel(x,y,z)) {this->filter_3d_border_voxel(filter, copy, x, y, z, borderflag, mask, maskflag);}
				}
				for (int z=z_offs_max; z<zsize; z++) {
					if (mask->get_voxel(x,y,z)) {this->filter_3d_border_voxel(filter, copy, x, y, z, borderflag, mask, maskflag);}
				}
			}
		}
		for (int x=0; x<xsize; x++) {
			for (int z=z_offs_min; z<z_offs_max; z++) {
				for (int y=0; y<y_offs_min; y++) {
					if (mask->get_voxel(x,y,z)) {this->filter_3d_border_voxel(filter, copy, x, y, z, borderflag, mask, maskflag);}
				}
				for (int y=y_offs_max; y<ysize; y++) {
					if (mask->get_voxel(x,y,z)) {this->filter_3d_border_voxel(filter, copy, x, y, z, borderflag, mask, maskflag);}
				}
			}
		}
		for (int z=z_offs_min; z<z_offs_max; z++) {
			for (int y=y_offs_min; y<y_offs_max; y++) {
				for (int x=0; x<x_offs_min; x++) {
					if (mask->get_voxel(x,y,z)) {this->filter_3d_border_voxel(filter, copy, x, y, z, borderflag, mask, maskflag);}
				}
				for (int x=x_offs_min; x<x_offs_max; x++) { // voxels where the filter neighbourhood includes no voxels outside the image borders:
					if (mask->get_voxel(x,y,z)) {
						float* neighbourhood = new float[filter->data.rows()];
						for (int i=0; i<filter->data.rows(); i++) {
							neighbourhood[i]=this->get_voxel(x+filter->data.get(i,0),y+filter->data.get(i,1),z+filter->data.get(i,2));
						}
						float res=filter->apply(neighbourhood);
						delete neighbourhood;
						copy->set_voxel(x,y,z,(ELEMTYPE)res );
					}
				}
				for (int x=x_offs_max; x<xsize; x++) {
					if (mask->get_voxel(x,y,z)) {this->filter_3d_border_voxel(filter, copy, x, y, z, borderflag, mask, maskflag);}
				}
			}
		}
	}else if (mask!=NULL && maskflag==1) {
		// voxels where the filter neighbourhood includes voxels outside image borders are handled by image_scalar::filter_3d_border_voxel
		for (int z=0; z<zsize; z++) {
			for (int y=0; y<ysize; y++) {
				for (int x=0; x<xsize; x++) {
					if (mask->get_voxel(x,y,z)) {this->filter_3d_border_voxel(filter, copy, x, y, z, borderflag, mask, maskflag);}
				}
			}
		}
	}else {
		// voxels where the filter neighbourhood includes voxels outside image borders are handled by image_scalar::filter_3d_border_voxel
		for (int y=0; y<ysize; y++) {
			for (int x=0; x<xsize; x++) {
				for (int z=0; z<z_offs_min; z++) {
					this->filter_3d_border_voxel(filter, copy, x, y, z, borderflag, mask, 0);
				}
				for (int z=z_offs_max; z<zsize; z++) {
					this->filter_3d_border_voxel(filter, copy, x, y, z, borderflag, mask, 0);
				}
			}
		}
		for (int x=0; x<xsize; x++) {
			for (int z=z_offs_min; z<z_offs_max; z++) {
				for (int y=0; y<y_offs_min; y++) {
					this->filter_3d_border_voxel(filter, copy, x, y, z, borderflag, mask, 0);
				}
				for (int y=y_offs_max; y<ysize; y++) {
					this->filter_3d_border_voxel(filter, copy, x, y, z, borderflag, mask, 0);
				}
			}
		}
		for (int z=z_offs_min; z<z_offs_max; z++) {
			for (int y=y_offs_min; y<y_offs_max; y++) {
				for (int x=0; x<x_offs_min; x++) {
					this->filter_3d_border_voxel(filter, copy, x, y, z, borderflag, mask, 0);
				}
				for (int x=x_offs_min; x<x_offs_max; x++) { // voxels where the filter neighbourhood includes no voxels outside the image borders:
					float* neighbourhood = new float[filter->data.rows()];
					for (int i=0; i<filter->data.rows(); i++) {
						neighbourhood[i]=this->get_voxel(x+filter->data.get(i,0),y+filter->data.get(i,1),z+filter->data.get(i,2));
					}
					float res=filter->apply(neighbourhood);
					delete neighbourhood;
					copy->set_voxel(x,y,z,(ELEMTYPE)res );
				}
				for (int x=x_offs_max; x<xsize; x++) {
					this->filter_3d_border_voxel(filter, copy, x, y, z, borderflag, mask, 0);
				}
			}
		}
	}

	copy_data(copy,this);
	delete copy;
}

template <class ELEMTYPE, int IMAGEDIM>
//void image_scalar<ELEMTYPE, IMAGEDIM>::filter_3d_border_voxel(filter_base* filter, image_scalar<float,3>* copy, int x, int y, int z, int borderflag, image_binary<IMAGEDIM>* mask, int maskflag)
void image_scalar<ELEMTYPE, IMAGEDIM>::filter_3d_border_voxel(filter_base* filter, image_scalar<ELEMTYPE,IMAGEDIM>* copy, int x, int y, int z, int borderflag, image_binary<IMAGEDIM>* mask, int maskflag)
{
	int xsize=this->datasize[0];
	int ysize=this->datasize[1];
	int zsize=this->datasize[2];

	if (borderflag==0 && maskflag==0) { // Ignore voxels outside image border
		int input_x; int input_y; int input_z;
		float* neighbourhood = new float[filter->data.rows()];
		for (int i=0; i<filter->data.rows(); i++) {
			input_x = x+filter->data.get(i,0);
			input_y = y+filter->data.get(i,1);
			input_z = z+filter->data.get(i,2);
			if (!(input_x<0 || input_y<0 || input_z<0 || input_x>=xsize || input_y>=ysize || input_z>=zsize)) { // Ignore voxels outside image border
				neighbourhood[i]=this->get_voxel(input_x,input_y,input_z);
			}
			else {neighbourhood[i]= std::numeric_limits<float>::max();} //strange... but can now be checked/found in the filter
		}
		float res=filter->apply(neighbourhood);
		delete neighbourhood;
		copy->set_voxel(x,y,z,(ELEMTYPE)res );
	}
	else if (borderflag==0 && maskflag ==1) { // Ignore voxels outside image border
		int input_x; int input_y; int input_z;
		float* neighbourhood = new float[filter->data.rows()];
		for (int i=0; i<filter->data.rows(); i++) {
			input_x = x+filter->data.get(i,0);
			input_y = y+filter->data.get(i,1);
			input_z = z+filter->data.get(i,2);
			if (!(input_x<0 || input_y<0 || input_z<0 || input_x>=xsize || input_y>=ysize || input_z>=zsize || mask->get_voxel(input_x, input_y, input_z)==0)) { // Ignore voxels outside image border
				neighbourhood[i]=this->get_voxel(input_x,input_y,input_z);
			}
			else {neighbourhood[i]= std::numeric_limits<float>::max();}
		}
		float res=filter->apply(neighbourhood);
		delete neighbourhood;
		copy->set_voxel(x,y,z,(ELEMTYPE)res );
	}
	else if (borderflag==1 && maskflag==0) { // Ignore voxels outside image border and renormalize filter weight (sum of weights=1) for border voxels
		float weight=0;
		int num_image_elements=0;
		int input_x; int input_y; int input_z;
		float* neighbourhood = new float[filter->data.rows()];
		for (int i=0; i<filter->data.rows(); i++) {
			input_x = x+filter->data.get(i,0);
			input_y = y+filter->data.get(i,1);
			input_z = z+filter->data.get(i,2);
			if (!(input_x<0 || input_y<0 || input_z<0 || input_x>=xsize || input_y>=ysize || input_z>=zsize)) { // Ignore voxels outside image border and calculate weight compensation
				neighbourhood[i]=this->get_voxel(input_x,input_y,input_z);
				weight+= filter->data.get(i,3);
				num_image_elements++;
			}
			else {neighbourhood[i]=std::numeric_limits<float>::max();}
		}
		if (num_image_elements==0) {weight=1;}
		float res=filter->apply(neighbourhood);
		delete neighbourhood;
		copy->set_voxel(x,y,z,(ELEMTYPE)res/weight );
	}
	else if (borderflag==1 && maskflag==1) { // Ignore voxels outside image border and renormalize filter weight (sum of weights=1) for border voxels
		float weight=0;
		int num_image_elements=0;
		int input_x; int input_y; int input_z;
		float* neighbourhood = new float[filter->data.rows()];
		for (int i=0; i<filter->data.rows(); i++) {
			input_x = x+filter->data.get(i,0);
			input_y = y+filter->data.get(i,1);
			input_z = z+filter->data.get(i,2);
			if (!(input_x<0 || input_y<0 || input_z<0 || input_x>=xsize || input_y>=ysize || input_z>=zsize || mask->get_voxel(input_x, input_y, input_z)==0)) { // Ignore voxels outside image border and calculate weight compensation
				neighbourhood[i]=this->get_voxel(input_x,input_y,input_z);
				weight+= filter->data.get(i,3);
				num_image_elements++;
			}
			else {neighbourhood[i]=std::numeric_limits<float>::max();}
		}
		if (num_image_elements==0) {weight=1;}
		float res=filter->apply(neighbourhood);
		delete neighbourhood;
		copy->set_voxel(x,y,z,(ELEMTYPE)res/weight );
	}
	else if (borderflag==2) { // Treat voxels outside image border as nearest border voxel
		int input_x; int input_y; int input_z;
		float* neighbourhood = new float[filter->data.rows()];
		for (int i=0; i<filter->data.rows(); i++) {
			input_x = x+filter->data.get(i,0);
			if (input_x<0) {input_x=0;}
			if (input_x>=xsize) {input_x=xsize-1;}
			input_y = y+filter->data.get(i,1);
			if (input_y<0) {input_y=0;}
			if (input_y>=ysize) {input_y=ysize-1;}
			input_z = z+filter->data.get(i,2);
			if (input_z<0) {input_z=0;}
			if (input_z>=zsize) {input_z=zsize-1;}
			neighbourhood[i]=this->get_voxel(input_x,input_y,input_z);
		}
		float res=filter->apply(neighbourhood);
		delete neighbourhood;
		copy->set_voxel(x,y,z,(ELEMTYPE)res );
	}
	else if (borderflag==3) { // Calculate values of voxels outside image border as value of voxel "reflected" in image border
		int input_x; int input_y; int input_z;
		float* neighbourhood = new float[filter->data.rows()];
		for (int i=0; i<filter->data.rows(); i++) {
			input_x = x+filter->data.get(i,0);
			if (input_x<0) {input_x=-input_x;}
			if (input_x>=xsize) {input_x=(xsize-1)-(input_x-(xsize-1));}
			input_y = y+filter->data.get(i,1);
			if (input_y<0) {input_y=-input_y;}
			if (input_y>=ysize) {input_y=(ysize-1)-(input_y-(ysize-1));}
			input_z = z+filter->data.get(i,2);
			if (input_z<0) {input_z=-input_z;}
			if (input_z>=zsize) {input_z=(zsize-1)-(input_z-(zsize-1));}
			neighbourhood[i]=this->get_voxel(input_x,input_y,input_z);
		}
		float res=filter->apply(neighbourhood);
		delete neighbourhood;
		copy->set_voxel(x,y,z,(ELEMTYPE)res );
	}
}

/*
template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::set_parameters (image_scalar<ELEMTYPE, IMAGEDIM> * sourceImage)
    {
	this->set_parameters( dynamic_cast<image_general<ELEMTYPE,IMAGEDIM>*>(input) );
    }
*/


// old
//template <class ELEMTYPE, int IMAGEDIM>
//Vector3D image_scalar<ELEMTYPE, IMAGEDIM>::get_pos_of_type_in_region_voxel ( Vector3D center, Vector3D radius, POINT_TYPE point_type )
//{
//	image_scalar<ELEMTYPE, IMAGEDIM> * res = new image_scalar<ELEMTYPE, IMAGEDIM>( this, 0 );
//	res->fill( 0 );
//
//	Vector3D pos;
//	
//	if ( ! is_voxelpos_within_image_3D( center ) )
//	{	// the point is not within the image
//		// TODO: use pt_error
//		return pos;
//		// return center;
//	}
//
//	center[0] = floor( center[0] );
//	center[1] = floor( center[1] );
//	center[2] = floor( center[2] );
//
//
//	Vector3D start = center - radius;
//	Vector3D end = center + radius;
//
//	pos = center;	// the center position is returned if no other is found
//
//	for ( int z = start[2]; z <= end[2]; z++ )
//	{
//		for ( int y = start[1]; y <= end[1]; y++ )
//		{
//			for ( int x = start[0]; x <= end[0]; x++ )
//			{
//			
//				switch ( point_type )
//				{
//					case MAX_GRAD_MAG_X:
//					case MAX_GRAD_MAG_Y:
//					case MAX_GRAD_MAG_Z:
//					case MAX_GRAD_MAG:
//						// use chessboard distance as weight
//						int dist_x = abs ( x - center[0] );
//						int dist_y = abs ( y - center[1] );
//						int dist_z = abs ( z - center[2] );
//						float weight = max( max( dist_x, dist_y ), dist_z );
//						if ( weight != 0 )
//						{ // not the center voxel (i.e. weight > 0)
//							GRAD_MAG_TYPE grad_mag_type;
//							switch ( grad_mag_type )
//							{
//								case MAX_GRAD_MAG_X: grad_mag_type = X; break;
//								case MAX_GRAD_MAG_Y: grad_mag_type = Y; break;
//								case MAX_GRAD_MAG_Z: grad_mag_type = Z; break;
//								case MAX_GRAD_MAG: grad_mag_type = XYZ; break;
//							}
//							float current_grad_mag = grad_mag_voxel( x, y, z, grad_mag_type );
//
//							// This line should be outside the loop because the value does not change
//							// but it is more intuitive to have it here
//							float center_grad_mag = grad_mag_voxel( center, point_type );
//							
//							if ( ( current_grad_mag / weight ) > center_grad_mag )
//								{ pos = create_Vector3D( x, y, z ); }
//							
////							std::cout << "test " << current_grad_mag << std::endl;
////							std::cout << "test floor " << floor( current_grad_mag ) << std::endl;
//							res->set_voxel( x, y, z, floor( current_grad_mag / weight ) );
//								
//						}
//						break;
//						
//
//				}
//			}
//		}
//	}
//
//	res->set_voxel( pos[0], pos[1], pos[2], 511 );
//	datamanagement.add( res );
//
//	return pos;
//}


//Vector3D get_pos_of_type_in_region_voxel ( Vector3D center, Vector3D radius, POINT_TYPE point_type )

template <class ELEMTYPE, int IMAGEDIM>
float image_scalar<ELEMTYPE, IMAGEDIM>::get_mean_squared_difference_to_template_3D(Vector3D pos, image_scalar<ELEMTYPE, IMAGEDIM> *small_template)
{
	double cost=0;
	double fact = small_template->nx()*small_template->ny()*small_template->nz();
	int nr=0;
	
	int m_x=0;
	for(int x=pos[0];  x<pos[0]+small_template->nx();  x++){
		int m_y=0;
		for(int y=pos[1];  y<pos[1]+small_template->ny();  y++){
			int m_z=0;
			for(int z=pos[2];  z<pos[2]+small_template->nz();  z++){
				cost += pow( double(small_template->get_voxel(m_x,m_y,m_z)-this->get_voxel(x,y,z)), 2 );
				nr++;
				m_z++;
			}
			m_y++;
		}
		m_x++;
	}
//	cout<<nr<<",";
	return cost/fact;
}

template <class ELEMTYPE, int IMAGEDIM>
float image_scalar<ELEMTYPE, IMAGEDIM>::get_mean_squared_difference_to_template_centered_3D(Vector3D pos, image_scalar<ELEMTYPE, IMAGEDIM> *small_template)
{
	return this->get_mean_squared_difference_to_template_3D(pos - small_template->get_size()/2.0, small_template);
}

template <class ELEMTYPE, int IMAGEDIM>
image_scalar<float, IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::get_mean_squared_difference_image_3D(Vector3D fcp, Vector3D tcp, image_scalar<ELEMTYPE, IMAGEDIM> *small_template)
{
	image_scalar<float,3> *res = new image_scalar<float,3>(this,0);
	res->fill(0);
	Vector3D full_size = small_template->get_size();
	Vector3D half_size = small_template->get_size()/2;
	cout<<"full_size="<<full_size<<endl;
	cout<<"half_size="<<half_size<<endl;
	Vector3D from_pos;
	Vector3D to_pos;
	for(int d=0;d<3;d++){
		from_pos[d] = std::max( int(0), int(fcp[d]-half_size[d]) );
		to_pos[d] = std::min( int(tcp[d]-half_size[d]), int(this->get_size_by_dim(d)-full_size[d]-1) );
	}
	cout<<"from_pos="<<from_pos<<endl;
	cout<<"to_pos="<<to_pos<<endl;
	
	float r=0;
	for(int k=from_pos[2];k<=to_pos[2];k++){
//		cout<<"k="<<k<<endl;
		for(int j=from_pos[1];j<=to_pos[1];j++){
			for(int i=from_pos[0];i<=to_pos[0];i++){
				r = this->get_mean_squared_difference_to_template_3D(create_Vector3D(i,j,k),small_template);
				res->set_voxel(i+half_size[0],j+half_size[1],k+half_size[2],r);
			}
		}
	}
	return res;
}
	 /*
template<class ELEMTYPE, int IMAGEDIM>
fit_gaussian_2d_cost_function<ELEMTYPE,IMAGEDIM>::fit_gaussian_2d_cost_function(image_scalar<ELEMTYPE, IMAGEDIM> *im, gaussian_2d *g):vnl_least_squares_function(6,im->get_size_by_dim(0)*im->get_size_by_dim(1)+9,vnl_least_squares_function::no_gradient) // cost function used by fit_gaussian_2D_to_this_image_2D
{
	the_image = im;
	the_gaussian = g;
	xsize = im->get_size_by_dim(0);
	ysize = im->get_size_by_dim(1);
}

template<class ELEMTYPE, int IMAGEDIM>
void fit_gaussian_2d_cost_function<ELEMTYPE,IMAGEDIM>::f(vnl_vector<double> const &x, vnl_vector<double> &fx) 
{
	//x contains the variables of the gaussian_2d
	//the same order is used... amp, cent_x, cent_y, sig_u, sig_v, angle
	the_gaussian->amplitude = x[0];
	the_gaussian->center_x = x[1];
	the_gaussian->center_y = x[2];
	the_gaussian->sigma_u = x[3];
	the_gaussian->sigma_v = x[4];
	the_gaussian->phi = x[5];
	
	cout<<"x="<<x<<endl;
	
	for (int xx=0; xx<xsize; xx++)
		for (int yy=0; yy<ysize; yy++)
			fx[xx+xsize*yy] = the_gaussian->evaluate_at(xx,yy)-the_image->get_voxel(xx,yy);
	
	fx[xsize*ysize + 0] = (x[0]<0)*std::numeric_limits<double>::max();
	fx[xsize*ysize + 1] = (x[1]<0)*std::numeric_limits<double>::max();
	fx[xsize*ysize + 2] = (x[2]<0)*std::numeric_limits<double>::max();
	fx[xsize*ysize + 3] = (x[3]<0)*std::numeric_limits<double>::max();
	fx[xsize*ysize + 4] = (x[4]<0)*std::numeric_limits<double>::max();
	fx[xsize*ysize + 5] = (x[5]<0)*std::numeric_limits<double>::max();
	fx[xsize*ysize + 6] = (x[1]>xsize-1)*std::numeric_limits<double>::max();
	fx[xsize*ysize + 7] = (x[2]>ysize-1)*std::numeric_limits<double>::max();
	fx[xsize*ysize + 8] = (x[5]>pt_PI/2)*std::numeric_limits<double>::max();
}
*/

#include "image_scalarprocess.hxx"

#endif
