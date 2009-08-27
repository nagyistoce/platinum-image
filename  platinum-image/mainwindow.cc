//////////////////////////////////////////////////////////////////////////
//
//  Intialization of main Platinum window $Revision$
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

#include "platinumprogram.h"

#include "FLTKviewport.h"

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Overlay_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Pack.H>
//#include <FL/Fl_Output.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Tile.H>

//Linux (?)
#include <FL/x.H>
#if !defined(WIN32) && !defined(__APPLE__)
#include "list_visuals.cxx"
#endif

#include "ptconfig.h"
#include "fileutils.h"

/*int platinum_run (int argc, char *argv[])
{
////
// ...finish the window creation
window.end();

////
//  start the main loop

window.show(argc, argv);

return Fl::run();
}*/



void close_program_callback(Fl_Widget *callingwidget, void *data)
{
	if ( Fl::event() == FL_SHORTCUT && Fl::event_key() == pt_event::escape_key )
		{ return; }	// ignore [esc]

	std::cout<<"close_program_callback..."<<std::endl;
	datamanagement.FLTK_running(false);
	exit( EXIT_SUCCESS );	// terminates the program normally
}

void platinum_init ()
{
    std::cout << "You are going Platinum, please stand by!" << std::endl;

#if !defined(WIN32) && !defined(__APPLE__)
    //setup for Xwindows only
    int i = 1;
    //cout << "Xwindows" << endl;
    Fl::args(argc,argv,i,arg);
    if (visid >= 0) {
        fl_open_display();
        XVisualInfo templt; int num;
        templt.visualid = visid;
        fl_visual = XGetVisualInfo(fl_display, VisualIDMask, &templt, &num);
        if (!fl_visual) {
            fprintf(stderr, "No visual with id %d, use one of:\n",visid);
            list_visuals();
            exit(1);
            }
    fl_colormap = XCreateColormap(fl_display, RootWindow(fl_display,fl_screen),
        fl_visual->visual, AllocNone);
    fl_xpixel(FL_BLACK); // make sure black is allocated in overlay visuals
        } else {
            Fl::visual(FL_RGB);
            }
#else
    //prevent dithering on legacy platforms
    Fl::visual(FL_RGB);
#endif

    //// Init FLTK

#ifdef __APPLE__
    //more OSX-like theme

    //"none", "plastic" or "gtk+"
    /*Fl::scheme("plastic");
    Fl::get_system_colors();*/

    Fl::background(250,250,250);
#endif

//JK-Remove pragma warnings in VS...
#ifdef _MSC_VER
	//disable warning C4068: "unknown pragma"
	#pragma warning(disable: 4068)
#endif

    }

void platinum_setup (Fl_Window & window, int num_viewports_h, int num_viewports_v,int list_area_w )
    {
	window.begin();
    const int win_w = window.w();
    const int win_h = window.h();
    
    const int status_area_h = 24;

    const int view_w = win_w - list_area_w;
    const int view_h = win_h - status_area_h;

    //set up the window;
    window.size (win_w,win_h);
    window.resizable(&window);

	window.callback(close_program_callback);
    
    Fl_Pack * viewsNstatusStack = new Fl_Pack(0,0,win_w,win_h);
    viewsNstatusStack->type(FL_VERTICAL);

    //allows resizing the proportions of views & data/tools
    Fl_Tile * viewsNlists = new Fl_Tile(0,0,win_w,win_h-status_area_h);

    // *** skapa ett antal views ***
    //
    const int num_vp = num_viewports_h * num_viewports_v;
    for (int i=0;i < num_vp; i++) // create some views w/ renderers
        {
        viewmanagement.create_viewport();
        }

    // *** Ge viewmanager och datamanager l‰mpliga FLTK-pekare och initiera ***

    // viewmanagement.demo_setup_tiles(); // initialize tile layout; could as well load from file or ...

    viewmanagement.setup_regular_tiles(num_viewports_h, num_viewports_v);  //initialize regular tile layout
//	viewmanagement.setup_irregular_tiles_h(); //used in MetaVis application

    viewmanagement.erase_all_connections(); // only do this once...
    viewmanagement.connect_views_viewports(); // kopplar ev. lediga VPs till behˆvande views

#pragma mark *** Create viewport widgets ***

    viewmanagement.setup_views(0, view_w, view_h); // 0...antal som anv‰nds-1

    //viewmanagement.list_viewports();
    
#pragma mark *** Create tool area ***
    
    Fl_Tile * tool_area = new Fl_Tile(view_w,0,list_area_w,win_h-status_area_h);   //group containing datawidgets and feedback, so they'll be resized properly
    datamanagement.datawidgets_setup();

    userIOmanagement.setup();
    
    //Fl_Progress* prg = userIOmanagement.progress;

    tool_area->box(FL_BORDER_BOX);
    tool_area->resizable(tool_area);
    tool_area->end();

	Fl_Box * emptyBox = new Fl_Box(0, 0, win_w - list_area_w, win_h - status_area_h);
	emptyBox->hide();
	viewsNlists->resizable(emptyBox);
    //viewsNlists->resizable(viewsNlists);
    viewsNlists->end();

#pragma mark *** Create status area ***
    
    Fl_Group * toolsNstatus = new Fl_Group (0,viewsNlists->y()+viewsNlists->h(),viewsNstatusStack->w(),status_area_h);
    //toolsNstatus->type(FL_HORIZONTAL);
    
    Fl_Group::current (NULL); //statusarea has to be after toolbox in pack...
    userIOmanagement.status_area = new statusarea (0,win_h-status_area_h,toolsNstatus->w(),status_area_h); 
    
    Fl_Group::current (NULL);
    viewporttool::init(toolsNstatus->x(),toolsNstatus->y(),userIOmanagement.status_area); //!spawn toolbox and give viewporttool pointer to statusarea
    toolsNstatus->add(viewporttool::toolbox);
    
    toolsNstatus->add(userIOmanagement.status_area);//...time to add statusarea
    userIOmanagement.status_area->resize(viewporttool::toolbox->x()+viewporttool::toolbox->w(),toolsNstatus->y(),toolsNstatus->w()-viewporttool::toolbox->w(),toolsNstatus->h());
         
    toolsNstatus->resizable(userIOmanagement.status_area);
    toolsNstatus->end();
            
    viewsNstatusStack->resizable(viewsNlists);
    viewsNstatusStack->end();
	
	//window.size_range(win_w, win_h);	// the initial size of the window is also the minimum size ie the window size can only be increased
    window.resizable(viewsNlists);
    window.end();


	//Initialize config file (.inp)... Try reading / create a new default if non-existing!
	pt_config::initialize();

    }
	

