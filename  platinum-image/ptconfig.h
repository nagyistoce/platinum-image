//////////////////////////////////////////////////////////////////////////
//
//  ptconfig $Revision:$
//
//  Platinum configuration file handling: "last_path", "view_port_setup", "load_this_image_on_startup"
//
//  $LastChangedBy: joel.kullberg $
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

#ifndef __ptconfig__
#define __ptconfig__

#include <string>
#include <fstream>
#include "Utilities/configfile/configfile.h"
#include "fileutils.h"
#include "error.h"

#define DEFAULT_CONFIG_FILE "pt_settings.inp"

using namespace std;

class pt_config{
    private:
//		ConfigFile cf;

	public:
        static void initialize(string filename=DEFAULT_CONFIG_FILE);
		template<class T> 
			static T read(string key, string filename=DEFAULT_CONFIG_FILE);  // call as read<T>
		template<class T> 
			static void write(string key, T value, string filename=DEFAULT_CONFIG_FILE);  // call as read<T>
};


template<class T> 
T pt_config::read(string key, string filename)
{
	ConfigFile cf = ConfigFile(filename, "=", "//");	
	T t;

	if ( cf.keyExists(key) )
		{ t = cf.read<T>(key); }
	else
		{ throw pt_error("pt_config::read(): the key \"" + key + "\" is not found in \"" +  filename + "\"", pt_error::warning); }		
	return t;
}


template<class T> 
void pt_config::write(string key, T value, string filename)
{
	ConfigFile cf = ConfigFile(filename, "=", "//");
	if ( cf.keyExists(key) )
		{ 
			//cout<<"keyExists"<<endl;
			cf.update_value_in_file(filename,key,value); }
	else
	{
		//cout<<"key does not exist..."<<endl;
		cf.add<T>(key, value);
		cf.save_to_file(filename);
	}
}

#endif __ptconfig__