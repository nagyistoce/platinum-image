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

#include "FLTKuserIOparameters.h"

#include <algorithm>

#include <FL/fl_draw.H>

#include <iomanip>

#include "userIOmanager.h"
#include "datamanager.h"
#include "viewmanager.h"
#include "rendermanager.h"

extern datamanager datamanagement;
extern viewmanager viewmanagement;
extern rendermanager rendermanagement;
extern userIOmanager userIOmanagement;

//for testing, various 2D histogram classes can be interchanged:
#define HISTOGRAM2DVARIETY histogram_2D

FLTKuserIOparameter_base::FLTKuserIOparameter_base() : Fl_Group (NA,NA,NA,NA)
    {
    }

FLTKuserIOparameter_base::FLTKuserIOparameter_base(int w, int h,const std::string name) : Fl_Group (NA,NA,w,h)
    {
    box(FL_THIN_DOWN_BOX);
    align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE);
    labelsize(FLTK_SMALL_LABEL);

    strcpy(widget_label,name.c_str());
    label(widget_label);
    resizable (this);
    }

void FLTKuserIOparameter_base::set_callback(userIO_callback * p_callback)
{
    par_callback=p_callback;
}

void FLTKuserIOparameter_base::par_update_callback (Fl_Widget *callingwidget, void * foo)
    {
    FLTKuserIOparameter_base * par_group;
    userIO * userIO_group;
    int par_num;

    par_group=(FLTKuserIOparameter_base * )callingwidget->parent();
    userIO_group=(userIO *)par_group->parent();

    par_num=userIO_group->get_par_num(par_group);
    //check own parameter num
    if (par_group->par_callback != NULL)
        {
        par_group->par_callback (userIO_group->id(),par_num);
        }
    }



#pragma mark *** FLTKuserIOpar_path ***

FLTKuserIOpar_path::FLTKuserIOpar_path(const std::string name, bool filepath, const std::string default_path) : FLTKuserIOparameter_base(INITPARWIDGETWIDTH,STDPARWIDGETHEIGHT, name)
	{
		int butt_width = 30;	// ;-)
		file_path = filepath;

		browse_button = new Fl_Button(x()+w()-butt_width,y(),butt_width,PARTITLEMARGIN,"Browse");
		browse_button->callback(browse_button_cb);

		control = new Fl_Input(x(),y()+PARTITLEMARGIN,w(),h()-PARTITLEMARGIN);
		control->textsize(10);
		control->value(default_path.c_str());

		resizable(control);
		end();
	}

const std::string FLTKuserIOpar_path::type_name ()
	{
		return "path_chooser";
	}

void FLTKuserIOpar_path::par_value (std::string &v)
    {
    v = string(control->value());
    }

void FLTKuserIOpar_path::browse_button_cb(Fl_Widget *callingwidget, void *)
{
//	cout<<"browse_button_cb..."<<endl;

	string last_path = pt_config::read<std::string>("latest_path");
	//the callingwidget will always be the load_button
	FLTKuserIOpar_path* fp = (FLTKuserIOpar_path*)callingwidget->parent();
	char *path = NULL;

	string default_path="";
	fp->par_value(default_path);
	if(default_path!=""){
		last_path = default_path;		//if default path is set... use that in first case
	}

	//----------------------------------
	if(fp->file_path){

	//	Fl_File_Chooser fc(last_path.c_str(),"Any file(*)",Fl_File_Chooser::CREATE,"Choose file");
		Fl_File_Chooser fc(last_path.c_str(),"Any file(*)",Fl_File_Chooser::SINGLE,"Choose file");
		fc.show();
		while(fc.shown())
			{ Fl::wait(); }

		cout<<"fc.value(1)="<<fc.value(1)<<endl; 
		fp->control->value(fc.value(1));
		pt_config::write("latest_path",fc.value(1));
		

	//----------------------------------
	}else{ //folder path

		path = fl_dir_chooser("Choose a directory", last_path.c_str(), 0);
		fp->control->value(path);
		cout<<"path="<<path<<endl; 
		if(path == NULL){
			pt_error::error("FLTKuserIOpar_path loading cancelled",pt_error::notice);
			return;
		}
		pt_config::write("latest_path",path_parent(path));
	}

}


#pragma mark *** FLTKuserIOpar_coord3Ddisplay ***

std::string FLTKuserIOpar_coord3Ddisplay::resolve_teststring()
	{
//		cout<<"FLTKuserIOpar_coord3Ddisplay...resolve_teststring()"<<endl;
//		cout<<"coord[0]="<<coord[0]<<endl;
//		cout<<"coord[1]="<<coord[1]<<endl;
//		cout<<"coord[2]="<<coord[2]<<endl;

		char s[30];
		string tmp = "(";
		sprintf(s, "%5.1f", coord[0] );		tmp+=std::string(s);	//%5.1f - means five digits and one decimal...
		sprintf(s, "%5.1f", coord[1] );		tmp+=", "+std::string(s);
		sprintf(s, "%5.1f", coord[2] );		tmp+=", "+std::string(s)+")";

		return coord_name+" - "+coord_type_name+" - "+tmp;
	}

FLTKuserIOpar_coord3Ddisplay::FLTKuserIOpar_coord3Ddisplay(std::string c_name, std::string c_type_name, Vector3D v) : FLTKuserIOparameter_base(INITPARWIDGETWIDTH,int(STDPARWIDGETHEIGHT/2), "")
	{
//		cout<<"FLTKuserIOpar_coord3Ddisplay..."<<endl;
		coord_name = c_name;
		coord_type_name = c_type_name;
		set_coordinate(v);

		control = new Fl_Output(x(),y(),w(),STDPARWIDGETHEIGHT-PARTITLEMARGIN);
		control->box(FL_FLAT_BOX);
		control->color(FL_BACKGROUND_COLOR);
		control->textsize(12);
		control->value(resolve_teststring().c_str()); //update();

		resizable(control);
		end();
	}

const std::string FLTKuserIOpar_coord3Ddisplay::type_name ()
	{
		return "coord3Ddisplay";
	}

void FLTKuserIOpar_coord3Ddisplay::par_value (Vector3D &v)
    {
	v = coord;
    }

void FLTKuserIOpar_coord3Ddisplay::set_coordinate(Vector3D v)
	{
	coord = v;
	}
 
void FLTKuserIOpar_coord3Ddisplay::update()
    {
//	cout<<"FLTKuserIOpar_coord3Ddisplay...update()"<<endl;
	control->value(resolve_teststring().c_str());
	control->redraw();
    }

#pragma mark *** FLTKuserIOpar_landmarks ***

// --- ColResizeBrowser ---

// CHANGE CURSOR
//     Does nothing if cursor already set to value specified.
//
void FLTKuserIOpar_landmarks::ColResizeBrowser::change_cursor(Fl_Cursor newcursor) 
{
	if ( newcursor != _last_cursor )
	{
		fl_cursor(newcursor, FL_BLACK, FL_WHITE);
		_last_cursor = newcursor;
	}
}

// RETURN THE COLUMN MOUSE IS 'NEAR'
//     Returns -1 if none.
//
int FLTKuserIOpar_landmarks::ColResizeBrowser::which_col_near_mouse()
{
	int X,Y,W,H;
	Fl_Browser::bbox(X,Y,W,H);		// area inside browser's box()
	// EVENT NOT INSIDE BROWSER AREA? (eg. on a scrollbar)
	if ( ! Fl::event_inside(X,Y,W,H) )
		{ return(-1); }
	int mousex = Fl::event_x() + hposition();
	int colx = this->x();
	for ( int t=0; _widths[t]; t++ )
	{
		colx += _widths[t];
		int diff = mousex - colx;
		// MOUSE 'NEAR' A COLUMN?
		//     Return column #
		//
		if ( diff >= -4 && diff <= 4 )
			{ return(t); }
	}
	return(-1);
}

// MANAGE EVENTS TO HANDLE COLUMN RESIZING
int FLTKuserIOpar_landmarks::ColResizeBrowser::handle(int e)
{
	// Not showing column separators? Use default Fl_Browser::handle() logic
	if ( ! showcolsep() )
		{ return(Fl_Browser::handle(e)); }

	// Handle column resizing
	int ret = 0;
	switch ( e )
	{
		case FL_ENTER:
		{
			ret = 1;
			break;
		}
		case FL_MOVE:
		{
			if ( which_col_near_mouse() >= 0 )
				{ change_cursor(FL_CURSOR_WE); }
			else
				{ change_cursor(FL_CURSOR_DEFAULT); }
			ret = 1;
			break;
		}
		case FL_PUSH:
		{
			int whichcol = which_col_near_mouse();
			if ( whichcol >= 0 )
			{
				// CLICKED ON RESIZER? START DRAGGING
				ret = 1;
				_dragging = 1;
				_dragcol = whichcol;
				change_cursor(FL_CURSOR_DEFAULT);
			}
			break;
		}
		case FL_DRAG:
		{
			if ( _dragging )
			{
				ret = 1;
				// Sum up column widths to determine position
				int mousex = Fl::event_x() + hposition();
				int newwidth = mousex - x();
				for ( int t=0; _widths[t] && t<_dragcol; t++ )
					{ newwidth -= _widths[t]; }
				if ( newwidth > 0 )
				{
					// Apply new width, redraw interface
					_widths[_dragcol] = newwidth;
					if ( _widths[_dragcol] < 2 )
						{ _widths[_dragcol] = 2; }
					redraw();
				}
			}
			break;
		}
		case FL_LEAVE:
		case FL_RELEASE:
		{
			_dragging = 0;						// disable drag mode
			change_cursor(FL_CURSOR_DEFAULT);	// ensure normal cursor
			ret = 1;
			break;
		}
	}
	if ( _dragging )
		{ return(1); }							// dragging? don't pass event to Fl_Browser

	return ( Fl_Browser::handle(e) ? 1 : ret );
}

void FLTKuserIOpar_landmarks::ColResizeBrowser::draw()
{
	// DRAW BROWSER
	Fl_Browser::draw();
	if ( _showcolsep )
	{
		// DRAW COLUMN SEPARATORS
		int colx = this->x() - hposition();
		int X,Y,W,H;
		Fl_Browser::bbox(X,Y,W,H);
		fl_color(_colsepcolor);
		for ( int t=0; _widths[t]; t++ )
		{
			colx += _widths[t];
			if ( colx > X && colx < (X+W) )
				{ fl_line(colx, Y, colx, Y+H-1); }
		}
	}
}

// CTOR
FLTKuserIOpar_landmarks::ColResizeBrowser::ColResizeBrowser(int X, int Y, int W, int H, const char * L) : Fl_Hold_Browser(X, Y, W, H, L)
{
	_colsepcolor = Fl_Color(FL_GRAY);
	_last_cursor = FL_CURSOR_DEFAULT;
	_showcolsep  = 0;
	_dragging    = 0;
	_nowidths[0] = 0;
	_widths      = _nowidths;
}

// GET/SET COLUMN SEPARATOR LINE COLOR
Fl_Color FLTKuserIOpar_landmarks::ColResizeBrowser::colsepcolor() const 
{
	return(_colsepcolor);
}

void FLTKuserIOpar_landmarks::ColResizeBrowser::colsepcolor(Fl_Color val) 
{
	_colsepcolor = val;
}

// GET/SET DISPLAY OF COLUMN SEPARATOR LINES
//     1: show lines, 0: don't show lines
//
int FLTKuserIOpar_landmarks::ColResizeBrowser::showcolsep() const 
{
	return(_showcolsep);
}

void FLTKuserIOpar_landmarks::ColResizeBrowser::showcolsep(int val) 
{
	_showcolsep = val;
}

// GET/SET COLUMN WIDTHS ARRAY
//    Just like fltk method, but array is non-const.
//
int * FLTKuserIOpar_landmarks::ColResizeBrowser::column_widths() const 
{
	return(_widths);
}

void FLTKuserIOpar_landmarks::ColResizeBrowser::column_widths(int *val) 
{
	_widths = val;
	Fl_Browser::column_widths(val);
}


// --- landmark ---
FLTKuserIOpar_landmarks::landmark::landmark()
{
	index = -1;
	description = "";
	option = "";
}

FLTKuserIOpar_landmarks::landmark::landmark( const int l_index, const std::string l_name, const std::string l_option )
{
	index = l_index;
	description = l_name;
	option = l_option;
}

// --- FLTKuserIOpar_landmarks ---
FLTKuserIOpar_landmarks::FLTKuserIOpar_landmarks( const std::string name ) : FLTKuserIOparameter_base( INITPARWIDGETWIDTH, int(STDPARWIDGETHEIGHT * 14), name )
{
	const int checkBtnWidth = 30;
	const int btnWidth = 50;
	const int btnHeight = 20;	// BUTTONHEIGHT = 25
	
	this->

	showGroup = new Fl_Group(x(), y() + PARTITLEMARGIN, w(), btnHeight);
	{		
		emptyBoxThree = new Fl_Box(x(), y() + PARTITLEMARGIN, w() - 4 * checkBtnWidth - btnWidth, btnHeight);
		
		for ( int i = 0; i < 4; i++ )
		{
			std::string index = int2str(i + 1);
			
			Fl_Check_Button * temp = new Fl_Check_Button(w() - (4 - i) * checkBtnWidth - btnWidth, y() + PARTITLEMARGIN, checkBtnWidth, btnHeight);
			temp->copy_label(index.c_str());
			if ( i < 3 )
				{ temp->set(); }	// check viewport 1,2,3 by default
			
			checkBtns.push_back(temp);
		}
		
		showBtn = new Fl_Button(w() - btnWidth, y() + PARTITLEMARGIN, btnWidth, btnHeight, "Show");
		showBtn->callback(showCallback, (void *) this);
		showBtn->tooltip("Show working image in selected viewports");
	}
	showGroup->resizable(emptyBoxThree);
	showGroup->end();

	goGroup = new Fl_Group(x(), y() + STDPARWIDGETHEIGHT + PARTITLEMARGIN, w(), btnHeight);
	{ 
		emptyBoxTwo = new Fl_Box(x(), y() + STDPARWIDGETHEIGHT + PARTITLEMARGIN, w() - 4 * btnWidth, btnHeight);
		
		xInput = new Fl_Float_Input(w() - 4 * btnWidth, y() + STDPARWIDGETHEIGHT + PARTITLEMARGIN, btnWidth, btnHeight); //, "Go to coordinate: ");
		xInput->callback(goCallback, (void *) this);
		xInput->when(FL_WHEN_ENTER_KEY);
		xInput->tooltip("x-value");

		yInput = new Fl_Float_Input(w() - 3 * btnWidth, y() + STDPARWIDGETHEIGHT + PARTITLEMARGIN, btnWidth, btnHeight);
		yInput->callback(goCallback, (void *) this);
		yInput->when(FL_WHEN_ENTER_KEY);
		yInput->tooltip("y-value");

		zInput = new Fl_Float_Input(w() - 2 * btnWidth, y() + STDPARWIDGETHEIGHT + PARTITLEMARGIN, btnWidth, btnHeight);
		zInput->callback(goCallback, (void *) this);
		zInput->when(FL_WHEN_ENTER_KEY);
		zInput->tooltip("z-value");
		
		goBtn = new Fl_Button(w() - btnWidth, y() + STDPARWIDGETHEIGHT + PARTITLEMARGIN, btnWidth, btnHeight, "Go");
		goBtn->tooltip("Go to the entered coordinate");
		goBtn->callback(goCallback, (void *) this);
	}
	goGroup->resizable(emptyBoxTwo);
	goGroup->end();

	buttonGroup = new Fl_Group(x(), y() + 2 * STDPARWIDGETHEIGHT + PARTITLEMARGIN, w(), btnHeight);
	{
		emptyBox = new Fl_Box(x(), y() + 2 * STDPARWIDGETHEIGHT + PARTITLEMARGIN, w() -  3 * btnWidth, btnHeight);
		
		loadSetBtn = new Fl_Button(w() - 3 * btnWidth, y() + 2 * STDPARWIDGETHEIGHT + PARTITLEMARGIN, btnWidth, btnHeight, "Load");
		loadSetBtn->tooltip("Load landmark set");
		loadSetBtn->callback(loadSetCallback, (void *) this);
		
		saveSetBtn = new Fl_Button(w() - 2 * btnWidth, y() + 2 * STDPARWIDGETHEIGHT + PARTITLEMARGIN, btnWidth, btnHeight, "Save");
		saveSetBtn->tooltip("Save landmark set");
		saveSetBtn->callback(saveSetCallback, (void *) this);
		
		resetSetBtn = new Fl_Button(w() - btnWidth, y() + 2 * STDPARWIDGETHEIGHT + PARTITLEMARGIN, btnWidth, btnHeight, "Reset");
		resetSetBtn->tooltip("Reset landmark set");
		resetSetBtn->callback(resetSetCallback, (void *) this);
	}
	buttonGroup->resizable(emptyBox);
	buttonGroup->end();
	
	browser = new ColResizeBrowser(x(), y() + 3 * STDPARWIDGETHEIGHT + PARTITLEMARGIN, w(), 11 * STDPARWIDGETHEIGHT - PARTITLEMARGIN);
	browser->callback(browserCallback, (void*) this);
	browser->textsize(12);
	browser->showcolsep(1);
	browser->colsepcolor(FL_BLACK);

	// n columns requires n - 1 column widths + an ending zero element to terminate the array
	column_widths[0] = 30;
	column_widths[1] = 60;
	column_widths[2] = 0;
	// Remove the row above, umcomment the following two and the row in
	// resolve_string() to enable the option column in the browser
	// column_widths[2] = 40;
	// column_widths[3] = 0;	// zero-terminated

	browser->column_widths(column_widths);
	browser->column_char(';');

	end();

	point_collection * points = new point_collection();
	points->name("Landmark collection");
	
	datamanagement.add(points);

	landmarksID = points->get_id();
	
	// show landmarks in viewport 1,2,3
	for ( int viewportID = 1; viewportID < 4; viewportID++ )
	{
		rendermanagement.enable_data(viewportID, landmarksID);
	}
}

void FLTKuserIOpar_landmarks::data_vector_has_changed()
{
	update_browser();
	//next();
}

void FLTKuserIOpar_landmarks::resetSetCallback( Fl_Widget * callingwidget, void * thisLandmarks )
{
	FLTKuserIOpar_landmarks * l = reinterpret_cast<FLTKuserIOpar_landmarks *>(thisLandmarks);

	if ( l->browser->size() == 0 )
		{ return; }

	point_collection * points = dynamic_cast<point_collection *>( datamanagement.get_data(l->get_landmarksID()) );
	
	if ( points == NULL )
	{
		pt_error::error("FLTKuserIOpar_landmarks::resetSetCallback(): point_collection does not exist", pt_error::warning);
		return;
	}

	points->clear();

	l->update_browser();
		
	l->browser->value(1);	// set the first landmark as active
	points->set_active(l->landmarks[0].index);	// 0 = l->browser->value() - 1
	
	std::vector<int> combinationIDs = rendermanagement.combinations_from_data ( l->get_landmarksID() );
	
	for ( std::vector<int>::const_iterator itr = combinationIDs.begin(); itr != combinationIDs.end(); itr++ )
	{
		viewmanagement.refresh_viewports_from_combination( *itr );
	}
}

void FLTKuserIOpar_landmarks::saveSetCallback(Fl_Widget * callingwidget, void * thisLandmarks)
{
	FLTKuserIOpar_landmarks * l = reinterpret_cast<FLTKuserIOpar_landmarks *>(thisLandmarks);

	if ( l->browser->size() == 0 )
		{ return; }

	point_collection * points = dynamic_cast<point_collection *>( datamanagement.get_data( l->get_landmarksID() ) );

	if ( points == NULL )
	{
		pt_error::error("FLTKuserIOpar_landmarks::resetSetCallback(): point_collection does not exist", pt_error::warning);
		return;
	}

	std::string last_path;
	try 
		{ last_path = pt_config::read<std::string>("latest_path-landmarks_save"); } 
	catch ( pt_error )
		{ last_path = "."; }

	Fl_File_Chooser chooser(last_path.c_str(), "Landmark files (*.txt)\tAny file (*)", Fl_File_Chooser::CREATE, "Save landmarks");
    chooser.ok_label( "Save" );	
	chooser.show();
		
	while( chooser.shown() )
		{ Fl::wait(); }
		
	if ( chooser.value() == NULL )
	{
        pt_error::error ( "Save landmarks dialog cancel", pt_error::notice );
		return;
	}

	pt_config::write("latest_path-landmarks_save", path_parent(chooser.value(1)));
	
	// add the prefix ".txt" if not present
	string temp( chooser.value() );
	string prefix = temp.substr( temp.size() - 4, 4 );
	if ( prefix != ".txt" )
		{ temp.append( ".txt" ); }

	ofstream ofs ( temp.c_str() );
	
	if ( !ofs )
	{
		// TODO: use the pt_error class to generate an error message
		return;
	}

	for ( unsigned int i = 0; i < l->landmarks.size(); i++ )
	{
		ofs << l->landmarks[i].index << ";" << l->landmarks[i].description << ";" << l->landmarks[i].option;
		
		if ( points->contains(l->landmarks[i].index) )
		{
			Vector3D point = points->get_point(l->landmarks[i].index);
			ofs << ";" << point;			
		}
		ofs << std::endl;
	}
			
	ofs.close();
}

void FLTKuserIOpar_landmarks::showCallback(Fl_Widget * callingwidget, void * thisLandmarks)
{
	userIO * userIO_block = reinterpret_cast<userIO *>(callingwidget->parent()->parent()->parent());
	
	FLTKuserIOpar_image * image = NULL;
	
	int nc = userIO_block->children();

	for (int c = 0; c < nc; c++)
	{
		if ( image = dynamic_cast<FLTKuserIOpar_image *>(userIO_block->child(c)) )
			{ break; }
	}
	
	if ( image == NULL )
		{ return; }

	int imageID;
	 image->par_value(imageID);

	if ( imageID == NOT_FOUND_ID )
		{ return; }
	
	FLTKuserIOpar_landmarks * l = reinterpret_cast<FLTKuserIOpar_landmarks *>(thisLandmarks);
	
	int viewportID = 1;
	for ( std::vector<Fl_Check_Button *>::const_iterator itr = l->checkBtns.begin(); itr != l->checkBtns.end(); itr++ )
	{
		if ( (*itr)->value() )
		{	// is checked
			rendermanagement.enable_data( viewportID, imageID );
			rendermanagement.center3d_and_fit( viewportID, imageID );			
		}
		else
		{
			rendermanagement.disable_data( viewportID, imageID );			
		}	
		viewportID++;
	}
}

void FLTKuserIOpar_landmarks::goCallback(Fl_Widget * callingwidget, void * thisLandmarks)
{
	FLTKuserIOpar_landmarks * l = reinterpret_cast<FLTKuserIOpar_landmarks *>(thisLandmarks);
	
	Vector3D point = create_Vector3D( atof(l->xInput->value()), atof(l->yInput->value()), atof(l->zInput->value()) );
	viewmanagement.show_point_by_data(point, l->get_landmarksID());
}

void FLTKuserIOpar_landmarks::loadSetCallback( Fl_Widget *callingwidget, void * thisLandmarks )
{
	std::string last_path;
	try 
		{ last_path = pt_config::read<std::string>("latest_path-landmarks_load"); } 
	catch ( pt_error )
		{ last_path = "."; }

	Fl_File_Chooser chooser(last_path.c_str(), "Landmark files (*.txt)\tAny file (*)", Fl_File_Chooser::SINGLE, "Load landmark file");	
    chooser.ok_label("Load");	
	chooser.show();
		
	while ( chooser.shown() )
		{ Fl::wait(); }
		
	if ( chooser.value() == NULL )
	{
        pt_error::error( "FLTKuserIOpar_landmarks::loadSetCallback():Landmark load dialog cancel", pt_error::notice );
		return;
	}

	pt_config::write("latest_path-landmarks_load", path_parent(chooser.value(1)));

	ifstream ifs( chooser.value() );
	
	if ( !ifs )
	{
		pt_error::error("FLTKuserIOpar_landmarks::loadSetCallback(): unable to read file", pt_error::warning);
		return;
	}

	FLTKuserIOpar_landmarks * l = reinterpret_cast<FLTKuserIOpar_landmarks *>(thisLandmarks);

	point_collection * points = dynamic_cast<point_collection *>( datamanagement.get_data( l->get_landmarksID() ) );

	if ( points == NULL )
	{
		// TODO: create a new point_collection instead of just returning an error message
		pt_error::error("FLTKuserIOpar_landmarks::loadSetCallback(): point_collection does not exist", pt_error::warning);
		return;
	}

	points->clear();
	l->landmarks.clear();

	const int line_length = 500;
	char buffer[line_length];

	while ( !ifs.eof() )
	{
		ifs.getline(buffer, line_length);
		
		std::string s = std::string(buffer);

		if ( !s.empty() )
		{	// not a blank line

			if ( s.at(0) != '#' )
			{	// not a comment line (ie a row beginning with '#')

				std::vector<std::string> v;
				split(s, ';', v);

				int index = atoi( v[0].c_str() );
								
				l->landmarks.push_back( landmark(index, v[1], v[2]) );

				if ( v.size() > 3 )
				{
					std::string coordinate = v[3];	
					coordinate = coordinate.substr( coordinate.find('[', 0) + 1, coordinate.find(']', 0) - ( coordinate.find('[', 0) + 1) );	// get the string inside [...]
					std::vector<std::string> vv;
					split(coordinate, ',', vv);
										
					Vector3D point = create_Vector3D( atof(vv[0].c_str()), atof(vv[1].c_str()), atof(vv[2].c_str()) );
					
					points->add_pair(index, point);
					
				}
			}
		}		
	}
	
	ifs.close();
	
	l->update_browser();

	std::vector<int> combinationIDs = rendermanagement.combinations_from_data ( l->get_landmarksID() );
	
	if ( points->empty() )
	{ 
		l->browser->value(1);	// activate the first landmark in the browser
		points->set_active(l->landmarks[0].index);	// 0 is equal to l->browser->value() - 1
		for ( std::vector<int>::const_iterator itr = combinationIDs.begin(); itr != combinationIDs.end(); itr++ )
			{ viewmanagement.refresh_viewports_from_combination( *itr ); }
	}
	else
	{	// find the first landmark in the browser that is "connected" to a coordinate
		for ( unsigned int i = 0; i < l->landmarks.size(); i++ )
		{
			if ( points->contains(l->landmarks[i].index) )
			{
				l->browser->value(i + 1);
				points->set_active(l->landmarks[i].index);
				viewmanagement.show_point_by_data(points->get_point(l->landmarks[i].index), l->get_landmarksID());
				break;
			}
		}
	}		
}

void FLTKuserIOpar_landmarks::browserCallback(Fl_Widget *callingwidget, void * thisLandmarks )
{
	FLTKuserIOpar_landmarks * l = reinterpret_cast<FLTKuserIOpar_landmarks *>(thisLandmarks);

	if ( l->browser->value() < 1 )
		{ return; }

//	use this if a row with labels are implemented in the future
//	if ( l->browser->value() == 1 )
//	{	// the first row contains all the labels
//		browser->deselect()
//		return;
//	}

	point_collection * points = dynamic_cast<point_collection *>( datamanagement.get_data( l->get_landmarksID() ) );

	if ( points == NULL )
	{
		pt_error::error("FLTKuserIOpar_landmarks::resetSetCallback(): point_collection does not exist", pt_error::warning);
		return;
	}
	
	int index = l->landmarks[l->browser->value() - 1].index;
	points->set_active(index);
	
	if ( points->contains(index) )
	{
		viewmanagement.show_point_by_data(points->get_point(index), l->get_landmarksID());
	}
}

std::string FLTKuserIOpar_landmarks::resolve_string( int i )
{
	std::string str = "";
	str += "@r@." + int2str(landmarks[i].index);	// index
	str += ";@b@." + landmarks[i].description;		// description
	// Uncomment the following row and the column_widths[] rows in the constructor
	// FLTKuserIOpar_landmarks() to enable the option column in the browser
	// str += ";" + landmarks[i].option;			// option

	return str;
}

const std::string FLTKuserIOpar_landmarks::type_name()
{
	return "active landmark";
}

void FLTKuserIOpar_landmarks::par_value(landmarksIndexType &v)
{	
	if ( browser->value() > 0 )
		{ v = landmarks[browser->value() - 1].index; }
	else
		{ v = -1; }
//	v = browser->value();
}


void FLTKuserIOpar_landmarks::update_browser()
{
	int browser_index = browser->value();	// remember current browser index and use it again after the update is made
	
	browser->clear();
	
	point_collection * points = dynamic_cast<point_collection *>( datamanagement.get_data( landmarksID ) );

	if ( points == NULL )
		{ return; }

	if ( points->empty() )
	{
		for ( unsigned int i = 0; i < landmarks.size(); i++ )
		{
			std::string str = resolve_string(i);
			browser->add(str.c_str());
		}
	}
	else
	{			
		for ( unsigned int i = 0; i < landmarks.size(); i++ )
		{
			std::string coordinate = "";
			if ( points->contains(landmarks[i].index) )
			{
				Vector3D point = points->get_point(landmarks[i].index);

				std::ostringstream oss;
				oss.setf(ios::fixed);
				oss << " " << setprecision(1) << point;
				
				coordinate = oss.str();
			}	
			
			std::string str = resolve_string(i) + ";";
			str += "@b@." + coordinate;

			browser->add(str.c_str());
		}
	}
	
	browser->value(browser_index);
}

void FLTKuserIOpar_landmarks::next()
{
	if ( browser->value() != 0 && browser->value() < browser->size() ) 	// 0 means that no line in the Fl_Hold_Browser i chosen (the index of the first row in Fl_Hold_Browser is 1)
	{
		browser->value(browser->value() + 1);
		point_collection * points = dynamic_cast<point_collection *>( datamanagement.get_data( landmarksID ) );

		if ( points == NULL )
		{
			pt_error::error("FLTKuserIOpar_landmarks::resetSetCallback(): point_collection does not exist", pt_error::warning);
			return;
		}

		points->set_active(landmarks[browser->value() - 1].index);
	}
}

int FLTKuserIOpar_landmarks::get_landmarksID()
{
	return landmarksID;
}

int FLTKuserIOpar_landmarks::handle(int event)
{
	int ret = 0;
	
	if ( event == FL_KEYDOWN )
	{				
		if ( Fl::event_key() == FL_Delete )
		{			
			if ( browser->size() == 0 )
			{	// browser is empty
				ret = 0;
			}
			else
			{
				point_collection * points = dynamic_cast<point_collection *>(datamanagement.get_data(landmarksID));
				
				if ( points == NULL )
				{
					pt_error::error("FLTKuserIOpar_landmarks::resetSetCallback(): point_collection does not exist", pt_error::warning);
					return 1;
				}

				int index = landmarks[browser->value() - 1].index;
				
				if ( points->contains(index) )
				{
					points->remove(index);
					
					update_browser();
					
					std::vector<int> combinationIDs = rendermanagement.combinations_from_data(landmarksID);
					
					for ( std::vector<int>::const_iterator itr = combinationIDs.begin(); itr != combinationIDs.end(); itr++ )
						{ viewmanagement.refresh_viewports_from_combination( *itr ); }
				}
				ret = 1;
			}
		}
	}
	return ( FLTKuserIOparameter_base::handle(event) ? 1 : ret );
}


#pragma mark *** FLTKuserIOpar_float ***

FLTKuserIOpar_float::FLTKuserIOpar_float (const std::string name, float ma, float mi) : FLTKuserIOparameter_base (INITPARWIDGETWIDTH,STDPARWIDGETHEIGHT, name)
    {
    //label set by superclass constructor
    //position & width set by parent userIO later, including margin
    control = new Fl_Value_Slider(x(),y()+PARTITLEMARGIN,w(),h()-PARTITLEMARGIN);

    control->callback(par_update_callback);

    control->type(FL_HOR_SLIDER);
    control->maximum(ma);
    control->minimum(mi);

    resizable(control);

    //this->add(control);
    end();
    }

void FLTKuserIOpar_float::par_value (float & v)
    {
    v=control->value();
    }

const std::string FLTKuserIOpar_float::type_name ()
    {
    return "float";
    }

void FLTKuserIOpar_float::set_value (float val)
{
	control->value(val);
}

#pragma mark *** FLTKuserIOpar_float_box ***

FLTKuserIOpar_float_box::FLTKuserIOpar_float_box (const std::string name, float ma, float mi, float start_val) : FLTKuserIOparameter_base (INITPARWIDGETWIDTH,STDPARWIDGETHEIGHT, name)
    {
    //label set by superclass constructor
    //position & width set by parent userIO later, including margin
    control = new Fl_Value_Input(x(),y()+PARTITLEMARGIN,PARMENUWIDTH,h()-PARTITLEMARGIN);

    control->callback(par_update_callback);

    control->maximum(ma);
    control->minimum(mi);
    control->step(0.001);
	control->value(start_val);

    resizable(NULL);

    end();
    }

void FLTKuserIOpar_float_box::par_value (float & v)
    {
    v=static_cast<float>(control->value());
    }

const std::string FLTKuserIOpar_float_box::type_name ()
    {
    return "float_box (Fl_Value_Input)";
    }


#pragma mark *** FLTKuserIOpar_longint ***

FLTKuserIOpar_longint::FLTKuserIOpar_longint (const std::string name, long ma, long mi) : FLTKuserIOparameter_base (INITPARWIDGETWIDTH,STDPARWIDGETHEIGHT, name)
    {
    //label set by superclass constructor
    //position & width set by parent userIO later, including margin

//    control = new Fl_Value_Slider(x(),y()+PARTITLEMARGIN,w(),h()-PARTITLEMARGIN);
    control = new FLTK_Editable_Slider(x(),y()+PARTITLEMARGIN,w(),h()-PARTITLEMARGIN,"",20);
	

    control->callback(par_update_callback);

    control->type(FL_HOR_SLIDER);
    control->maximum(ma);
    control->minimum(mi);
    control->step(1);

    resizable(control);

    end();
    }

void FLTKuserIOpar_longint::par_value (long & v)
    {
    v=static_cast<long>(control->value());
    }

const std::string FLTKuserIOpar_longint::type_name ()
    {
    return "long (slider)";
    }

void FLTKuserIOpar_longint::set_value (long val)
{
	control->value(val);
}

void FLTKuserIOpar_longint::set_value_no_Fl_callback(long val)
{
	control->value_no_Fl_callback(val);
}

#pragma mark *** FLTKuserIOpar_longint_box ***

FLTKuserIOpar_longint_box::FLTKuserIOpar_longint_box (const std::string name, long ma, long mi) : FLTKuserIOparameter_base (INITPARWIDGETWIDTH,STDPARWIDGETHEIGHT, name)
    {
    //label set by superclass constructor
    //position & width set by parent userIO later, including margin
    control = new Fl_Value_Input(x(),y()+PARTITLEMARGIN,PARMENUWIDTH,h()-PARTITLEMARGIN);

    control->callback(par_update_callback);

    control->maximum(ma);
    control->minimum(mi);
    control->step(1);

    resizable(NULL);

    end();
    }

void FLTKuserIOpar_longint_box::par_value (long & v)
    {
    v=static_cast<long>(control->value());
    }

const std::string FLTKuserIOpar_longint_box::type_name ()
    {
    return "long (slider)";
    }

#pragma mark *** FLTKuserIOpar_bool ***

FLTKuserIOpar_bool::FLTKuserIOpar_bool (const std::string name, bool init_status ) : FLTKuserIOparameter_base (INITPARWIDGETWIDTH,STDPARWIDGETHEIGHT, name)
    {
    //label set by superclass constructor
    //position & width set by parent userIO later, including margin
    control = new Fl_Check_Button(x(),y()+PARTITLEMARGIN,w(),h()-PARTITLEMARGIN);

    control->callback(par_update_callback);

    control->type(FL_TOGGLE_BUTTON);
    control->value(init_status);

    resizable(control);

    end();
    }

void FLTKuserIOpar_bool::par_value (bool & v)
    {
    v=(control->value() == 0 ? false : true);
    }

const std::string FLTKuserIOpar_bool::type_name ()
    {
    return "boolean";
    }


#pragma mark *** FLTKuserIOpar_string ***

FLTKuserIOpar_string::FLTKuserIOpar_string (const std::string name, std::string init_status ) : FLTKuserIOparameter_base (INITPARWIDGETWIDTH,STDPARWIDGETHEIGHT, name)
{
    //label set by superclass constructor
    //position & width set by parent userIO later, including margin
    control = new Fl_Input(x(),y()+PARTITLEMARGIN,w(),h()-PARTITLEMARGIN);
	
    control->callback(par_update_callback);
	//    control->type(FL_TOGGLE_BUTTON);
	control->value(init_status.c_str());
	
    resizable(control);
	
    end();
}

void FLTKuserIOpar_string::par_value (std::string & s)
{
    s = std::string(control->value());
}

const std::string FLTKuserIOpar_string::type_name ()
{
	return "std::string";
}


#pragma mark *** FLTKuserIOpar_stringlist ***

FLTKuserIOpar_stringlist::FLTKuserIOpar_stringlist (const std::string name, std::vector<std::string> strlist ) : FLTKuserIOparameter_base (INITPARWIDGETWIDTH,STDPARWIDGETHEIGHT, name)
{
	Fl_Menu_Item* menuitems = (Fl_Menu_Item*)malloc((strlist.size()+1)*sizeof(Fl_Menu_Item));
	for (int i = 0; i<strlist.size(); i++) {
		init_fl_menu_item(menuitems[i]);
		menuitems[i].label(strdup(strlist[i].c_str()));
		menuitems[i].flags=FL_MENU_RADIO;
		menuitems[i].callback(set_string_static_callback, this);
	}
	menuitems[strlist.size()].label(NULL);
    menuitems[0].setonly();
	control = new Fl_Menu_Button(x(),y()+PARTITLEMARGIN,w(),h()-PARTITLEMARGIN,current_value);
	strcpy(current_value,strlist[0].c_str());
	
	control->copy(menuitems);
	control->box(FL_THIN_UP_BOX);
	control->labelsize(FLTK_SMALL_LABEL);
	
    end();
}

void FLTKuserIOpar_stringlist::set_string_callback(Fl_Widget* callingwidget) {
	//string value = (((Fl_Menu_*)callingwidget)->mvalue())->label();
	//strcpy(current_value, value.c_str());
	strcpy(current_value, (((Fl_Menu_*)callingwidget)->mvalue())->label());
}

void FLTKuserIOpar_stringlist::set_string_static_callback(Fl_Widget* callingwidget, void* the_object) {
	FLTKuserIOpar_stringlist* object = (FLTKuserIOpar_stringlist*)the_object;
	object->set_string_callback(callingwidget);
}

void FLTKuserIOpar_stringlist::par_value (std::string & s) {
	s = (control->mvalue())->label();
}

const std::string FLTKuserIOpar_stringlist::type_name () {
	return "std::string";
}


#pragma mark *** FLTKuserIOpar_image ***

FLTKuserIOpar_image::FLTKuserIOpar_image (const std::string name) : FLTKuserIOparameter_base (INITPARWIDGETWIDTH, STDPARWIDGETHEIGHT, name)
    {
    control = new FLTKimage_choice(x(),y()+PARTITLEMARGIN);
    control->callback(par_update_callback);

    control->data_vector_has_changed();
    
    resizable(NULL);

    end();
    }

void FLTKuserIOpar_image::par_value (imageIDtype & v)
    {
    v = control->id_value();
    }

void FLTKuserIOpar_image::data_vector_has_changed ()
    {
    control->data_vector_has_changed();
    }


const std::string FLTKuserIOpar_image::type_name ()
    {
    return "image ID";
    }

#pragma mark *** FLTKuserIOpar_imageshow ***

FLTKuserIOpar_imageshow::FLTKuserIOpar_imageshow( const std::string name) : FLTKuserIOpar_image( name )
{
	current( this );
	
	const int width = 30;	

	std::vector<int> rendererIDs = rendermanagement.get_renderers();
	
	for ( std::vector<int>::const_iterator itr = rendererIDs.begin(); itr != rendererIDs.end(); itr++ )
	{
		std::string index = int2str( *itr );
		
		Fl_Check_Button * temp = new Fl_Check_Button( x() + PARMENUWIDTH + (*itr - 1) * width , y() + PARTITLEMARGIN, width, h() - PARTITLEMARGIN );
		temp->copy_label( index.c_str() );
		if ( *itr < 4 )
			{ temp->set(); }		// check viewport 1,2,3 by default
		
		checkbuttons.push_back( temp );
	}
	
	control->callback( show_callback, (void*) this );
		
    end();
}

void FLTKuserIOpar_imageshow::show_callback( Fl_Widget * callingwidget, void * this_image_show )
{
	par_update_callback( callingwidget, NULL );

	FLTKuserIOpar_imageshow * image_show = reinterpret_cast<FLTKuserIOpar_imageshow *>(this_image_show);
	
	int imageID = image_show->control->id_value();
	
	if ( imageID == NOT_FOUND_ID )
		{ return; }
	
	int viewportID = 1;
	for ( std::vector<Fl_Check_Button *>::const_iterator itr = image_show->checkbuttons.begin(); itr != image_show->checkbuttons.end(); itr++ )
	{
		if ( (*itr)->value() )
		{	// is checked
			rendermanagement.enable_data( viewportID, imageID );
			rendermanagement.center3d_and_fit( viewportID, imageID );			
		}
		else
		{
			rendermanagement.disable_data( viewportID, imageID );			
		}	
		viewportID++;
	}
	
	
//	for ( int i = 1; i < 4; i++ )
//	{	// show the image in viewport 1,2,3
//		rendermanagement.enable_data( i, imageID );
//		rendermanagement.center3d_and_fit( i, imageID );		
//	}
}


#pragma mark *** FLTKuserIOpar_points ***

FLTKuserIOpar_points::FLTKuserIOpar_points (const std::string name) : FLTKuserIOparameter_base (INITPARWIDGETWIDTH,STDPARWIDGETHEIGHT, name)
{
    control = new FLTKpoint_choice(x(),y()+PARTITLEMARGIN);
    control->callback(par_update_callback);
    
    control->data_vector_has_changed();
    
    resizable(NULL);
    
    end();
}

void FLTKuserIOpar_points::par_value (Vector3D & v)
{
    point * p = dynamic_cast<point *> (datamanagement.get_data(control->id_value()));

    pt_error::error_if_null(p,"requested Point3D value, point object is of different subclass",pt_error::serious);
    
    v = p->get_origin();
}

void FLTKuserIOpar_points::par_value (pointIDtype & v)
{
    v = control->id_value();
}

void FLTKuserIOpar_points::data_vector_has_changed ()
{
    control->data_vector_has_changed();
}

const std::string FLTKuserIOpar_points::type_name ()
{
    return "point ID";
}


/*
//FLTKuserIOpar_voxelseed
#pragma mark *** FLTKuserIOpar_voxelseed ***

FLTKuserIOpar_voxelseed::FLTKuserIOpar_voxelseed (const std::string name) : FLTKuserIOparameter_base(INITPARWIDGETWIDTH,STDPARWIDGETHEIGHT, name)
{
	//ˆˆˆ
    catch_button = new Fl_Button(x(),y()+PARTITLEMARGIN,50,STDPARWIDGETHEIGHT-PARTITLEMARGIN,"Catch..");
    catch_button->callback(catch_button_cb);
	
	v = new FLTK_Vector3D(x()+60,y()+PARTITLEMARGIN,150,STDPARWIDGETHEIGHT-PARTITLEMARGIN);
    resizable(NULL);
    end();
}

void FLTKuserIOpar_voxelseed::par_value (Vector3D & vec)
{
	vec = v->get(); //ˆˆˆ
}

const std::string FLTKuserIOpar_voxelseed::type_name ()
{
    return "Vector3D voxel seedpoint";
}

void FLTKuserIOpar_voxelseed::catch_button_cb(Fl_Widget *callingwidget, void *)
{
	cout<<"catch_button_cb..."<<end;
	FLTKuserIOpar_voxelseed *vs = (FLTKuserIOpar_voxelseed*)callingwidget->parent();
	Vector3D a;
	a[0]=0;
	a[1]=1;
	a[2]=2;
	cursor_tool::get_global_selection_coords();
	vs->v->set(a);
}
*/


//data choice widget base class

FLTKdataitem_choice::FLTKdataitem_choice (int x, int y):Fl_Choice(x,y,PARMENUWIDTH,STDPARWIDGETHEIGHT-PARTITLEMARGIN)
{}

void FLTKdataitem_choice::data_change (const Fl_Menu_Item * base_menu)
{
    //store selection
    int selected_item=value();
    long selected_vol=NOT_FOUND_ID;
    
    const Fl_Menu_Item * cur_menu = menu();   
    
    if (cur_menu != NULL)
        {selected_vol=cur_menu[selected_item].argument();}
    
    selected_item=0; //set to no selection if previous selection isn't found below
    
    //rebuild
    Fl_Menu_Item new_menu[datamanager::DATA_VECTOR_MAX+2];
    
    new_menu[0].label("(no selection)");
    new_menu[0].shortcut(0);
    new_menu[0].callback((Fl_Callback *)NULL,0);
    new_menu[0].argument(0);
    new_menu[0].flags= 0;
    new_menu[0].labeltype(FL_NORMAL_LABEL);
    new_menu[0].labelfont(0);
    new_menu[0].labelsize(FLTK_LABEL_SIZE);
    new_menu[0].labelcolor(FL_BLACK);
    
    int m=1;
    
    while (base_menu[m-1].label() !=NULL) 
        {    
        memcpy (&new_menu[m],&base_menu[m-1],sizeof(Fl_Menu_Item));
        
        if (new_menu[m].argument() == selected_vol)
            {selected_item=m;}
        m++;
        }
    new_menu[m].label(NULL);
    
    copy(new_menu);
    value(selected_item);
    redraw();
}

#pragma mark *** General image choice widget ***

FLTKimage_choice::FLTKimage_choice (int x, int y) : FLTKdataitem_choice(x,y)
    {
    data_vector_has_changed();
    }

imageIDtype FLTKimage_choice::id_value ()
    {
    const Fl_Menu_Item *cur_menu=menu();   
    int selected_item=value();

    if (selected_item==0)
        {return NOT_FOUND_ID;}

    if (cur_menu !=NULL)
        {return cur_menu[selected_item].argument();}

    return NOT_FOUND_ID;
    }

void FLTKimage_choice::data_vector_has_changed()
{
    data_change(datamanagement.FLTK_image_menu());
}

#pragma mark *** General point choice widget ***

FLTKpoint_choice::FLTKpoint_choice (int x, int y) : FLTKdataitem_choice(x,y)
{
    data_vector_has_changed();
}

pointIDtype FLTKpoint_choice::id_value ()
{
    const Fl_Menu_Item *cur_menu=menu();   
    int selected_item=value();
    
    if (selected_item==0)
        {return NOT_FOUND_ID;}
    
    if (cur_menu !=NULL)
        {return cur_menu[selected_item].argument();}
    
    return NOT_FOUND_ID;
}

void FLTKpoint_choice::data_vector_has_changed()
{
    data_change(datamanagement.FLTK_point_menu());
}

#pragma mark *** Histogram display widget ***

    //TODO: variable resolution
    //right mouse button on whole widget acts as a slider
    //up-down, with single bucket resolution at the top and
    //1:1 (depending on widget size) at the bottom

FLTK_histogram_base::FLTK_histogram_base (int x, int y, int w, int h) : Fl_Widget (x,y,w,h)
    {
    //screen_image=NULL;
    screen_image_data=NULL;

    screen_image_data = new unsigned char [w*h*RGB_pixmap_bpp];
    //screen_image = new Fl_RGB_Image(screen_image_data, w, h,RGB_pixmap_bpp, 0);
    }

FLTK_histogram_base::~FLTK_histogram_base ()
    {
    //delete screen_image;
    delete []screen_image_data;
    }

void FLTK_histogram_base::resize(int x, int y, int w, int h)
    {
    if (w != this->w() || h != this->h())
        {
        //delete screen_image;
        delete [] screen_image_data;

        screen_image_data = new unsigned char [w*h*RGB_pixmap_bpp];
        //screen_image = new Fl_RGB_Image(screen_image_data, w, h,RGB_pixmap_bpp, 0);
        }

    Fl_Widget::resize(x, y, w, h);

    refresh();
    }

FLTK_histogram_2D::FLTK_histogram_2D (int x, int y, int w, int h) : FLTK_histogram_base (x, y, w, h)
    {
    histogram = NULL;
    histogram = new HISTOGRAM2DVARIETY ();
    drag_mode=READY_RECT;
    }

FLTK_histogram_2D::~FLTK_histogram_2D()
    {
    delete histogram;
    }

int FLTK_histogram_2D::handle(int event)
    {
    //define thresholds as percentage, let histogram object transform these into actual values

    threshold=histogram->get_threshold();

    if ( histogram->ready() )
        {
        switch (event)
            {
            case FL_PUSH:
                {
                drag_ref_x=(Fl::event_x()-x())/(float)w();
                drag_ref_y=1.0-(Fl::event_y()-y())/(float)h();

                if (Fl::event_button() == FL_LEFT_MOUSE && threshold.id[0]!=NOT_FOUND_ID && threshold.id[1]!=NOT_FOUND_ID)
                    {
                    if (drag_mode==READY_OVAL)
                        {
                        if (std::sqrt(powf((drag_ref_x-((h_treshold_max+h_treshold_min)/2.0))/((h_treshold_max-h_treshold_min)/(v_treshold_max-v_treshold_min)),2.0)+powf(drag_ref_y-((v_treshold_max+v_treshold_min)/2.0),2.0)) <= (v_treshold_max-v_treshold_min)/2.0)
                            {drag_mode=MOVE_OVAL;}
                        else
                            {drag_mode=SET_OVAL;}
                        }

                    if (drag_mode==READY_RECT)
                        {
                        if (drag_ref_x >= h_treshold_min && drag_ref_x <= h_treshold_max && drag_ref_y >= v_treshold_min && drag_ref_y <= v_treshold_max)
                            {drag_mode=MOVE_RECT;}
                        else
                            {drag_mode=SET_RECT;}
                        }

                    thresholders = viewmanagement.get_overlays(& threshold);
                    }
                }
            break;

            case FL_DRAG:
                {
                float drag_point_x = (float)(Fl::event_x()-x())/(float)w();
                float drag_point_y = 1.0-(float)(Fl::event_y()-y())/(float)h();

                if (Fl::event_button() == FL_LEFT_MOUSE)
                    {
                    switch (drag_mode)
                        {
                        case SET_RECT:                    
                            //check positive/negative dimensions to make
                            //thresholding values more digestible
                            if (drag_point_x - drag_ref_x > 0)
                                {
                                h_treshold_min=drag_ref_x;
                                h_treshold_max=drag_point_x;
                                }
                            else
                                {
                                h_treshold_min=drag_point_x;
                                h_treshold_max=drag_ref_x;
                                }

                            if (drag_point_y -drag_ref_y > 0)
                                {
                                v_treshold_min=drag_ref_y;
                                v_treshold_max=drag_point_y;
                                }
                            else
                                {
                                v_treshold_min=drag_point_y;
                                v_treshold_max=drag_ref_y;
                                }
                            break;
                        case SET_OVAL:
                            {
                            //check positive/negative dimensions to make
                            //thresholding values more digestible
                            float size_x=fabs(drag_point_x-drag_ref_x)*std::sqrt(2.0);
                            float size_y=fabs(drag_point_y-drag_ref_y)*std::sqrt(2.0);                            
                            //float r_extra=fabs(size_x-size_y);

                            h_treshold_min=drag_ref_x-size_x;
                            h_treshold_max=drag_ref_x+size_x;

                            v_treshold_min=drag_ref_y-size_y;
                            v_treshold_max=drag_ref_y+size_y;
                            }
                        break;

                        case MOVE_OVAL:
                        case MOVE_RECT:

                            h_treshold_min+=drag_point_x-drag_ref_x;
                            h_treshold_max+=drag_point_x-drag_ref_x;

                            v_treshold_min+=drag_point_y-drag_ref_y;
                            v_treshold_max+=drag_point_y-drag_ref_y;

                            //move is relative, set reference to last coordinate
                            drag_ref_x=drag_point_x;
                            drag_ref_y=drag_point_y;
                            break;
                        default:
                            {
                                //suppress GCC enum warning
                            }
                        }
                    }

                if (Fl::event_button() == FL_RIGHT_MOUSE)
                    {
                    //float drag_point_x = (float)(Fl::event_x()-x())/(float)w();
                    //float drag_point_y = 1.0-(float)(Fl::event_y()-y())/(float)h();
                    //change histogram resolution
                    int resolution=std::min(std::max(h()*(1-drag_point_y),(float)1),(float)h());

                    histogram->calculate_from_image_data(resolution);

                    /*refresh();*/

                    //screen_image->uncache();
                    
                    histogram->render(screen_image_data,w(),h());

                    draw();
                    }
                
                }  //end of FL_DRAG block
            break;

            case FL_RELEASE:
                if (Fl::event_button() == FL_LEFT_MOUSE)
                    {
                    if (drag_mode == SET_RECT ||drag_mode == MOVE_RECT  )
                        {drag_mode = READY_RECT;}

                    if (drag_mode == SET_OVAL ||drag_mode == MOVE_OVAL  )
                        {drag_mode = READY_OVAL;}
                    }

                damage(FL_DAMAGE_ALL);
                thresholders.clear();
                break;

            case FL_MOUSEWHEEL:
                { 
                //let user widen/shrink selection
                float center_x=(h_treshold_max+h_treshold_min)/2.0;
                float center_y=(v_treshold_max+v_treshold_min)/2.0;

                float size_x=fabs(h_treshold_max-h_treshold_min)*(0.5+((float)Fl::event_dy())*0.02);
                float size_y=fabs(v_treshold_max-v_treshold_min)*(0.5+((float)Fl::event_dy())*0.02);   

                Fl::event_dy();

                h_treshold_min=center_x-size_x;
                h_treshold_max=center_x+size_x;

                v_treshold_min=center_y-size_y;
                v_treshold_max=center_y+size_y;

                thresholders = viewmanagement.get_overlays(& threshold);
                }
            break;

            default:
                return 0;
            }

            if (thresholders.size() > 0)
                {
                //selection changed;
                //set new values for threshold
                threshold = ((HISTOGRAM2DVARIETY *)histogram)->get_threshold(h_treshold_min,h_treshold_max,v_treshold_min,v_treshold_max, (drag_mode > MOVE_RECT ? THRESHOLD_2D_MODE_OVAL : THRESHOLD_2D_MODE_RECT ));

                std::vector<threshold_overlay *>::iterator oitr=thresholders.begin();
                while (oitr != thresholders.end())
                    {
                    (*oitr)->render (&threshold);
                    oitr++;
                    }

                damage (FL_DAMAGE_ALL);
                }
        }
    else
        {
        // < 2 images selected, no event processed
        return 0;
        }

    return 1;
    }

void FLTK_histogram_2D::set_selmode(int mode)
	{
		if (mode==THRESHOLD_2D_MODE_OVAL)
		{drag_mode=READY_OVAL;}

		if (mode==THRESHOLD_2D_MODE_RECT)
		{drag_mode=READY_RECT;}

		damage (FL_DAMAGE_ALL);
	}

   void FLTK_histogram_2D::set_images (int hor, int vert)
        {
        ((HISTOGRAM2DVARIETY *)histogram)->images(hor,vert);

        thresholders.clear();
        refresh();
        }

    thresholdparvalue FLTK_histogram_base::get_threshold ()
        {
        threshold = histogram->get_threshold();
        return threshold;
        }

    void FLTK_histogram_base::draw ()
        {
        Fl_RGB_Image screen_image(screen_image_data, w(), h(),RGB_pixmap_bpp, 0);
                
        screen_image.draw(x(),y());
        }

    void FLTK_histogram_2D::draw ()
        {
        fl_push_clip(x(),y(),w(),h());
        
        Fl_RGB_Image screen_image(screen_image_data, w(), h(),RGB_pixmap_bpp, 0);

        screen_image.draw(x(),y());

        fl_color(fl_rgb_color(255, 0, 0));

        if ( drag_mode > MOVE_RECT)
            {
            fl_arc(h_treshold_min*w()+x(),h()-v_treshold_max*h()+y(),(h_treshold_max-h_treshold_min)*w(),(v_treshold_max-v_treshold_min)*h(),0,360) ;
            }
        else
            {
            fl_rect(h_treshold_min*w()+x(),h()-v_treshold_max*h()+y(),(h_treshold_max-h_treshold_min)*w(),(v_treshold_max-v_treshold_min)*h()) ;
            }

        fl_pop_clip();
        }

    void FLTK_histogram_base::refresh()
        {
        //allocate image data, 
        //screen_image_data = new unsigned char [w()*h()*RGB_pixmap_bpp];

        //screen_image->uncache();
        histogram->render(screen_image_data,w(),h());

        damage (FL_DAMAGE_ALL);
        }

    #pragma mark *** Histogram parameters ***

    FLTKuserIOpar_histogram2D::FLTKuserIOpar_histogram2D (std::string name) : FLTKuserIOparameter_base (INITPARWIDGETWIDTH,STDPARWIDGETHEIGHT+INITPARWIDGETWIDTH, name)
        {
        const int small_widget_size= STDPARWIDGETHEIGHT-PARTITLEMARGIN;

        histogram_widget = new FLTK_histogram_2D(x(), y()+STDPARWIDGETHEIGHT, w(), w());

        image_v= new FLTKimage_choice(x(),y()+PARTITLEMARGIN);
        image_h= new FLTKimage_choice(x()+w()-PARMENUWIDTH,y()+PARTITLEMARGIN+STDPARWIDGETHEIGHT+w());

        image_h->callback(vol_change_callback);
        image_v->callback(vol_change_callback);
        ((HISTOGRAM2DVARIETY *)histogram_widget->histogram)->images(image_v->id_value(),image_h->id_value());

        oval_button=new Fl_Button (x(),y()+PARTITLEMARGIN+STDPARWIDGETHEIGHT+w(),small_widget_size,small_widget_size,"@circle");
        oval_button->labelsize(9);
        oval_button->callback(selmode_change_callback);
        oval_button->argument(THRESHOLD_2D_MODE_OVAL);

        rect_button=new Fl_Button (x()+small_widget_size,y()+PARTITLEMARGIN+STDPARWIDGETHEIGHT+w(),small_widget_size,small_widget_size,"@square");
        rect_button->labelsize(9);
        rect_button->callback(selmode_change_callback);
        rect_button->argument(THRESHOLD_2D_MODE_RECT);

        oval_button->value(1);
        histogram_widget->set_selmode(THRESHOLD_2D_MODE_OVAL);

        resizable (NULL);
        end();
        }

    void FLTKuserIOpar_histogram2D ::vol_change_callback (Fl_Widget *callingwidget, void * foo)
        {
        FLTKuserIOpar_histogram2D * that = (FLTKuserIOpar_histogram2D *)callingwidget->parent();

        that->set_images (that->image_h->id_value(),that->image_v->id_value());
        }

    void FLTKuserIOpar_histogram2D::set_images (int hor, int vert)
        {
        histogram_widget->set_images (hor,vert);
        }

    void FLTKuserIOpar_histogram2D ::selmode_change_callback (Fl_Widget *callingwidget, void * foo)
        {
        FLTKuserIOpar_histogram2D * that = (FLTKuserIOpar_histogram2D *)callingwidget->parent();

        that->rect_button->value(callingwidget->argument() == THRESHOLD_2D_MODE_RECT );
        that->oval_button->value(callingwidget->argument() == THRESHOLD_2D_MODE_OVAL);

        that->histogram_widget->set_selmode(callingwidget->argument());
        }

    void FLTKuserIOpar_histogram2D::resize(int x, int y, int w, int h)
        {
        Fl_Widget::resize(x,y,w,w+STDPARWIDGETHEIGHT*2-PARTITLEMARGIN);

        image_v->position(this->x(),this->y()+PARTITLEMARGIN);

        histogram_widget->resize(this->x(), this->y()+STDPARWIDGETHEIGHT, this->w(), this->w());

        image_h->position(this->x()+this->w()-image_h->w(),this->y()+STDPARWIDGETHEIGHT+histogram_widget->h());

        oval_button->position(this->x(),this->y()+STDPARWIDGETHEIGHT+histogram_widget->h());
        rect_button->position(this->x()+STDPARWIDGETHEIGHT-PARTITLEMARGIN,this->y()+STDPARWIDGETHEIGHT+histogram_widget->h());
        }

    void FLTKuserIOpar_histogram2D::data_vector_has_changed ()
        {
        image_h->data_vector_has_changed();
        image_v->data_vector_has_changed();

        set_images (image_h->id_value(),image_v->id_value());
        }

    int FLTKuserIOpar_histogram2D::histogram_image_ID (int axis)
        {
        return histogram_widget->histogram->image_ID(axis);
        }

    void FLTKuserIOpar_histogram2D::highlight_ROI (regionofinterest * region)
        {
        ((HISTOGRAM2DVARIETY *)histogram_widget->histogram)->highlight(region);
        histogram_widget->refresh();
        }

    void FLTKuserIOpar_histogram2D::par_value (thresholdparvalue & v)
        {
        v = histogram_widget->get_threshold();
        }

    const std::string FLTKuserIOpar_histogram2D::type_name ()
        {
        return "2D threshold";
        }


FLTK_Vector3D::FLTK_Vector3D(int x, int y, int w, int h) : Fl_Group (x,y,w,h)
{
	inp_x = new Fl_Value_Input(x,		y, 30, h);
	inp_y = new Fl_Value_Input(x+35,	y, 30, h);
	inp_z = new Fl_Value_Input(x+2*35,	y, 30, h);
	Vector3D v;
	v.Fill(0);
	set(v);
}

Vector3D FLTK_Vector3D::get()
{
	Vector3D v;
	v[0] = inp_x->value();
	v[1] = inp_y->value();
	v[2] = inp_z->value();
	return v;
}
void FLTK_Vector3D::set(Vector3D v)
{
	inp_x->value(v[0]);
	inp_y->value(v[1]);
	inp_z->value(v[2]);
}



#pragma mark *** Message "parameter" ***

    FLTKuserIOpar_message::FLTKuserIOpar_message (std::string name,std::string msgtext) : FLTKuserIOparameter_base (INITPARWIDGETWIDTH,STDPARWIDGETHEIGHT, name)
        {
        message = new Fl_Output (x(),y()+PARTITLEMARGIN,w(),STDPARWIDGETHEIGHT-PARTITLEMARGIN);

        message->box(FL_FLAT_BOX);
        message->color(FL_BACKGROUND_COLOR);
        message->value (msgtext.c_str());

        resizable (message);
        end();
        }

    const std::string FLTKuserIOpar_message::type_name ()
        {
        return "message";
        }