// $Id$

///  Window for importing single/multiple dicom images
///	 This file is adapted from "sortapp.cxx" example from The Fl_Table class (Version 3.12, 04/02/2006 )
///  created by Greg Ercolano (http://seriss.com/people/erco/fltk/).

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

#define __dicom_importer_cc__

#include <iostream>

#include "dicom_importer.h"
#include "datamanager.h"
//#include <sstream>

extern datamanager datamanagement;

FLTK_dcmtable::FLTK_dcmtable(int x, int y, int w, int h, DCM_IMPORT_WIN_TYPE type, string settings_file_path, const char *l) : Fl_Table_Row(x,y,w,h,l)
{
	the_file_path = settings_file_path;
	_sort_reverse = 0;
	_sort_lastcol = -1;
	end();
	callback(event_callback, (void*)this);

//	if(!file_exists("dcm_import_tags.csv")){
	if(!file_exists(the_file_path)){

		//		vector<string> row;
		if(type == DCM_FILES){
			dcmtags.add_three_strings_row("Name","0010","0010");
			dcmtags.add_three_strings_row("Study Date","0008","0020");
			dcmtags.add_three_strings_row("Series Date","0008","0021");
			dcmtags.add_three_strings_row("Series ID","0020","000e");
			dcmtags.add_three_strings_row("Modality","0008","0060");
			dcmtags.add_three_strings_row("Image Type","0008","0008");
			dcmtags.add_three_strings_row("Study Descr.","0008","1030");
			dcmtags.add_three_strings_row("Series Descr.","0008","103e");

			dcmtags.add_three_strings_row("Patient ID","0010","0020");
			dcmtags.add_three_strings_row("Birth Date","0010","0030");
			dcmtags.add_three_strings_row("Sex","0010","0040");
			dcmtags.add_three_strings_row("Weight","0010","1030");

			dcmtags.add_three_strings_row("TR","0018","0080");
			dcmtags.add_three_strings_row("TE","0018","0081");
			dcmtags.add_three_strings_row("Flip","0018","1314");
			dcmtags.add_three_strings_row("NSA","0018","0083");
			dcmtags.add_three_strings_row("Slc/Thickn","0018","0050");
			dcmtags.add_three_strings_row("Slc/Space","0018","0088");

			dcmtags.add_three_strings_row("FOV?","0018","1100");
			dcmtags.add_three_strings_row("B0(T)","0018","0087");

			dcmtags.add_three_strings_row("No phase enc.","0018","0089");
			dcmtags.add_three_strings_row("Perc. sampl.","0018","0093");
			dcmtags.add_three_strings_row("RFOV","0018","0094");
			dcmtags.add_three_strings_row("Protocol","0018","1030");

			dcmtags.add_three_strings_row("Transm Coil","0018","1251");
			dcmtags.add_three_strings_row("Rcv Coil","0018","1250");

		}else if(type == DCM_SERIES){
			dcmtags.add_three_strings_row("Series Descr.","0008","103e");
			dcmtags.add_three_strings_row("Study Descr.","0008","1030");
			dcmtags.add_three_strings_row("Name","0010","0010");
			dcmtags.add_three_strings_row("Patient ID","0010","0020");
		}


		dcmtags.write_to_csvfile(the_file_path);

		//The saved file has typically this format...
		//Patient Name;0010;0010;
		//Modality;0008;0060;

	}else{
		dcmtags.read_from_csvfile(the_file_path);
	}
	//	dcmtags.print_all();

	data = stringmatrix(1,dcmtags.rows()+1," ");
	update_tabledata();
}

FLTK_dcmtable::~FLTK_dcmtable()
{ }


// Handle drawing all cells in table
void FLTK_dcmtable::draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H)
{
	//char *s = '';

	switch ( context )
	{
	case CONTEXT_STARTPAGE:
		fl_font(FL_COURIER, 12);
		return;

	case CONTEXT_ROW_HEADER:
		fl_color(FL_RED);
		fl_rectf(X, Y, W, H);
		return;

	case CONTEXT_COL_HEADER:
		fl_push_clip(X, Y, W, H);
		{

			fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, color());
			if ( C < dcmtags.rows() + 1 )
			{
				fl_color(FL_BLACK);
				if(C==0){
					fl_draw("Filename", X+2, Y, W, H, FL_ALIGN_LEFT, 0, 0);	// +2=pad left
				}else{
					fl_draw(dcmtags.get(C-1,0).c_str(), X+2, Y, W, H, FL_ALIGN_LEFT, 0, 0);	// +2=pad left
				}

				// DRAW SORT ARROW
				if ( C == _sort_lastcol )
				{
					int xlft = X+(W-6)-8,
						xctr = X+(W-6)-4,
						xrit = X+(W-6)-0,
						ytop = Y+(H/2)-4,
						ybot = Y+(H/2)+4;

					if ( _sort_reverse )
					{
						// ENGRAVED DOWN ARROW
						fl_color(FL_WHITE);
						fl_line(xrit, ytop, xctr, ybot);
						fl_color(41);	// dark gray
						fl_line(xlft, ytop, xrit, ytop);
						fl_line(xlft, ytop, xctr, ybot);
					}
					else
					{
						// ENGRAVED UP ARROW
						fl_color(FL_WHITE);
						fl_line(xrit, ybot, xctr, ytop);
						fl_line(xrit, ybot, xlft, ybot);
						fl_color(41);	// dark gray
						fl_line(xlft, ybot, xctr, ytop);
					}
				}
			}
		}
		fl_pop_clip();
		return;

	case CONTEXT_CELL:
		{
			fl_push_clip(X, Y, W, H);
			{
				// BG COLOR
				fl_color( row_selected(R) ? selection_color() : FL_WHITE);
				fl_rectf(X, Y, W, H);

				fl_color(FL_BLACK);

				fl_draw(data.get(R,C).c_str(), X+2, Y, W, H, FL_ALIGN_LEFT);	// +2=pad left

				// BORDER
				fl_color(FL_LIGHT2); 
				fl_rect(X, Y, W, H);
			}
			fl_pop_clip();
			return;
		}

	case CONTEXT_ENDPAGE:
	case CONTEXT_RC_RESIZE:
	case CONTEXT_NONE:
	case CONTEXT_TABLE:
		return;
	}
}

// Automatically set column widths to widest data in each column
void FLTK_dcmtable::autowidth(int pad)
{
	fl_font(FL_COURIER, 12);

	// Initialize all column widths to lowest value
	for ( int c=0; c<cols(); c++ )
	{ col_width(c, pad); }

	for ( int r=0; r<(int)data.rows(); r++ )
	{
		int w1, w2, h;
		for ( int c=0; c<(int)data.cols(); c++ )
		{

			//fl_measure(_rowdata[r].words[c].c_str(), w, h, 0);	//let FLTK measure the text w/h
			//w = fl_width(_rowdata[r].words[c].c_str(),_rowdata[r].words[c].size()); //will include "white characters"

			if(c==0){ //compensate for the fact that the first col header is "Filename"
				w1 = fl_width("Filename",8); //will include "white characters"
			}else{
				w1 = fl_width(dcmtags.get(c-1,0).c_str(),dcmtags.get(c-1,0).size()); //will include "white characters"
			}
			w2 = fl_width(data.get(r,c).c_str(),data.get(r,c).size()); //will include "white characters"
			w1 = max(w1,w2);
			h = fl_height();

			if ( (w1 + pad) > col_width(c))
			{ col_width(c, w1 + pad); }					//update w if needed...
		}
	}
	table_resized();
	redraw();
}

void FLTK_dcmtable::print_all()
{
	cout<<"***DATA***"<<endl;
	data.print_all();
	cout<<endl<<"***DCMTAGS***"<<endl;
	dcmtags.print_all();
}


// Callback whenever someone clicks on different parts of the table
void FLTK_dcmtable::event_callback(Fl_Widget*, void *data)
{
	FLTK_dcmtable *o = (FLTK_dcmtable*)data;
	o->event_callback2();
}

void FLTK_dcmtable::event_callback2()
{
	//	int R = callback_row();			// currently unused
	int C = callback_col();

	TableContext context = callback_context();
	switch ( context )
	{
	case CONTEXT_COL_HEADER:		// someone clicked on column header
		{
			if ( Fl::event() == FL_RELEASE && Fl::event_button() == 1 )
			{
				if ( _sort_lastcol == C )
				{ _sort_reverse ^= 1; }	// Click same column? Toggle sort
				else
				{ _sort_reverse = 0; } 	// Click diff column? Up sort 

				//				sort_column(C, _sort_reverse);

				//TODO: JK-note what files are selected...
				selected_filenames.clear();
				for(int r=0;r<data.rows();r++){
					if(row_selected(r)){
						selected_filenames.push_back(data.get(r,0));
						select_row(r,0);
					}
				}

				data.sort_table_using_col(C,_sort_reverse);

				//TODO: JK-select the same files again...
				string s;
				for(int i=0;i<selected_filenames.size();i++){
					for(int r=0;r<data.rows();r++){
						s = data.get(r,0);
						if(selected_filenames[i]==s){
							select_row(r);
						}
					}
				}

				cout<<"Sort...+redraw"<<endl;
				redraw();

				_sort_lastcol = C;
			}
			break;
		}

	default:
		return;
	}
}

void FLTK_dcmtable::fill_table(vector<string> dcm_files)
{
	itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();
	vector<string> dcm_data_row;
	string dcmdata;

	data.clear();
	for(int i=0; i<dcm_files.size(); i++) {
		dcm_data_row.push_back(dcm_files[i]);

		dicomIO->SetFileName(dcm_files[i].c_str());
		dicomIO->ReadImageInformation();		//get basic DICOM header
		for(int j=0; j<dcmtags.rows(); j++) 
		{
			dcmdata="";
			dicomIO->GetValueFromTag(dcmtags.get(j,1)+"|"+dcmtags.get(j,2),dcmdata);
			dcm_data_row.push_back(dcmdata);
		}

		data.add_row(dcm_data_row);
//		data.print_all();
		dcm_data_row.clear();
	}
	update_tabledata();
}

void FLTK_dcmtable::update_tabledata()
{
	dcmtags.read_from_csvfile(the_file_path);

	rows((int)data.rows());
	cols((int)data.cols());

	autowidth(20);	// Auto-calculate widths, with 20 pixel padding
	table_resized();
	redraw();
}

vector<string> FLTK_dcmtable::get_selected_filenames()
{
	vector<string> v;
	for(int r=0;r<data.rows();r++){
		if(row_selected(r)){
			v.push_back(data.get(r,0));
		}
	}
	return v;
}

//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------
void FLTK_dcmimportwin::button_cb(Fl_Button* b, const void* bstring)
{
	FLTK_dcmimportwin *w = (FLTK_dcmimportwin*)b->parent();
	w->button_cb2(string((const char*)bstring));
}

void FLTK_dcmimportwin::button_cb2(string s)
{
	cout<<"("<<s<<")"<<endl;

	if(s=="load"){
		//open file/folder chooser... import selected files/folders (incl "subfolders" if checked...)
		//return as a vector of strings...

		string last_path = pt_config::read<std::string>("latest_path");
		//char * path = fl_file_chooser("Choose a directory", "", 0);
		char * path = fl_dir_chooser("Choose a directory", "", 0);		
		string path2 = string(path);
		if(path2 !=""){
			pt_config::write("latest_path",string(path));
		}

		pt_error::error("dcm_import path="+string(path),pt_error::notice);

		if(path2 != ""){
			cout<<"fl_dir_chooser-->"<<path<<endl;
			cout<<"name-->"<<fl_filename_name(path)<<endl;

//			dcm_file_vector = get_dcm_files_from_dir(path, dcm_file_vector, incl_subfolder_check_button->value());
			vector<string> dcm_file_vector = get_dcm_files_of_interest( path, incl_subfolder_check_button->value() );
			cout<<"***TOTAL NO files = "<<dcm_file_vector.size()<<endl;

			table->fill_table(dcm_file_vector); //for each file... get the dcm info specified in the FLTK_dcmtable-"dcmtags" object... fill the FLTK_dcmtable-"data"
		}


	}else if(s=="settings"){
		FLTK_settingswin::create(50,50,700,800,table,"Settings");
		//		table->update_tabledata();

	}else if(s=="import"){
		load_selected_images();

	}else if(s=="close"){
		this->hide();
	}else{
		pt_error::pt_error("FLTK_dcmimportwin::button_cb2... No matching string",pt_error::debug);
	}
}


vector<string> FLTK_dcmimportwin::get_dcm_files_of_interest(string path, bool include_subfolders)
{
	if(this->the_win_type == DCM_FILES){
//		vector<string> tmp;
//		return get_dcm_files_from_dir(path, tmp, include_subfolders);
		return get_dicom_files_in_dir(path, true, include_subfolders);
	}else if(this->the_win_type == DCM_SERIES){

		vector<string> tag_combo;
		tag_combo.push_back(DCM_PATIENT_NAME);
		tag_combo.push_back(DCM_SERIES_DESCRIPTION);
/*
		vector<string> dcm_files = get_dicom_files_in_dir(path, true, include_subfolders);
		vector<vector<string> >	v = get_header_combinations_from_these_dicom_files(dcm_files, tag_combo);
		cout<<endl;
		for(int i=0; i<v.size();i++){
			for(int j=0; j<v[i].size();j++){
				cout<<v[i][j]<<" ";
			}
			cout<<endl;
		}
		return get_first_dicom_files_corresponding_to_these_combos(path, tag_combo, v, include_subfolders, true);
*/
		return get_first_dicom_files_corresponding_to_these_combos2(path, tag_combo, include_subfolders, true);

	}
	vector<string> tmp;
	return tmp;
}


void FLTK_dcmimportwin::load_selected_images()
{
	datamanagement.load_dcm_import_vector(table->get_selected_filenames(), import_vol_name_input->value(),this->the_win_type);
}



FLTK_dcmimportwin* FLTK_dcmimportwin::create(int xx, int yy, int ww, int hh, DCM_IMPORT_WIN_TYPE type, string settings_file, const char *ll)
{
	Fl_Group::current(NULL);// *Warning* - If this is forgotten, The window/graphics might end up in 
	return new FLTK_dcmimportwin(xx,yy,ww,hh,type,settings_file,ll);
}


FLTK_dcmimportwin::FLTK_dcmimportwin(int xx, int yy, int ww, int hh, DCM_IMPORT_WIN_TYPE type, string settings_file, const char *ll):Fl_Window(xx,yy,ww,hh,ll)
{
	the_win_type = type;
	settings_file_path = settings_file;

	int wm = 10;	//widget margin
	int wh = 30;	//widget height

	table = new FLTK_dcmtable(wm, 2*wm+wh, w()-2*wm, h()-4*wm-2*wh, the_win_type, settings_file_path); //the window type 
	table->selection_color(FL_YELLOW);
	table->col_header(1);
	table->col_resize(1);
	table->when(FL_WHEN_RELEASE);		// handle table events on release
//	table->print_all();
	table->row_height_all(18);			// height of all rows
	table->end();

	Fl_Button* o;
	o = new Fl_Button(wm, wm, 100, wh, "Load directory");
	o->callback((Fl_Callback*)button_cb, (void*)"load");
	//	o->callback((Fl_Callback*)load_button_cb, (void*)this);

	//	Fl_Check_Button* incl_subfolder_check_button = new Fl_Check_Button(100+2*wm, wm, 160, wh, "Include subdirectories");
	incl_subfolder_check_button = new Fl_Check_Button(100+2*wm, wm, 160, wh, "Include subdirectories");
	incl_subfolder_check_button->down_box(FL_DOWN_BOX);
	incl_subfolder_check_button->value(1);


	o = new Fl_Button(wm, h()-wm-wh, 140, wh, "Dicom Tag Settings");
	o->callback((Fl_Callback*)button_cb, (void*)"settings");
	//	o->callback((Fl_Callback*)settings_button_cb, (void*)this);

	//	Fl_Input* import_vol_name_input = new Fl_Input(w()-3*wm-2*65-170, h()-wm-wh, 170, wh, "Import Volume Name");
	import_vol_name_input = new Fl_Input(w()-3*wm-2*65-170, h()-wm-wh, 170, wh, "Import Volume Name");
	import_vol_name_input->box(FL_DOWN_BOX);
	import_vol_name_input->color(FL_BACKGROUND2_COLOR);
	import_vol_name_input->selection_color(FL_SELECTION_COLOR);
	import_vol_name_input->labeltype(FL_NORMAL_LABEL);
	import_vol_name_input->labelfont(0);
	import_vol_name_input->labelsize(14);
	import_vol_name_input->labelcolor(FL_FOREGROUND_COLOR);
	//	import_vol_name_input->callback((Fl_Callback*)volume_name_field_cb, (void*)this);
	import_vol_name_input->align(FL_ALIGN_LEFT);
	import_vol_name_input->when(FL_WHEN_RELEASE);

	import_vol_name_input->value("Dicom import volume");


	o = new Fl_Button(w()-2*wm-2*65, h()-wm-wh, 65, wh, "Import");
	o->callback((Fl_Callback*)button_cb, (void*)"import");
	//	o->callback((Fl_Callback*)import_button_cb, (void*)this);

	o = new Fl_Button(w()-wm-65, h()-wm-wh, 65, wh, "Close");
	o->callback((Fl_Callback*)button_cb, (void*)"close");
	//	o->callback((Fl_Callback*)close_button_cb, (void*)this);

	end();
	resizable(table);
	show();
}


/*
vector<string> FLTK_dcmimportwin::get_dcm_files_from_dir(const char *dir, vector<string> dcm_files, bool incl_sub_dirs)
{
	cout<<"***get_dcm_files_from_dir...="<<dir<<endl;
	struct dirent **files;
	int num_files = fl_filename_list(dir, &files);
	cout<<"num_files="<<num_files<<endl;

	itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();
	string d = string(dir);
	string f;
	string p;

	for(int i=0; i<num_files; i++) {
		f = string(files[i]->d_name);
		p = d + f;
//		cout<<"d="<<d<<" (f="<<f<<")"<<endl;

		if(incl_sub_dirs && fl_filename_isdir(p.c_str()) && f!="../" && f!="./")
		{
			dcm_files = get_dcm_files_from_dir( p.c_str(), dcm_files, incl_sub_dirs);
//			cout<<"*path*"<<endl;
		}
		else if(dicomIO->CanReadFile(p.c_str()))
		{
			dcm_files.push_back(p);
			//				dicomIO->SetFileName(files[i]->d_name);
			//				dicomIO->ReadImageInformation();		//get basic DICOM header
//			cout<<"*dcm*"<<endl;
		}else{
//			cout<<"*none*"<<endl;
		}
		free(files[i]);
	}

	if (num_files > 0) 
		free(files);

	//		cout<<"dcm_files.size()="<<dcm_files.size()<<endl;
	//		for (int i=0; i<dcm_files.size(); i++) {
	//			cout<<dcm_files[i]<<endl;
	//		}
	return dcm_files;
}
*/

//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------

string_edit_table::string_edit_table(int x, int y, int w, int h, const char *l, int r_nr, int c_nr, string settings_file) : Fl_Table(x,y,w,h,l)
{
	settings_file_path = settings_file;
	dcmtags.read_from_csvfile(settings_file_path);

	callback(&event_callback, (void*)this);
	input = new Fl_Input(20,20,50,20);
	input->hide();
	input->callback(input_cb, (void*)this);
	input->when(FL_WHEN_ENTER_KEY_ALWAYS);
	input->maximum_size(150);

	(new Fl_Box(9999,9999,0,0))->hide();  // HACK: prevent flickering in Fl_Scroll
	end();

	update_tabledata();
}

string_edit_table::~string_edit_table()
{}



void string_edit_table::set_value()
{
	dcmtags.set( row_edit,col_edit,string(input->value()) ); 
	input->hide(); 
}

void string_edit_table::read_from_csvfile(string file){
	dcmtags.read_from_csvfile(file);
}

void string_edit_table::write_to_csvfile(string file){
	dcmtags.write_to_csvfile(file);
}

void string_edit_table::update_tabledata()
{
	cols(dcmtags.cols());
	rows(dcmtags.rows());
}

// Handle drawing all cells in table
void string_edit_table::draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H)
{
	static char s[100];

	switch ( context )
	{
	case CONTEXT_COL_HEADER:
		fl_font(FL_HELVETICA | FL_BOLD, 12);
		fl_push_clip(X, Y, W, H);
		{
			fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, col_header_color());
			fl_color(FL_BLACK);

			if(C==0){fl_draw("Title:", X, Y, W, H, FL_ALIGN_CENTER);}
			else if(C==1){fl_draw("Tag Group:", X, Y, W, H, FL_ALIGN_CENTER);}
			else if(C==2){fl_draw("Tag Name:", X, Y, W, H, FL_ALIGN_CENTER);}

		}
		fl_pop_clip();
		return;

	case CONTEXT_ROW_HEADER:
		fl_font(FL_HELVETICA | FL_BOLD, 12);
		fl_push_clip(X, Y, W, H);
		{
			fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, row_header_color());
			fl_color(FL_BLACK);

			sprintf(s, "%d", R);
			fl_draw(s, X, Y, W, H, FL_ALIGN_CENTER);

		}
		fl_pop_clip();
		return;

	case CONTEXT_CELL:
		{
			if (R == row_edit && C == col_edit && input->visible())
			{ return; }

			// BACKGROUND
			fl_push_clip(X, Y, W, H);
			fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, FL_WHITE);
			fl_pop_clip();

			// TEXT
			fl_push_clip(X+3, Y+3, W-6, H-6);
			{
				fl_color(FL_BLACK);

				if (C <dcmtags.cols() && R <dcmtags.rows())
				{
					fl_font(FL_HELVETICA, 12);
					sprintf(s, "%s", dcmtags.get(R,C).c_str());
					fl_draw(s, X+3, Y+3, W-6, H-6, FL_ALIGN_CENTER);
				}
			}
			fl_pop_clip();

			return;
		}

	case CONTEXT_RC_RESIZE:
		{
			if (!input->visible()) return;
			find_cell(CONTEXT_TABLE, row_edit, col_edit, X, Y, W, H);
			if (X==input->x() && Y==input->y() && W==input->w() && H==input->h()) return;
			input->resize(X,Y,W,H);
			return;
		}

	default:
		return;
	}
}


void string_edit_table::event_callback(Fl_Widget*, void *data)
{
	string_edit_table *o = (string_edit_table*)data;
	o->event_callback2();
}

void string_edit_table::event_callback2()
{
	int R = callback_row();
	int C = callback_col();
	TableContext context = callback_context();

	switch ( context )
	{
	case CONTEXT_CELL:
		{
			//			if (C == cols()-1 || R == rows()-1) return;
			if (input->visible()) 
				input->do_callback();
			row_edit = R;
			col_edit = C;

			int XX,YY,WW,HH;
			find_cell(CONTEXT_CELL, R, C, XX, YY, WW, HH);
			input->resize(XX,YY,WW,HH);
			input->value(dcmtags.get(R,C).c_str());
			input->show();
			input->take_focus();
			return;
		}
	default:
		return;
	}
}

void string_edit_table::input_cb(Fl_Widget*, void* v)
{ 
	((string_edit_table*)v)->set_value(); 
}


/*
void input_cb(Fl_Widget*, void* v)
{ 
((string_edit_table*)v)->set_value(); 
}
*/


//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------
FLTK_settingswin* FLTK_settingswin::create(int xx, int yy, int ww, int hh, FLTK_dcmtable *dt, const char *ll)
{
	Fl_Group::current(NULL);// *Warning* - If this is forgotten, The window/graphics might end up in 
	return new FLTK_settingswin(xx,yy,ww,hh,dt,ll);
}

FLTK_settingswin::FLTK_settingswin(int x, int y, int w, int h, FLTK_dcmtable *dt, const char *l):Fl_Window(x,y,w,h,l)
{
	int wm = 10;	//widget margin
	int wh = 30;	//widget height

	FLTK_dcmtable_ptr=dt;

	table = new string_edit_table(wm, wm, w-2*wm, h-3*wm-wh,"",5,3, FLTK_dcmtable_ptr->the_file_path);
	table->read_from_csvfile(FLTK_dcmtable_ptr->the_file_path);


	// ROWS
	table->row_header(1);
	table->row_header_width(70);
	table->row_resize(1);
	table->row_height_all(25);

	// COLS
	table->col_header(1);
	table->col_header_height(25);
	table->col_resize(1);
	table->col_width_all(70);

	Fl_Button* o;
	int y_level = h-wm-wh;
	int x_acc = wm;		//accumulated x-value;

	o = new Fl_Button(x_acc, y_level, 70, wh, "Add Row");
	o->callback((Fl_Callback*)button_cb, (void*)"Add Row");


	x_acc += 3*wm+70;
	Fl_Box *b1 = new Fl_Box(FL_DOWN_BOX,x_acc-7, y_level-5, 167, wh+10, "");
	//----
	o = new Fl_Button(x_acc, y_level, 120, wh, "Insert Row");
	o->callback((Fl_Callback*)button_cb, (void*)"Insert Row");
	ins_row_input = new Fl_Input(x_acc+125, y_level, 30, wh,"");
	ins_row_input->value("0");
	cout<<"ins_row_input->value()="<<ins_row_input->value()<<endl;
	//----

	x_acc += 180;
	Fl_Box *b2 = new Fl_Box(FL_DOWN_BOX,x_acc-7, y_level-5, 167, wh+10, "");
	//----
	o = new Fl_Button(x_acc, y_level, 120, wh, "Delete Row");
	o->callback((Fl_Callback*)button_cb, (void*)"Delete Row");
	del_row_input = new Fl_Input(x_acc+125,y_level, 30, wh,"");
	char tmp[100];
	sprintf(tmp, "%d", max(0,table->rows()-1));
	del_row_input->value(tmp);
	cout<<"del_row_input->value()="<<del_row_input->value()<<endl;
	//----


	o = new Fl_Button(w-120-2*wm, y_level, 60, wh, "Cancel");
	o->callback((Fl_Callback*)button_cb, (void*)"Cancel");

	o = new Fl_Button(w-60-wm, y_level, 60, wh, "OK");
	o->callback((Fl_Callback*)button_cb, (void*)"OK");


	resizable(table);
	show();
	Fl::run();
}

void FLTK_settingswin::button_cb(Fl_Button* b, void* bstring)
{
	FLTK_settingswin *w = (FLTK_settingswin*)b->parent();
	string s = string((const char*)bstring);
	w->button_cb2(s);
}


void FLTK_settingswin::button_cb2(string s)
{
	cout<<"("<<s<<")"<<endl;

	if(s=="Add Row"){
		vector<string> v;
		v.push_back("");
		v.push_back("");
		v.push_back("");
		table->dcmtags.add_row(v);
		update_tabledata();

	}else if(s=="Insert Row"){

		int r = atoi(ins_row_input->value());
		cout<<"Insert Row: r="<<r<<endl;

		if( r>=0 && r < table->dcmtags.rows())
		{
			vector<string> v;
			v.push_back("");
			v.push_back("");
			v.push_back("");
			table->dcmtags.insert_row(r,v);
			update_tabledata();
		}

	}else if(s=="Delete Row"){

		int r = atoi(del_row_input->value());
		cout<<"Delete Row: r="<<r<<endl;

		if( r>=0 && r < table->dcmtags.rows())
		{
			table->dcmtags.remove_row(r);
			update_tabledata();
		}

	}else if(s=="OK"){
		table->write_to_csvfile(FLTK_dcmtable_ptr->the_file_path);
		FLTK_dcmtable_ptr->update_tabledata();
		hide();

	}else if(s=="Cancel"){
		hide();

	}else{
		pt_error::pt_error("FLTK_settingswin::button_cb2... No matching string",pt_error::debug);
	}
}


void FLTK_settingswin::update_tabledata()
{
	table->update_tabledata();
}