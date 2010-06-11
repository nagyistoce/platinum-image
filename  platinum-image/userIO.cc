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

#define __userIO_cc__

#include "userIO.h"

#include <iterator>

#include <FL/Fl.H>

#include "userIOmanager.h"
extern userIOmanager userIOmanagement;

int userIO::new_uIO_ID=1;
const unsigned int userIO::main_button_width= 50;

using namespace std;

int userIO::id()
    {
    return ID;
    }

userIO::userIO(int x_init, int y_init, int w_init, int h_init, string n,userIO_callback* the_callback, std::string ok_label) : Fl_Pack(x_init, y_init,w_init,h_init)
    {
    int button_pos= w()-main_button_width;

    ID=new_uIO_ID++;
    action_callback=the_callback;

    box(FL_DOWN_FRAME);
    align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE);
    labelsize(FLTK_SMALL_LABEL);
	labelfont(FL_HELVETICA_BOLD);

    strcpy(name,n.c_str());
    label(name);

    Fl_Group * headergroup = new Fl_Group(x(),y(),w(),block_header_height);

    if (ok_label.length() > 0)
        {
        OK_button = new Fl_Button(x()+button_pos,y(),main_button_width,BUTTONHEIGHT);
        OK_button->callback(FLTK_callback);
        OK_button->copy_label (ok_label.c_str());
        OK_button->box(FL_UP_BOX);

        button_pos-=main_button_width;
        }
    else
        {
        OK_button = NULL;
        }

	close_button = new Fl_Button(x()+button_pos,y(),main_button_width,BUTTONHEIGHT, "Close");
    close_button->callback(FLTK_callback);
    close_button->box(FL_THIN_UP_BOX);

    headergroup->resizable(NULL);
    headergroup->end();

    resizable (this);
    end();
    }

void userIO::FLTK_callback (Fl_Widget *callingwidget, void * foo)
    {
    userIO * par_group;

    //buttons are enclosed in the headergroup
    par_group=(userIO * )callingwidget->parent()->parent();

    if (par_group->action_callback != NULL)
        {
        if (callingwidget == par_group->close_button)
            {
				par_group->action_callback(par_group->id(),USERIO_CB_CLOSE);
            }
        if (callingwidget == par_group->OK_button)
            {
				par_group->action_callback(par_group->id(),USERIO_CB_OK);
				cout<<"UserIO successfully called!"<<endl;
			}
        }

    if (callingwidget == par_group->close_button)
        {
        //this is how deletion should be done theoretically,
        //first remove and "safe delete" the button,
        //then delete thw whole group (everything but
        //the button)
        /*par_group->close_button->hide();
        par_group->remove(par_group->close_button);
        Fl::delete_widget(par_group->close_button);*/

        userIOmanagement.delete_userIO(par_group->id());
        }
    }

void userIO::data_vector_has_changed()
    {	
    std::vector<FLTKuserIOparameter_base *>::iterator itr=parameters.begin();

    while (itr != parameters.end())
        {
        (**itr).data_vector_has_changed();
        itr++;
        }
    }

void userIO::resize (int newx, int newy, int neww, int newh)
    {
    Fl_Pack::resize (newx, newy, neww, newh);

    if (OK_button != NULL)
        {
        close_button->position(x()+w()-main_button_width*2,y());
        OK_button->position(x()+w()-main_button_width,y());
        }
    else
        {
        close_button->position(x()+w()-main_button_width,y());
        }
    }
    
int userIO::add_par (FLTKuserIOparameter_base * par)
    {
    parameters.push_back(par);

    std::vector<FLTKuserIOparameter_base *>::difference_type dist = std::distance (parameters.begin(), parameters.end());

    //set correct width, and give the parameter widget an approximate y position so Fl_Pack
    //has something to work on when packing the widgets

    par->resize(x(),y()+block_header_height+parameters.back()->y()+parameters.back()->h()+PARTITLEMARGIN, w(), par->h());
    
    par->set_callback(action_callback);
    
    add(parameters.back()); //add to block

    return dist;                                            //return index of last item, which happens
                                                            //to be the same as the vector length,
                                                            //but this is a good principle to use for other cases
    }

int userIO::get_par_num(FLTKuserIOparameter_base * par) 
    {
    std::vector<FLTKuserIOparameter_base *>::iterator itr=parameters.begin();
    int n=0;

    while (itr != parameters.end())
        {
        if (par == *itr)
            { return n; }
        itr++;
        n++;
        }

    return -1;
    }

std::vector<FLTKuserIOpar_histogram2D *> userIO::get_histogram_for_image(int imageID)
    {
    std::vector<FLTKuserIOpar_histogram2D *> result;

    std::vector<FLTKuserIOparameter_base *>::iterator itr=parameters.begin();

    while (itr != parameters.end())
        {
        //match image ID and ensure histogram is active (both images set)
        if (((*itr)->histogram_image_ID(0) == imageID || (*itr)->histogram_image_ID(1) == imageID ) && ((*itr)->histogram_image_ID(0) != NOT_FOUND_ID && (*itr)->histogram_image_ID(1) != NOT_FOUND_ID ))
            {result.push_back((FLTKuserIOpar_histogram2D *)*itr);}

        itr++;
        }
    return result;
    }