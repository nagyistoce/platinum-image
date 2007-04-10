//////////////////////////////////////////////////////////////////////////
//
//  Image_multi
//
//  Abstract image class for data types with multiple values per voxel,
//  such as vector, complex and RGB
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

#ifndef __image_multi__
#define __image_multi__

#include "image_general.h"

//#include <complex>

template<class ELEMTYPE, int IMAGEDIM = 3>
class image_multi : public image_general <ELEMTYPE, IMAGEDIM>
    {
    //redundant declaration of constructor, since those cannot be inherited
    public:
        image_multi ();
        image_multi (std::vector<std::string>, long , long , bool = false, long  = 0, Vector3D  = Vector3D (1,1,4), unsigned int = 1,unsigned int = 1);
    };

// templates in standard C++ requires entire class definition in header
#ifndef __image_multi__hxx__
#define __image_multi__hxx__
#include "image_multi.hxx"
#endif

#endif
