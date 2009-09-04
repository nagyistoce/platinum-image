//////////////////////////////////////////////////////////////////////////
//
//  FLTKviewport
//
//  FLTK implementation of the central pane of each viewport that
//  displays the rendered image and can be clicked/dragged in.
//
//	Class structure:
//	viewport
//		rgb_pixmap
//		bool needs_rerendering
//		FLTKviewport* the_widget
//
//	FLTKviewport
//		FL_widgets //buttons
//		FLTKpane (Fl_Double_Window)
//			FLTK_Pt_pane 
//				FLTK_Event_pane *event_pane		//Catches mouse events
//				FLTK_Pt_MPR_pane?
//				FLTK_Pt_MIP_pane?
//			FLTK_VTK_pane
//				FLTK_VTK_Cone_pane	//Displays an example cone...
//				FLTK_VTK_MIP_pane	//Volume-Renders (MIP) the top image... (Not implemented yet...)
//				FLTK_VTK_MPR_pane	//
//				FLTK_VTK_ISO_pane	//
//				FLTK_VTK_COMBO_pane	//
//
//	* FLTK_VTK_pane / FLTK_VTK_pane need to enterpret update/refresh differently
//	* FLTK_VTK_pane will need reformatting from data_base objects to VTK-pointers
//	* Ev läs in all data, buffra vid "start"
//	* Transfer functions Pt <-- --> VTK
//	* "Reason-argument" might be needed for data_has_changed()? --> save formatting time...

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

#ifndef __FLTKviewport__
#define __FLTKviewport__

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Overlay_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Pack.H>
//#include <FL/Fl_Image.H>
#include <FL/fl_draw.H>

#include <iostream>
#include <vector>
#include <sstream>
#include <string>

#include "viewport.h" //JK circular #include?
#include "global.h"
#include "threshold.h"
#include "histo2D_tool.h"
#include "event.h"

#include "listedfactory.h" 


//------------------------------------
//Test - vtkFlRenderWindowInteractor -  here we have all the usual VTK stuff that we need for our pipeline

#include "Utilities/vtkFl/vtkFlRenderWindowInteractor.h"

//------------------------------------
//FLTK_VTK_Cone_pane

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkConeSource.h>
#include <vtkSuperquadricSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkActor.h>

//------------------------------------
//FLTK_VTK_MIP_pane

#include "vtkPiecewiseFunction.h"
#include "vtkStructuredPointsReader.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"
#include "vtkVolumeRayCastMIPFunction.h"
#include "vtkVolumeRayCastMapper.h"
#include "vtkVolumeMapper.h"
#include "vtkPolyDataMapper.h"
#include "vtkVolume16Reader.h"
#include "vtkDataSetMapper.h"
#include "vtkCamera.h" //SO
#include "vtkInteractorStyleSwitch.h" //SO
#include "vtkPointPicker.h" //SO
#include "vtkCommand.h" //SO
#include "vtkRendererCollection.h" //SO

#include "vtkImageReader.h"

//#include "itkImageToImageFilter.h"
//#include "itkVTKImageImport.h"
//#include "itkVTKImageExport.h"
//#include "Utilities/itkImageToVTKImageFilter.h"
//#include "Utilities/vtkITKImageToImageFilter.h"

//------------------------------------
#include "itkVTKImageImport.h"
#include "itkVTKImageExport.h"
#include "vtkImageImport.h"
#include "vtkImageExport.h"
#include "image_scalar.hxx" //JK TODO remove...
#include "vtkImageCast.h"
#include "vtkImageToStructuredPoints.h"
#include "vtkFixedPointVolumeRayCastMapper.h"
/*
#include "itkCommand.h"
#include "itkImage.h"
#include "itkCurvatureFlowImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkRGBPixel.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "vtkImageData.h"

#include "vtkImageImport.h"
#include "vtkImageExport.h"
*/


//Callback action identifiers.

enum callbackAction 
{
    CB_ACTION_NONE,
    CB_ACTION_DRAW,
    CB_ACTION_RESIZE,
    CB_ACTION_HOVER,
    CB_ACTION_CLICK_PASS,   //pass click event through to renderer or specific application
    CB_ACTION_DRAG_PASS,    //pass drag event through to renderer or specific application
    CB_ACTION_DRAG_PAN,     //pan rendered image
    CB_ACTION_DRAG_ZOOM,    //zoom rendered image
    CB_ACTION_DRAG_FLIP,    //flip through rendered image sequence
    CB_ACTION_WHEEL_ZOOM,	//mouse wheel zoom
    CB_ACTION_WHEEL_FLIP	//mouse wheel flip through rendered image sequence
};


//---------------------------------------------
//---------------------------------------------
class FLTKpane : public Fl_Overlay_Window //JK2
//class FLTKpane : public Fl_Window
{
	friend class FLTKviewport;

	private:
		virtual void draw_overlay(){};		//FLTK_Pt_pane implements this....
		virtual viewport* get_viewport_parent()=0;

	public:
	    FLTKpane();  //JK2 - Default constructor, needed for the listedfactory "Create()" function...
	    FLTKpane(int X,int Y,int W,int H);  //constructor
		virtual int h_pane()=0;
		virtual void needs_rerendering()=0;			//passes this on to the "viewport_parent"...
	    virtual void resize_content(int w,int h);
		static const std::string typekey () //JK2 - Used in the listedfactory to set GUI-list-names
            {return "base_key";}
		int get_renderer_id();

		virtual void set_renderer_direction(preset_direction direction);	//only implement for panes can use a "direction" 
		virtual void refresh_menus();
};


//---------------------------------------------
//---------------------------------------------
//class FLTK_VTK_pane : public Fl_Overlay_Window
class FLTK_VTK_pane : public FLTKpane
{
	private:

	protected:
		vtkFlRenderWindowInteractor *fl_vtk_window;
		virtual void initialize_vtkRenderWindow();
		
		double Xcam; //SO
		double Ycam; 
		double Zcam;

		double XcamUp; 
		double YcamUp; 
		double ZcamUp;

	public:
	    FLTK_VTK_pane();  //JK2 - Default constructor, needed for the listedfactory "Create()" function...
	    FLTK_VTK_pane(int X,int Y,int W,int H);  //constructor
	    ~FLTK_VTK_pane();
		virtual int h_pane();
		virtual viewport* get_viewport_parent();
		virtual void needs_rerendering();


	    void resize_content(int w,int h);
		void draw_overlay();
		int handle(int e);

		static const std::string typekey () //JK2 - Used in the listedfactory to set GUI-list-names
            {return "FLTK_VTK_pane";}
};

class FLTK_VTK_Cone_pane : public FLTK_VTK_pane
{
	private:
		void initialize_vtkRenderWindow();

	public:
	    FLTK_VTK_Cone_pane();  //JK2 - Default constructor, needed for the listedfactory "Create()" function...
	    FLTK_VTK_Cone_pane(int X,int Y,int W,int H);  //constructor
	    ~FLTK_VTK_Cone_pane();

		static const std::string typekey () //JK2 - Used in the listedfactory to set GUI-list-names
            {return "FLTK_VTK_Cone_pane";}
};

class FLTK_VTK_MIP_pane : public FLTK_VTK_pane
{
	private:
		void initialize_vtkRenderWindow();

	public:
	    FLTK_VTK_MIP_pane();  //JK2 - Default constructor, needed for the listedfactory "Create()" function...
	    FLTK_VTK_MIP_pane(int X,int Y,int W,int H);  //constructor
	    ~FLTK_VTK_MIP_pane();

		static const std::string typekey () //JK2 - Used in the listedfactory to set GUI-list-names
            {return "FLTK_VTK_MIP_pane";}
};

//--------------------------------------------------------
//------------TILLFÄLLIG PLACERING-------------------/SO--
//--------------------------------------------------------
//vtkWorldPointPicker *w_picker;


class PickPosCommand : public vtkCommand
{
public:
	static PickPosCommand *New(); //static
	void Delete(); 
	virtual void Execute(vtkObject *caller, unsigned long eid, void *callData);

};

//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------


/*
class FLTKpane2 : public FLTKpane
{
	public:
	    FLTKpane2();
	    FLTKpane2(int X,int Y,int W,int H);
};
*/

//---------------------------------------------
//---------------------------------------------
class FLTK_Event_pane : public Fl_Widget		//This class catches events and passes them on to "viewport" and draws the "rgbpixmap"
{
    friend class FLTKviewport;
    friend class FLTK_Pt_pane;
	friend class threshold_overlay;

	private:
		FLTK_Pt_pane *my_base_pt_pane;
	    int callback_action;    //which action to perform during click or drag processed by callback
        viewport_event callback_event;
		void do_callback(callbackAction action = CB_ACTION_NONE);  //do callback with specified action

	public:
		FLTK_Event_pane(int X,int Y,int W,int H, FLTK_Pt_pane *fpp);  //constructor
		int handle(int event);
		void resize(int x, int y, int w, int h);
		void draw();						//FLTK draw call - called when FLTK wants the viewport updated
	    void draw(unsigned char *rgbimage); //JK //our "active" draw method - will redraw directly whenever it is called
                                            //this method draws the argument rgbimage...

		void needs_rerendering();			//passes this on to the "viewport_parent"...
};

//---------------------------------------------
//---------------------------------------------
class FLTK_Pt_pane : public FLTKpane
{
    //friend class viewport;
	//friend class FLTKviewport;
    //friend class FLTK_Event_pane; //allows acces to e.g. callback_event
	//friend class histo2D_tool;

	private:

	protected:

	public:
	    FLTK_Pt_pane();    //JK2 - Default constructor, needed for the listedfactory "Create()" function...
	    FLTK_Pt_pane(int X,int Y,int W,int H);  //constructor
        ~FLTK_Pt_pane();

		FLTK_Event_pane *event_pane; //used to catch events in the viewport...

		int h_pane();
		virtual void needs_rerendering();			//passes this on to the "viewport_parent"...

		virtual viewport* get_viewport_parent();

		void draw_overlay();

	    void resize_content(int w,int h);

        static const std::string typekey() //JK2 - Used in the listedfactory to set GUI-list-names
            {return "FLTK_Pt_pane";}

};


class FLTK_Pt_MPR_pane : public FLTK_Pt_pane
{
    friend class viewport;
	friend class FLTKviewport;
    friend class FLTK_Event_pane; //allows acces to e.g. callback_event
	friend class histo2D_tool;

	private:
	    Fl_Pack			*button_pack2;		//group containing per-viewport widgets such as the image menu
		Fl_Menu_Button	*directionmenu_button;
	    Fl_Menu_Button	*blendmenu_button;
		Fl_Menu_Button *geom_button;
		Fl_Menu_Button *info;
		void create_MPR_menu(int W);
		void create_geom_menu(int W);
//		Fl_Button *b;	//	Fl_Button *b = new Fl_Button(0,buttonheight,100,100-buttonheight, "FLTK_Pt_MPR_pane()_button");	 //JK //RN test

   
	protected:

	public:
	    FLTK_Pt_MPR_pane();    //JK2 - Default constructor, needed for the listedfactory "Create()" function...
	    FLTK_Pt_MPR_pane(int X,int Y,int W,int H);  //constructor
        ~FLTK_Pt_MPR_pane();

		virtual viewport* get_viewport_parent();

		static void set_direction_callback(Fl_Widget *callingwidget, void *params);
	    static void set_blendmode_callback(Fl_Widget *callingwidget, void *params);
		static void set_geom_callback(Fl_Widget *callingwidget, void *params);

  //      static const std::string typekey () //JK2 - Used in the listedfactory to set GUI-list-names
    //        {return "undef";}

		void set_direction_button_label(preset_direction direction);
		void set_renderer_direction( preset_direction direction );
		void change_geom(int vp_id);
	    void rebuild_blendmode_menu();//update checkmark for current blend mode
		virtual void refresh_menus();
};
class FLTK_Pt_Curve_pane : public FLTK_Pt_pane
{
    friend class viewport;
	friend class FLTKviewport;
    friend class FLTK_Event_pane; //allows acces to e.g. callback_event
	friend class histo2D_tool;

	private:
	    Fl_Pack			*button_pack2;		//group containing per-viewport widgets such as the image menu
		Fl_Menu_Button	*colormenu_button;
	    Fl_Menu_Button	*bgmenu_button;
		Fl_Menu_Button *geom_button;
		Fl_Menu_Button *info;
		void create_curve_menu(int W);
		void create_geom_menu(int W);
   
	protected:

	public:
	    FLTK_Pt_Curve_pane();    //JK2 - Default constructor, needed for the listedfactory "Create()" function...
	    FLTK_Pt_Curve_pane(int X,int Y,int W,int H);  //constructor
        ~FLTK_Pt_Curve_pane();

		virtual viewport* get_viewport_parent();

		static void set_color_callback(Fl_Widget *callingwidget, void *params);
		static void set_line_callback(Fl_Widget *callingwidget, void *params);
		static void set_geom_callback(Fl_Widget *callingwidget, void *params);
		void change_color(colors color);
		void change_line(char a);
		void change_geom(int vp_id);
	    //static void set_blendmode_callback(Fl_Widget *callingwidget, void *params);

  //      static const std::string typekey () //JK2 - Used in the listedfactory to set GUI-list-names
    //        {return "undef";}

		void set_color_button_label(colors color);
	//	void set_renderer_direction( preset_direction direction );
	  //  void rebuild_blendmode_menu();//update checkmark for current blend mode
		//virtual void refresh_menus();
};



//---------------------------------------------
//---------------------------------------------
class FLTKviewport : public Fl_Window   //handles the FLTK part of the viewport class
{
	friend class FLTKpane;
	friend class FLTK_VTK_pane;
	friend class FLTK_VTK_MIP_pane;
	friend class FLTK_Event_pane;
	friend class FLTK_Pt_pane;
	friend class FLTK_Pt_MPR_pane;
	friend class FLTK_Pt_Curve_pane;

private:
	viewport *viewport_parent;

    Fl_Pack			*button_pack_top;		//group containing per-viewport widgets such as the image menu
    Fl_Menu_Button	*datamenu_button;   
    Fl_Menu_Button	*renderermenu_button;
//    Fl_Menu_Button	*directionmenu_button;
//    Fl_Menu_Button	*blendmenu_button;

    void update_data_menu();   //set rendering status for images from rendercombination for this viewport's renderer
    void rebuild_renderer_menu();//update checkmark for current renderer type
    static void cb_renderer_select(Fl_Widget *o, void *v);
    static void cb_renderer_select2(Fl_Widget *o, void *v); //JK2
    static void cb_renderer_select3(Fl_Widget *o, void *v); //JK2
//    void cb_renderer_select3b(FLTKpane* new_pane); //JK2
//    void rebuild_blendmode_menu();//update checkmark for current blend mode
	int get_renderer_id();
	
	
public:
    FLTKviewport(int xpos,int ypos,int width,int height, viewport *vp_parent, int buttonheight=20, int buttonwidth=70);
    virtual ~FLTKviewport();

    FLTKpane *pane_widget;      //the frame ("viewport-image-area") displaying a rendered image 

	int h_pane();

	void viewport_callback(Fl_Widget *callingwidget);                               //Callback that handles events always redraws (pt_event::draw/resize)
    static void viewport_callback(Fl_Widget *callingwidget, void *thisviewport);    //FLTK callback wrapper

    static void toggle_data_callback(Fl_Widget *callingwidget, void *params);
    static void set_direction_callback(Fl_Widget *callingwidget, void *params);
    static void set_blendmode_callback(Fl_Widget *callingwidget, void *params);
//	void set_direction_button_label(preset_direction direction);
    void switch_pane(factoryIdType type); //JK2
//	void set_renderer_button_label(factoryIdType type); //JK2

	
    bool render_if_needed();
        
    // *** refresh methods ***
    //called when any update of the visual parts of viewport is affected, i.e. image and/or menu of images
    void refresh_menus();
    void refresh_overlay();                     //calls the redraw_overlay on the original FLTK_Overlay class... which schedules the redrawing... 
    
//    void update_fbstring(FLTK_Pt_pane *fp);   //refresh values in number-at-pointer string
};

#endif
