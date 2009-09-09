//////////////////////////////////////////////////////////////////////////
//
//   metadata $Revision:$
///
///  helper class for handling (reading, storage, copying, printing) of meta data.
///
//   $LastChangedBy: $
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

#ifndef __metadata__
#define __metadata__

#include <iostream>
#include "string.h"
#include "error.h"
#include "itkGDCMImageIO.h"	//DCM meta data reading
#include "global.h"			//DCM_XXX - definitions
#include "ptmath.h"			//string to num conversion


using namespace std;

class base
{
public:
	virtual void get(int &v, string key_info)
	{throw pt_error("metadata base --> " + type_name() + " did not match key: "+key_info, pt_error::serious);}
	virtual void get(float &v, string key_info)
	{throw pt_error("metadata base --> " + type_name() + " did not match key: "+key_info, pt_error::serious);}
	virtual void get(string &v, string key_info)
	{throw pt_error("metadata base --> " + type_name() + " did not match key: "+key_info, pt_error::serious);}

	virtual string type_name() = 0;
	
	virtual string get_data_as_string ( std::string key )  =  0;
};

class base_int : public base
{
public:
	base_int(int v){value = v;};
	int value;
	void get(int &v, string key_info){	v = value;}
	
	string type_name(){return "INT";}
	
	std::string get_data_as_string ( std::string key );
};

class base_float : public base
{
public:
	base_float(float v){value = v;};
	float value;
	void get(float &v, string key_info){	v = value;}
	
	string type_name(){return "FLOAT";}

	std::string get_data_as_string ( std::string key );
};

class base_string : public base
{
public:
	base_string(string v){value = v;};
	string value;
	void get(string &v, string key_info){v = value;}
	
	string type_name(){return "STRING";}

	std::string get_data_as_string ( std::string key );
};




class metadata
{
public:
	metadata();
	metadata(metadata* const source);
//	~metadata();

	//a metadata map of: string key -- base*
	map<string,base*> meta_map;				
	
	//public functions for adding meta data of different types...
	//overwrites excisting data...
	void set_data_int(string key, int data);		
	void set_data_float(string key, float data);
	void set_data_string(string key, string data);

	//public functions for accessing meta data of different types...
	int get_data_int(string key);
	float get_data_float(string key);
	string get_data_string(string key);


	metadata& operator=(const metadata& source);                

/*	friend std::ostream &operator<<(std::ostream &ut, const data_base &k)
	{
		ut<<"metadata:"<<std::endl;
		// "\"" << k.name() << "\" (ID " << k.ID << ")";
		return ut;
	}
*/

	void read_metadata_from_dcm_file(string dcm_file);

//	float get_dx(); //already stored in image_base (voxel_size)
	float get_flip();
	float get_te();
	
	void print_all();
	
	string metadata::get_data_as_string ( std::string key );

	std::string get_name();
	std::string get_slice_orientation();
	float get_b_value();
	void set_b_value(float b);


private:
	void add_dcm_data_int(itk::GDCMImageIO::Pointer dcmIO, string DCM_TAG);
	void add_dcm_data_float(itk::GDCMImageIO::Pointer dcmIO, string DCM_TAG);
	void add_dcm_data_string(itk::GDCMImageIO::Pointer dcmIO, string DCM_TAG);
	void print_int(string key);
	void print_float(string key);
	void print_string(string key);

	int get_dcm_parameter_as_int(itk::GDCMImageIO::Pointer dcmIO, string DCM_TAG_STRING);
	float get_dcm_parameter_as_float(itk::GDCMImageIO::Pointer dcmIO, string DCM_TAG_STRING);
	string get_dcm_parameter_as_string(itk::GDCMImageIO::Pointer dcmIO, string DCM_TAG_STRING);
};

#endif