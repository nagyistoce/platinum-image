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

#ifndef __brainops__
#define __braindops__
#include <vector>
#include "ptconfig.h"
#include "ptmath.h"
#include "image_binary.hxx"
#include "string.h"
#include "datamanager.h"
#include "filters.h"

using namespace std;

class brainops{
    private:
	public:
		static void remove_bg(vector<image_scalar<unsigned short,3>* > brain);
		static image_binary<3>* get_CSF(vector<image_scalar<unsigned short,3>* > brain);
		static void bridge_burner(vector<image_scalar<unsigned short,3>* > brain, float t_min = 0.53, float t_max = 1.35, float t_grad = 0.36, float p = 2.7, float g = 6.4);
		static void morph(vector<image_scalar<unsigned short,3>* > brain, float t_min = 0.53, float t_max = 1.35);
		static void PLRG(vector<image_scalar<unsigned short,3>* > brain){};
};

#endif __brainops__