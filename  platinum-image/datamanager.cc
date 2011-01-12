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

#include "datamanager.h"
#include "viewmanager.h"
#include "rendermanager.h"

#include "rawimporter.h"
#include "dicom_importer.h"
#include "userIOmanager.h"
#include "image_integer.hxx"
#include "stringmatrix.h"
//#include "renderer_base.h"
#include "image_base.h"


datamanager datamanagement;
extern rendermanager rendermanagement;
extern viewmanager viewmanagement;
extern userIOmanager userIOmanagement;

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Group.H>

using namespace std;


//uchar *animage;
/*
struct IDENTIFIER
    {
    char name[100];
    int identtype; // +-1 =char-array +-2 =short +4 =float -4 =int  0 =size-based
    int noofparams; // (only valid for identtype != 0; all params must be of same type
    };

IDENTIFIER IDENTIFIERS[] = {
    {"Version",2,1},
    {"Volumesize",2,3} // 3 params of type unsigned short
    };

int NOOFIDENTIFIERS = 2;
*/
//char prioriteter[10][2]; //possible priorities: 0...99
//int prioindex = 0;
//int prioritet = -1;


datamanager::datamanager()
{    
    image_menu = NULL;
    rebuild_image_menu();
    
    objects_menu = NULL;
    rebuild_objects_menu();
    
    point_menu = NULL;
    rebuild_point_menu();
    
    closing_program = false;
}

datamanager::~datamanager()
{
    closing_program = true;

    for (unsigned int i=0; i < dataItems.size(); i++)
        { delete dataItems[i]; }
	dataItems.clear();
}

void datamanager::datawidgets_setup()
{
    const int margin=10;

    const unsigned int xpos=Fl_Group::current()->x();
    const unsigned int width=Fl_Group::current()->w();
    const unsigned int ypos=0;

    Fl_Group *outergroup; //group containing data list and load control
    Fl_Widget *load_button;

    outergroup=new Fl_Group(xpos,ypos,width,DATALISTINITHEIGHT);

	//-----------------------
    Fl_Box *datalabel = new Fl_Box(xpos,ypos,width,LIST_HEADER_HEIGHT,"Data");
    datalabel->labelfont(FL_HELVETICA_BOLD);

    Fl_Button *clear_data_list_button = new Fl_Button(xpos+width-50,ypos+5,45,BUTTONHEIGHT-10,"Clear All");
    clear_data_list_button->labelfont(FL_HELVETICA);
	clear_data_list_button->labelsize(8);
    clear_data_list_button->callback(clear_data_list_button_callback,this);

    //we should create a bitmap here (i.e. the "animage"), and fill it with
    //color gradient or solid color depending on image type

    data_widget_box = new horizresizeablescroll(xpos,ypos+LIST_HEADER_HEIGHT,width,DATALISTINITHEIGHT-BUTTONHEIGHT-margin*2-LIST_HEADER_HEIGHT*2);
    data_widget_box->end();

	//-----------------------
    Fl_Group *buttongroup = new Fl_Group(xpos,data_widget_box->y()+data_widget_box->h(),width,BUTTONHEIGHT+margin*2);
    buttongroup->box(FL_NO_BOX);
	
	int b_width = 80;
    load_button = new Fl_Button(xpos,data_widget_box->y()+data_widget_box->h()+margin,b_width,BUTTONHEIGHT, "Load...");
    load_button->callback(loadimage_callback,this);

	//JK - dicom_import testing....
	#ifdef TESTMODE
		Fl_Widget *dcm_import_button = new Fl_Button(xpos+b_width+margin,data_widget_box->y()+data_widget_box->h()+margin,b_width,BUTTONHEIGHT, "Dcm Import...");
		dcm_import_button->callback(dcm_import_callback,this);

		Fl_Widget *dcm_series_button = new Fl_Button(xpos+2*b_width+2*margin,data_widget_box->y()+data_widget_box->h()+margin,b_width,BUTTONHEIGHT, "Dcm Series...");
		dcm_series_button->callback(dcm_series_callback,this);
	#endif

    buttongroup->resizable(NULL);
    buttongroup->end();

	//-----------------------
    Fl_Box * toolslabel = new Fl_Box(xpos,load_button->y()+load_button->h()+margin,width,LIST_HEADER_HEIGHT,"Tools");
    toolslabel->labelfont(FL_HELVETICA_BOLD );

    outergroup->resizable(outergroup);
    outergroup->box(FL_FLAT_BOX);
    outergroup->end();
}


// -------------------------- CALLBACKS -------------------------------

void datamanager::removedata_callback(Fl_Widget *callingwidget, void *thisdatamanager)
{
    datawidget_base *the_datawidget=(datawidget_base*)(callingwidget->user_data());
    ((datamanager*)thisdatamanager)->delete_data( the_datawidget->get_data_id() );
}

void datamanager::save_additional_data_callback(Fl_Widget *callingwidget, void * thisdatamanager){
	datawidget_base * the_datawidget=(datawidget_base *)(callingwidget->user_data());
    int image_index=((datamanager*)thisdatamanager)->find_data_index(the_datawidget->get_data_id());

	string last_path = pt_config::read<string>("latest_path");
	Fl_File_Chooser chooser(last_path.c_str(),"*",Fl_File_Chooser::CREATE,"Save additional data");
    chooser.ok_label("Save") ;
    chooser.preview(false); 
    chooser.show();
    while(chooser.shown())
        { Fl::wait(); }

    if ( chooser.value() == NULL )
    {
        fprintf(stderr, "(User hit 'Cancel')\n");
        return;
    }
	std::cout << "Yatta! Additional data saved :D " << std::endl;
	((datamanager*)thisdatamanager)->dataItems[image_index]->save_helper_data_to_file(chooser.value(1));
	pt_config::write("latest_path",path_parent(chooser.value(1)));
}

void datamanager::save_curve_callback(Fl_Widget *callingwidget, void * thisdatamanager){
	datawidget_base * the_datawidget=(datawidget_base *)(callingwidget->user_data());
    int image_index=((datamanager*)thisdatamanager)->find_data_index(the_datawidget->get_data_id());

	string last_path = pt_config::read<string>("latest_path");
	Fl_File_Chooser chooser(last_path.c_str(),"*",Fl_File_Chooser::CREATE,"Save additional data");
    chooser.ok_label("Save") ;
    chooser.preview(false); 
    chooser.show();
    while(chooser.shown())
        { Fl::wait(); }

    if ( chooser.value() == NULL )
    {
        fprintf(stderr, "(User hit 'Cancel')\n");
        return;
    }
	std::cout << "Yatta! Curve saved :D " << std::endl;
    //((datamanager*)thisdatamanager)->dataItems[image_index]->save_to_DCM_file(chooser.value(1));
	(dynamic_cast<curve_base*>(((datamanager*)thisdatamanager)->dataItems[image_index]))->save_curve_to_file(chooser.value(1));
	pt_config::write("latest_path",path_parent(chooser.value(1)));
}

void datamanager::toggle_additional_data(Fl_Widget *callingwidget, void * thisdatamanager){
	datawidget_base * the_datawidget=(datawidget_base *)(callingwidget->user_data());
    int image_index=((datamanager*)thisdatamanager)->find_data_index(the_datawidget->get_data_id());
	((datamanager*)thisdatamanager)->dataItems[image_index]->draw_additional_data = 
		!((datamanager*)thisdatamanager)->dataItems[image_index]->draw_additional_data;
}

void datamanager::connect_additional_data_callback(Fl_Widget *callingwidget, void * thisdatamanager){
	datawidget_base * the_datawidget=(datawidget_base *)(callingwidget->user_data());
    int image_index=((datamanager*)thisdatamanager)->find_data_index(the_datawidget->get_data_id());

	string last_path = pt_config::read<string>("latest_path");
	Fl_File_Chooser chooser(last_path.c_str(),"Platinum additional data (*.pad)",Fl_File_Chooser::CREATE,"Save additional data");
    chooser.ok_label("Open") ;
    chooser.preview(false); 
    chooser.show();
    while(chooser.shown())
        { Fl::wait(); }

    if ( chooser.value() == NULL )
    {
        fprintf(stderr, "(User hit 'Cancel')\n");
        return;
    }
	
    ((datamanager*)thisdatamanager)->dataItems[image_index]->read_helper_data_from_file(chooser.value(1));
	pt_config::write("latest_path",path_parent(chooser.value(1)));
	std::cout << "Yatta! Additional data connected :D " << std::endl;
}

void datamanager::save_dcm_callback(Fl_Widget *callingwidget, void * thisdatamanager)
{
    datawidget_base * the_datawidget=(datawidget_base *)(callingwidget->user_data());
    int image_index=((datamanager*)thisdatamanager)->find_data_index(the_datawidget->get_data_id());

	string last_path = pt_config::read<string>("latest_path");
	Fl_File_Chooser chooser(last_path.c_str(),"DICOM files (*.dcm)",Fl_File_Chooser::CREATE,"Save DICOM image");
    chooser.ok_label("Save") ;
    chooser.preview(false); 
    chooser.show();
    while(chooser.shown())
        { Fl::wait(); }

    if ( chooser.value() == NULL )
    {
        fprintf(stderr, "(User hit 'Cancel')\n");
        return;
    }

    ((datamanager*)thisdatamanager)->dataItems[image_index]->save_to_DCM_file(chooser.value(1));
	pt_config::write("latest_path",path_parent(chooser.value(1)));
}

void datamanager::save_vtk_callback(Fl_Widget *callingwidget, void * thisdatamanager)
{
    datawidget_base * the_datawidget=(datawidget_base *)(callingwidget->user_data());
    int image_index=((datamanager*)thisdatamanager)->find_data_index(the_datawidget->get_data_id());

	string last_path = pt_config::read<string>("latest_path");
	Fl_File_Chooser chooser(last_path.c_str(),"Visualization Toolkit image (*.vtk)",Fl_File_Chooser::CREATE,"Save VTK image");
    chooser.ok_label("Save") ;
    chooser.preview(false); 
    chooser.show();
    while(chooser.shown())
        { Fl::wait(); }

    if ( chooser.value() == NULL )
    {
        fprintf(stderr, "(User hit 'Cancel')\n");
        return;
    }

    ((datamanager*)thisdatamanager)->dataItems[image_index]->save_to_VTK_file(chooser.value(1));
	pt_config::write("latest_path",path_parent(chooser.value(1)));
}


void datamanager::save_hist_callback(Fl_Widget *callingwidget, void * thisdatamanager)
{
    datawidget_base * the_datawidget=(datawidget_base *)(callingwidget->user_data());
    int image_index=((datamanager*)thisdatamanager)->find_data_index(the_datawidget->get_data_id());

	string last_path = pt_config::read<string>("latest_path");
	Fl_File_Chooser chooser(last_path.c_str(),"Histogram text file (*.txt)",Fl_File_Chooser::CREATE,"Save Histogram");
    chooser.ok_label("Save") ;
    chooser.preview(false); 
    chooser.show();
    while(chooser.shown())
        { Fl::wait(); }

    if ( chooser.value() == NULL )
    {
        fprintf(stderr, "(User hit 'Cancel')\n");
        return;
    }

    ((datamanager*)thisdatamanager)->dataItems[image_index]->save_histogram_to_txt_file(chooser.value(1));
	pt_config::write("latest_path",path_parent(chooser.value(1)));
}

void datamanager::add_hist_callback(Fl_Widget *callingwidget, void * thisdatamanager)
{
    datawidget_base * the_datawidget=(datawidget_base *)(callingwidget->user_data());
    int image_index=((datamanager*)thisdatamanager)->find_data_index(the_datawidget->get_data_id());
	((datamanager*)thisdatamanager)->dataItems[image_index]->make_histogram_curve();
}



void datamanager::add(image_base *im, string name, bool data_changed, string slc_orient)
{
    if(im != NULL){
        if(dataItems.size() < DATA_VECTOR_MAX){
            if(find_data_index(im->get_id()) == -1){
				if(name!=""){
					im->name(name);
				}
				if(slc_orient!=""){
					im->set_slice_orientation(slc_orient);
				}

				dataItems.push_back(im);
				im->activate();
                viewmanagement.show_in_empty_viewport(im->get_id());
				data_vector_has_changed();
                data_has_changed(im->get_id(),data_changed);
				refresh_datawidgets(); //JKJK
			}else{
				pt_error::error("Trying to re-add image ID ("+im->name()+")",pt_error::warning);
			}
		}else{
            //This error condition should really never happen, if it does there is
            //reason to rethink the dependency on a fixed image capacity the way DATA_VECTOR_MAX works
            pt_error::error("Error when adding image: number of data items in datamanager exceeds DATA_VECTOR_MAX ("+im->name()+")",pt_error::fatal);
        }

	}else{
		pt_error::error("Error when adding image: image_base* = NULL... ",pt_error::debug);
	}
}

/* Added support for curve */
void datamanager::add(curve_base *curve, string name){
	
	pt_error::error_if_false(dataItems.size() < DATA_VECTOR_MAX,"Error when adding curve: number of data items in datamanager exceeds IMAGEVECTORMAX",pt_error::fatal);
    
    if(pt_error::error_if_null(curve,"Can't add curve to datamanager, pointer is NULL",pt_error::serious) != NULL){
		if(name != "")
			curve->name("name");
        dataItems.push_back(curve);
        curve->activate();
		//viewmanagement.show_in_empty_viewport(curve->get_id()); anvander image_base specifika saker i nulaget...
        
        data_vector_has_changed();
        data_has_changed(curve->get_id());
	}
}


void datamanager::add(point_collection *p)
{
    pt_error::error_if_false(dataItems.size() < DATA_VECTOR_MAX,"Error when adding point: number of data items in datamanager exceeds IMAGEVECTORMAX",pt_error::fatal);
    
    if(pt_error::error_if_null(p,"Can't add point to datamanager, pointer is NULL",pt_error::serious) != NULL){
        dataItems.push_back(p);
        p->activate();
        
        data_vector_has_changed();
        data_has_changed(p->get_id());
	}
}

void datamanager::delete_data(data_base *d)
{
    for(vector<data_base*>::iterator itr=dataItems.begin();itr != dataItems.end();){
        if(*itr == d){
            delete *itr; //the data_base destructor calls remove_data() to
                         //remove it from dataItems 
			itr=dataItems.begin(); //SO
            //break;
		}else{
			itr++;
		}
	}
}

void datamanager::delete_data(int id)
{
	// pt_error::error("datamanager::delete_data",pt_error::debug);
    for(vector<data_base*>::iterator itr=dataItems.begin();itr != dataItems.end();){
        if((*itr)->get_id() == id){
            delete *itr;
			itr=dataItems.begin(); //SO
		}else{
			itr++;
		}
	}
}

void datamanager::delete_all()
{
    for(vector<data_base*>::iterator itr=dataItems.begin();itr != dataItems.end();){
		delete *itr; //the data_base destructor calls remove_data() to remove it from dataItems 
		itr=dataItems.begin(); //SO - needed for VS 2008
	}
}

void datamanager::delete_all_if_true(bool b)
{
	if(b){
		this->delete_all();
	}
}


// Use delete_data() to remove data (data_base::~data_base() calls remove_data() after the allocated memory is removed)
void datamanager::remove_data(int id)
{
    for(vector<data_base*>::iterator itr=dataItems.begin();itr != dataItems.end();){
        if( (*itr)->get_id() == id ){
            itr = dataItems.erase(itr); 
            data_vector_has_changed();
            //break;
		}else{
			itr++;
		}
	}
}

// Use delete_data() to remove data (data_base::~data_base() calls remove_data() after the allocated memory is removed)
void datamanager::remove_data(data_base *d)
{
    for(vector<data_base*>::iterator itr=dataItems.begin();itr != dataItems.end();){
        if(*itr == d){
			itr = dataItems.erase(itr); 
            data_vector_has_changed();
            //break;
		}else{
			itr++;
		}
	}
}

string datamanager::get_data_name(int ID)
{
    data_base * d = get_data(ID);
    
    if (pt_error::error_if_null(d,"Attempt to get name from NULL data object",pt_error::serious) != NULL)
        { return d->name();}
    
    string error_string="(get_data_name error)";
    return error_string;
}

const char* datamanager::get_data_name_ptr(int ID)
{
    data_base * d = get_data(ID);
    
    if (pt_error::error_if_null(d,"Attempt to get name from NULL data object",pt_error::serious) != NULL)
	{ return d->name_ptr();}
    
    return NULL;
}

void datamanager::set_image_name(int ID,string n)
{
    int index=find_data_index(ID);
    if (index >=0)
    {
        dataItems[index]->name(n);
        data_vector_has_changed();
    }
}

void datamanager::dcm_import_callback(Fl_Widget *callingwidget, void *thisdatamanager)
// argument must tell us which instance, if multiple
{
#ifdef TESTMODE
	FLTK_dcmimportwin::create(50,50,1200,800,DCM_FILES,"pt_settings_dcm_file_import_tags.csv","Dicom File Import");
#endif
}

void datamanager::dcm_series_callback(Fl_Widget *callingwidget, void *thisdatamanager)
// argument must tell us which instance, if multiple
{
#ifdef TESTMODE
	FLTK_dcmimportwin::create(50,50,1200,800,DCM_SERIES,"pt_settings_dcm_series_import_tags.csv","Dicom Series Import");
#endif
}

void datamanager::clear_data_list_button_callback(Fl_Widget *callingwidget, void *thisdatamanager)
{
	((datamanager*)thisdatamanager)->delete_all();
}

void datamanager::loadimage_callback(Fl_Widget *callingwidget, void *thisdatamanager)
// argument must tell us which instance, if multiple
{
    ((datamanager*)thisdatamanager)->loadimages();	
}


void datamanager::loadimages() // argument must tell us which instance, if multiple
{
	string last_path = pt_config::read<string>("latest_path");
	//cout<<"last_path="<<last_path<<endl;
	Fl_File_Chooser chooser(last_path.c_str(),"Any file - raw (*)\tDICOM image file (*.dcm)\tVisualization Toolkit image (*.vtk)\tAnalyze .hdr image (*.hdr)\tAnalyze .obj image (*.obj)\tNifTi file (*.nii)",Fl_File_Chooser::MULTI,"Load DICOM/VTK/Analyze/NifTi/Raw image");
	//cout<<"last_path="<<last_path<<endl;

    chooser.show();

    while(chooser.shown())
        { Fl::wait(); }

    if ( chooser.value() == NULL )
        {
        pt_error::error("Image load dialog cancel",pt_error::notice);

        return;
        }
    //Load user's choice

    vector<string> files;

    for ( int t=1; t<=chooser.count(); t++ )
        {
        files.push_back(std::string(chooser.value(t)));
        }


    if (!files.empty())
        {  
        image_base::load(files);
		pt_config::write("latest_path",path_parent(files[0]));
        }
}


//JK geometry information will be difficult to import, as files can be chosen arbitraryly...
//Set the geometry info to "default" (i.e no rotation, origin (0,0,0), scaling (1,1,1) )

void datamanager::load_dcm_import_vector(std::vector<std::string> dcm_filenames, std::string import_vol_name, DCM_IMPORT_WIN_TYPE win_type)
{
	if(dcm_filenames.size()>0){
		//JK Warning... there is some error in the selected slices / the slice order...
		cout<<"load_dcm_import_vector..."<<endl;
		for(int i=0;i<dcm_filenames.size();i++){
			cout<<"i="<<i<<" "<<dcm_filenames[i]<<endl;
		}

		if(win_type == DCM_FILES){
			dicomloader dl = dicomloader(&dcm_filenames,DCM_LOAD_ALL);
			datamanagement.add(dl.read());

		}else if(win_type == DCM_SERIES){
			image_base::load(dcm_filenames);
//			dicomloader dl = dicomloader(&dcm_filenames,DCM_LOAD_SERIES_ID_ONLY);
//			datamanagement.add(dl.read());
		}

	}
}



int datamanager::create_empty_image(int x, int y, int z, int unit) // argument must tell us which instance, if multiple
{
    image_base *animage = NULL;
    
    switch (unit)
        {
        case VOLDATA_CHAR:      animage=new image_integer<char>(); break;
        case VOLDATA_UCHAR:     animage=new image_integer<unsigned char>(); break;
        case VOLDATA_SHORT:     animage=new image_integer<short>(); break;
        case VOLDATA_USHORT:    animage=new image_integer<unsigned short>(); break;
        case VOLDATA_DOUBLE:    animage=new image_scalar<double>(); break;
        default: pt_error::error("Unsupported data type",pt_error::serious);
        }

    animage->initialize_dataset(x,y,z);

    add(animage);

    return animage->get_id();
}

int datamanager::create_empty_image(image_base * blueprint, imageDataType unit)
    {
    image_base *animage = NULL;

    animage = blueprint->alike(unit);

    //no initialize_dataset, relevant information (except image) is taken
    //from blueprint

    add(animage);

    return animage->get_id();
    }

void datamanager::print_dataItems()
    {
    for (unsigned int i=0; i < dataItems.size(); i++)
        { cout << *dataItems[i] << endl; } 
    }

int datamanager::find_data_index(int uniqueID)
    {
    for (unsigned int i=0; i < dataItems.size(); i++)
        {
        if (*dataItems[i]==uniqueID)
            { return i; }
        }
    return -1; // not found
    }

data_base * datamanager::get_data (int ID)
{
    for(vector<data_base*>::iterator itr=dataItems.begin();itr != dataItems.end();itr++){
        if(**itr == ID){
            return *itr;
		}
	}
//	cout<<"datamanager::get_data("<<ID<<") --> NULL"<<endl;
	return NULL;//Pretty important line...
}

//Denna ar definierad i H filen pga template
/*template<class T>
T* datamanager::get_image (int ID)
    {
    vector<data_base*>::iterator itr=dataItems.begin();

    while (itr != dataItems.end())
        {
        if (**itr == ID)
            {
            T * i = dynamic_cast<T *>(*itr); //TODO_R andra denna rad och return tpe
            
            if (pt_error::error_if_null(i,"Trying to get_image when requested ID is not image type",pt_error::fatal) != NULL)
                { return i; }
        }
        itr++;
    }

    return NULL;
}*/
int datamanager::last_image()
    {
	if(dataItems.size()==0)
		return 0;
    return dataItems.back()->get_id();
    }

void datamanager::FLTK_running(bool running)
	{
	closing_program = !running;
	}

bool datamanager::FLTK_running()
    {
    return !closing_program;
    }

void datamanager::add_datawidget(datawidget_base *data_widget)
{
    //add FLTK widget belonging to datawidget object to the list

    data_widget->resize(data_widget_box->interior->x(),data_widget_box->interior->y(),data_widget_box->interior->w(),data_widget->h());

	data_widget_box->interior->add(data_widget);
	data_widget_box->interior->redraw(); //JKJK
    refresh_datawidgets();
}

void datamanager::refresh_datawidgets()
{
//	cout<<"datamanager::refresh_datawidgets()"<<endl;

	int x = data_widget_box->interior->x();
	int y = data_widget_box->interior->y();
	int w = data_widget_box->interior->w();
//	int h = data_widget_box->interior->h();
	int h = 0;
	for (unsigned int i=0; i < dataItems.size(); i++){
		h += dataItems[i]->get_widget_height();
	}
	data_widget_box->interior->resize(x,y,w,h);
	data_widget_box->interior->redraw();
	
//	x = data_widget_box->x();
//	y = data_widget_box->y();
//	w = data_widget_box->w();
//	data_widget_box->resize(x,y,w,h);

	data_widget_box->redraw();
}

void datamanager::remove_datawidget(datawidget_base * the_fl_widget)
{
    //remove FLTK widget belonging to datawidget object from list
    //the datawidget itself is deleted by its owner data_base

    the_fl_widget->hide();

    data_widget_box->interior->remove(the_fl_widget);
    }

void datamanager::data_has_changed (int dataID, bool recalibrate)
    {
    //when thumbnails are implemented, they should be re-rendered at this point
    get_data(dataID)->data_has_changed(recalibrate);

    vector<int> changed_combinations=rendermanagement.combinations_from_data(dataID);
    vector<int>::iterator citr=changed_combinations.begin();

    citr=changed_combinations.begin();
    while (citr != changed_combinations.end())
        {
        viewmanagement.refresh_viewports_from_combination (*citr);
        citr++;
        }
    }

void datamanager::data_vector_has_changed()
{
	if ( FLTK_running() ) // i.e. the application is not closing
	{	
		rebuild_objects_menu();
		rendermanagement.data_vector_has_changed();
		userIOmanagement.data_vector_has_changed();
	}
}

template <class OCLASS>
Fl_Menu_Item* datamanager::object_menu()
{    
    std::vector<OCLASS *> objects;
    std::vector<data_base *>::iterator itr = dataItems.begin();
    
	//Plan: lagg till vilkor som hamtar om den stodjs av aktuell rendrerare
	//dynamic_cast<data_base*>(ptr).is_supported(aktuell rendrerare);

	while(itr != dataItems.end()){
        OCLASS * ptr = dynamic_cast<OCLASS *>(*itr); 		
        if(ptr != NULL)
            {objects.push_back(ptr);}
        itr++;
    }
    

    Fl_Menu_Item *newMenu = new Fl_Menu_Item[objects.size()+1];

	typename std::vector<OCLASS *>::iterator oitr = objects.begin();
    int m=0;
    while(oitr != objects.end()){
        init_fl_menu_item(newMenu[m]);
        newMenu[m].callback((Fl_Callback *)NULL,0);
        newMenu[m].argument((long)(*oitr)->get_id());

		string labelstring=datamanagement.get_data_name((*oitr)->get_id());
		char * menulabel=strdup(labelstring.c_str());
		//const char * menulabel = datamanagement.get_data_name_ptr((*oitr)->get_id());
		//cout<<"menulabel="<<menulabel<<endl;

        newMenu[m].label(menulabel);
        m++;
        oitr++;
	}
    
    //terminate menu
    init_fl_menu_item(newMenu[m]);
    newMenu[m].label(NULL);
    return newMenu;
}

void datamanager::rebuild_point_menu()
{
if (point_menu != NULL)
    { 
    delete point_menu; 
    point_menu = NULL;
    }
    point_menu = object_menu<point_collection>();
}

void datamanager::rebuild_objects_menu()
{
//	cout<<"datamanager::rebuild_objects_menu()"<<endl;
    if (objects_menu != NULL)
        { 
        delete objects_menu; 
        objects_menu = NULL;
        }
    objects_menu = object_menu<data_base>();
    
    rebuild_point_menu();
    rebuild_image_menu();
}

void datamanager::rebuild_image_menu()
{
    if (image_menu != NULL)
        { 
        delete image_menu; 
        image_menu = NULL;
        }
    image_menu = object_menu<image_base>();
}

const Fl_Menu_Item * datamanager::FLTK_image_menu() const
{
    return image_menu;
}

const Fl_Menu_Item * datamanager::FLTK_objects_menu() const
{
    return objects_menu;
}

const Fl_Menu_Item * datamanager::FLTK_point_menu() const
{
    return point_menu;
}
