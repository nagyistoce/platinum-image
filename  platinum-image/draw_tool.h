//////////////////////////////////////////////////////////////////////////
//
//  curve_tool $Revision: $
//
/// The base class for implementing mouse behaviors/actions in a viewport
///
//  $LastChangedBy:$
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


#ifndef __draw_tool__
#define __draw_tool__

#include "viewporttool.h"
#include "userIO.h"


class draw_tool : public nav_tool
{
	protected:
//		static int userIO_ID;
//		static int point_collection_ID;	
		
	public:
		draw_tool(viewport_event &);
		~draw_tool();
		
		static const std::string name();
		static void init();
		virtual void handle(viewport_event &);
		
//		static void register_userIO_ID(int ID);
//		static void register_point_collection_ID(int ID);
	private:
		char draw_type;
		Vector2D start;
		bool drawed;
		vector<Vector2Dint> free;
};

#endif