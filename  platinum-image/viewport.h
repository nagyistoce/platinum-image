//////////////////////////////////////////////////////////////////////////
//
//   Viewport $Revision$
///
///  Class for viewport handling. 
///  Holds a widget (FLTK, *the_widget) that handles controls and the actual drawing.
///  The viewport class holds the "uchar *rgbpixmap" and a boolean "needs_re_rendering" that is set when the bitmap needs updating... 
///  (avoids multiple redrawing...)
///
//   $LastChangedBy$


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

#ifndef __viewport__
#define __viewport__

#include "FLTKviewport.h"
#include "viewporttool.h"
#include "global.h"
#include "listedfactory.h"
#include "rendercombination.h"
#include "renderer_spectrum.h"


class threshold_overlay;
class thresholdparvalue;
//extern datamanager datamanagement; //declared in .cc-file

#define NO_RENDERER_ID 0

class FLTKviewport; //Forward declaration needed by Xcode (version 3.2.1)...

class viewport
{
    friend class FLTKviewport;	//allow access to for example "rendererIndex"...
    friend class FLTK_Pt_pane;	//allow access to for example "set_renderer_direction"...
    friend class FLTK_Pt_MPR_pane;	//allow access to for example "set_renderer_direction"...
	friend class FLTK_Pt_Curve_pane;
	friend class FLTK_Pt_Spectrum_pane;
    friend class FLTK_Event_pane;	//TMP
    friend class viewporttool;

private:

    FLTKviewport *the_widget;	//Fl_Window
    static panefactory pfactory; //instatiated in transferfactory.cc

	int ID;						//viewport ID
	VIEWPORT_TYPE vp_type;		//{PT_MPR, PT_MIP, VTK_EXAMPLE, VTK_MIP, VTK_ISOSURF, PT_CURVE};

    static int maxviewportID;	//keeps track of how many has been created...
	int rendererID;				//this guy will render for us (each renderer instance contains an unique ID)
	int rendererIndex;			//direct look up to vector array, //DEPRECATED: use either rendererID or - for fast access - pointer to renderer

	bool needs_re_rendering;
    viewporttool *busyTool;

    uchar *rgbpixmap;
    int rgbpixmap_size[2];
    void clear_rgbpixmap();		//fill for viewport without renderer

//	Matrix3D get_renderer_direction();
	void set_renderer_direction(const Matrix3D &dir);
	void set_renderer_direction(preset_direction direction); 

	void change_color( colors color, VIEWPORT_TYPE type); //Used in curve
	void change_line_type(char line, VIEWPORT_TYPE type); //Used in curve
	void change_geom_type(int vp_id, VIEWPORT_TYPE type);
	void change_x(char x);
	void change_y(char y);

	bool paint_rendergeometry;

	void set_vp_type_from_factoryIdType(string type);	//called when changed from GUI...

	
public:
    viewport(VIEWPORT_TYPE vpt=PT_MPR);
    virtual ~viewport();

	void initialize_viewport(int xpos, int ypos, int width, int height, VIEWPORT_TYPE vpt=PT_MPR);  //enum VIEWPORT_TYPE {PT_MPR, PT_MIP, VTK_EXAMPLE, VTK_MIP, VTK_ISOSURF};
	int x();	
	int y();
	int w();
	int h();
	int h_pane(); //height of the pane ( total height minus button height)
    

    int get_id() const ;
	int get_renderer_id() const ;
	void connect_renderer(int rID);

	void needs_rerendering();
	void render_data(int data_id);

	const int* pixmap_size() const;
    void update_viewsize(int width, int height);

//	void set_timer_delay(int delay = 0); // if zero, remove timer and always render directly
	void enable_and_set_direction(preset_direction direction);
	void disable_all_data();
	void set_renderer(string renderer_type);
	void set_blend_mode(blendmode bm);

    bool render_if_needed();


    // *** refresh methods ***
    //called when any update of the visual parts of viewport is affected, i.e. image and/or menu of images

    void refresh_after_toolswitch();			//!refresh if viewport has a busy tool (selection etc.). !The tool will be deleted
    void refresh_from_geometry(int g);			//!refresh if it uses the geometry specified by argument
    void refresh_from_combination(int c);		//!refresh if it uses the geometry specified by argument
    void refresh();								//!re-builds menu and makes viewport re-render and redraw eventually
    void refresh_overlay();                     //calls the redraw_overlay on the original FLTK_Overlay class... which schedules the redrawing... 
    void paint_overlay();						//calls the actual painting of the overlay (done by renderer)
    
    threshold_overlay * get_threshold_overlay (thresholdparvalue *);

    #pragma mark *** operators ***
	// virtual const viewport &operator=(const viewport &k) { return k; }
	bool virtual operator<<(const viewport &k) { return ID==k.ID; }
	bool virtual operator==(const viewport &k) { return ID==k.ID; }
	bool virtual operator==(const int &k) { return ID==k; }
	bool virtual operator!=(const viewport &k) { return ID!=k.ID; }
	bool virtual operator<(const viewport &k) { return ID<k.ID; }
	bool virtual operator>(const viewport &k) { return ID>k.ID; }
	friend std::istream &operator>>(std::istream &in, viewport &k) { in >> k.ID; return in; }
	friend std::ostream &operator<<(std::ostream &ut, const viewport &k) { ut << "[viewport. ID= " << k.ID << " rendererID: " << k.rendererID << " rendererIndex:  " << k.rendererIndex << "] "; return ut; }

	int ROI_rectangle_x;	//SO - Region of interest drawn (using FL_Overlay) when zooming a region
	int ROI_rectangle_y;
	int ROI_rectangle_w;
	int ROI_rectangle_h;
	bool ROI_rect_is_changing;

	void set_look_at_and_zoom(Vector3D pos, float zoom);
};

#endif