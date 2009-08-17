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

#include "colormap.h"

//
// Hanterar en f‰rgtabell fˆr voxelv‰rden. Interpolerar mellan de 
// f‰rgsamples som finns. Kan omvandla RGB <-> HSV 
// All interpolering mm i HSV (inte RGB)
//

colormap::colormap()
{
	colors = std::map<float,colornode>();
}

colormap::colormap(std::vector<colornode> c)
{
	for(unsigned short i=0;i<c.size();i++){
		colors[c[i].position] = c[i];
	}
	print_all();
}

void colormap::set_color(float position, IMGELEMCOMPTYPE r, IMGELEMCOMPTYPE g, IMGELEMCOMPTYPE b)
{
//	colors.insert(position,);
}

void colormap::get_color(float position, IMGELEMCOMPTYPE &r, IMGELEMCOMPTYPE &g, IMGELEMCOMPTYPE &b){
	print_all();
	std::map<float,colornode>::iterator low = colors.lower_bound( position );
	std::map<float,colornode>::iterator high = colors.upper_bound( position );
	float dist = high->first - low->first;
	r = low->second.r() + (position - low->first)/dist * (high->second.r() - low->second.r());
	g = low->second.g() + (position - low->first)/dist * (high->second.g() - low->second.g());
	b = low->second.b() + (position - low->first)/dist * (high->second.b() - low->second.b());
}
 // returns RGB at the interpolated position


void colormap::print_all()
{
	std::map<float,colornode>::iterator iter;
	for( iter = colors.begin(); iter != colors.end(); ++iter ) {
		std::cout << "Key: '" << iter->first << "', Value: " << iter->second.r() << std::endl; 
	}
}
