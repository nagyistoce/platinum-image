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

#define __rawimporter_cc__
#include "rawimporter.h"

#include <sstream>
#include <iostream>

#include "fileutils.h"
#include "datamanager.h"
extern datamanager datamanagement;
#include "image_integer.h"

Fl_Menu_Item menu_voxeltypemenu[] = {
 {"Bool (1-bit)", 0,  0, (void*)(VOLDATA_BOOL), 1, FL_NORMAL_LABEL, 0, 12, 0},
 {"Char (8-bit)", 0,  0, (void*)(VOLDATA_UCHAR), 0, FL_NORMAL_LABEL, 0, 12, 0},
 {"Short (16-bit)", 0,  0, (void*)(VOLDATA_USHORT), 0, FL_NORMAL_LABEL, 0, 12, 0},
 {"Long (32-bit)", 0,  0, (void*)(VOLDATA_ULONG), 0, FL_NORMAL_LABEL, 0, 12, 0},
 {"Double (32-bit float)", 0,  0, (void*)(VOLDATA_DOUBLE), 0, FL_NORMAL_LABEL, 0, 12, 0},
 {0,0,0,0,0,0,0,0,0}
};


enum {
    VOXELMENU_BOOL=0,
    VOXELMENU_CHAR,
    VOXELMENU_SHORT,
    VOXELMENU_LONG,
    VOXELMENU_DOUBLE
    };

// *** Helpers ***

unsigned short voxel_byte_size (imageDataType v)
{
    switch (v)
        {
        case VOLDATA_DOUBLE:
            return (sizeof (double));
            break;
        case VOLDATA_LONG:
        case VOLDATA_ULONG:
            return (sizeof (long));
            break;
        case VOLDATA_SHORT:
        case VOLDATA_USHORT:
            return (sizeof (short));
            break;
        case VOLDATA_CHAR:
        case VOLDATA_UCHAR:
            return (sizeof (char));
            break;
        default:
            {
                //suppress GCC enum warning
            }
        }
    
    return 0;
}

//templated string to num conversion

template <class T>
bool dec_from_string(T& outType, const std::string& inString)
    {
    std::istringstream convstream(inString);

    return !(convstream >> std::dec >> outType).fail();
    }

void dec_to_stringvalue (Fl_Input* i, long n)
    {
    char s [256];

    sprintf (  s,"%li",n);
    i->value(s);
    i->position(i->size());
    }

template <class WidgetType>
void ease_field (WidgetType * w)
    {
    mark_field(w,false);
    }

template <class WidgetType>
void mark_field (WidgetType * w,bool markIt = true)
    {
    if (markIt)
        {
        w->textcolor(FL_RED);
        }
    else
        {
        w->textcolor(FL_FOREGROUND_COLOR);
        }
    }
        
void rawimporter::update_fields (Fl_Widget* w)
    {
    //in argument w is which widget has been changed, to recalculate others based on it
    
    // *** calculate field values ***
    if (w == imagesizex || w == imagesizey || w == voxeltypemenu || w == headersizefield || w == guessnumz)
    {
        switch (mode)
        {
            case multifile:
                
                if (w != headersizefield)
                    {
                    imageSize[2] = (files.size()-sliceStart+1) / sliceIncrement;
                    //imagenumz->value( imageSize[2]);
                    }

                break;
                
            case single_file:
            {
                unsigned long d = imageSize[0] * imageSize[1] * voxel_byte_size (voxeltype);
                
                if (d > 0)
                    {
                    imageSize[2] = (fileSize - headerSize) / d;
                    ease_field(imagenumz);
                    }
                else
                    {
                    mark_field(imagenumz);
                    imageSize[2] = 1;
                    }
            }
                break;
            default:
                {
                    //suppress GCC enum warning
                }
        }
    }
    
    if (w == guessvoxeltype)
        {
        unsigned long dataSize = (imageSize[0] * imageSize[1] );
        
        if (mode == single_file)
            { dataSize *= imageSize[2];}
        
        imageDataType allegedType = VOLDATA_UNDEFINED;
        bool match = false;
        
        while (!match)
            {
            switch (allegedType)   //step through possible data sizes in descending order
                {
                case VOLDATA_UNDEFINED:
                    allegedType = VOLDATA_DOUBLE;
                    break;
                case VOLDATA_DOUBLE:
                    allegedType = VOLDATA_ULONG;
                    break;
                case VOLDATA_ULONG:
                    allegedType = VOLDATA_USHORT;
                    break;
                case VOLDATA_USHORT:
                    allegedType = VOLDATA_UCHAR;
                    break;
                    //case VOLDATA_CHAR:
                    //                    allegedType = VOLDATA_BOOL;
                    //                    break;
                default:
                    //no match
                    match = true;
                    mark_field(voxeltypemenu);
                }
            
            if (dataSize * voxel_byte_size(allegedType) <= fileSize)
                {
                match = true;
                voxeltype = allegedType;
                
                //"guess" header size as well
                headerSize = fileSize - dataSize * voxel_byte_size(voxeltype);

                ease_field (voxeltypemenu);
                }            
            }
        }
    else
        {
        ease_field(voxeltypemenu);
        }

    if (w == guessheadersize)
        {
        unsigned long dataSize = (imageSize[0] * imageSize[1] * voxel_byte_size (voxeltype));

        if (mode == single_file)
            { dataSize *= imageSize[2];}

        if (dataSize <= fileSize)
            {
            //valid inputs, "guess" header size
            headerSize = fileSize - dataSize;
            ease_field(headersizefield);
            }
        else
            {
            headerSize = 0;
            mark_field(headersizefield);
            }
        }
    else
        {
        ease_field(headersizefield);       
        }

    // *** set field values ***
    imagesizex->value(imageSize[0]);
    imagesizey->value(imageSize[1]);
    imagenumz->value(imageSize[2]);

    //voxeltype
    switch (voxeltype)
        {
        case VOLDATA_LONG:
            signedbtn->value (1);
            signedbtn->activate();
            voxeltypemenu->value(VOXELMENU_LONG);
            break;

        case VOLDATA_ULONG:
            signedbtn->value (0);
            signedbtn->activate();
            voxeltypemenu->value(VOXELMENU_LONG);
            break;

        case VOLDATA_SHORT:
             signedbtn->value (1);
            signedbtn->activate();
            voxeltypemenu->value(VOXELMENU_SHORT);
            break;
        case VOLDATA_USHORT:
            signedbtn->value (0);
            signedbtn->activate();
            voxeltypemenu->value(VOXELMENU_SHORT);
            break;

        case VOLDATA_CHAR:
            signedbtn->value (1);
            signedbtn->activate();
            voxeltypemenu->value(VOXELMENU_CHAR);
            break;
        case VOLDATA_UCHAR:
            signedbtn->value (0);
            signedbtn->activate();
            voxeltypemenu->value(VOXELMENU_CHAR);
            break;

        case VOLDATA_BOOL:
            voxeltypemenu->value(VOXELMENU_BOOL);
            signedbtn->value (0);
            signedbtn->deactivate();
            break;

        case VOLDATA_DOUBLE:
            voxeltypemenu->value(VOXELMENU_DOUBLE);
            signedbtn->value (1);
            signedbtn->deactivate();
            break;

        default: {}
        }

    //Endian
    endianbigbtn->value(bigEndian);
    endianlittlebtn->value(!bigEndian);

    //Header size
    dec_to_stringvalue(headersizefield,headerSize);


    //Interleave settings
    startfield->value(sliceStart);
    incrementfield->value(sliceIncrement);
    }

rawimporter::rawimporter(std::vector<std::string> in_files) : Fl_Window ( 478, 307)
    {
    //assumption: all files are from same directory
    //this can be made here because the file list originates from a FLTK file chooser,
    //it can NOT be made in image_general's raw constructor however, since that may be called
    //by applications

    files=in_files;
    mode = undefined;
    imageSize[0] = 1;
    imageSize[1] = 1;
    imageSize[2] = 0;
    sliceStart = 1;
    sliceIncrement = 1;
    headerSize = 0;
    bigEndian = false;
    voxeltype = VOLDATA_UCHAR;

    struct stat firstFileStats;

    if (stat(files.front().c_str(), &firstFileStats) == 0)
        {
        fileSize = firstFileStats.st_size;

        if (files.size() > 1)
            {
            mode = multifile;
            imageSize[2] = files.size();
            }
        else
            {mode = single_file;}
        }
    else
        { fileSize = 0; }

    //set label to directory name (or file name for single file/multislice)  
    std::string dirname = files.front();

    std::string windowLabel = "Importing raw from \"";

    windowLabel.append (path_end (dirname).c_str());
    windowLabel.append ("\"");
    label (windowLabel.c_str());

    // *** begin FLUID widget declarations ***

    box(FL_FLAT_BOX);
    color(FL_BACKGROUND_COLOR);
    selection_color(FL_BACKGROUND_COLOR);
    labeltype(FL_NO_LABEL);
    labelfont(0);
    labelsize(12);
    labelcolor(FL_FOREGROUND_COLOR);
    align(FL_ALIGN_TOP);
    when(FL_WHEN_RELEASE);
        { Fl_Value_Input* o = imagesizex = new Fl_Value_Input(95, 16, 70, 25, "Image x size");
        o->labelsize(10);
        o->minimum(1);
        o->maximum(65535);
        o->step(1);
        o->value(1);
        o->textsize(12);
        o->callback((Fl_Callback*)cb_field_changed);
        o->when(3);
        }
        { Fl_Box* o = new Fl_Box(335, 19, 128, 128, "Preview");
        o->box(FL_DOWN_BOX);
        o->color(FL_FOREGROUND_COLOR);
        o->labelsize(10);
        o->align(FL_ALIGN_TOP);
        o->deactivate();
        }
        { Fl_Value_Input* o = imagesizey = new Fl_Value_Input(95, 41, 70, 25, "Image y size");
        o->labelsize(10);
        o->minimum(1);
        o->maximum(65535);
        o->step(1);
        o->value(1);
        o->textsize(12);
        o->callback((Fl_Callback*)cb_field_changed);
        o->when(3);
        }
        { Fl_Group* o = voxeltypegroup = new Fl_Group(96, 67, 228, 98, "Voxel type");
        o->tooltip("For interleaved series: where to start and what to skip");
        o->box(FL_THIN_DOWN_FRAME);
        o->labelsize(10);
        o->align(FL_ALIGN_LEFT);
            { Fl_Choice* o = voxeltypemenu = new Fl_Choice(103, 73, 136, 25);
            o->down_box(FL_BORDER_BOX);
            o->labelsize(10);
            o->textsize(12);
            o->callback((Fl_Callback*)cb_field_changed);
            o->when(FL_WHEN_CHANGED);
            o->menu(menu_voxeltypemenu);
            }
            { Fl_Button* o = guessvoxeltype = new Fl_Button(239, 78, 17, 15, "@<");
            o->box(FL_NO_BOX);
            o->labelsize(9);
            o->callback((Fl_Callback*)cb_field_changed);
            }
            { Fl_Check_Button* o = signedbtn = new Fl_Check_Button(256, 73, 65, 25, "Signed");
            o->down_box(FL_DOWN_BOX);
            o->labelsize(12);
            o->when(FL_WHEN_NEVER);
            }
            { Fl_Group* o = new Fl_Group(103, 116, 156, 43, "Byte order");
            o->box(FL_ENGRAVED_FRAME);
            o->labelsize(10);
                { Fl_Round_Button* o = endianbigbtn = new Fl_Round_Button(108, 117, 85, 25, "Big endian");
                o->type(102);
                o->down_box(FL_ROUND_DOWN_BOX);
                o->labelsize(12);
                o->when(FL_WHEN_NEVER);
                }
                { Fl_Round_Button* o = endianlittlebtn = new Fl_Round_Button(108, 136, 151, 23, "Little-endian (Intel)");
                o->type(102);
                o->down_box(FL_ROUND_DOWN_BOX);
                o->labelsize(12);
                o->when(FL_WHEN_NEVER);
                }
            o->end();
            }
        o->end();
        }
        { Fl_Group* o = headersizegroup = new Fl_Group(30, 166, 152, 25);
        o->labelsize(11);
        o->when(FL_WHEN_NEVER);
            { Fl_Input* o = headersizefield = new Fl_Input(95, 166, 70, 25, "Header size");
            o->tooltip("Size of each file\'s header (in bytes)");
            o->type(2);
            o->labelsize(10);
            o->textsize(12);
            o->callback((Fl_Callback*)cb_field_changed);
            o->when(FL_WHEN_CHANGED);
            }
            { Fl_Button* o = guessheadersize = new Fl_Button(165, 172, 17, 15, "@<");
            o->box(FL_NO_BOX);
            o->labelsize(9);
            o->callback((Fl_Callback*)cb_field_changed);
            }
        o->end();
        }
        { Fl_Group* o = series_group = new Fl_Group(95, 192, 189, 65, "Series settings");
        o->tooltip("For interleaved series: where to start and what to skip");
        o->box(FL_THIN_DOWN_FRAME);
        o->labelsize(10);
        o->align(FL_ALIGN_LEFT);
            { Fl_Group* o = numzgroup = new Fl_Group(101, 197, 183, 26);
            o->labelsize(11);
            o->when(FL_WHEN_NEVER);
                { Fl_Value_Input* o = imagenumz = new Fl_Value_Input(189, 198, 70, 25, "Number of images");
                o->tooltip("Number of slices");
                o->labelsize(10);
                o->minimum(1);
                o->maximum(65535);
                o->step(1);
                o->value(1);
                o->textsize(12);
                o->callback((Fl_Callback*)cb_field_changed);
                o->when(FL_WHEN_NEVER);
                }
                { Fl_Button* o = guessnumz = new Fl_Button(259, 205, 17, 12, "@<");
                o->box(FL_NO_BOX);
                o->labelsize(9);
                o->callback((Fl_Callback*)cb_field_changed);
                o->hide();
                o->deactivate();
                }
            o->end();
            }
            { Fl_Group* o = interleave_group = new Fl_Group(101, 230, 155, 20);
            { Fl_Spinner* o = startfield = new Fl_Spinner(128, 230, 35, 20, "Start");
            o->labelsize(10);
            o->textsize(12);
            }
            { Fl_Spinner* o = incrementfield = new Fl_Spinner(221, 230, 35, 20, "Increment");
            o->labelsize(10);
            o->textsize(12);
            }
        o->end();
            }
        o->end();
        }
        { Fl_Group* o = new Fl_Group(95, 258, 189, 32, "Voxel aspect ratio");
        o->tooltip("Proportions for voxel size (no scale, will be normalized)");
        o->box(FL_THIN_DOWN_FRAME);
        o->labelsize(10);
        o->align(FL_ALIGN_LEFT);
            { Fl_Input* o = voxsizex = new Fl_Input(111, 264, 35, 20, "x");
            o->type(1);
            o->labelsize(10);
            o->textsize(12);
            o->when(FL_WHEN_NEVER);
            o->value ("1");
            }
            { Fl_Input* o = voxsizey = new Fl_Input(165, 264, 35, 20, "y");
            o->type(1);
            o->labelsize(10);
            o->textsize(12);
            o->when(FL_WHEN_NEVER);
            o->value ("1");
            }
            { Fl_Input* o = voxsizez = new Fl_Input(219, 264, 35, 20, "z");
            o->type(1);
            o->labelsize(10);
            o->textsize(12);
            o->when(FL_WHEN_NEVER);
            o->value ("4");
            }
        o->end();
        }
        { Fl_Button* o = rawimportcancel = new Fl_Button(306, 265, 60, 25, "Cancel");
        o->callback((Fl_Callback*)cb_rawimportcancel);
        }
        { Fl_Return_Button* o = rawimportok = new Fl_Return_Button(376, 265, 84, 25, "Import");
        o->callback((Fl_Callback*)cb_rawimportok);
        }
    end();

    // *** end FLUID widget declarations ***

    if (mode == multifile)
        {numzgroup->deactivate();}

    if (mode == single_file)
        {startfield->parent()->deactivate();}

    update_fields (NULL);

    show();
    }

template <class intype, class outtype>
    static void raw_convert (intype* &inpointer, outtype* &outpointer, long numVoxels, bool bigEndian)
    {
    for (; numVoxels >= 0; numVoxels-- )
        {
        (*outpointer) = static_cast<outtype>(*inpointer);
        //do endian conversion here
        inpointer++;
        outpointer++;
        }
    }

void rawimporter::get_input ()    {
    //Voxeltype
    voxeltype = (imageDataType)voxeltypemenu->mvalue()->argument();

    //Signedness
    if (signedbtn->active() && signedbtn->value() == 1)
        switch (voxeltype)
        {
            case VOLDATA_ULONG:
                voxeltype = VOLDATA_LONG;
                break;
            case VOLDATA_USHORT:
                voxeltype = VOLDATA_SHORT;
                break;
            case VOLDATA_UCHAR:
                voxeltype = VOLDATA_CHAR;
                break;
            default: {}
        }

    //Endian
    bigEndian = endianbigbtn->value() == 1 ? true : false;

    //Header size
    dec_from_string(headerSize, headersizefield->value());

    //Interleave settings
    sliceStart = startfield->value();
    sliceIncrement = incrementfield->value();

    //Image size
    imageSize[0] = imagesizex->value();
    imageSize[1] = imagesizey->value();
    imageSize[2] = imagenumz->value();        
    }

void rawimporter::cb_field_changed(Fl_Widget* o, void*) {
    rawimporter * importwindow = (rawimporter *) o->window();

    importwindow->get_input();
    importwindow->update_fields(o);
    }

void rawimporter::cb_rawimportcancel(Fl_Button* o, void*) {
    Fl_Window * importwindow = o->window();

    importwindow->hide();
    Fl::delete_widget(importwindow);
    }

void rawimporter::cb_rawimportok(Fl_Return_Button* o, void*)
    {
    rawimporter * importer =(rawimporter *) o->window();

    // *** copy input & check integrity ***

    importer->get_input();

    //Voxel size
    Vector3D voxel_aspect;
    dec_from_string(voxel_aspect[0], importer->voxsizex->value());
    dec_from_string(voxel_aspect[1], importer->voxsizey->value());
    dec_from_string(voxel_aspect[2], importer->voxsizez->value());

    //voxel_aspect.Normalize();
    min_normalize (voxel_aspect);

    image_base * new_image = NULL;

    //Do import
    //single-slice volumes can be imported, but the assumption
    //is that the user wants a 3D volume anyway
    switch (importer->voxeltype)
        {  
        case VOLDATA_DOUBLE:
            new_image = new image_scalar<double> (importer->files, importer->imageSize[0], importer->imageSize[1], importer->bigEndian, importer->headerSize, voxel_aspect);
            break;

        case VOLDATA_USHORT:
            new_image = new image_integer<unsigned short> (importer->files, importer->imageSize[0], importer->imageSize[1], importer->bigEndian, importer->headerSize, voxel_aspect);
            break;

        case VOLDATA_SHORT:
            new_image = new image_integer<signed short> (importer->files, importer->imageSize[0], importer->imageSize[1], importer->bigEndian, importer->headerSize, voxel_aspect);
            break;

        case VOLDATA_UCHAR:
            new_image = new image_integer<unsigned char> (importer->files, importer->imageSize[0], importer->imageSize[1], importer->bigEndian, importer->headerSize, voxel_aspect);
            break;
        default:
            {
                //suppress GCC enum warning
            }
        }

    if (new_image != NULL)
        {datamanagement.add(new_image);}

    importer->hide();
    Fl::delete_widget(importer);
    }