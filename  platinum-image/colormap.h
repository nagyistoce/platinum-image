//////////////////////////////////////////////////////////////////////////
//
//  Colormap
//
//  Interpolated color map class.
//
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

#ifndef __colormap__
#define __colormap__

#include "color.h"

#include <iostream>
//#include <vector>
#include <map>

#include "global.h"

////
// lagrar en samling fargnoder. Varden daremellan interpoleras fram.
//
class colormap
    {
    private:
        std::map<float,colornode> colors;

	public:
		colormap();
		colormap(std::vector<colornode> c);
        void set_color(float position, IMGELEMCOMPTYPE r, IMGELEMCOMPTYPE g, IMGELEMCOMPTYPE b);    // glom inte att sortera/stoppa in pa ratt plats
        void get_color(float position, IMGELEMCOMPTYPE &r, IMGELEMCOMPTYPE &g, IMGELEMCOMPTYPE &b); //TODO: returns RGB at the interpolated position
                                                                    // - no use to return an instance of color
        void remove_color(float position);
        void initialize_colortable();  // always set colors at -oo, oo  in addition to user input
		void print_all();
    };

#endif
