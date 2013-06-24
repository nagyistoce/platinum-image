/////////////////////////////////////////////////////////////////////////////////
//
//  FLTK userIO parameters $Revision$
//  
//  Widgets that present and allow setting values for userIO blocks
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

#ifndef __FLTKuserIOparameters__
#define __FLTKuserIOparameters__

#include <string>

#include <FL/Fl_Image.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Box.H>

//#include <FL/Fl_Counter.H>
#include <FL/Fl_Menu_Button.H>

#include "ptmath.h"

//class histogram_base;
//class histogram_2D;
struct regionofinterest;

#include "global.h"
#include "FLTKutilities.h"	//Editable slider...
#include "histogram.h"
#include "threshold.h"

#include "error.h"

// *** layout constants ***

//default width for layout, widget will be resized as it is added to the block
#define INITPARWIDGETWIDTH 100
//height of one parameter, so those that require just one line get uniform sizes
#define STDPARWIDGETHEIGHT 35
//portion of this used for title
#define PARTITLEMARGIN 15
//inset of parameters in userIO box
#define PARBOXBORDER 6
//width of popup menu boxes
#define PARMENUWIDTH 75

typedef int imageIDtype;
typedef int pointIDtype;
typedef int landmarksIndexType;

const int par_control_height=20;   //height for each parameter control

typedef void (userIO_callback)(int block_ID,int param_num);

// *** custom widgets ***

class FLTKdataitem_choice : public Fl_Choice //base class
{
protected:
    FLTKdataitem_choice (int x, int y);
    void data_change (const Fl_Menu_Item * base_menu);
};

class FLTKimage_choice : public FLTKdataitem_choice  //widget for choosing images, in FLTKuserIOpar_image
                                            //and other widgets using specific images, like histograms or points
    {
    public:
        FLTKimage_choice (int x, int y);

        void data_vector_has_changed ();   //update image menu from datamanager

        imageIDtype id_value ();                   //return selected image ID
    };

class FLTKpoint_choice : public FLTKdataitem_choice
{
public:
    FLTKpoint_choice (int x, int y);
    
    void data_vector_has_changed ();   //update image menu from datamanager
    
    imageIDtype id_value ();                   //return selected image ID
};

class FLTK_histogram_base : public Fl_Widget        //base class for widget displaying a histogram
    {
    protected:
        //Fl_RGB_Image * screen_image;
        unsigned char * screen_image_data;
        thresholdparvalue threshold;        //current threshold - borrowed by viewport highlighting
        
        std::vector<threshold_overlay *> thresholders;

        //void wipe_image_data();             //clear image & image data

        void draw ();                       //FLTK redraw method
    public:
        histogram_base * histogram;     //histogram that is displayed
                                        //and interacted with

        thresholdparvalue get_threshold ();
        
        FLTK_histogram_base (int x, int y, int w, int h);
        ~FLTK_histogram_base();
        
        void resize(int x, int y, int w, int h);    //delete screen_image for later reallocation on resize
        
        virtual void refresh ();                    //make sure image is allocated, call histogram object
                                                    //to render
    };

class FLTK_histogram_2D : public FLTK_histogram_base
    {
    protected:
        float h_treshold_min,h_treshold_max,v_treshold_min,v_treshold_max;
        float drag_ref_x, drag_ref_y;               //starting point of drag, in fraction-of-box coordinates
        void draw();
        enum histogram2D_drag_mode {READY_RECT, SET_RECT,MOVE_RECT, READY_OVAL,SET_OVAL,MOVE_OVAL};   //selection mode
        histogram2D_drag_mode drag_mode;

    public:
        FLTK_histogram_2D (int x, int y, int w, int h);
        ~FLTK_histogram_2D();

        int handle(int);                           //handle FLTK events

        void set_selmode (int);                    //set rectangular/oval selection mode
        void set_images (int hor, int vert);      //set image IDs and update
    };

class FLTK_Vector3D : public Fl_Group
{
	protected:
		Fl_Value_Input *inp_x;
		Fl_Value_Input *inp_y;
		Fl_Value_Input *inp_z;

    public:
        FLTK_Vector3D(int x, int y, int w, int h);
		Vector3D get();
		void set(Vector3D v);
};


// *** Parameter classes ***

//Base class for numerous parameter objects populating userIO blocks. Handles value retrieval, updates and creation/deletion
class FLTKuserIOparameter_base : public Fl_Group  {
protected:
    char widget_label[512];    //parameter name
                               //C string 'cos that's the way FLTK rolls
    userIO_callback * par_callback;
    
public:
    FLTKuserIOparameter_base();
    FLTKuserIOparameter_base(int w, int h,const std::string name);
    
    void set_callback(userIO_callback * p_callback);
    static void par_update_callback (Fl_Widget *callingwidget, void *);

    virtual void data_vector_has_changed () {}   //trigger updating of parameters depending
                                                          //on available images
    
    //here are all possible value types in any subclass
    //subclass implements only supported types
    //these cannot be made pure virtuals (= 0), or
    //all subclasses would have to implement all
    //(and any additional) par_value functions

    virtual void par_value (float & v)
        {throw pt_error("requested float, actual type " + type_name(), pt_error::serious);}
    virtual void par_value (long & v)
        {throw pt_error("requested long, actual type " + type_name(), pt_error::serious );}
    virtual void par_value (Vector3D & v)
        {throw pt_error("requested Vector3D, actual type " + type_name(), pt_error::serious );}
    virtual void par_value (imageIDtype & v)
        {throw pt_error("requested image ID, actual type " + type_name(), pt_error::serious );}
    virtual void par_value (bool & v)
        {throw pt_error("requested bool, actual type " + type_name(), pt_error::serious );}
	virtual void par_value (std::string & s)
		{throw pt_error("requested std::string, actual type " + type_name(), pt_error::serious );}
    virtual void par_value (thresholdparvalue & v)
        {throw pt_error("requested thresholds, actual type " + type_name(), pt_error::serious );}
			
    virtual const std::string type_name ()
        {return "unknown (base)";}

    //hack-ish information for histograms
    virtual int histogram_image_ID (int )
        {return NOT_FOUND_ID;}
    };

//For simple input of file paths...
class FLTKuserIOpar_path : public FLTKuserIOparameter_base    
    {
    protected:
        Fl_Button *browse_button;
		bool file_path; //or directory_path

	public:
        FLTKuserIOpar_path(const std::string name, bool filepath=true, const std::string default_path="");
        Fl_Input *control;	//public for simple access of static callback...
        const std::string type_name();
		void par_value(std::string &v);
        static void browse_button_cb(Fl_Widget *callingwidget, void *);
    };

//For Display and handling of 3D coordinate value: 50% of the "normal" GUI parameter height...
class FLTKuserIOpar_coord3Ddisplay : public FLTKuserIOparameter_base    
    {
    protected:
        Fl_Output * control;
		Vector3D coord;
		std::string coord_name;
		std::string coord_type_name;
		std::string resolve_teststring();

	public:
        FLTKuserIOpar_coord3Ddisplay(const std::string c_name, const std::string c_type_name, Vector3D v);
        const std::string type_name();
        void par_value(Vector3D &v);

        void set_coordinate(Vector3D v);
		void update();
    };


class FLTKuserIOpar_landmarks : public FLTKuserIOparameter_base
{
	protected:

		// Used an example from http://seriss.com/people/erco/fltk/ with some modifications
		class ColResizeBrowser : public Fl_Hold_Browser
		{
			private:
				Fl_Color _colsepcolor;	// color of column separator lines
				int _showcolsep;		// flag to enable drawing column separators
				Fl_Cursor _last_cursor;	// saved cursor state info
				int _dragging;			// 1 = user dragging a column
				int _dragcol;			// col# user is currently dragging
				int *_widths;			// pointer to user's width[] array
				int _nowidths[1];		// default width array (non-const)

				void change_cursor(Fl_Cursor newcursor);
				int which_col_near_mouse();

			protected:
				int handle(int e);
				void draw();
				
			public:
				ColResizeBrowser(int X, int Y, int W, int H, const char * L = 0);
				Fl_Color colsepcolor() const;
				void colsepcolor(Fl_Color val);
				int showcolsep() const;
				void showcolsep(int val);
				int * column_widths() const;
				void column_widths(int * val);
		};
		
		class landmark
		{
			public:
				int index;	
				std::string description;
				std::string option;
				
				landmark();
				landmark( const int l_index, const std::string l_description, const std::string l_option );
		};

		Fl_Box * emptyBoxThree;
		Fl_Group * showGroup;
		std::vector<Fl_Check_Button *> checkBtns;
		Fl_Button * showBtn;

		Fl_Box * emptyBoxTwo;
		Fl_Group * goGroup;
		Fl_Float_Input * xInput;
		Fl_Float_Input * yInput;
		Fl_Float_Input * zInput;
		Fl_Button * goBtn;

		Fl_Box * emptyBox;
		Fl_Group * buttonGroup;
		Fl_Button * loadSetBtn;		
		Fl_Button * saveSetBtn;
		Fl_Button * save2SetBtn;
		Fl_Button * resetSetBtn;
		
		ColResizeBrowser * browser;
		
		int column_widths[4];	// n columns requires n - 1 column widths + and an ending zero element to terminate the array
			
		std::vector<landmark> landmarks;

		std::string resolve_string(int index);

		int landmarksID;
		
	public:
		FLTKuserIOpar_landmarks ( const std::string name );

        void data_vector_has_changed ();	//update browser from datamanager

		static void showCallback(Fl_Widget * callingwidget, void * thisLandmarks);
		static void goCallback(Fl_Widget * callingwidget, void * thisLandmarks);
		static void resetSetCallback( Fl_Widget * callingwidget, void * thisLandmarks );
		static void saveSetCallback( Fl_Widget * callingwidget, void * thisLandmarks );
		static void save2SetCallback( Fl_Widget * callingwidget, void * thisLandmarks );
		static void loadSetCallback( Fl_Widget * callingwidget, void * thisLandmarks );
		static void browserCallback( Fl_Widget * callingwidget, void * thisLandmarks );

		const std::string type_name();
        void par_value(landmarksIndexType &v);

		void update_browser();

		void next();
		
		int get_landmarksID();
		
		int handle(int e);
};

class FLTKuserIOpar_float : public FLTKuserIOparameter_base    //float value (using slider)
    {
    protected:
        Fl_Value_Slider * control;
    public:
        FLTKuserIOpar_float (const std::string name, float ma=1, float mi=0);
                                                   //order of parameters reverse of expected,
                                                   //because min is more typically left at default
                                                   //than max
        const std::string type_name ();
        void par_value (float & v);
		void set_value (float val);
    };

class FLTKuserIOpar_float_box : public FLTKuserIOparameter_base    //float value (using input box)
    {
    protected:
        Fl_Value_Input * control;
    public:
        FLTKuserIOpar_float_box (const std::string name, float ma=255, float mi=0, float start_val=std::numeric_limits<float>::max());
                                                   //order of parameters reverse of expected,
                                                   //because min is more typically left at default
                                                   //than max

        const std::string type_name ();
        void par_value (float & v);
    };

class FLTKuserIOpar_longint : public FLTKuserIOparameter_base    //integer value (using slider)
    {
    protected:
//        Fl_Value_Slider * control;
		FLTK_Editable_Slider * control;
    public:
        FLTKuserIOpar_longint (const std::string name, long ma=255, long mi=0);
                                                   //order of parameters reverse of expected,
                                                   //because min is more typically left at default
                                                   //than max
        const std::string type_name ();
        void par_value (long & v);
		void set_value (long val);
		void set_value_no_Fl_callback(long val);
    };

class FLTKuserIOpar_longint_box : public FLTKuserIOparameter_base    //integer value (using input box)
    {
    protected:
        Fl_Value_Input * control;
    public:
        FLTKuserIOpar_longint_box (const std::string name, long ma=255, long mi=0);
                                                   //order of parameters reverse of expected,
                                                   //because min is more typically left at default
                                                   //than max
        const std::string type_name ();
        void par_value (long & v);
    };

class FLTKuserIOpar_bool : public FLTKuserIOparameter_base    //boolean value (using check box)
    {
    protected:
        Fl_Check_Button * control;
    public:
        FLTKuserIOpar_bool (const std::string name, bool init_status);
                                                  
        const std::string type_name ();
        void par_value (bool & v);
    };


class FLTKuserIOpar_string : public FLTKuserIOparameter_base    //string value
    {
    protected:
        Fl_Input * control;
    public:
		FLTKuserIOpar_string (const std::string name, std::string init_status);
                                                  
        const std::string type_name ();
		void par_value (std::string & s);
    };

class FLTKuserIOpar_stringlist : public FLTKuserIOparameter_base    //menu of strings given in vector<string>
    {
    protected:
        Fl_Menu_Button* control;
		char current_value[512];
		void set_string_callback(Fl_Widget* callingwidget);
		static void set_string_static_callback(Fl_Widget* callingwidget, void* the_object);
		std::vector<std::string> my_strlist;
		
	public:
		FLTKuserIOpar_stringlist (const std::string name, std::vector<std::string> strlist);
		
        const std::string type_name ();
		void par_value (std::string & s);
    };

class FLTKuserIOpar_image : public FLTKuserIOparameter_base   //image selection (using popup menu)
    {
    protected:
        FLTKimage_choice * control;
    public:
        FLTKuserIOpar_image(std::string name);

        void data_vector_has_changed ();

        void par_value(imageIDtype & v);                     //image ID

        const std::string type_name ();
    };
	
class FLTKuserIOpar_imageshow : public FLTKuserIOpar_image
{
	protected:
		std::vector<Fl_Check_Button *> checkbuttons;
//		Fl_Button * show_button;
		
	public:
		FLTKuserIOpar_imageshow( std::string name );
		static void show_callback( Fl_Widget * callingwidget, void * this_image_show );
};

class FLTKuserIOpar_points : public FLTKuserIOparameter_base   //point selection (using popup menu)
{
protected:
    FLTKpoint_choice * control;
public:
    FLTKuserIOpar_points(std::string name);
    
    void data_vector_has_changed ();
    
    void par_value(pointIDtype & v);
    void par_value(Vector3D & v);
    
    const std::string type_name ();
};

/*
class FLTKuserIOpar_voxelseed : public FLTKuserIOparameter_base //direct seed point allocation via the "landmark tool"
{
protected:
    Fl_Button *catch_button;
	FLTK_Vector3D *v;
	static void catch_button_cb(Fl_Widget *callingwidget, void *);//ˆˆˆ

public:
    FLTKuserIOpar_voxelseed(std::string name);
    void par_value(Vector3D & v);
    
    const std::string type_name ();
};
*/

class FLTKuserIOpar_histogram2D : public FLTKuserIOparameter_base 
    {
    protected:
        FLTKimage_choice * image_h;
        FLTKimage_choice * image_v;

        FLTK_histogram_2D * histogram_widget;

        Fl_Button * oval_button;
        Fl_Button * rect_button;
    public:
        FLTKuserIOpar_histogram2D(std::string name);
        void resize(int x, int y, int w, int h);
        void par_value (thresholdparvalue & v);
        const std::string type_name ();

        static void vol_change_callback (Fl_Widget *callingwidget, void *);       //called when images are changed in popup menus
        static void selmode_change_callback (Fl_Widget *callingwidget, void *);   //called when oval/rect button is pushed
        void data_vector_has_changed ();
        int histogram_image_ID (int n);                                          //image used for axis n in histogram
        void highlight_ROI (regionofinterest *);       //highlight region of interest
        void set_images (int hor, int vert);                                     //set images in histogram and refresh image
    };

class FLTKuserIOpar_message : public FLTKuserIOparameter_base                  //message text
    {
    protected:
        Fl_Output * message;
    public:
        FLTKuserIOpar_message(std::string name, std::string message_text);

        //no par_value

        const std::string type_name ();
    };



// *** Planned parameters ***

/*class FLTKuserIOpar_action : public FLTKuserIOparameter_base    //push button for doing something,
    //like testing the integrity of parameters
    {
    protected:
        Fl_Button * control;
    public:
        FLTKuserIOpar_action(std::string name);
        ~FLTKuserIOpar_action();

        void par_value (bool & v);
    };*/

#endif