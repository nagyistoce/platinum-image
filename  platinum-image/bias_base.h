//////////////////////////////////////////////////////////////////////////
//
//  bias_base.h $Revision $
//
//  Untemplated base class for bias field handling.
//  Smoothly varying bias fields are commonly seen in MRI data.
//  bias_base -- bias_general<DIM> -- bias_poly<DIM> //for bias fields based on one polynomial
//                                 -- bias_spline<DIM> //for bias fields based on splines
//
//  $LastChangedBy $
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

#ifndef __bias_base__
#define __bias_base__

#include "data_base.h"


//#include "global.h"
//#include "color.h"
//#include "itkVTKImageIO.h"
//#include "bruker.h"

//Take note: the purpose of the base class is to allow access to instances in a
//template-unaware fashion. Hence the base class cannot be templated.
/*
class bias_base : public data_base
//class bias_base
    {
    private:

    protected:
//        bias_base();
        void redraw();
 
    public:
        bias_base();
//        bias_base(bias_base* const b);
		static vector<RENDERER_TYPE> get_supported_renderers();
//        virtual void data_has_changed(bool stats_refresh = true) = 0;
        virtual void data_has_changed(bool stats_refresh = true);
        virtual void save_to_DCM_file(const std::string, const bool useCompression = true, const bool anonymize = true);
        virtual void save_to_VTK_file(const std::string, const bool useCompression = true);

    };
*/

#endif