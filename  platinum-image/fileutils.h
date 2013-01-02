//////////////////////////////////////////////////////////////////////////
//
//  FileUtils
//
//  File & directory functions
//	String handling
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

#ifndef __fileutils__
#define __fileutils__

#include <string>
#include <vector>
#include <sys/stat.h>
#include <sstream>

#include "itkGDCMImageIO.h"
#include <itksys/SystemTools.hxx>  //JK -- allows for example creation of new folder...
#include "global.h"

using namespace std;

#define MAXPATHLENGTH 512

//S_ISDIR is not defined in MSVC
#ifndef S_ISDIR
#define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
#endif


void ensure_trailing_slash(string &s);  //! ensures an accurate directory path, adds a trailing slash ( / ) if there wasn't one already
void remove_trailing_slash(string &s);  //


//------------- File and directory handling ----------------------

vector<string> get_dir_entries(string path, bool full_path=false, bool use_recursion=false);    //return string vector listing directory contents
vector<string> get_files_in_dir(string path, bool full_path=false, bool use_recursion=false);    //return string vector listing directory contents

vector<string> get_dir_entries_ending_with(string path, string ending);    //return string vector listing directory contents that end with (ending)

string path_parent(string);                         //!get parent (full path except file/indicated dir)
string path_end(string);                            //!get file/dir name pointed to without path
vector<string> subdirs(string dir_path, bool fullpath=true, bool use_recursion=false);
vector<string> subdirs_where_name_contains(string dir_path, string name_substring, bool fullpath=true, bool use_recursion=false);
vector<string> get_files_in_dir_where_name_contains(string dir_path, string name_substring, bool fullpath=true, bool use_recursion=false);

bool file_exists(string file_path);   //! return whether file exists. //! NOTE: returns false for existing directory
bool dir_exists(string file_path);                              
void create_dir(string dir_path);
void create_dir_if_not_already_existing(string dir_path);
void create_file(string file_path);
void copy_dir(string from_path, string to_path, bool always=true);
void copy_file(string from_path, string to_path, bool always=true);
void copy_files(vector<string> from_paths, vector<string> to_paths, bool always=true);
void copy_files(vector<string> from_paths, string to_dir_path, bool always=true);
void remove_dir(string dir_path);
void remove_file(string file_path);
void move_file(string from_path, string to_path, bool always=true);
void move_files(vector<string> from_paths, string to_dir_path, bool always=true);


string find_first_file_where_filename_contains(string dir_path, string substring);

unsigned long get_file_size(string filepath);
string get_first_line_from_file_containing(string file_path, string target, int &resulting_line_number);
string get_line_with_number(string file_path, int resulting_line_number);
string get_line_with_offset_from_first_line_containing(string file_path, string target, int line_offset=1);

//------------- Advanced file and directory handling ----------------------
void read_text_file_and_create_subfolders_in_dir(string file_path, string target_dir_base);


//------------- String vector specific handling ----------------------

void add_to_string_vector_if_not_present(vector<string> &v, string s);
bool combinations_equal(vector<string> tag_combo_1, vector<string> tag_combo_2);
vector<string> remove_strings_that_contain_any_of_these(vector<string> v, vector<string> samples);
int does_string_vector_contain_item_containing(vector<string> v, string substring);


//------------- Dicom specific file handling ----------------------

struct TagValueDirInfo{ //SO
	string firstDir;
	int similarDirs;
};

bool			is_dicom_file(string path);
vector<string>	get_dicom_files_in_dir(string dir_path, bool full_path=false, bool recursive_search=false);
vector<string>	get_dicom_files_in_dir(string dir_path, vector<string> dcm_files, bool full_path=false, bool recursive_search=true);
vector<string>	get_first_dicom_files_in_all_subdirs(string dir_path, bool full_path=true);
string			get_first_dicom_file_in_dir(string dir_path, bool full_path=false);
string			get_second_dicom_file_in_dir(string dir_path, bool full_path=false); //can for example be used for loading image 2 in dual echo sequences...
vector<string>	get_dicom_files_in_dir(string dir_path, int first, int every_no, bool full_path=true);

vector<string>	get_dicom_files_with_dcm_tag_value(vector<string> files, string dcm_tag, string tag_val);
int				get_number_of_dicom_files_in_dir(string dir_path);
string			get_dicom_tag_value(string file_path, string dcm_tag, bool remove_garbage_char=true);
bool			does_dir_contain_dcmfile_with_tag_value(string dir_path, string dcm_tag, string tag_val, bool recursive_search=false);
bool			does_first_dcmfile_in_dir_contain(string dir_path, string dcm_tag, string tag_val);
string			find_first_sub_dir_containing_dcm_file_with_tag_value(string dir_path, string dcm_tag, string tag_val, bool recursive_search=false);
vector<string>	find_sub_dirs_containing_dcm_files_with_tag_value(string dir_path, string dcm_tag, string tag_val, bool recursive_search=false, bool check_only_first_file=true);
TagValueDirInfo get_number_of_dirs_and_first_sub_dir_containing_dcm_file_with_tag_value(string dir_path, string dcm_tag, string tag_val); //SO
vector<string>	list_dicom_tag_values_for_this_ref_tag_value(vector<string> files, string dcm_tag, string dcm_tag_val, string dcm_ref_tag);
vector<string>	list_dicom_tag_values_in_dir(string dir_path, string dcm_tag, bool recursive_search=false, bool exclusive_values=false);

vector<string>	get_top_folders_where_first_dicom_file_contains(string dir_path, string dcm_tag, string dcm_tag_val);


bool			does_dicom_file_tag_contain(string file_path, string tag, string content);
bool			is_dicom_file_magnitude_image(string file_path);
bool			is_dicom_file_real_image(string file_path);
bool			is_dicom_file_imaginary_image(string file_path);
string			get_elemtype_in_image_file(string file_path);
string			get_elemtype_in_dicom_file(string file_path);

bool			is_vtk_file(string path);
string			get_elemtype_in_vtk_file(string file_path);



//------  Dicom tag combos -------
vector<string>	get_dicom_tag_value_combination(string file_path, vector<string> dcm_tags, bool remove_garbage_char=true);
vector<string>	get_dicom_files_with_dcm_tag_value_combos(vector<string> files, vector<string> dcm_tags, vector<string> tag_vals, bool remove_garbage_char=true);
vector<vector<string> >	get_header_combinations_from_these_dicom_files(vector<string> dcm_files, vector<string> tag_combo);
vector<vector<string> >	get_header_combinations_from_these_dicom_files_sort_files(string dir_path, bool recursive_search, vector<string> tag_combo, vector<vector<string> > &sorted_files);
vector<vector<string> >	get_header_combinations_from_these_dicom_files_sort_files(vector<string> dcm_files, vector<string> tag_combo, vector<vector<string> > &sorted_files);
//vector<string>	get_header_combinations_from_dicom_files_in_dir(string dir_path, bool use_folder_recursion=false, vector<string> res2=vector<string>())
vector<string>	get_first_dicom_files_corresponding_to_these_combos(string dir_path, vector<string> dcm_tags, vector<vector<string> > combos, bool recursive_search=false, bool full_path=true);
vector<string>	get_first_dicom_files_corresponding_to_these_combos2(string dir_path, vector<string> dcm_tags, bool recursive_search=false, bool full_path=true);

void save_all_dicom_series_to_specific_file_format(string dir_path, vector<string> tag_combo, bool use_recursive_search=true, string format = ".dcm");
void save_all_dicom_series_to_VTK_files(string dir_path, vector<string> tag_combo, bool use_recursive_search=true);
void save_all_dicom_series_to_DCM_files(string dir_path, vector<string> tag_combo, bool use_recursive_search=true);

//------------- String handling functions ----------------------

bool does_string_start_with(string s, string start);
bool does_string_end_with(string s, string ending);
bool remove_file_lastname(string &s, int max_no_lastname_chars=3);
bool remove_string_ending(string &s, string ending=" ");
string replace_last_substring(string s, string val, string replacement="");
string replace_substrings(string s, string val, string replacement="");
string make_string_folder_compatible(string s);
string get_csv_item(string s, int item_num, string separator="\t"); //item_num=0 is the first...
bool string_contains(string s, string sample);

void get_vector_of_substrings_separated_by(string s, string separator, vector<string> &v);
vector<string> get_vector_of_substrings_separated_by(string s, string separator = "\t");

string int2str(int i);
string float2str(float f);

template <class T>
string float2str(complex<T> f){
	return "(" + templ_to_string(f.real()) + "," + templ_to_string(f.imag()) + ")";
}

template <class ELEMTYPE >
string templ_to_string (ELEMTYPE t)
    {
    ostringstream output;
	output.flags( ios::boolalpha | ios::dec );
    output.fill(' ');

    //a true templated function would *not* cast to float,
    //instead use a specialization for bool and unsigned char,
    //however that causes a problem with the current structure (see below)
    output << static_cast<float>(t);

    return output.str();
    }

//template specialization seems to be treated as regular function body,
//and creates a link error for being multiply defined here

/*template <>
string templ_to_string (unsigned char t)
    {
    ostringstream output;
    output.flags( ios::boolalpha | ios::dec );
    output.fill(' ');

    output << static_cast<unsigned char>(t);

    return output.str();
    }*/


// split a delimited string into multiple strings
// example: "aaa;bbb;ccc" -> "aaa" "bbb" "ccc"
void split(const std::string & s, char c, std::vector<std::string> & v);

template <class ELEMTYPE>
vector<ELEMTYPE> read_file_into_data_vector(string file_path)
{
	vector<ELEMTYPE> data;

	ifstream myfile(file_path.c_str());

	if(myfile.is_open()){
		int i=0;
		ELEMTYPE sh;

		while(!myfile.eof()){
			myfile.read( (char *)(&sh), sizeof(sh) );
			data.push_back(sh);
			i++;
		}
	}
	myfile.close();
	return data;
}

template <class ELEMTYPE>
vector<ELEMTYPE> read_file_into_data_vector_binary(string file_path, int num_data_points)
{
	vector<ELEMTYPE> data;

	ELEMTYPE *buffer = new ELEMTYPE[num_data_points];
	for(int i=0;i<num_data_points;i++){
		buffer[i]=0;
	}

	for(int i=0;i<10;i++){
		cout<<buffer[i]<<" ";
	}
	cout<<endl;
	cout<<endl;

	FILE *input = fopen(file_path.c_str(), "rb");
	fread( (void*)buffer, sizeof(ELEMTYPE), num_data_points, input);

	for(int i=0;i<num_data_points;i++){
		data.push_back(buffer[i]);
		cout<<buffer[i]<<" ";
	}

	fclose(input);

	return data;
}

#endif