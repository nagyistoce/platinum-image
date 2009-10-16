// $Id$

// This file is part of the Platinum library.
// Copyright (c) 2007 Uppsala University.
//
//    The Platinum library is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    The Platinum library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with the Platinum library; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#include "FLTKviewport.h"

#include "datamanager.h"
#include "rendermanager.h"
#include "viewmanager.h"

extern datamanager datamanagement;
extern rendermanager rendermanagement;
extern viewmanager viewmanagement;

//#include <FL/Enumerations.H>

using namespace std;


const char * preset_direction_labels[] = {"Default","Axial","Sagittal","Coronal","Z","Y","X","-Z","-Y","-X"};
const char * preset_color_labels[] = {"White", "Black", "Red", "Green", "Blue", "Yellow", "Magenta", "Cyan"};
//const char * preset_direction_labels[] ={"Axial","Coronal","Sagittal","-Axial","-Coronal","-Sagittal"};
const char * blend_mode_labels[] = {"Overwrite","Max","Min","Average","Diff","Tint","Grey+Tint","Grey+Red","Grey+Blue"};

struct menu_callback_params
{
    int rend_index;				//for blend & image callbacks
    blendmode mode;				//for blend callback
    int vol_id;					//for image callback
    viewport *vport;			//for preset direction callback    
    preset_direction direction;	//for preset direction callback  
	colors color;				//For color selection in curve
	char line;					//Determines the line type
	int vp_id;
};

string eventnames[] =
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


FLTKpane::FLTKpane():Fl_Overlay_Window(0,0,100,100)
{
	//FLTKpane(0,0,100,100);
	Fl_Button *b = new Fl_Button(10,10,80,80, "FLTKpane()_button");
	b->color(FL_RED);
//	this->end();
}

FLTKpane::FLTKpane(int X,int Y,int W,int H) : Fl_Overlay_Window(X,Y,W,H)
//FLTKpane::FLTKpane(int X,int Y,int W,int H) : Fl_Window(X,Y,W,H)
{
//	Fl_Button *b = new Fl_Button(10,10,80,80, "FLTKpane(xywh)_button");
//	b->color(FL_RED);
//	this->end();
}

int FLTKpane::h_pane()
{return 0;}


void FLTKpane::needs_rerendering()
{
//	cout<<"FLTKpane::needs_rerendering()..."<<endl;
	((FLTKviewport*)this->parent())->viewport_parent->needs_rerendering();
}

void FLTKpane::resize_content(int w,int h)
{}

int FLTKpane::get_renderer_id()
{
	return ( (FLTKviewport*)this->parent())->get_renderer_id();
}

void FLTKpane::set_renderer_direction( preset_direction direction ) 
{}

void FLTKpane::refresh_menus()
{}

//------------------------------------------------
//------------------------------------------------

FLTK_VTK_pane::FLTK_VTK_pane(): FLTKpane(0,0,100,100)
{
//	cout<<"FLTK_VTK_pane..."<<endl;
//	fl_vtk_window = new vtkFlRenderWindowInteractor(0,0,100,100,"");
//	this->initialize_vtkRenderWindow();
//	this->resizable(fl_vtk_window);
//	this->end();
}

FLTK_VTK_pane::FLTK_VTK_pane(int X,int Y,int W,int H) : FLTKpane(X,Y,W,H)
{
//	cout<<"FLTK_VTK_pane..."<<endl;
//	fl_vtk_window = new vtkFlRenderWindowInteractor(0,0,W,H,"");
//	this->initialize_vtkRenderWindow();
//	this->resizable(fl_vtk_window);
//	this->end();
}
FLTK_VTK_pane::~FLTK_VTK_pane()
{
	delete fl_vtk_window;
}

int FLTK_VTK_pane::h_pane()
{
	return this->h();
}

viewport* FLTK_VTK_pane::get_viewport_parent()
{
	return NULL;
}

void FLTK_VTK_pane::needs_rerendering()
{
}

void FLTK_VTK_pane::resize_content(int w,int h)
{
	cout<<"FLTK_VTK_pane::resize_content("<<w<<","<<h<<")"<<endl;
	fl_vtk_window->resize(0,0,w,h);
}


void FLTK_VTK_pane::initialize_vtkRenderWindow()
{}

void FLTK_VTK_pane::draw_overlay()
{
//	((FLTKviewport*)this->parent())->viewport_parent->paint_overlay(); //ˆˆˆˆ JK
}

int FLTK_VTK_pane::handle(int event)
{
	int ret = 0;
	
	if ( event == FL_KEYDOWN )
	{				
		cout << "Any key press - entering handle()." << endl;
		if ( Fl::event_key() == FL_Delete )
		{			
			cout << "Delete_key pressed..." << endl;
			ret = 1;
		}

		if ( Fl::event_key() == FL_Right )
		{			
			cout << "Right arrow pressed..." << endl;

//			vtkRenderWindowInteractor *iren = reinterpret_cast<vtkRenderWindowInteractor*>(fl_vtk_window);
			vtkRenderer *ren = ( reinterpret_cast<vtkRendererCollection*>(fl_vtk_window->GetRenderWindow()->GetRenderers()) )->GetFirstRenderer();
			vtkCamera *camera = ren->GetActiveCamera();

			//fastnar vid innan vtkCamera m texten:
			/* - - - Run-Time Check Failure #0 - - -
			The value of ESP was not properly saved across a function call.  
			This is usually a result of calling a function declared with one 
			calling convention with a function pointer declared with a different 
			calling convention. */

			//Testing - possible to interact?
			//cout << "Rotating camera 30 degrees..." << endl;
			//camera->OrthogonalizeViewUp();
			//camera->Azimuth(30);
			//fl_vtk_window->GetRenderWindow()->Render();
			//-------------------------------

			int eventPos[2];
			fl_vtk_window->GetEventPosition(eventPos[0], eventPos[1]); //view coordinates
			cout << "fl_vtk_window->GetEventPosition = [" << eventPos[0] << ", " << eventPos[1] << "]" << endl;

			double displayPos[3];
			displayPos[0] = eventPos[0];
			displayPos[1] = eventPos[1];
			displayPos[2] = 0;

			//display2world
			ren->SetDisplayPoint(displayPos);
			double worldP_2[4];
			ren->DisplayToWorld();
			ren->GetWorldPoint(worldP_2);
			cout << "ren worldP_2 (display) = [" << worldP_2[0] << ", " << worldP_2[1] << ", " << worldP_2[2] << ", " << worldP_2[3] << "]" << endl;

			double direction[3];
			camera->GetDirectionOfProjection(direction);
			cout << "Direction of projection of the camera: [" << direction[0] << ", " 
				<< direction[1] << ", " << direction[2] << "]" << endl;

			
			image_base *im = rendermanagement.get_top_image_from_renderer(this->get_renderer_id());
			cout<<"name="<<im->name()<<endl;
			cout << "im maxvalue (if created) : " << im->get_max_float() << endl;
		

			image_scalar<float,3> *im2 = dynamic_cast<image_scalar<float,3>* >(im);
			//image_scalar<unsigned short,3> *im2 = dynamic_cast<image_scalar<unsigned short,3>* >(im);
			//image_scalar<signed char,3> *im2 = dynamic_cast<image_scalar<signed char,3>* >(im);
			cout << "Image pointer created?" << endl;
			//cout << "im2 maxvalue (if created) : " << im2->get_max() << endl;

			if(im2!=NULL) 
			{
				cout<<"jippie"<<endl;
				line3D line = line3D(worldP_2[0],worldP_2[1],worldP_2[2],direction[0],direction[1],direction[2]);
				Vector3D max_pos = im2->get_phys_pos_of_max_intensity_along(line,1);
				cout<<"max_pos="<<max_pos<<endl;
				im2->draw_line_3D(line,11000);


				//::::::::::::::::::::::::::::::::::::::::::::::::::
				//Fill voxels around world position:
				/*
				leta upp bild -> position -> f‰rgl‰gg -> spara
				*/
		
//				image_scalar<signed short,3> *testImage = dynamic_cast<image_scalar<signed short,3>* >( rendermanagement.get_top_image_from_renderer(2) );
				//s‰tt max_pos i bilden till l‰mplig intensistet
				//max_pos ‰r v‰rldskoord -> voxel!
				//max_pos
				Vector3Dint vox_coord = im2->get_voxelpos_integers_from_physical_pos_3D(max_pos);
				
				cout << "Setting new voxel values to vox_coord = [" << vox_coord[0] << ", " << vox_coord[1] << ", " << vox_coord[2] << "]" << endl;
				//for (int i=-5; i=5; i++)
				//{
				vox_coord[0]=vox_coord[0];
				im2->set_voxel(vox_coord, 255);
/*
				vox_coord[0]=vox_coord[0]+1;
				testImage->set_voxel(vox_coord, 1000);

				vox_coord[0]=vox_coord[0]+2;
				testImage->set_voxel(vox_coord, 1000);

				vox_coord[0]=vox_coord[0]+3;
				testImage->set_voxel(vox_coord, 1000);

				vox_coord[0]=vox_coord[0]+4;
				testImage->set_voxel(vox_coord, 1000);



				vox_coord[1]=vox_coord[1]+1;
				testImage->set_voxel(vox_coord, 1000);

				vox_coord[1]=vox_coord[1]+2;
				testImage->set_voxel(vox_coord, 1000);

				vox_coord[1]=vox_coord[1]+3;
				testImage->set_voxel(vox_coord, 1000);

				vox_coord[1]=vox_coord[1]+4;
				testImage->set_voxel(vox_coord, 1000);
*/
				//}

				//for (int n=-5; n=5; n++)
				//{
				//vox_coord[1]=vox_coord[1]+n;
				//testImage->set_voxel_in_physical_pos(max_pos, 500); //skriver utanfˆr bilden...
				//testImage->set_voxel(vox_coord, 500);
				//}

//				testImage->save_to_file("C:/Sandra/Data/testImage.vtk");

				//liknande fˆr vp = viewmanagement.get_viewport(4); -bilden

				//::::::::::::::::::::::::::::::::::::::::::::::::::

				viewmanagement.zoom_specific_vp(2, max_pos, 0.2 );
				viewmanagement.zoom_specific_vp(3, max_pos, 0.2 );
//				viewmanagement.zoom_specific_vp(4, max_pos, 1 );
//				viewmanagement.zoom_specific_vp(5, max_pos, 0.8 );
//				viewmanagement.zoom_specific_vp(6, max_pos, 0.6 );
				cout << "Should have zoomed by now..." << endl;
			}


			ret = 1;
		}
	}

	return ( FLTKpane::handle(event) ? 1 : ret );
}


//----------------------------------------------------------------
//----------------------------------------------------------------
FLTK_VTK_Cone_pane::FLTK_VTK_Cone_pane(): FLTK_VTK_pane(0,0,100,100)
{
//	cout<<"FLTK_VTK_Cone_pane..."<<endl;
	fl_vtk_window = new vtkFlRenderWindowInteractor(0,0,100,100,"");
	this->initialize_vtkRenderWindow();
	this->resizable(fl_vtk_window);
	this->end();
}

FLTK_VTK_Cone_pane::FLTK_VTK_Cone_pane(int X,int Y,int W,int H) : FLTK_VTK_pane(X,Y,W,H)
{
//	cout<<"FLTK_VTK_Cone_pane..."<<endl;
	fl_vtk_window = new vtkFlRenderWindowInteractor(0,0,W,H,"");
	this->initialize_vtkRenderWindow();
	this->resizable(fl_vtk_window);
	this->end();
}

FLTK_VTK_Cone_pane::~FLTK_VTK_Cone_pane()
{}


void FLTK_VTK_Cone_pane::initialize_vtkRenderWindow()
{
	//--------------------------------------------------
  	vtkRenderWindow *renWindow = vtkRenderWindow::New();
	vtkRenderer *ren = vtkRenderer::New();
	ren->SetBackground(0.1,0.1,0.1);
	renWindow->AddRenderer(ren);

	// uncomment the statement below if things aren't rendering 100% on your
	// configuration; the debug output could give you clues as to why
	renWindow->DebugOn();
	   
	// Here we treat the vtkFlRenderWindowInteractor just like any other old vtkRenderWindowInteractor
	fl_vtk_window->SetRenderWindow(renWindow);

	// just like with any other vtkRenderWindowInteractor(), you HAVE to call
	// Initialize() before the interactor will function.  See the docs in vtkRenderWindowInteractor.h
	fl_vtk_window->Initialize();

	// create an actor and give it cone geometry
	vtkConeSource *cone = vtkConeSource::New();
	cone->SetResolution(8);
	vtkPolyDataMapper *coneMapper = vtkPolyDataMapper::New();
	coneMapper->SetInput(cone->GetOutput());
	vtkActor *coneActor = vtkActor::New();
	coneActor->SetMapper(coneMapper);
	coneActor->GetProperty()->SetColor(1.0, 0.0, 1.0);

	ren->AddActor(coneActor);

	//:: 080908 :: //testa vtkCallbackCOmmand
	vtkPointPicker *picker = vtkPointPicker::New();
	cout << "Picker set" << endl;
	//w_picker = vtkWorldPointPicker::New();
	PickPosCommand *PickObserver = PickPosCommand::New();
	picker->AddObserver(vtkCommand::EndPickEvent, PickObserver);
	//::		::

	// We can now delete all our references to the VTK pipeline (except for
	// our reference to the vtkFlRenderWindowInteractor) as the objects
	// themselves will stick around until we dereference fl_vtk_window
	ren->Delete();
	renWindow->Delete();
	cone->Delete();
	coneMapper->Delete();
	coneActor->Delete();
	//--------------------------------------------------
}

//----------------------------------------------------------------
//----------------------------------------------------------------

FLTK_VTK_MIP_pane::FLTK_VTK_MIP_pane(): FLTK_VTK_pane(0,0,100,100)
{
//	cout<<"FLTK_VTK_MIP_pane..."<<endl;
	fl_vtk_window = new vtkFlRenderWindowInteractor(0,0,100,100,"");
	this->initialize_vtkRenderWindow();
	this->resizable(fl_vtk_window);
	this->end();
}

FLTK_VTK_MIP_pane::FLTK_VTK_MIP_pane(int X,int Y,int W,int H) : FLTK_VTK_pane(X,Y,W,H)
{
//	cout<<"FLTK_VTK_MIP_pane..."<<endl;
	fl_vtk_window = new vtkFlRenderWindowInteractor(0,0,W,H,"");
	this->initialize_vtkRenderWindow();
	this->resizable(fl_vtk_window);
	this->end();
}

FLTK_VTK_MIP_pane::~FLTK_VTK_MIP_pane()
{}

//--------------------------------------------------------
//------------TILLFƒLLIG PLACERING-------------------/SO--
//--------------------------------------------------------

	PickPosCommand* PickPosCommand::New()
	{ 
		return new PickPosCommand; 
	}

	void PickPosCommand::Delete() 
	{ 
		delete this;
	}

	void PickPosCommand::Execute(vtkObject *caller, unsigned long eid, void *callData)
	{
		cout << "- INSIDE EXECUTE() -" << endl;
		vtkPointPicker *picker = reinterpret_cast<vtkPointPicker*>(caller);
		double PickPos[3];
		
		picker->GetPickPosition(PickPos);
		
		//TESTING***

		Vector3D position;
		position[0]=PickPos[0];
		position[1]=PickPos[1];
		position[2]=PickPos[2];

		//send_position(position);
		cout << "sending position to view port 2" << endl;
		viewmanagement.zoom_specific_vp(2, position, 3);
		//***

		cout << ":: :: PickPosCommand: [" << PickPos[0] << ", " << PickPos[1] << ", " << PickPos[2] << "] :: ::" << endl;
	}

double Xcam2;
double Ycam2; 
double Zcam2;

class MyCommand2 : public vtkCommand
{
public:
 	static MyCommand2 *New()
	{
		return new MyCommand2;
	}

	void Delete()
	{ 
		delete this;
	}

  virtual void Execute(vtkObject* caller, unsigned long eventId, void* callData)
  {
	 vtkRenderWindowInteractor *iren = reinterpret_cast<vtkRenderWindowInteractor*>(caller);

	 cout << "INSIDE MyCommand Execute()." << endl;

	//Sifferkoder: http://www.asciitable.com/ "Dec"

	vtkCamera *camera =( reinterpret_cast<vtkRendererCollection*>(iren->GetRenderWindow()->GetRenderers()) )->GetFirstRenderer()->GetActiveCamera();
		iren->GetRenderWindow()->Render();
		cout << "Camera position 2 = [" << Xcam2 << ", " << Ycam2 << ", " << Zcam2 << "]" << endl;

	 if(Fl::event_key()==101)
	 {
		cout << "'e' PRESSED" << endl;
	 }

	 else if(Fl::event_key()==32)
	 {
		cout << "SPACE PRESSED" << endl;	
	 }	

	 else if(Fl::event_key()==FL_Enter)
	 {
		cout << "ENTER PRESSED" << endl;	
	 }	

	 else if(Fl::event_key() == FL_Right)
	 {
		cout << "RIGHT ARROW PRESSED" << endl;	
		camera->Azimuth(30);
		iren->GetRenderWindow()->Render();
	 }

	 else if(Fl::event_key() == FL_Left)
	 {
		cout << "LEFT ARROW PRESSED" << endl;
		camera->Azimuth(-30);
		iren->GetRenderWindow()->Render();
	 }

	 else if(Fl::event_key() == FL_Up)
	 {
		cout << "UP ARROW PRESSED" << endl;	
		camera->Elevation(30);
		iren->GetRenderWindow()->Render();
	 }

	 else if(Fl::event_key() == FL_Down)
	 {
		cout << "DOWN ARROW PRESSED" << endl;
		camera->Elevation(-30);
		iren->GetRenderWindow()->Render();
	 }

	 else if(Fl::event_key() == 'r') //114="r"
	 {
		cout << "'r' PRESSED" << endl;
		camera->SetPosition(Xcam2, Ycam2, Zcam2);
		cout << "Camera position 3 = [" << Xcam2 << ", " << Ycam2 << ", " << Zcam2 << "]" << endl;
		iren->GetRenderWindow()->Render();
	 }

  } 
};

//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------

void FLTK_VTK_MIP_pane::initialize_vtkRenderWindow()
{
	//JK TODO - rewrite in object oriented style...
	image_base *top_image;
	if( top_image = rendermanagement.get_top_image_from_renderer(this->get_renderer_id()) )
	{
	vtkRenderer *ren = vtkRenderer::New();  
	vtkRenderWindow *renWin = vtkRenderWindow::New();
	renWin->AddRenderer(ren);

  	// uncomment the statement below if things aren't rendering 100% on your
	// configuration; the debug output could give you clues as to why
	//renWindow->DebugOn();

	fl_vtk_window->SetRenderWindow(renWin);
	// just like with any other vtkRenderWindowInteractor(), you HAVE to call
	fl_vtk_window->Initialize();

	// Read the data from a vtk file
/*
	vtkStructuredPointsReader *reader = vtkStructuredPointsReader::New();
//	reader->SetFileName("C:/Sandra/Data/MTEA01_anon2/whole_body_diff.vtk"); //C:/Sandra/Data/MTEA01_anon2/whole_body_diff.vtk C:/Sandra/Data/FLAIR.vtk
//	reader->SetFileName("D:/Joel/TMP/750001_WML01.vtk");
//	reader->SetFileName("D:/Joel/TMP/brain.vtk");
	reader->SetFileName("D:/Joel/TMP/brain8bit.vtk");
	reader->Update();
//	reader->GetOutput()
*/

	cout<<"hej"<<endl;
//	top = datamanagement.get_image(1);
//	image_integer<unsigned short,3> *f = dynamic_cast<image_integer<unsigned short,3>* >(top);

//--------------------------------------------------------
	/*
//	image_scalar<unsigned char,3> *f = new image_scalar<unsigned char,3>("D:/Joel/TMP/brain8bit.vtk");

//	typename theImageType::Pointer image = f->get_image_as_itk_output();
//	cout<<"dir="<<endl<<image->GetDirection()<<endl;
    vtkImageImport* vtkImporter = vtkImageImport::New();  

	typedef itk::VTKImageExport< itk::OrientedImage<unsigned short,3> > ExportFilterType;
    ExportFilterType::Pointer itkExporter = ExportFilterType::New();
    itkExporter->SetInput( f->get_image_as_itk_output() );

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
	
	vtkImporter->Update();
*/
	//---------------------------------------------------------

/*
	vtkImageReader *reader2 = vtkImageReader::New();
//	reader2->SetFileName("C:/Sandra/Data/MTEA01_anon2/whole_body_diff.vtk"); //C:/Sandra/Data/MTEA01_anon2/whole_body_diff.vtk C:/Sandra/Data/FLAIR.vtk
	reader2->SetFileName("D:/Joel/TMP/brain8bit.vtk");
	reader2->Update();

//	vtkImageCast *caster = vtkImageCast::New();
//	caster->SetInput( reader2->GetOutput() );
//	caster->SetOutputScalarTypeToUnsignedShort();
	
	vtkImageToStructuredPoints *cast2 = vtkImageToStructuredPoints::New();
	cast2->SetInput( reader2->GetOutput() );
//	cast2->SetInput( caster->GetOutput() );
//	cast2->SetOutputScalarTypeToUnsignedShort();
*/
/*
	typedef vtk::itk::CastImageFilter<theImageType2, theImageType> castType;
	castType::Pointer caster = castType::New();
	caster->SetInput(vesselnessFilter->GetOutput());
	caster->Update();
*/
//---------------------------

//	vtkImageReader *reader = vtkImageReader::New();
//	reader->SetFileName("D:/Joel/TMP/brain.vtk");
//	reader->Update();

//	itkImageToVTKImageFilter *filter = itkImageToVTKImageFilter::New();
//  vtkITKImageToImageFilter *filter = vtkITKImageToImageFilter::New();

//  typedef itk::CastImageFilter<theImageType2, theImageType> castType;
//	castType::Pointer caster = castType::New();
//	caster->SetInput(vesselnessFilter->GetOutput());
//	caster->Update();

// vtkImageCast filter casts the input type to match the output type in
// the image processing pipeline.  The filter does nothing if the input
// already has the correct type.  To specify the "CastTo" type,
// use "SetOutputScalarType" method.

//	vtkImageCast *caster = vtkImageCast::New();
//	caster->SetOutputScalarTypeToUnsignedShort();
//	caster->SetInput( reader->GetOutputDataObject(1) );

//---------------------------

	// Create transfer mapping scalar value to opacity
	vtkPiecewiseFunction *opacityTF = vtkPiecewiseFunction::New();
	opacityTF->AddSegment(0, 0.1, 200, 0.9);

	// Create a transfer function mapping scalar value to color (grey)
	vtkPiecewiseFunction *grayTransferFunction = vtkPiecewiseFunction::New();
    grayTransferFunction->AddSegment( top_image->get_min_float() , 0.0 , top_image->get_max_float() , 1.0 );

	// Create mip properties
	vtkVolumeProperty *mipprop = vtkVolumeProperty::New();
//	mipprop->SetScalarOpacity(opacityTF); // oTFun2/opacityTF/tFun
	mipprop->SetInterpolationTypeToLinear(); // alt ToLinear/ToNearest
	mipprop->SetColor(grayTransferFunction); // gTFun/cTFun/colorTF/grayTransferFunction

  vtkVolume *volumeMIP = vtkVolume::New();
	volumeMIP->SetProperty(mipprop);
//	volumeMIP->AddPosition(10,20,30);

	vtkVolumeRayCastMIPFunction *MIPFunction = vtkVolumeRayCastMIPFunction::New();
	MIPFunction->SetMaximizeMethodToScalarValue();


   vtkVolumeRayCastMapper *raycastMapperMIP = vtkVolumeRayCastMapper::New();
//   vtkFixedPointVolumeRayCastMapper *raycastMapperMIP = vtkFixedPointVolumeRayCastMapper::New();

   


	raycastMapperMIP->SetInputConnection(top_image->getvtkStructuredPoints());
//	raycastMapperMIP->SetInput(reader->GetOutput());
//	raycastMapperMIP->SetInput(caster->GetOutput());
//	raycastMapperMIP->SetInputConnection(cast2->GetOutputPort());
//	raycastMapperMIP->SetInputConnection(reader2->GetOutputPort());
//	raycastMapperMIP->SetInput(reader2->GetOutput());								//JK
//	raycastMapperMIP->SetInput(vtkImporter->GetOutput());							//JK
//	raycastMapperMIP->SetInputConnection(vtkImporter->GetOutputPort());				//JK
//	raycastMapperMIP->SetInputConnection(reader->GetOutputPort());


  //raycastMapperMIP->SetGradientEstimator(gradest); 

	raycastMapperMIP->SetVolumeRayCastFunction(MIPFunction);  // MIPFunction1/2 

	volumeMIP->SetMapper(raycastMapperMIP);
	ren->AddViewProp(volumeMIP);

	ren->ResetCamera();
	ren->GetActiveCamera()->Elevation(-90);
	//ren->GetActiveCamera()->SetViewUp( 0, 1, 0); //SO
	ren->GetActiveCamera()->GetPosition(Xcam2, Ycam2, Zcam2);
	ren->GetActiveCamera()->SetParallelProjection(1);
//	ren->SetBackground(0.4392, 0.5020, 0.5647);
	ren->SetBackground(0.1, 0.1, 0.1);

	//:: 080908 :: //testa vtkCallbackCOmmand
	vtkPointPicker *picker = vtkPointPicker::New();
	cout << "Picker set" << endl;
	//w_picker = vtkWorldPointPicker::New();
	PickPosCommand *PickObserver = PickPosCommand::New();
	picker->AddObserver(vtkCommand::EndPickEvent, PickObserver);
	//::		::

   vtkInteractorStyleSwitch *intStyle = vtkInteractorStyleSwitch::New();
    intStyle->SetCurrentStyleToTrackballCamera();
    fl_vtk_window->SetInteractorStyle(intStyle);
	fl_vtk_window->SetPicker(picker);
	//fl_vtk_window->AddObserver(vtkCommand::KeyPressEvent //999
	

	fl_vtk_window->SetDesiredUpdateRate(3.0);
	fl_vtk_window->Initialize();

	// Clean up
//	reader->Delete();
	picker->RemoveObserver(PickObserver);
	picker->Delete();

	opacityTF->Delete();
	mipprop->Delete();
	MIPFunction->Delete();
	volumeMIP->Delete();
	raycastMapperMIP->Delete();
	ren->Delete();
	renWin->Delete();

	} 
}


/*
FLTKpane2::FLTKpane2(): FLTKpane(0,0,100,100)
{
	cout<<"FLTKpane2()..."<<endl;
	Fl_Button *b = new Fl_Button(10,10,80,80, "FLTKpane2()_button");
	b->color(FL_YELLOW);

	this->end();
}

FLTKpane2::FLTKpane2(int X,int Y,int W,int H): FLTKpane(X,Y,W,H)
{
	cout<<"FLTKpane2(xywh)..."<<endl;
	Fl_Button *b = new Fl_Button(10,10,80,80, "FLTKpane2(xywh)_button");
	b->color(FL_YELLOW);
	this->end();
}
*/



FLTK_Event_pane::FLTK_Event_pane(int X,int Y,int W,int H, FLTK_Pt_pane *fpp) : Fl_Widget(X,Y,W,H)
{
	//cout<<"FLTK_Event_pane("<<X<<","<<Y<<","<<W<<","<<H<<")"<<endl;
	my_base_pt_pane = fpp;
}

int FLTK_Event_pane::handle(int event){
//	cout<<"FLTK_Event_pane::handle("<<eventnames[event]<<") ";

	this->callback_event = viewport_event(event,this);

    switch(event)
        {
        case FL_ENTER:
            Fl::focus(this);
            //allows keyboard events to be received once the mouse is inside
        case FL_FOCUS:
        case FL_UNFOCUS:
            //usually, one wants to show that a widget has focus
            //which is shown/hidden with these events
            //with mouse-over focus however, this can be annoying
            
            this->callback_event.grab();
            break;
        }
    
   this->do_callback();

/*
    if(callback_event.handled()){
		return 1; 
	}else{
		return 0; 
	}
    
    callback_event = viewport_event (pt_event::no_type,NULL);
    return 1;
*/

return 1;//	return Fl_Widget::handle(event);
}

void FLTK_Event_pane::resize(int new_in_x,int new_in_y, int new_in_w,int new_in_h)
{
//	cout<<"-----------------------------------------------"<<endl;
//	cout<<"FLTK_Event_pane::resize("<<new_in_x<<","<<new_in_y<<","<<new_in_w<<","<<new_in_h<<")"<<endl;

	Fl_Widget::resize(new_in_x,new_in_y,new_in_w,new_in_h);	//We have to update "our own" size aswell

    //store new size so CB_ACTION_RESIZE will know about it (via callback)

//	resize_w = new_in_w; 
//	resize_h = new_in_h;
	callback_event = viewport_event(pt_event::resize, this);
    callback_event.set_resize(new_in_w,new_in_h);
    do_callback();
    
    //do the actual resize - redraw will follow, eventually
 
    //Update "new" size so that pixmap reevaluation won't be triggered until next resize
//    resize_w=w(); 
//	resize_h=h();
}


void FLTK_Event_pane::draw()
{
//	cout << "Nu ska h‰r ritas!!!" << endl;
    callback_event = viewport_event(pt_event::draw,this);
	do_callback(CB_ACTION_DRAW); //JK2
}

void FLTK_Event_pane::draw(unsigned char *rgbimage)
{
    if (w()>0 && h()>0){
	//	cout<<"Nu ritar vi!!!("<<this->x()<<","<<this->y()<<","<<w()<<","<<h()<<endl;
        // do not redraw zero-sized viewport, fl_draw_image will break down
		
//		fl_draw_image(rgbimage,0,0,w(),h());
//		Fl_Group::current(this); //JK-tmp
//		Fl_Widget::current(this); //JK-tmp
		fl_draw_image(rgbimage,this->x(),this->y(),w(),h()); //JK3

    }
}

void FLTK_Event_pane::do_callback(callbackAction action)
{
    callback_action=action;
    Fl_Widget::do_callback();
    callback_action=CB_ACTION_NONE;
}

void FLTK_Event_pane::needs_rerendering()
{
	my_base_pt_pane->needs_rerendering();
//	((FLTK_Pt_pane*)this->parent())->needs_rerendering();
}


//---------------------------------------------
//---------------------------------------------
//---------------------------------------------

FLTK_Pt_pane::FLTK_Pt_pane():FLTKpane(0,0,100,100)
{
	cout<<"FLTK_Pt_pane()"<<endl;
    int buttonleft=0;
	int buttonheight=20;
	int buttonwidth=70;


//	event_pane = new FLTK_Event_pane(0,buttonheight,100,100-buttonheight);

	Fl_Button *b = new Fl_Button(10,50,20,200, "pt_pane()_button");
	b->color(FL_BLUE);

 //   this->box(FL_BORDER_BOX);
//    this->align(FL_ALIGN_CLIP);

//	this->resizable(event_pane);			//Make sure thes is resized too...
//	this->end();
}


//FLTK_Pt_pane::FLTK_Pt_pane(int X,int Y,int W,int H, viewport *vp_parent) : Fl_Overlay_Window(X,Y,W,H)
FLTK_Pt_pane::FLTK_Pt_pane(int X,int Y,int W,int H) : FLTKpane(X,Y,W,H)
{
//	cout<<"FLTK_Pt_pane("<<X<<","<<Y<<","<<W<<","<<H<<")"<<endl;
	Fl_Button *b = new Fl_Button(10,50,20,200, "pt_pane()_button");
	b->color(FL_BLUE);

}

FLTK_Pt_pane::~FLTK_Pt_pane()
{
	delete event_pane;
}

int FLTK_Pt_pane::h_pane()
{
	return event_pane->h();
}

void FLTK_Pt_pane::needs_rerendering()
{
	get_viewport_parent()->needs_rerendering();
}

void FLTK_Pt_pane::draw_overlay()
{
	((FLTKviewport*)this->parent())->viewport_parent->paint_overlay();
}

void FLTK_Pt_pane::resize_content(int w,int h)
{
	cout<<"FLTK_Pt_pane::resize_content("<<w<<","<<h<<")"<<endl;
	if(event_pane == NULL)
		cout << "NULL!!!" << endl;
	else{
//		event_pane->resize(0,0,w,h); //JK //RN --> se fˆr fan till att det stÂr 20 h‰r!!!!
		event_pane->resize(event_pane->x(),event_pane->y(),w,h-20); //JK //RN --> se fˆr fan till att det stÂr 20 h‰r!!!!
	}
}

viewport* FLTK_Pt_pane::get_viewport_parent()
{
	return ((FLTKviewport*)this->parent())->viewport_parent;
}


//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------

FLTK_Pt_MPR_pane::FLTK_Pt_MPR_pane() : FLTK_Pt_pane(0,0,100,100)
{
//	cout<<"FLTK_Pt_MPR_pane()"<<endl;
    int buttonleft=0;
	int buttonheight=20;
	int buttonwidth=70;
	create_MPR_menu(100);

//	Fl_Group::current(this);	//JK //RN test
//	Fl_Button *b = new Fl_Button(0,buttonheight,100,100-buttonheight, "FLTK_Pt_MPR_pane()_button");	 //JK //RN test
	Fl_Button *b = new Fl_Button(0,buttonheight,100,100-buttonheight, "FLTK_Pt_MPR_pane()_button");	 //JK //RN test
	b->color(FL_RED);

	event_pane = NULL;
	event_pane = new FLTK_Event_pane(0,buttonheight,100,100-buttonheight, this); //JK //RN test

//	viewport::set_the_widget_static_callback(viewport_callback, this->parent());
//	event_pane->callback(viewport_callback, this->parent()); //viewport (_not_ FLTK_Pt_pane) handles the callbacks


	this->resizable(event_pane);			//Make sure this is resized too...
	this->end();
}

//FLTK_Pt_pane::FLTK_Pt_pane(int X,int Y,int W,int H, viewport *vp_parent) : Fl_Overlay_Window(X,Y,W,H)
FLTK_Pt_MPR_pane::FLTK_Pt_MPR_pane(int X,int Y,int W,int H) : FLTK_Pt_pane(X,Y,W,H)
{
//	cout<<"FLTK_Pt_MPR_pane("<<X<<","<<Y<<","<<W<<","<<H<<")"<<endl;

	int buttonleft=0;
	int buttonheight=20;
	int buttonwidth=70;

	create_MPR_menu(W);
	//--------------------------------

	event_pane = new FLTK_Event_pane(0,buttonheight,W,H-buttonheight,this);

//	Fl_Button *b = new Fl_Button(25,5,200,200, "pt_pane(xywh)_button");
//	b->color(FL_BLUE);

//    this->box(FL_BORDER_BOX);
//    this->align(FL_ALIGN_CLIP);
	this->resizable(event_pane);			//Make sure thes is resized too...
	this->end();
}

FLTK_Pt_MPR_pane::~FLTK_Pt_MPR_pane()
{}

void FLTK_Pt_MPR_pane::create_MPR_menu(int W){
	int buttonleft=0;
	int buttonheight=20;
	int buttonwidth=70;

//	Fl_Group::current(this);

	//--------------------------------
    button_pack2 = new Fl_Pack(0,0,W,buttonheight,"");
    button_pack2->type(FL_HORIZONTAL);

	Fl_Menu_Item dir_menu_items[NUM_DIRECTIONS+1];
    
    int m;
    for(m=0;m<NUM_DIRECTIONS;m++){
        menu_callback_params * cbp=new menu_callback_params;
        cbp->direction=(preset_direction)m;
        cbp->vport=get_viewport_parent();
        
        init_fl_menu_item(dir_menu_items[m]);
        dir_menu_items[m].label(preset_direction_labels[m]);
        dir_menu_items[m].callback(&set_direction_callback);
        dir_menu_items[m].user_data(cbp);
        dir_menu_items[m].flags= FL_MENU_RADIO;
    }

    dir_menu_items[m].label(NULL);	//terminate menu
    dir_menu_items[DEFAULT_DIR].setonly();	//DEFAULT_DIR is pre-set, set checkmark accordingly

    directionmenu_button = new Fl_Menu_Button(0,0,buttonwidth,buttonheight,preset_direction_labels[Z_DIR]);
    directionmenu_button->copy(dir_menu_items);
	directionmenu_button->box(FL_THIN_UP_BOX);
	directionmenu_button->labelsize(FLTK_SMALL_LABEL);

    Fl_Menu_Item blend_menu_items [NUM_BLEND_MODES+1];
    for(m=0;m<NUM_BLEND_MODES;m++){
//		cout<<"m="<<m<<"/"<<NUM_BLEND_MODES<<endl;

        menu_callback_params * cbp=new menu_callback_params;
        cbp->mode=(blendmode)m;
		int rID = this->get_viewport_parent()->rendererID;
//		rendermanagement.print_renderers();
		cbp->rend_index = rendermanagement.find_renderer_index(rID); //might execute befor the renderer has been created...
        
        init_fl_menu_item(blend_menu_items[m]);
        
        blend_menu_items[m].label(blend_mode_labels[m]);
        blend_menu_items[m].callback(&set_blendmode_callback);
        blend_menu_items[m].user_data(cbp);
        blend_menu_items[m].flags= FL_MENU_RADIO;
        
//        if (rendererIndex < 0)
  //          {blend_menu_items[m].deactivate();}
      }
    blend_menu_items[m].label(NULL);    //terminate menu
    
    blendmenu_button = new Fl_Menu_Button(0+(buttonleft+=buttonwidth),0,buttonwidth,buttonheight, blend_mode_labels[1] );
    blendmenu_button->copy(blend_menu_items);
    blendmenu_button->box(FL_THIN_UP_BOX);
    blendmenu_button->labelsize(FLTK_SMALL_LABEL);
	//---------------------------------------
	create_geom_menu(W);
	//---------------------------------------
	button_pack2->end();
}
void FLTK_Pt_MPR_pane::create_geom_menu(int W){

	int buttonleft=0;
	int buttonheight=20;
	int buttonwidth=70;
	int m = 0;
	int nr_view = viewmanagement.get_nr_viewports();
//	cout << "nr  of viewports: " << nr_view << endl;
	//std::vector<rendergeometry_base*> geo = rendermanagement.get_geometries();
	Fl_Menu_Item *geom_menu_items = (Fl_Menu_Item*)malloc((nr_view+1)*sizeof(Fl_Menu_Item));
	int preset_vp = 0;
	char *def = "Own geom";

	//int id = rendermanagement.get_geometry(get_viewport_parent()->get_renderer_id())->get_id();
	for(m = 0; m<nr_view; m++){    
		menu_callback_params * cbp=new menu_callback_params;
		cbp->vport=get_viewport_parent();
		cbp->vp_id = viewmanagement.get_viewport_id_from_index(m);
		char *label = (char*)malloc(3*sizeof(char));// = itoa(m);
		sprintf(label,"%d",m);
		init_fl_menu_item(geom_menu_items[m]);
		geom_menu_items[m].label(label);
		geom_menu_items[m].callback(&set_geom_callback);
		geom_menu_items[m].user_data(cbp);
		geom_menu_items[m].flags= FL_MENU_RADIO;
		if(cbp->vp_id == get_viewport_parent()->get_id()){
			preset_vp = m;
			geom_menu_items[m].label(def);
		}
	}
    geom_menu_items[m].label(NULL);	//terminate menu
    geom_menu_items[preset_vp].setonly();	//DEFAULT_DIR is pre-set, set checkmark accordingly


	//Fl_Menu_Item dummy[1];
	//dummy[0].label(NULL);
	char *l = (char*)malloc((11)*sizeof(char));
	sprintf(l,"Geom: %d",preset_vp);
	/*info = new Fl_Menu_Button(0,0,buttonwidth,buttonheight,l);
    info->copy(dummy);
	info->box(FL_THIN_UP_BOX);
	info->labelsize(FLTK_SMALL_LABEL);*/

	text_box = new Fl_Output(0,0,buttonwidth,buttonheight,'\0');//Does not feel completely right. Isn't there any Fl_label or something?
	text_box->value(l);

	
	geom_button = new Fl_Menu_Button(0,0,buttonwidth,buttonheight,"Geometry");
    geom_button->copy(geom_menu_items);
	geom_button->box(FL_THIN_UP_BOX);
	geom_button->labelsize(FLTK_SMALL_LABEL);
}
viewport* FLTK_Pt_MPR_pane::get_viewport_parent()
{
	return ((FLTKviewport*)this->parent())->viewport_parent;
}



void FLTK_Pt_MPR_pane::set_direction_callback(Fl_Widget *callingwidget, void * p )
{
    menu_callback_params * params = (menu_callback_params *) p;
	params->vport->set_renderer_direction( params->direction );

	params->vport->refresh();
	viewmanagement.update_overlays();
}

void FLTK_Pt_MPR_pane::set_blendmode_callback(Fl_Widget *callingwidget, void * p )
{
    menu_callback_params * params=(menu_callback_params *)p;
    
    rendermanagement.set_blendmode(params->rend_index,params->mode);
}

void FLTK_Pt_MPR_pane::set_geom_callback(Fl_Widget *callingwidget, void * p )
{
    menu_callback_params * params = (menu_callback_params *) p;
	params->vport->change_geom_type( params->vp_id, PT_MPR);
	params->vport->refresh();
	viewmanagement.update_overlays();
}

void FLTK_Pt_MPR_pane::set_direction_button_label(preset_direction direction)
{
	directionmenu_button->label( preset_direction_labels[direction] );
	( (Fl_Menu_Item*)directionmenu_button->menu() )[direction].setonly(); 	//also activate the right radio-button...
}

void FLTK_Pt_MPR_pane::change_geom(int vp_id){

	rendergeometry_base *geom;// = rendermanagement.get_renderer(viewmanagement.get_renderer_id(vp_id))->the_rg;//   get_geometry(viewmanagement.get_renderer_id(vp_id));

	if(vp_id == this->get_viewport_parent()->get_id()){
		geom = rendermanagement.get_renderer(viewmanagement.get_renderer_id(vp_id))->original_rg;
	}else{
		geom = rendermanagement.get_renderer(viewmanagement.get_renderer_id(vp_id))->the_rg;//   get_geometry(viewmanagement.get_renderer_id(vp_id));
	}
	if(geom == NULL){
		return;
	}
	rendermanagement.get_renderer(this->get_renderer_id())->use_other_geometry(geom);
	((rendergeom_image*)geom)->refresh_viewports(); //Safe because it can oly be an image in other viewport
}
void FLTK_Pt_MPR_pane::set_renderer_direction( preset_direction direction ) 
{

    enum { x, y, z };
    
    Matrix3D dir;
	dir.SetIdentity();

	image_base *im;
    
    /**dir[0][0]=1; //voxel direction of view x
        *dir[2][1]=1; //voxel direction of view y
    *dir[1][2]=1; //voxel direction of slicing*/
    
    //remember,
    // A sagittal plane divides the body into left and right portions.
    // The midsagittal plane is in the midline, i.e. it would pass through midline structures such as the navel or spine, and all other sagittal planes are parallel to it.
    // A coronal plane divides the body into dorsal and ventral portions.
    // A transverse plane divides the body into cranial (cephalic) and caudal portions.
    
	//enum preset_direction {Z_DIR, Y_DIR, X_DIR, Z_DIR_NEG, Y_DIR_NEG, X_DIR_NEG, AXIAL};

    switch(direction){
		case DEFAULT_DIR:
        //case AXIAL:
//            dir[x][0]=1;	// the x direction of the viewport ("0") lies in the positive ("+1") x direction ("x") of the world coordinate system
  //          dir[y][1]=1;	// the y direction of the viewport ("1") lies in the positive ("+1") y direction ("y") of the world coordinete system
    //        dir[z][2]=1;	// the z direction of the viewport ("2") lies in the positive ("+1") z direction ("z") of the world coordinate system

			im = rendermanagement.get_top_image_from_renderer(this->get_renderer_id());
			if(im!=NULL){
				dir = im->get_dir_rendering_matrix(direction);
			}
            break;

		case AXIAL:
			im = rendermanagement.get_top_image_from_renderer(this->get_renderer_id());
			if(im!=NULL){
				dir = im->get_dir_rendering_matrix(direction);
			}
            break;

		case SAGITTAL:
			im = rendermanagement.get_top_image_from_renderer(this->get_renderer_id());
			if(im!=NULL){
				dir = im->get_dir_rendering_matrix(direction);
			}
            break;

		case CORONAL:
			im = rendermanagement.get_top_image_from_renderer(this->get_renderer_id());
			if(im!=NULL){
				dir = im->get_dir_rendering_matrix(direction);
			}
            break;

	
		case Z_DIR:
			dir.Fill(0);
            dir[x][0]=1;	// the x direction of the viewport ("0") lies in the positive ("+1") x direction ("x") of the world coordinate system
            dir[y][1]=1;	// the y direction of the viewport ("1") lies in the positive ("+1") y direction ("y") of the world coordinete system
            dir[z][2]=1;	// the z direction of the viewport ("2") lies in the positive ("+1") z direction ("z") of the world coordinate system
            break;
            
        case Y_DIR:
			dir.Fill(0);
            dir[x][0]=1;
            dir[z][1]=-1;
            dir[y][2]=1;
            break;
            
        case X_DIR:
			dir.Fill(0);
            dir[y][0]=-1;
            dir[z][1]=-1;
            dir[x][2]=1;
            break;
            
        case Z_DIR_NEG:
			dir.Fill(0);
            dir[x][0]=-1;
            dir[y][1]=1;
            dir[z][2]=-1;
            break;
            
        case Y_DIR_NEG:
			dir.Fill(0);
            dir[x][0]=-1;
            dir[z][1]=-1;
            dir[y][2]=-1;
            break;
            
        case X_DIR_NEG:
			dir.Fill(0);
            dir[y][0]=1;
            dir[z][1]=-1;
            dir[x][2]=-1;
            break;

    }

	
	this->get_viewport_parent()->set_renderer_direction( dir );
	this->set_direction_button_label(direction);
}


void FLTK_Pt_MPR_pane::rebuild_blendmode_menu()//update checkmark for current blend mode
{
    if(blendmenu_button != NULL){
        Fl_Menu_Item *blendmodemenu=(Fl_Menu_Item *)blendmenu_button->menu();

		//blend modes are different - or not available - for other renderer types (than MPR...)
		//basic check for this:
		//int this_renderer_type=rendermanagement.get_renderer_type(rendererIndex);
		
		int rendInd = this->get_viewport_parent()->rendererIndex;
		for(int m=0; m<NUM_BLEND_MODES; m++){
			((menu_callback_params*)(blendmodemenu[m].argument()))->rend_index = rendInd;
			if(rendInd<0 || !rendermanagement.renderer_supports_mode(rendInd,m)){
				blendmodemenu[m].deactivate();
			}else{
				blendmodemenu[m].activate();
			}
		}
		if(rendInd >= 0){
			int this_blend_mode=rendermanagement.get_blend_mode(rendInd);
			blendmodemenu[this_blend_mode].setonly();
			blendmenu_button->label(blend_mode_labels[this_blend_mode]);
		}
	}
}

void FLTK_Pt_MPR_pane::refresh_menus()
{
	rebuild_blendmode_menu();
}

//--------------------------------------------------------
//--------------------------------------------------------

FLTK_Pt_Curve_pane::FLTK_Pt_Curve_pane() : FLTK_Pt_pane(0,20,100,100)
{
	cout<<"FLTK_Pt_Curve_pane()"<<endl;
    int buttonleft=0;
	int buttonheight=20;
	int buttonwidth=70;
	create_curve_menu(100);
	event_pane = new FLTK_Event_pane(0,buttonheight,100,100-buttonheight, this);

	this->resizable(event_pane);			//Make sure thes is resized too...
	this->end();
	
}

//FLTK_Pt_pane::FLTK_Pt_pane(int X,int Y,int W,int H, viewport *vp_parent) : Fl_Overlay_Window(X,Y,W,H)
FLTK_Pt_Curve_pane::FLTK_Pt_Curve_pane(int X,int Y,int W,int H) : FLTK_Pt_pane(X,Y,W,H)
{
	cout<<"FLTK_Pt_Curve_pane("<<X<<","<<Y<<","<<W<<","<<H<<")"<<endl;
    int buttonleft=0;
	int buttonheight=20;
	int buttonwidth=70;
/*
Here the code for the menu was before
*/
	create_curve_menu(W);
	event_pane = new FLTK_Event_pane(0,buttonheight,W,H-buttonheight,this);
	this->resizable(event_pane);			//Make sure thes is resized too...
	this->end();
}
void FLTK_Pt_Curve_pane::create_geom_menu(int W){

	int buttonleft=0;
	int buttonheight=20;
	int buttonwidth=70;
	int m = 0;

	int nr_view = viewmanagement.get_nr_viewports();
//	cout << "nr  of viewports: " << nr_view << endl;
	//std::vector<rendergeometry_base*> geo = rendermanagement.get_geometries();
	Fl_Menu_Item *geom_menu_items = (Fl_Menu_Item*)malloc((nr_view+1)*sizeof(Fl_Menu_Item));
	int preset_vp = 0;
	char *def = "Own geom";
	//int id = rendermanagement.get_geometry(get_viewport_parent()->get_renderer_id())->get_id();
	for(m = 0; m<nr_view; m++){    
		menu_callback_params * cbp=new menu_callback_params;
		cbp->vport=get_viewport_parent();
		cbp->vp_id = viewmanagement.get_viewport_id_from_index(m);
		char *label = (char*)malloc(3*sizeof(char));// = itoa(m);
		sprintf(label,"%d",m);
		init_fl_menu_item(geom_menu_items[m]);
		geom_menu_items[m].label(label);
		geom_menu_items[m].callback(&set_geom_callback);
		geom_menu_items[m].user_data(cbp);
		geom_menu_items[m].flags= FL_MENU_RADIO;
		if(cbp->vp_id == get_viewport_parent()->get_id()){
			preset_vp = m;
			geom_menu_items[m].label(def);
		}
	}
    geom_menu_items[m].label(NULL);	//terminate menu
    geom_menu_items[preset_vp].setonly();	//DEFAULT_DIR is pre-set, set checkmark accordingly
	
	

	Fl_Menu_Item dummy[1];
	dummy[0].label(NULL);
	char *l = (char*)malloc((11)*sizeof(char));
	sprintf(l,"Geom: %d",preset_vp);
	info = new Fl_Menu_Button(0,0,buttonwidth,buttonheight,l);
    info->copy(dummy);
	info->box(FL_THIN_UP_BOX);
	info->labelsize(FLTK_SMALL_LABEL);

	geom_button = new Fl_Menu_Button(0,0,buttonwidth,buttonheight,"Geometry");
    geom_button->copy(geom_menu_items);
	geom_button->box(FL_THIN_UP_BOX);
	geom_button->labelsize(FLTK_SMALL_LABEL);

}
void FLTK_Pt_Curve_pane::create_curve_menu(int W){
	int buttonleft=0;
	int buttonheight=20;
	int buttonwidth=70;

	//Fl_Group::current(this);

	//--------------------------------
    button_pack2 = new Fl_Pack(0,0,W,buttonheight,"");
    button_pack2->type(FL_HORIZONTAL);
	Fl_Menu_Item col_menu_items[9];
	//enum colors {WHITE, BLACK, RED, GREEN, BLUE, YELLOW, MAGENTA, CYAN};
    
    int m = 0;
    
	for(m = 0; m<8; m++){    
		menu_callback_params * cbp=new menu_callback_params;
		cbp->color=(colors)m;
		cbp->vport=get_viewport_parent();
		init_fl_menu_item(col_menu_items[m]);
		col_menu_items[m].label(preset_color_labels[m]);
		col_menu_items[m].callback(&set_color_callback);
		col_menu_items[m].user_data(cbp);
		col_menu_items[m].flags= FL_MENU_RADIO;
	}

    col_menu_items[m].label(NULL);	//terminate menu
    col_menu_items[RED].setonly();	//DEFAULT_DIR is pre-set, set checkmark accordingly

    colormenu_button = new Fl_Menu_Button(0,0,buttonwidth,buttonheight,"color");
    colormenu_button->copy(col_menu_items);
	colormenu_button->box(FL_THIN_UP_BOX);
	colormenu_button->labelsize(FLTK_SMALL_LABEL);
	//------------------------------------
	Fl_Menu_Item bg_menu_items [4];
	
	menu_callback_params * cbp=new menu_callback_params;
	cbp->line = '.';
	cbp->vport=get_viewport_parent();
    init_fl_menu_item(bg_menu_items[0]);
        
    bg_menu_items[0].label(".");
    bg_menu_items[0].callback(&set_line_callback);
    bg_menu_items[0].user_data(cbp);
    bg_menu_items[0].flags= FL_MENU_RADIO;

	menu_callback_params * cb=new menu_callback_params;
	cb->line = '-';
	cb->vport=get_viewport_parent();
    init_fl_menu_item(bg_menu_items[1]);

	bg_menu_items[1].label("-");
    bg_menu_items[1].callback(&set_line_callback);
    bg_menu_items[1].user_data(cb);
    bg_menu_items[1].flags= FL_MENU_RADIO;


	menu_callback_params * bp=new menu_callback_params;
	bp->line = '|';
	bp->vport=get_viewport_parent();
    init_fl_menu_item(bg_menu_items[2]);

	bg_menu_items[2].label("|");
    bg_menu_items[2].callback(&set_line_callback);
    bg_menu_items[2].user_data(bp);
    bg_menu_items[2].flags= FL_MENU_RADIO;


        
    bg_menu_items[3].label(NULL);    //terminate menu
	bg_menu_items[1].setonly(); //lines
    
    bgmenu_button = new Fl_Menu_Button(0+(buttonleft+=buttonwidth),0,buttonwidth,buttonheight, "line");
    bgmenu_button->copy(bg_menu_items);
    bgmenu_button->box(FL_THIN_UP_BOX);
    bgmenu_button->labelsize(FLTK_SMALL_LABEL);
	//---------------------------------------
	create_geom_menu(W);
	//---------------------------------------
	button_pack2->end();
	//--------------------------------
}
FLTK_Pt_Curve_pane::~FLTK_Pt_Curve_pane()
{}

viewport* FLTK_Pt_Curve_pane::get_viewport_parent()
{
	return ((FLTKviewport*)this->parent())->viewport_parent;
}



void FLTK_Pt_Curve_pane::set_color_callback(Fl_Widget *callingwidget, void * p )
{
    menu_callback_params * params = (menu_callback_params *) p;
	params->vport->change_color( params->color , PT_CURVE);
	params->vport->refresh();
	viewmanagement.update_overlays();
}

void FLTK_Pt_Curve_pane::set_line_callback(Fl_Widget *callingwidget, void * p )
{
    menu_callback_params * params = (menu_callback_params *) p;
	params->vport->change_line_type( params->line , PT_CURVE);
	params->vport->refresh();
	viewmanagement.update_overlays();
}
void FLTK_Pt_Curve_pane::set_geom_callback(Fl_Widget *callingwidget, void * p )
{
    menu_callback_params * params = (menu_callback_params *) p;
	params->vport->change_geom_type( params->vp_id, PT_CURVE);
	params->vport->refresh();
	viewmanagement.update_overlays();
}

void FLTK_Pt_Curve_pane::set_color_button_label(colors c)
{
	colormenu_button->label( preset_color_labels[c] );
	( (Fl_Menu_Item*)colormenu_button->menu() )[c].setonly(); 	//also activate the right radio-button...
}
//--------------------------------------------------------


void FLTK_Pt_Curve_pane::change_line(char line){
	curve_base *curve = ((rendergeom_curve *)rendermanagement.get_geometry(this->get_renderer_id()))->curve;
	//curve_base *curve = ((renderer_curve *)rendermanagement.get_renderer(this->get_renderer_id))->get_top();
	if(curve == NULL){
		return;
	}
	curve->set_line(line);
}

void FLTK_Pt_Curve_pane::change_color(colors color){
	curve_base *curve = ((rendergeom_curve *)rendermanagement.get_geometry(this->get_renderer_id()))->curve;
	//curve_base *curve = ((renderer_curve *)rendermanagement.get_renderer(this->get_renderer_id))->get_top();
	if(curve == NULL){
		return;
	}
	switch(color){
		case BLACK:
			curve->set_color(0,0,0);
			break;
		case WHITE:
			curve->set_color(255,255,255);
			break;
		case RED:
			curve->set_color(255,0,0);
			break;
		case GREEN:
			curve->set_color(0,255,0);
			break;
		case BLUE:
			curve->set_color(0,0,255);
			break;
		case YELLOW:
			curve->set_color(255,255,0);
			break;
		case MAGENTA:
			curve->set_color(255,0,255);
			break;
		case CYAN:
			curve->set_color(0,255,255);
			break;
		default:
			curve->set_color(0,0,0);
			break;
	}
}
void FLTK_Pt_Curve_pane::change_geom(int vp_id){
	rendergeometry_base *geom;
	if(vp_id == this->get_viewport_parent()->get_id()){
		geom = rendermanagement.get_renderer(viewmanagement.get_renderer_id(vp_id))->original_rg;
	}else{
		geom = rendermanagement.get_renderer(viewmanagement.get_renderer_id(vp_id))->the_rg;//   get_geometry(viewmanagement.get_renderer_id(vp_id));
	}
	//rendergeometry_base *base;
	//base = rendermanagement.get_geometry(this->get_renderer_id());
	
	//curve_base *curve = ((renderer_curve *)rendermanagement.get_renderer(this->get_renderer_id))->get_top();
	if(geom == NULL){
		return;
	}
	rendermanagement.get_renderer(this->get_renderer_id())->use_other_geometry(geom);
	((rendergeom_image*)geom)->refresh_viewports(); //Safe because it can oly be an image in other viewport
}
//--------------------------------------------------------




//************************************************************************//
FLTK_Pt_Spectrum_pane::FLTK_Pt_Spectrum_pane() : FLTK_Pt_pane(0,20,100,100)
{
	cout<<"FLTK_Pt_Curve_pane()"<<endl;
    int buttonleft=0;
	int buttonheight=20;
	int buttonwidth=70;
	create_curve_menu(100);
	event_pane = new FLTK_Event_pane(0,buttonheight,100,100-buttonheight, this);

	this->resizable(event_pane);			//Make sure thes is resized too...
	this->end();
	
}

//FLTK_Pt_pane::FLTK_Pt_pane(int X,int Y,int W,int H, viewport *vp_parent) : Fl_Overlay_Window(X,Y,W,H)
FLTK_Pt_Spectrum_pane::FLTK_Pt_Spectrum_pane(int X,int Y,int W,int H) : FLTK_Pt_pane(X,Y,W,H)
{
	cout<<"FLTK_Pt_Curve_pane("<<X<<","<<Y<<","<<W<<","<<H<<")"<<endl;
    int buttonleft=0;
	int buttonheight=20;
	int buttonwidth=70;
/*
Here the code for the menu was before
*/
	create_curve_menu(W);
	event_pane = new FLTK_Event_pane(0,buttonheight,W,H-buttonheight,this);
	this->resizable(event_pane);			//Make sure thes is resized too...
	this->end();
}
void FLTK_Pt_Spectrum_pane::create_geom_menu(int W){

	int buttonleft=0;
	int buttonheight=20;
	int buttonwidth=70;
	int m = 0;

	int nr_view = viewmanagement.get_nr_viewports();
//	cout << "nr  of viewports: " << nr_view << endl;
	//std::vector<rendergeometry_base*> geo = rendermanagement.get_geometries();
	Fl_Menu_Item *geom_menu_items = (Fl_Menu_Item*)malloc((nr_view+1)*sizeof(Fl_Menu_Item));
	int preset_vp = 0;
	char *def = "Own geom";
	//int id = rendermanagement.get_geometry(get_viewport_parent()->get_renderer_id())->get_id();
	for(m = 0; m<nr_view; m++){    
		menu_callback_params * cbp=new menu_callback_params;
		cbp->vport=get_viewport_parent();
		cbp->vp_id = viewmanagement.get_viewport_id_from_index(m);
		char *label = (char*)malloc(3*sizeof(char));// = itoa(m);
		sprintf(label,"%d",m);
		init_fl_menu_item(geom_menu_items[m]);
		geom_menu_items[m].label(label);
		geom_menu_items[m].callback(&set_geom_callback);
		geom_menu_items[m].user_data(cbp);
		geom_menu_items[m].flags= FL_MENU_RADIO;
		if(cbp->vp_id == get_viewport_parent()->get_id()){
			preset_vp = m;
			geom_menu_items[m].label(def);
		}
	}
    geom_menu_items[m].label(NULL);	//terminate menu
    geom_menu_items[preset_vp].setonly();	//DEFAULT_DIR is pre-set, set checkmark accordingly
	
	

	Fl_Menu_Item dummy[1];
	dummy[0].label(NULL);
	char *l = (char*)malloc((11)*sizeof(char));
	sprintf(l,"Geom: %d",preset_vp);
	info = new Fl_Menu_Button(0,0,buttonwidth,buttonheight,l);
    info->copy(dummy);
	info->box(FL_THIN_UP_BOX);
	info->labelsize(FLTK_SMALL_LABEL);

	geom_button = new Fl_Menu_Button(0,0,buttonwidth,buttonheight,"Geometry");
    geom_button->copy(geom_menu_items);
	geom_button->box(FL_THIN_UP_BOX);
	geom_button->labelsize(FLTK_SMALL_LABEL);

}

void FLTK_Pt_Spectrum_pane::create_x_menu(int W){

	int buttonleft=0;
	int buttonheight=20;
	int buttonwidth=70;
	int m = 0;

	const char * names[] = {"freq", "time"};
	//std::vector<rendergeometry_base*> geo = rendermanagement.get_geometries();
	Fl_Menu_Item x_menu_items[3];
	//int id = rendermanagement.get_geometry(get_viewport_parent()->get_renderer_id())->get_id();
	for(m = 0; m<2; m++){    
		menu_callback_params * cbp=new menu_callback_params;
		cbp->vport=get_viewport_parent();
		cbp->line = names[m][0];
		init_fl_menu_item(x_menu_items[m]);
		x_menu_items[m].label(names[m]);
		x_menu_items[m].callback(&set_x_callback);
		x_menu_items[m].user_data(cbp);
		x_menu_items[m].flags= FL_MENU_RADIO;
	}
    x_menu_items[m].label(NULL);	//terminate menu
    x_menu_items[0].setonly();	//DEFAULT_DIR is pre-set, set checkmark accordingly

	x_button = new Fl_Menu_Button(0,0,buttonwidth,buttonheight,"x axis");
    x_button->copy(x_menu_items);
	x_button->box(FL_THIN_UP_BOX);
	x_button->labelsize(FLTK_SMALL_LABEL);

}
void FLTK_Pt_Spectrum_pane::create_y_menu(int W){

	int buttonleft=0;
	int buttonheight=20;
	int buttonwidth=70;
	int m = 0;

	const char * names[] = {"real", "complex", "magnitude", "phase"};
	//std::vector<rendergeometry_base*> geo = rendermanagement.get_geometries();
	Fl_Menu_Item y_menu_items[5];
	//int id = rendermanagement.get_geometry(get_viewport_parent()->get_renderer_id())->get_id();
	for(m = 0; m<4; m++){    
		menu_callback_params * cbp=new menu_callback_params;
		cbp->vport=get_viewport_parent();
		cbp->line = names[m][0];
		init_fl_menu_item(y_menu_items[m]);
		y_menu_items[m].label(names[m]);
		y_menu_items[m].callback(&set_y_callback);
		y_menu_items[m].user_data(cbp);
		y_menu_items[m].flags= FL_MENU_TOGGLE;
	}
    y_menu_items[m].label(NULL);	//terminate menu
    //y_menu_items[0].set();	//DEFAULT_DIR is pre-set, set checkmark accordingly

	y_button = new Fl_Menu_Button(0,0,buttonwidth,buttonheight,"y axis");
    y_button->copy(y_menu_items);
	y_button->box(FL_THIN_UP_BOX);
	y_button->labelsize(FLTK_SMALL_LABEL);

}


void FLTK_Pt_Spectrum_pane::create_curve_menu(int W){
	int buttonleft=0;
	int buttonheight=20;
	int buttonwidth=70;

	//Fl_Group::current(this);

	//--------------------------------
    button_pack2 = new Fl_Pack(0,0,W,buttonheight,"");
    button_pack2->type(FL_HORIZONTAL);
	Fl_Menu_Item col_menu_items[9];
	//enum colors {WHITE, BLACK, RED, GREEN, BLUE, YELLOW, MAGENTA, CYAN};
    
    int m = 0;
    
	for(m = 0; m<8; m++){    
		menu_callback_params * cbp=new menu_callback_params;
		cbp->color=(colors)m;
		cbp->vport=get_viewport_parent();
		init_fl_menu_item(col_menu_items[m]);
		col_menu_items[m].label(preset_color_labels[m]);
		col_menu_items[m].callback(&set_color_callback);
		col_menu_items[m].user_data(cbp);
		col_menu_items[m].flags= FL_MENU_RADIO;
	}

    col_menu_items[m].label(NULL);	//terminate menu
    col_menu_items[RED].setonly();	//DEFAULT_DIR is pre-set, set checkmark accordingly

    colormenu_button = new Fl_Menu_Button(0,0,buttonwidth,buttonheight,"color");
    colormenu_button->copy(col_menu_items);
	colormenu_button->box(FL_THIN_UP_BOX);
	colormenu_button->labelsize(FLTK_SMALL_LABEL);
	//------------------------------------
	Fl_Menu_Item bg_menu_items [4];
	
	menu_callback_params * cbp=new menu_callback_params;
	cbp->line = '.';
	cbp->vport=get_viewport_parent();
    init_fl_menu_item(bg_menu_items[0]);
        
    bg_menu_items[0].label(".");
    bg_menu_items[0].callback(&set_line_callback);
    bg_menu_items[0].user_data(cbp);
    bg_menu_items[0].flags= FL_MENU_RADIO;

	menu_callback_params * cb=new menu_callback_params;
	cb->line = '-';
	cb->vport=get_viewport_parent();
    init_fl_menu_item(bg_menu_items[1]);

	bg_menu_items[1].label("-");
    bg_menu_items[1].callback(&set_line_callback);
    bg_menu_items[1].user_data(cb);
    bg_menu_items[1].flags= FL_MENU_RADIO;


	menu_callback_params * bp=new menu_callback_params;
	bp->line = '|';
	bp->vport=get_viewport_parent();
    init_fl_menu_item(bg_menu_items[2]);

	bg_menu_items[2].label("|");
    bg_menu_items[2].callback(&set_line_callback);
    bg_menu_items[2].user_data(bp);
    bg_menu_items[2].flags= FL_MENU_RADIO;


        
    bg_menu_items[3].label(NULL);    //terminate menu
	bg_menu_items[1].setonly(); //lines
    
    bgmenu_button = new Fl_Menu_Button(0+(buttonleft+=buttonwidth),0,buttonwidth,buttonheight, "line");
    bgmenu_button->copy(bg_menu_items);
    bgmenu_button->box(FL_THIN_UP_BOX);
    bgmenu_button->labelsize(FLTK_SMALL_LABEL);
	//---------------------------------------
	create_geom_menu(W);
	create_x_menu(W);
	create_y_menu(W);
	//---------------------------------------
	button_pack2->end();
	//--------------------------------
}
FLTK_Pt_Spectrum_pane::~FLTK_Pt_Spectrum_pane()
{}

viewport* FLTK_Pt_Spectrum_pane::get_viewport_parent()
{
	return ((FLTKviewport*)this->parent())->viewport_parent;
}



void FLTK_Pt_Spectrum_pane::set_color_callback(Fl_Widget *callingwidget, void * p )
{
    menu_callback_params * params = (menu_callback_params *) p;
	params->vport->change_color( params->color , PT_SPECTRUM);
	params->vport->refresh();
	viewmanagement.update_overlays();
}

void FLTK_Pt_Spectrum_pane::set_line_callback(Fl_Widget *callingwidget, void * p )
{
    menu_callback_params * params = (menu_callback_params *) p;
	params->vport->change_line_type( params->line , PT_SPECTRUM);
	params->vport->refresh();
	viewmanagement.update_overlays();
}
void FLTK_Pt_Spectrum_pane::set_geom_callback(Fl_Widget *callingwidget, void * p )
{
    menu_callback_params * params = (menu_callback_params *) p;
	params->vport->change_geom_type( params->vp_id, PT_SPECTRUM);
	params->vport->refresh();
	viewmanagement.update_overlays();
}

void FLTK_Pt_Spectrum_pane::set_x_callback(Fl_Widget *callingwidget, void * p )
{
    menu_callback_params * params = (menu_callback_params *) p;
	params->vport->change_x( params->line );
	params->vport->refresh();
	viewmanagement.update_overlays();
}
void FLTK_Pt_Spectrum_pane::set_y_callback(Fl_Widget *callingwidget, void * p )
{
    menu_callback_params * params = (menu_callback_params *) p;
	params->vport->change_y( params->line );
	params->vport->refresh();
	viewmanagement.update_overlays();
}




void FLTK_Pt_Spectrum_pane::set_color_button_label(colors c)
{
	colormenu_button->label( preset_color_labels[c] );
	( (Fl_Menu_Item*)colormenu_button->menu() )[c].setonly(); 	//also activate the right radio-button...
}
//--------------------------------------------------------


void FLTK_Pt_Spectrum_pane::change_line(char line){
	curve_base *curve = ((rendergeom_curve *)rendermanagement.get_geometry(this->get_renderer_id()))->curve;
	//curve_base *curve = ((renderer_curve *)rendermanagement.get_renderer(this->get_renderer_id))->get_top();
	if(curve == NULL){
		return;
	}
	curve->set_line(line);
}

void FLTK_Pt_Spectrum_pane::change_color(colors color){
	curve_base *curve = ((rendergeom_spectrum *)rendermanagement.get_geometry(this->get_renderer_id()))->curve;
	//curve_base *curve = ((renderer_curve *)rendermanagement.get_renderer(this->get_renderer_id))->get_top();
	if(curve == NULL){
		return;
	}
	switch(color){
		case BLACK:
			curve->set_color(0,0,0);
			break;
		case WHITE:
			curve->set_color(255,255,255);
			break;
		case RED:
			curve->set_color(255,0,0);
			break;
		case GREEN:
			curve->set_color(0,255,0);
			break;
		case BLUE:
			curve->set_color(0,0,255);
			break;
		case YELLOW:
			curve->set_color(255,255,0);
			break;
		case MAGENTA:
			curve->set_color(255,0,255);
			break;
		case CYAN:
			curve->set_color(0,255,255);
			break;
		default:
			curve->set_color(0,0,0);
			break;
	}
}
void FLTK_Pt_Spectrum_pane::change_x(char x){
	((renderer_spectrum *)rendermanagement.get_renderer(this->get_renderer_id()))->change_x_type(x); //RN add_geom_here
	/*if(curve == NULL){
		cout << "Nu ar den null???" << endl;
		return;
	}
	cout << "Nu ska jag ‰ndra"<<endl;
	curve->change_x_type(x);*/
}
void FLTK_Pt_Spectrum_pane::change_y(char y){
	((renderer_spectrum*)rendermanagement.get_renderer(this->get_renderer_id()))->toggle_y_type(y);
	//curve_base *curve = ((rendergeom_spectrum *)rendermanagement.get_geometry(this->get_renderer_id()))->curve; //RN add_geom_here
	/*if(curve == NULL){
		return;
	}
	curve->toggle_y_type(y);*/
}
void FLTK_Pt_Spectrum_pane::change_geom(int vp_id){
	rendergeometry_base *geom;
	if(vp_id == this->get_viewport_parent()->get_id()){
		geom = rendermanagement.get_renderer(viewmanagement.get_renderer_id(vp_id))->original_rg;
	}else{
		geom = rendermanagement.get_renderer(viewmanagement.get_renderer_id(vp_id))->the_rg;//   get_geometry(viewmanagement.get_renderer_id(vp_id));
	}
	//rendergeometry_base *base;
	//base = rendermanagement.get_geometry(this->get_renderer_id());
	
	//curve_base *curve = ((renderer_curve *)rendermanagement.get_renderer(this->get_renderer_id))->get_top();
	if(geom == NULL){
		return;
	}
	rendermanagement.get_renderer(this->get_renderer_id())->use_other_geometry(geom);
	((rendergeom_image*)geom)->refresh_viewports(); //Safe because it can oly be an image in other viewport
}
//**************************************************************************************************









FLTKviewport::FLTKviewport(int xpos,int ypos,int width,int height, viewport *vp_parent, int buttonheight, int buttonwidth):Fl_Window(xpos,ypos,width,height)
{
	viewport_parent = vp_parent;
    int buttonleft=0;

    button_pack_top = new Fl_Pack(0,0,width,buttonheight,"");
    button_pack_top->type(FL_HORIZONTAL);
    
    datamenu_button = new Fl_Menu_Button(0+(buttonleft+=buttonwidth),0,buttonwidth,buttonheight,"Data");
    
    renderermenu_button = new Fl_Menu_Button(0+(buttonleft+=buttonwidth),0,buttonwidth,buttonheight,"Renderer");
	//The factory below returnsconnects the  
	renderermenu_button->copy(rendermanager::pane_factory.menu(cb_renderer_select3,(void*)this)); 
    renderermenu_button->user_data(NULL);
   /* 
    //direction menu is constant for each viewport
    Fl_Menu_Item dir_menu_items[NUM_DIRECTIONS+1];
    
    int m;
    for(m=0;m<NUM_DIRECTIONS;m++){
        menu_callback_params * cbp=new menu_callback_params;
        cbp->direction=(preset_direction)m;
        cbp->vport=viewport_parent;
        
        init_fl_menu_item(dir_menu_items[m]);
        
        dir_menu_items[m].label(preset_direction_labels[m]);
        dir_menu_items[m].callback(&set_direction_callback);
        dir_menu_items[m].user_data(cbp);
        dir_menu_items[m].flags= FL_MENU_RADIO;
        
        //backwards slices do not work in the orthogonal renderer yet
        //so we'll disable those choices
//        if (m > SAGITTAL)
//            {dir_menu_items[m].deactivate();}
    }

    dir_menu_items[m].label(NULL);	//terminate menu
    dir_menu_items[DEFAULT_DIR].setonly();	//DEFAULT_DIR is pre-set, set checkmark accordingly
//    dir_menu_items[Z_DIR].setonly();
//    dir_menu_items[AXIAL].setonly(); //AXIAL_NEG
    

    directionmenu_button = new Fl_Menu_Button(0+(buttonleft+=buttonwidth),0,buttonwidth,buttonheight,preset_direction_labels[Z_DIR]);
    directionmenu_button->copy(dir_menu_items);
    */
/*
	int m;
    Fl_Menu_Item blend_menu_items [NUM_BLEND_MODES+1];
    for(m=0;m<NUM_BLEND_MODES;m++){
//		cout<<"m="<<m<<"/"<<NUM_BLEND_MODES<<endl;

        menu_callback_params * cbp=new menu_callback_params;
        cbp->mode=(blendmode)m;
        cbp->rend_index=viewport_parent->rendererIndex;
        
        init_fl_menu_item(blend_menu_items[m]);
        
        blend_menu_items[m].label(blend_mode_labels[m]);
        blend_menu_items[m].callback(&set_blendmode_callback);
        blend_menu_items[m].user_data(cbp);
        blend_menu_items[m].flags= FL_MENU_RADIO;
        
//        if (rendererIndex < 0)
  //          {blend_menu_items[m].deactivate();}
      }
    //terminate menu
    blend_menu_items[m].label(NULL);
    
    blendmenu_button = new Fl_Menu_Button(0+(buttonleft+=buttonwidth),0,buttonwidth,buttonheight);
    blendmenu_button->copy(blend_menu_items);

	//------ styling --------------
    blendmenu_button->box(FL_THIN_UP_BOX);
    blendmenu_button->labelsize(FLTK_SMALL_LABEL);
*/    renderermenu_button->box(FL_THIN_UP_BOX);
    renderermenu_button->labelsize(FLTK_SMALL_LABEL);
//    directionmenu_button->box(FL_THIN_UP_BOX);
 //   directionmenu_button->labelsize(FLTK_SMALL_LABEL);
    datamenu_button->box(FL_THIN_UP_BOX);
    datamenu_button->labelsize(FLTK_SMALL_LABEL);
	//------ styling --------------
    
    button_pack_top->end();
    
	// -------------- pane_widget -------------------

	if( (viewport_parent->vp_type == PT_MPR) || (viewport_parent->vp_type == PT_MIP) ){
		pane_widget = new FLTK_Pt_MPR_pane(0,0+buttonheight,width,height-buttonheight);
		((FLTK_Pt_pane*)pane_widget)->event_pane->callback(viewport_callback, this); //viewport (_not_ FLTK_Pt_pane) handles the callbacks

	}else if(viewport_parent->vp_type == PT_CURVE){
		pane_widget = new FLTK_Pt_Curve_pane(0,0+buttonheight,width,height-buttonheight);
		((FLTK_Pt_pane*)pane_widget)->event_pane->callback(viewport_callback, this);

	}else{ 
		pane_widget = new FLTK_VTK_Cone_pane(0,0+buttonheight,width,height-buttonheight);
	}

	// -------------- pane_widget -------------------

	this->resizable(pane_widget);
    this->end();
    this->box(FL_BORDER_BOX);
    this->align(FL_ALIGN_CLIP);
	}


FLTKviewport::~FLTKviewport()
{
// Following two lines give an error when exiting the program... 
// A likely cause is that the FLTK object has already been deleted when we try to 
	if(datamanagement.FLTK_running())
	{
		if (datamenu_button != NULL) //objects menu has user data which was allocated in line 542
		{
			fl_menu_userdata_delete(renderermenu_button->menu()); 
		}
	}
}

int FLTKviewport::h_pane()
{
	return pane_widget->h_pane();
}

void FLTKviewport::refresh_menus()
{
	update_data_menu();
    rebuild_renderer_menu();
	pane_widget->refresh_menus();
}

void FLTKviewport::refresh_overlay()
{
	pane_widget->redraw_overlay();
}

void FLTKviewport::rebuild_renderer_menu ()
{
    //since available renderer types do not change at runtime,
    //the menu isn't really rebuilt - only the radio checkmark is reassigned
    
    if ( renderermenu_button != NULL )
        {
        Fl_Menu_Item * renderermenu=(Fl_Menu_Item *)renderermenu_button->menu();
	
		int numItems = fl_menu_size (renderermenu);
		
		if (viewport_parent->rendererIndex >= 0)
			{
			std::string this_renderer_type=rendermanagement.get_renderer_type(viewport_parent->rendererIndex);
			
			for (int m = 0; m < numItems;m++)
				{
//JK2				listedfactory<renderer_base>::lf_menu_params * p = reinterpret_cast<listedfactory<renderer_base>::lf_menu_params * > (renderermenu[m].user_data());
//				if (p->type == this_renderer_type)
//					{ renderermenu[m].setonly(); }
				}
			}
						
		if (numItems <= 1) //no real choice, disable
			{
				renderermenu[0].deactivate();
			}
		
		if (numItems == 0) //no choice at all, disable even more!
			{
			renderermenu_button->deactivate();
			}
		
		}
}

/*
void FLTKviewport::rebuild_blendmode_menu()//update checkmark for current blend mode
{
    if (blendmenu_button != NULL)
        {
        Fl_Menu_Item * blendmodemenu=(Fl_Menu_Item *)blendmenu_button->menu();

		//blend modes are different - or not available - for other renderer types (than MPR...)
		//basic check for this:
		//int this_renderer_type=rendermanagement.get_renderer_type(rendererIndex);
		
		for (int m=0; m < NUM_BLEND_MODES ; m++)
			{
			((menu_callback_params*)(blendmodemenu[m].argument()))->rend_index = viewport_parent->rendererIndex;
			
			if (viewport_parent->rendererIndex <0 || !rendermanagement.renderer_supports_mode(viewport_parent->rendererIndex,m))
				{
					blendmodemenu[m].deactivate();
				}
			else
				{
					blendmodemenu[m].activate();
				}
			}
		
		if (viewport_parent->rendererIndex >= 0)
			{
			int this_blend_mode=rendermanagement.get_blend_mode(viewport_parent->rendererIndex);
			
			blendmodemenu[this_blend_mode].setonly();
			
			blendmenu_button->label(blend_mode_labels[this_blend_mode]);
			}
        }
}
*/

void FLTKviewport::update_data_menu()
{	
    unsigned int m=0;
    const Fl_Menu_Item *cur_menu, *base_menu;
    
    base_menu=datamanagement.FLTK_objects_menu();
    cur_menu=datamenu_button->menu();
    
    unsigned int baseMenuSize = fl_menu_size(base_menu);
    
    //Fl_Menu_Item new_menu[baseMenuSize+1];
	renderer_base * r = rendermanagement.get_renderer(viewport_parent->get_renderer_id());
	bool *supported = new bool[baseMenuSize]; //RN
	int size_of_support = 0;
	
	for(int i = 0; i < baseMenuSize; i++){
		supported[i] = datamanagement.get_data(base_menu[i].argument())->is_supported(r->type());
		if (supported[i])
			size_of_support++;
	} //RN stop
    //Fl_Menu_Item *new_menu = new Fl_Menu_Item[baseMenuSize+1];
	 Fl_Menu_Item *new_menu = new Fl_Menu_Item[size_of_support+1];
    
    if(cur_menu!=NULL){
        //delete old callback data (menu is deleted by fl_menu::copy)
        fl_menu_userdata_delete(cur_menu);
	}
	int counter = 0;
	
    if(base_menu != NULL && viewport_parent->rendererIndex >= 0){
        do{
			if(supported[m] || m == baseMenuSize){
				memcpy (&new_menu[counter],&base_menu[m],sizeof(Fl_Menu_Item));
	                
				if(new_menu[counter].label()!=NULL){
					//long v=base_menu[m].argument();
					//attach menu_callback_params and
					//set checkmarks according to displayed images
	                    
					menu_callback_params * p= new menu_callback_params;
					p->rend_index=viewport_parent->rendererIndex;
					p->vol_id=base_menu[m].argument();  //image ID is stored in user data initially

	//				p->vport = this; //JK
	                    
					new_menu[counter].callback((Fl_Callback *)toggle_data_callback);
					new_menu[counter].user_data(p);
					new_menu[counter].flags=FL_MENU_TOGGLE;
					if(rendermanagement.image_rendered(viewport_parent->rendererIndex,p->vol_id) !=BLEND_NORENDER){
						//checked
						new_menu[counter].set();
					}
				}
				counter++;
			}
			m++;
		}
		while(m <= baseMenuSize);
		//while(new_menu[counter].label() !=NULL && m <= baseMenuSize);
		
        m = size_of_support;
        datamenu_button->copy(new_menu);
        delete new_menu;
        }
    else
        { datamenu_button->menu(NULL); }
	free(supported);
}


void FLTKviewport::cb_renderer_select(Fl_Widget *o, void *v)
{
    listedfactory<renderer_base>::lf_menu_params * par = reinterpret_cast<listedfactory<renderer_base>::lf_menu_params *>(v);
    const Fl_Menu_Item * item = reinterpret_cast<Fl_Menu_*>(o)->mvalue();
    
    //par->receiver; //the viewport
    //par->Create(); //the new renderer
    
    const_cast<Fl_Menu_Item *>(item)->setonly();
}

void FLTKviewport::cb_renderer_select2(Fl_Widget *o, void *v)
{
//    listedfactory<renderer_base>::lf_menu_params * par = reinterpret_cast<listedfactory<renderer_base>::lf_menu_params *>(v);
//    const Fl_Menu_Item * item = reinterpret_cast<Fl_Menu_*>(o)->mvalue();
    
    //par->receiver; //the viewport
    //par->Create(); //the new renderer
    
//    const_cast<Fl_Menu_Item *>(item)->setonly();
}

void FLTKviewport::cb_renderer_select3(Fl_Widget *o, void *v)
{
	// --- Set radion button ---

	const Fl_Menu_Item *item = reinterpret_cast<Fl_Menu_*>(o)->mvalue();
    const_cast<Fl_Menu_Item*>(item)->setonly();

    listedfactory<FLTKpane>::lf_menu_params *par = reinterpret_cast<listedfactory<FLTKpane>::lf_menu_params *>(v);
    //par->receiver; //the viewport
    //par->type; //the new renderer type
	
	FLTKviewport *owner = (FLTKviewport*)par->receiver;
	owner->switch_pane(par->type);

/*
	//cout<<"Fl_Group::current()="<<Fl_Group::current()<<endl;
//	Fl_Group::current(owner_FLTKviewport);
	//cout<<"Fl_Group::current()="<<Fl_Group::current()<<endl;
	//Fl_Group::current(NULL);

	int x = owner_FLTKviewport->pane_widget->x();
	int y = owner_FLTKviewport->pane_widget->y();
	int w = owner_FLTKviewport->pane_widget->w();
	int h = owner_FLTKviewport->pane_widget->h();
	//cout<<"child="<<owner_FLTKviewport->children()<<endl;
	owner_FLTKviewport->pane_widget->parent()->remove(owner_FLTKviewport->pane_widget);
	//cout<<"child="<<owner_FLTKviewport->children()<<endl;
//	this->remove(pane_widget);

	delete owner_FLTKviewport->pane_widget;

	//cout<<"child="<<owner_FLTKviewport->children()<<endl;
	
//	FLTKpane *new_pane=;

	//cout<<"Fl_Group::current()="<<Fl_Group::current()<<endl;
	Fl_Group::current(owner_FLTKviewport);
	//cout<<"Fl_Group::current()="<<Fl_Group::current()<<endl;

//	owner_FLTKviewport->pane_widget = new FLTK_Pt_pane(x,y,w,h);
//	owner_FLTKviewport->pane_widget = new FLTK_VTK_Cone_pane();
	owner_FLTKviewport->pane_widget = rendermanager::pane_factory.Create(par->type);

	owner_FLTKviewport->pane_widget->x(x);
	owner_FLTKviewport->pane_widget->y(y);
	owner_FLTKviewport->pane_widget->w(w);
	owner_FLTKviewport->pane_widget->h(h);
	owner_FLTKviewport->pane_widget->resize_content(w,h);

	owner_FLTKviewport->pane_widget->callback(viewport_callback, owner_FLTKviewport); //viewport (_not_ FLTK_Pt_pane) handles the callbacks

	
//	owner_FLTKviewport->viewport_parent->render_if_needed();
//	owner_FLTKviewport->end();

//	owner_FLTKviewport->add(owner_FLTKviewport->pane_widget);
	//cout<<"child="<<owner_FLTKviewport->children()<<endl;
	cout<<"shown()"<<owner_FLTKviewport->pane_widget->shown()<<endl;
	owner_FLTKviewport->pane_widget->show();
	cout<<"shown()"<<owner_FLTKviewport->pane_widget->shown()<<endl;

	viewmanagement.list_viewports();

//	owner_FLTKviewport->pane_widget->damage(FL_DAMAGE_ALL);
//	owner_FLTKviewport->pane_widget->redraw();

//	owner_FLTKviewport->cb_renderer_select3b( (FLTKpane*)new_pane );
//	owner_FLTKviewport->cb_renderer_select3b( new_pane );
//	owner_FLTKviewport->cb_renderer_select3b( NULL );
*/

}



/*
void FLTKviewport::cb_renderer_select3b(FLTKpane* new_pane)
{

	cout<<"id="<<this->viewport_parent->get_id()<<endl;

	int x = this->pane_widget->x();
	int y = this->pane_widget->y();
	int w = this->pane_widget->w();
	int h = this->pane_widget->h();
	cout<<"x,y,w,h "<<this->x()<<","<<this->y()<<","<<this->w()<<","<<this->h()<<endl;

	cout<<"child="<<this->children()<<endl;
	pane_widget->parent()->remove(pane_widget);
	cout<<"child="<<this->children()<<endl;
//	this->remove(pane_widget);
	delete this->pane_widget;
	cout<<"child="<<this->children()<<endl;


//	this->pane_widget = new FLTK_Pt_pane(x, y, w, h);

//	this->pane_widget = new_pane; //JK2 memory leak...
//	this->add(this->pane_widget);
//	this->pane_widget->show();

//	this->pane_widget->x(0);
//	this->pane_widget->y(25);
//	this->pane_widget->w(w);
//	this->pane_widget->h(h-25);

//	this->pane_widget->show();

	this->resizable(pane_widget);
	this->end();

//	this->pane_widget->redraw(); //JK2
//	this->pane_widget->needs_rerendering();
}
*/


int FLTKviewport::get_renderer_id()
{
	return this->viewport_parent->get_renderer_id();
}


void FLTKviewport::set_blendmode_callback(Fl_Widget *callingwidget, void * p )
{
    menu_callback_params * params=(menu_callback_params *)p;
    
    rendermanagement.set_blendmode(params->rend_index,params->mode);
}

void FLTKviewport::toggle_data_callback(Fl_Widget *callingwidget, void * params )
{

	//H‰r kan man s‰tta rendrerare!!!!!!
    menu_callback_params * widget_user_data=(menu_callback_params *)params;

    rendermanagement.toggle_data(widget_user_data->rend_index,widget_user_data->vol_id);

	// update all viewports that shows at least one of the images in the current viewport:
	int c_id = rendermanagement.get_combination_id(widget_user_data->rend_index);
	if(c_id>=0){
		viewmanagement.refresh_viewports_from_combination(c_id);
//		viewmanagement.refresh_overlays();

		//since we are now in a static function... we have to send the window of the widget...
		viewmanagement.update_overlays();

		//JK also refresh other overlays...
	}else{
		viewmanagement.refresh_viewports(); //complicated to remember old settings... slow but simple solution... 
	}
	
	//JK-ˆˆˆˆ update direction if needed (for example if "DEFAULT_DIR" is used...)

	// TODO: only refresh viewports that holds the addded/removed image
	// hur kan det aktuella bild id:et erhÂllas? undersˆk om det verkligen ‰r en bild (dvs exkludera tex en point_collection)
	// mˆjlig lˆsning ‰r att j‰mfˆra de bilder combination innehÂller fˆre och efter toggle_data() 

//	cout<<"vport_id= "<<widget_user_data->vport->get_id()<<endl;
//	viewmanagement.refresh_viewports(); //time consuming to update all....
//	widget_user_data->vport->refresh(); //occasionally results in an error...
}


void FLTKviewport::set_direction_callback(Fl_Widget *callingwidget, void * p )
{
    menu_callback_params * params = (menu_callback_params *) p;
	params->vport->set_renderer_direction( params->direction );

	params->vport->refresh();
//	viewmanagement.refresh_overlays();
	viewmanagement.update_overlays();

/*
	// update all viewports that shows at least one of the images in the current viewport (slice locators)
	int combinationID = rendermanagement.get_combination_id( params->rend_index );
	std::vector<int> geometryIDs = rendermanagement.geometries_by_image ( combinationID );		// return geometries that holds at least one of the images in the input combination

	for ( std::vector<int>::const_iterator itr = geometryIDs.begin(); itr != geometryIDs.end(); itr++ )
	{
		viewmanagement.refresh_viewports_from_geometry( *itr );
	}
*/
}



void FLTKviewport::switch_pane(factoryIdType type)
{
	viewport_parent->set_vp_type_from_factoryIdType(type);	//we need to change this param also...

	int x = this->pane_widget->x();
	int y = this->pane_widget->y();
	int w = this->pane_widget->w();
	int h = this->pane_widget->h();

	this->pane_widget->parent()->remove(this->pane_widget); //l‰nkar av...
	delete this->pane_widget;

	Fl_Group::current(this); //TITTA HƒR!!! Dwenna kˆrs dÂ man byter rendrerare
	
	this->pane_widget = rendermanager::pane_factory.Create(type); //Denna kˆr fel konstruktor!!! Anv‰nd nÂt annat sÂ ˆvriga vitala saker skapas!

	this->pane_widget->x(x);
	this->pane_widget->y(y);
	this->pane_widget->w(w);
	this->pane_widget->h(h);
	this->pane_widget->resize_content(w,h);

	//--------------
	int old_rendID = viewmanagement.get_renderer_id(viewport_parent->ID);
	//RN Does not seem to work when changing from VTK.
	rendercombination *temp_rc = rendermanagement.get_renderer(old_rendID)->the_rc;
	vector<int> data_in_combo;
	for(rendercombination::iterator pairItr = temp_rc->begin();pairItr != temp_rc->end();pairItr++){
		pt_error::error_if_null(pairItr->pointer,"Rendered data object is NULL");//Crash here when closing an image
		data_in_combo.push_back((dynamic_cast<data_base *> (pairItr->pointer))->get_id());
	}

	rendermanagement.remove_renderer(old_rendID);
	//--------------

	viewport_parent->busyTool = NULL; //this will force the tool to re_init_its pointers etc.

	if(type.find("VTK") == string::npos){	//if a "platinum" p
		((FLTK_Pt_pane*)pane_widget)->event_pane->callback(viewport_callback, this);

		
		int tmp_rendID;
		if(viewport_parent->vp_type == PT_MPR){	//PT_MPR, PT_MIP, VTK_EXAMPLE, VTK_MIP, VTK_ISOSURF};
			tmp_rendID = rendermanagement.create_renderer(RENDERER_MPR);
		}else if(viewport_parent->vp_type == PT_MIP){
			tmp_rendID = rendermanagement.create_renderer(RENDERER_MIP);
		}else if(viewport_parent->vp_type == PT_CURVE){
			tmp_rendID = rendermanagement.create_renderer(RENDERER_CURVE);
		}else if(viewport_parent->vp_type == PT_SPECTRUM){
			tmp_rendID = rendermanagement.create_renderer(RENDERER_SPECTRUM);
		}
		viewmanagement.connect_renderer_to_viewport(viewport_parent->ID,tmp_rendID);     //attach MPR renderer - so that all viewports can be populated for additional views

		temp_rc = rendermanagement.get_renderer(tmp_rendID)->the_rc;
		for(int j = 0; j<data_in_combo.size(); j++){ //The filtering is done in "update_data_menu"
			//No filtering of data here. Old unsupported data remains in combination
			temp_rc->add_data(data_in_combo.at(j));
		}

	}

//	viewmanagement.list_viewports();
//	viewmanagement.list_connections();
//	rendermanagement.print_renderers();
	
	this->pane_widget->show();

	this->resizable(pane_widget);
    this->end();
    this->box(FL_BORDER_BOX);
    this->align(FL_ALIGN_CLIP);


	//viewmanagement.list_viewports();
}

/*
void FLTKviewport::set_renderer_button_label(factoryIdType type)
{
//	renderermenu_button->label( type.c_str() ); //always keep the "Renderer" label

	Fl_Menu_Item *item = (Fl_Menu_Item*)directionmenu_button->menu();
	for(int i=0;i<item->size();i++){
		if( item[i].label() == type.c_str() ){
			item[i].setonly(); 	//also change the right radio-button...
		}
	}

}
*/


void FLTKviewport::viewport_callback(Fl_Widget *callingwidget, void *thisFLTKviewport)
{
    ((FLTKviewport*)thisFLTKviewport)->viewport_callback(callingwidget);
}

void FLTKviewport::viewport_callback(Fl_Widget *callingwidget){
    FLTK_Event_pane *fp = (FLTK_Event_pane*)callingwidget;
    //fp points to the same GUI toolkit-dependent widget instance as pane_widget

    if(fp->callback_event.type() == pt_event::draw){
        fp->callback_event.grab();
		render_if_needed();
		
		fp->damage(FL_DAMAGE_ALL);
		fp->draw(viewport_parent->rgbpixmap); //JK2-ˆˆˆ, do this in FLTK_draw_vp???
		fp->damage(0);
	}

    if(viewport_parent->busyTool == NULL){ 
        viewport_parent->busyTool = viewporttool::taste(fp->callback_event,viewport_parent,rendermanagement.get_renderer(viewport_parent->rendererID));
	}else{
        viewport_parent->busyTool->handle(fp->callback_event);
        if(render_if_needed()){
			fp->redraw(); 
		}
	}
    
    //handle events "otherwise"
    if(!fp->callback_event.handled()){
        //"default" behavior for the viewport goes here
        //call render_if_needed(); if image may need re-rendering
	}
    
    //handle events regardless of whether a tool caught them
    switch(fp->callback_event.type()){
        case pt_event::resize:
            if((fp->w() != viewport_parent->rgbpixmap_size[0] || fp->h() != viewport_parent->rgbpixmap_size[1])){
                //resize: just update view size, re-render but don't redraw...yet
                const int *r = fp->callback_event.get_resize();
				//cout<<"r="<<r[0]<<"r="<<r[1]<<endl;
                viewport_parent->update_viewsize(r[0] ,r[1]);
//                fp->needs_rerendering();
				viewport_parent->needs_rerendering();
            }
        break;
	}
}

bool FLTKviewport::render_if_needed()
{
	return viewport_parent->render_if_needed();
}
/*
void FLTKviewport::update_fbstring (FLTK_Pt_pane* fp)
{
    std::map<std::string,float> values;
    ostringstream infostring;
    
    //Draw values picked from data at mouse position
    //in later revisions this event and clicks may be relayed to the actual application
    
    values=rendermanagement.get_values(viewport_parent->rendererIndex,fp->mouse_pos[0]-fp->x(),fp->mouse_pos[1]-fp->y(),viewport_parent->rgbpixmap_size[0],viewport_parent->rgbpixmap_size[1]);
    
    for (std::map<std::string,float>::iterator itr = values.begin(); itr != values.end();itr++)
        {
        if (itr != values.begin()) 
            {
            infostring << endl;
            }
        infostring << itr->first << ": " << itr->second;
        }
    
    fp->feedback_string=infostring.str();
}
*/
