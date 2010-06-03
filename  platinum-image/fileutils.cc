//$Id$

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

#include "fileutils.h"

#include <iostream>
#include "image_base.h"				//due to use of dicomloader

#ifdef WIN32
//Windows

#include "dirent.h"

#else
//UNIX
#include <dirent.h>
#endif

void ensure_trailing_slash(string &s)
    {
    if(*s.rbegin() != '/')
        {
        s.append("/");
        }
    }

void remove_trailing_slash(string &s)
	{
    if(*s.rbegin() == '/')
        {
		s = s.substr(0,s.size()-1);
        }
	}

vector<string> get_dir_entries(string path, bool full_path, bool use_recursion)
    {
    // *** POSIX ***

    vector<string> f;
    dirent *ep;
    char cpath[MAXPATHLENGTH];
    strcpy (cpath,path.c_str());

    DIR * dp = opendir (cpath);

	if(full_path){
		while (ep = readdir (dp))
			f.push_back (path + ep->d_name);
	}else{
		while (ep = readdir (dp))
			f.push_back (ep->d_name);
	}

    (void) closedir (dp);

	if(use_recursion){
		vector<string> dirs = subdirs(path, true);
	    vector<string> f2;
		for(int i=0;i<dirs.size();i++){
			f2 = get_dir_entries(dirs[i], true, true);
			for(int j=0;j<f2.size();j++){
				f.push_back(f2[j]);
			}
		}
	}

    return f;
    }

vector<string> get_files_in_dir(string path, bool full_path, bool use_recursion)
{
	vector<string> entries = get_dir_entries(path, true, use_recursion);
	vector<string> files;
	for(int i=0;i<entries.size();i++){
		if(file_exists(entries[i])){
			files.push_back(entries[i]);
		}
	}
	return files;
}

vector<string> get_dir_entries_ending_with(string path, string ending)
    {
    // *** POSIX ***

    vector<string> f;
    dirent *ep;
    char cpath[MAXPATHLENGTH];
    strcpy (cpath,path.c_str());

    DIR * dp = opendir (cpath);

	while (ep = readdir (dp)){
		string res = string(ep->d_name);
		int s = ending.size();
		int from = res.size()-s;
		if(from<0){from=0;}

		string this_ending = res.substr(from,s);
//		cout<<"res="<<res<<endl;
//		cout<<"this_ending="<<this_ending<<endl;
		
		if(this_ending == ending)
		{
			f.push_back (ep->d_name);
		}
	}

    (void) closedir (dp);
    return f;
    }


string path_end (string file_path)
    {
    unsigned int pos;

    pos=file_path.rfind("/",file_path.length()-2);
    
    if (pos !=string::npos)
        {
        file_path.erase(0,pos+1);
        }
    
    return file_path;
  
    }

string path_parent (string file_path)
    {
    unsigned int pos;

	//try "/"
    pos=file_path.rfind("/",file_path.length()-1);

	//try "\" (Windows paths might use these)
	if(pos ==string::npos){
	   pos=file_path.rfind("\\",file_path.length()-1);
	}
    
    if (pos !=string::npos)
        {
        file_path.erase(pos+1,file_path.length());
        }
    
    return file_path;
    }

bool file_exists (string file_path)
{
    struct stat fileStats;
    
    if (stat(file_path.c_str(), &fileStats) == 0)
        {
        if (!S_ISDIR(fileStats.st_mode))
            {return true;}
        else
            {return false;}
        }
    
    return false;

    //return (access(file_path.c_str(), F_OK) == 1);
}

bool dir_exists (string file_path)
{
	//on windows... a trailing slash "/" has to be removed... 
	#if defined _WIN32
		remove_string_ending(file_path, "/");
	#endif

    struct stat fileStats;
    
    if (stat(file_path.c_str(), &fileStats) == 0)
        {
        if (S_ISDIR(fileStats.st_mode))
            {return true;}
        else
            {return false;}
        }
    
    return false;
}

void create_dir(string dir_path)
{
	itksys::SystemTools::MakeDirectory( dir_path.c_str() );
}

void create_file(string file_path)
{
	ofstream myfile;
	myfile.open(file_path.c_str());
	myfile<<"";
	myfile.close();
}

void copy_dir(string from_path, string to_path, bool always)
{
	itksys::SystemTools::CopyADirectory(from_path.c_str(),to_path.c_str(),always); //current files are overwritten
}

void copy_file(string from_path, string to_path, bool always)
{
	itksys::SystemTools::CopyAFile(from_path.c_str(),to_path.c_str(),always); //current file is overwritten
}

void copy_files(vector<string> from_paths, vector<string> to_paths, bool always)
{
	if(from_paths.size()==to_paths.size()){
		
		for(int i=0; i<from_paths.size();i++){
			itksys::SystemTools::CopyAFile(from_paths[i].c_str(),to_paths[i].c_str(),always); //current file is overwritten
		}

	}else{
		cout<<"copy_files-vectors do not have the same size..."<<endl;
	}
}

void copy_files(vector<string> from_paths, string to_dir_path, bool always)
{
	string s="";
	ensure_trailing_slash(to_dir_path);
	for(int i=0; i<from_paths.size();i++){
		cout<<"from: "<<from_paths[i]<<endl;
		s = to_dir_path + path_end(from_paths[i]);
		cout<<"to: "<<s<<endl;
		itksys::SystemTools::CopyAFile(from_paths[i].c_str(),s.c_str(),always); //current file is overwritten
	}
}


void remove_dir(string dir_path)
{
	itksys::SystemTools::RemoveADirectory(dir_path.c_str());
}

void remove_file(string file_path)
{
	itksys::SystemTools::RemoveFile(file_path.c_str());
}

string find_first_file_where_filename_contains(string dir_path, string substring)
{
//	cout<<"find_first_file_where_filename_contains..."<<endl;

	vector<string> all_files = get_dir_entries(dir_path,true);
	int pos;
	for(int i=0; i<all_files.size(); i++){
		pos = all_files[i].find(substring);
//		cout<<i<<" "<<all_files[i]<<" "<<pos<<endl;
		if(pos>0 && pos<all_files[i].size()){
			return all_files[i];
		}
	}
	return "";
}

unsigned long get_file_size(string filepath){
	unsigned long f_length=0, begin, end;
	ifstream myfile(filepath.c_str(),ios::in | ios::binary); //BINARY SOLVED EVERYTHING!!!
	if(myfile.is_open()){
		//Get length of file
		begin = myfile.tellg();
		myfile.seekg(0, ios::end);
		end = myfile.tellg();
		f_length = end-begin;
	}
	return f_length;
}

string get_first_line_from_file_containing(string file_path, string target, int &resulting_line_number){
	ifstream myfile;
	myfile.open(file_path.c_str());


	if(myfile.is_open()){
		char buffer[10000];
		string s;
		int ind;
		resulting_line_number=-1;

		while(!myfile.eof()){
			myfile.getline(buffer,10000);
			resulting_line_number++;
			s=string(buffer);
//			cout<<"s="<<s<<endl;

			ind = s.find(target);
			if(ind>=0 && ind<s.size()){
				return s;
			}
		}
		myfile.close();
	}

	resulting_line_number=-1;
	return "";
}

string get_line_with_number(string file_path, int resulting_line_number){
	ifstream myfile;
	myfile.open(file_path.c_str());
	if(myfile.is_open()){
		char buffer[10000];
		string s;
		int line_number=-1;

		while(!myfile.eof()){
			myfile.getline(buffer,10000);
			line_number++;
			s=string(buffer);
//			cout<<"s="<<s<<endl;

			if(line_number==resulting_line_number){
				return s;
			}
		}
		myfile.close();
	}
	return "";
}

string get_line_with_offset_from_first_line_containing(string file_path, string target, int line_offset){
	int resulting_line_number;
	get_first_line_from_file_containing(file_path, target, resulting_line_number);

	return get_line_with_number(file_path,resulting_line_number+line_offset);;
}


void read_text_file_and_create_subfolders_in_dir(string file_path, string target_dir_base)
{
	ifstream myfile;
	myfile.open(file_path.c_str());

	if(myfile.is_open()){
		char buffer[10000];
		string s;

		while(!myfile.eof()){
			myfile.getline(buffer,10000);
			s=string(buffer);
//			cout<<target_dir_base+"/"+s<<endl;
			create_dir(target_dir_base+"/"+s);
		}
		myfile.close();
	}
}


void add_to_string_vector_if_not_present(vector<string> &v, string s)
{
	if(s!=""){
		bool present=false;
		for(int i=0; i<v.size() && !present; i++){
			if(v[i] == s){
				present=true;
			}
		}
		if(!present){
//			cout<<"******"<<endl;
			v.push_back(s);
		}
	}
}

bool combinations_equal(vector<string> tag_combo_1, vector<string> tag_combo_2)
{
	if(tag_combo_1.size() != tag_combo_2.size()){
		return false;
	}
	for(int i=0; i<tag_combo_1.size();i++){
		if(tag_combo_1[i] != tag_combo_2[i]){
			return false;
		}
	}
	return true;
}

vector<string> remove_strings_that_contain_any_of_these(vector<string> v, vector<string> samples)
{
	vector<string> res;
	bool keep_this = true;
	for(int i=0; i<v.size();i++){
		keep_this = true;
		for(int j=0; j<samples.size() && keep_this; j++){
			keep_this = !string_contains(v[i], samples[j]);
		}
		if(keep_this){
			res.push_back(v[i]);
		}
	}
	return res;
}

int does_string_vector_contain_item_containing(vector<string> v, string substring)
{
	int ind;
	for(int i=0; i<v.size();i++){
		ind = v[i].find(substring);
		if(ind>=0 && ind<v[i].size()){
			return i;
		}
	}
	return -1;
}


//------------- Dicom specific file handling ----------------------

bool is_dicom_file(string path)
{
	itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();
	if(dicomIO->CanReadFile(path.c_str())) {
		return true;
	}

	return false;
}


vector<string> get_dicom_files_in_dir(string dir_path, bool full_path, bool recursive_search)
{
	vector<string> tmp;
	return	get_dicom_files_in_dir(dir_path, tmp, full_path, recursive_search);
}

vector<string>	get_dicom_files_in_dir(string dir_path, vector<string> tmp_dcm_files, bool full_path, bool recursive_search)
{
//	cout<<"get_dicom_files_in_dir..."<<endl;
	vector<string> dcm_files = tmp_dcm_files;
	vector<string> all_files = get_dir_entries(dir_path,true);

	if(recursive_search){
		vector<string> dirs = subdirs(dir_path, true);
		for(int i=0;i<dirs.size();i++)	{
			dcm_files = get_dicom_files_in_dir(dirs[i], dcm_files, true, recursive_search);
		}
	}

	itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();

	for(int i=0;i<all_files.size();i++)	{
//		cout<<"*i="<<i<<" "<<all_files[i]<<endl;
		if(dicomIO->CanReadFile(all_files[i].c_str()))
		{
//			cout<<"****dcm***"<<endl;
			dcm_files.push_back(all_files[i]);
		}
	}
	return dcm_files;
}

vector<string> get_first_dicom_files_in_all_subdirs(string dir_path, bool full_path)
{
//	cout<<"get_first_dicom_files_in_all_subdirs..."<<endl;

	vector<string> all_dcm_files;
	vector<string> sub_dcm_files;
//	vector<string> these_dcm_files = get_dicom_files_in_dir(dir_path, full_path);
	string this_dcm_file = get_first_dicom_file_in_dir(dir_path, full_path);
	

	vector<string>dirs = subdirs(dir_path, true);
	for(int i=0;i<dirs.size();i++)	{
//		cout<<"*dir_i="<<i<<" "<<dirs[i]<<endl;
		sub_dcm_files = get_first_dicom_files_in_all_subdirs(dirs[i], full_path);
		for(int i=0;i<sub_dcm_files.size();i++)	{
			all_dcm_files.push_back( sub_dcm_files[i] );
		}
		sub_dcm_files.clear();
	}

//	for(int i=0;i<these_dcm_files.size();i++)
//	{
//		cout<<"*i="<<i<<" "<<these_dcm_files[i]<<endl;
//		all_dcm_files.push_back(these_dcm_files[i]);
//	}
	all_dcm_files.push_back(this_dcm_file);
	
	return all_dcm_files;
}

string get_first_dicom_file_in_dir(string dir_path, bool full_path)
{
	vector<string> v = get_dicom_files_in_dir(dir_path, full_path);
//	cout<<"v.size()="<<v.size()<<endl;
	if(v.size()>0){
		return v[0];
	}
	return "";
}

string get_second_dicom_file_in_dir(string dir_path, bool full_path)
{
	vector<string> v = get_dicom_files_in_dir(dir_path, full_path);
//	cout<<"v.size()="<<v.size()<<endl;
	if(v.size()>1){
		return v[1];
	}
	return "";
}

vector<string> get_dicom_files_in_dir(string dir_path, int first, int every_no, bool full_path)
{
	vector<string> v = get_dicom_files_in_dir(dir_path, full_path);
	vector<string> selected;
	for(int i=first; i<v.size(); i+=every_no){
		selected.push_back(v[i]);
	}
	cout<<"v.size()="<<v.size()<<endl;
	cout<<"selected.size()="<<selected.size()<<endl;
	return selected;
}

vector<string> get_dicom_files_with_dcm_tag_value(vector<string> files, string dcm_tag, string tag_val)
{
	vector<string> v;
	for(int i=0;i<files.size();i++){
		if( get_dicom_tag_value(files[i], dcm_tag)==tag_val ){
			v.push_back(files[i]);
		}
	}
	return v;
}

int get_number_of_dicom_files_in_dir(string dir_path)
{
return get_dicom_files_in_dir(dir_path).size();
}

string get_dicom_tag_value(string file_path, string dcm_tag, bool remove_garbage_char)
{
//	cout<<"get_dicom_tag_value...("<<file_path<<","<<dcm_tag<<")"<<endl;
	itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();
	string dcmdata = "";

	if(dicomIO->CanReadFile(file_path.c_str()))
	{
		dicomIO->SetFileName(file_path.c_str());
		dicomIO->ReadImageInformation();		//get basic DICOM header
		dicomIO->GetValueFromTag(dcm_tag,dcmdata);
		if(remove_garbage_char){
			remove_string_ending(dcmdata, " ");
		}
//		cout<<"CanReadFile... (dcmdata="<<dcmdata<<")"<<endl;	
	}
	return dcmdata;
}



bool does_dir_contain_dcmfile_with_tag_value(string dir_path, string dcm_tag, string tag_val, bool recursive_search)
{
//	cout<<"does_dir_contain_dcmfile_with_tag_value..."<<endl;

	vector<string> dcm_files = get_dicom_files_in_dir(dir_path, true);
	itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();
	string dcmdata = "";
	if(recursive_search){
		vector<string> dirs = subdirs(dir_path);
		bool result;
		for(int i=0;i<dirs.size();i++){
			result = does_dir_contain_dcmfile_with_tag_value(dirs[i], dcm_tag, tag_val, recursive_search);
			if(result){
				return true;
			}
		}

	}

	for(int i=0;i<dcm_files.size();i++){
		dicomIO->SetFileName(dcm_files[i].c_str());
		dicomIO->ReadImageInformation();		//get basic DICOM header
		dicomIO->GetValueFromTag(dcm_tag,dcmdata);
		remove_string_ending(dcmdata," "); //removes eventual last garbage char
		//cout<<"tag_val=("<<tag_val<<") dcmdata=("<<dcmdata<<")"<<endl;
		if(dcmdata == tag_val){
			return true;
		}
	}
	return false;
}

bool does_first_dcmfile_in_dir_contain(string dir_path, string dcm_tag, string tag_val)
{
//	cout<<"does_first_dcmfile_in_dir_contain...("<<dir_path<<")"<<endl;

	string dcm_file = get_first_dicom_file_in_dir(dir_path, true);

	if(file_exists(dcm_file)){	//the "dcm_file" might of course be empty...
		itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();
		string dcmdata = "";

		dicomIO->SetFileName(dcm_file.c_str());
		dicomIO->ReadImageInformation();		//get basic DICOM header
		dicomIO->GetValueFromTag(dcm_tag,dcmdata);
		remove_string_ending(dcmdata," "); //removes eventual last garbage char
//		cout<<"tag_val=("<<tag_val<<") dcmdata=("<<dcmdata<<")"<<endl;
		if(dcmdata == tag_val){
			return true;
		}
	}
	return false;
}


string find_first_sub_dir_containing_dcm_file_with_tag_value(string dir_path, string dcm_tag, string tag_val, bool recursive_search)
{
//	cout<<"find_first_sub_dir_containing_dcm_file_with_tag_value...("<<dir_path<<")"<<endl;
	vector<string> dirs = subdirs(dir_path);
	string result="";

	for(int i=0;i<dirs.size();i++){
		cout<<"dirs[i]="<<dirs[i]<<endl;
		if(recursive_search){
			result = find_first_sub_dir_containing_dcm_file_with_tag_value(dirs[i], dcm_tag, tag_val, recursive_search);
			if(result != ""){
				return result;
			}
		}
		if(does_dir_contain_dcmfile_with_tag_value(dirs[i],dcm_tag,tag_val,false)){
//			cout<<"***"<<endl;
			return dirs[i];
		}
	}
	return "";
}

vector<string> find_sub_dirs_containing_dcm_files_with_tag_value(string dir_path, string dcm_tag, string tag_val, bool recursive_search, bool check_only_first_file)
{
	vector<string> res;
	vector<string> res2;
	vector<string> dirs = subdirs(dir_path);

	string result="";

	for(int i=0;i<dirs.size();i++){
		//cout<<"dirs[i]="<<dirs[i]<<endl;
		if(recursive_search){
			res2 = find_sub_dirs_containing_dcm_files_with_tag_value(dirs[i], dcm_tag, tag_val, recursive_search, check_only_first_file);
			for(int j=0;j<res2.size();j++){
				res.push_back(res2[j]);
			}
			res2.clear();
		}

		if(check_only_first_file){
			if(does_first_dcmfile_in_dir_contain(dirs[i],dcm_tag,tag_val)){
				res.push_back(dirs[i]);
			}
		}else{
			if(does_dir_contain_dcmfile_with_tag_value(dirs[i],dcm_tag,tag_val,false)){
				//cout<<"***"<<endl;
				res.push_back(dirs[i]);
			}
		}
	}
	return res;
}

//----------------------------------------------work in progress-----/SO------------------
TagValueDirInfo get_number_of_dirs_and_first_sub_dir_containing_dcm_file_with_tag_value(string dir_path, string dcm_tag, string tag_val)
{
	vector<string> dirs = subdirs(dir_path);
	TagValueDirInfo tagVal;
	tagVal.firstDir="";
	tagVal.similarDirs = 0;

	for(int i=0;i<dirs.size();i++)
	{
		cout<<"dirs[i]="<<dirs[i]<<endl;

		if(does_dir_contain_dcmfile_with_tag_value(dirs[i],dcm_tag,tag_val,false))
		{
			tagVal.similarDirs++;

			if (tagVal.firstDir == "")
			{
			tagVal.firstDir = dirs[i];
			}
		}
	}
	cout << "* * * Number of similarDirs = " << tagVal.similarDirs << endl;
	cout << "* * * firstDir = " << tagVal.firstDir << endl;
	return tagVal;
}
//-----------------------------------------------------------------------------------------------------------

vector<string> list_dicom_tag_values_for_this_ref_tag_value(vector<string> files, string dcm_tag, string dcm_tag_val, string dcm_ref_tag)
{
//	cout<<"list_dicom_tag_values_for_this_ref_tag_value..."<<endl;
//	cout<<"dcm_tag="<<dcm_tag<<endl;
//	cout<<"dcm_tag_val=("<<dcm_tag_val<<")"<<endl;
//	cout<<"dcm_ref_tag="<<dcm_ref_tag<<endl;

	//for example dcm_tag = "series ID"
	//for example dcm_tag_val = 1.231434.3432545
	//for example ref_tag_val = "echo time"..

	vector<string> vec;
	vector<string> values_present;
	string tmp;
	string tmp2;

	for(int i=0;i<files.size();i++){
		tmp2 = get_dicom_tag_value(files[i], dcm_tag);
//		cout<<"tmp2=("<<tmp2<<")"<<endl;
		tmp2 = tmp2.c_str();	//remove last garbage char...
//		cout<<"tmp3=("<<tmp2<<")"<<endl;
		if(dcm_tag_val == tmp2)
		{
			tmp = get_dicom_tag_value(files[i], dcm_ref_tag);
//			cout<<"tmp="<<tmp<<endl;
			add_to_string_vector_if_not_present(values_present, tmp);
		}
	}
	return values_present;
}

vector<string> list_dicom_tag_values_in_dir(string dir_path, string dcm_tag, bool recursive_search, bool exclusive_values)
{
	cout<<"list_dicom_tag_values_in_dir("<<dir_path<<") "<<endl;
	vector<string> tags;
	vector<string> tags2;
	if(recursive_search){
		vector<string> dirs = subdirs(dir_path);
		for(int i=0;i<dirs.size();i++){
			tags2 = list_dicom_tag_values_in_dir(dirs[i], dcm_tag, recursive_search, exclusive_values);
			for(int j=0;j<tags2.size();j++){
				if(exclusive_values){
					add_to_string_vector_if_not_present(tags,tags2[j]);
				}else{
					tags.push_back(tags2[j]);
				}
//				cout<<"tags.size()="<<tags.size()<<endl; 
			}
		}
	}

	vector<string> files = get_dicom_files_in_dir(dir_path, true);
	string tmp;
	for(int i=0;i<files.size();i++){
		tmp = get_dicom_tag_value(files[i], dcm_tag);
//		cout<<"tmp="<<tmp<<endl;
		if(exclusive_values){
			add_to_string_vector_if_not_present(tags,tmp);
		}else{
			tags.push_back(tmp);
		}
	}
	return tags;
}

vector<string>	get_top_folders_where_first_dicom_file_contains(string dir_path, string dcm_tag, string dcm_tag_val)
{
	vector<string> first_files = get_first_dicom_files_in_all_subdirs(dir_path, true);
	vector<string> folders;
	string s;
	string val;

	for(int i=0;i<first_files.size();i++){
//		cout<<"first_files="<<first_files[i]<<endl;
		val = get_dicom_tag_value(first_files[i], dcm_tag, true);
//		cout<<"val="<<val<<endl;
		if(val == dcm_tag_val){
			folders.push_back(path_parent(first_files[i]));
//			cout<<"***folders="<<path_parent(first_files[i])<<endl;
		}
	}
	return folders;
}

bool does_dicom_file_tag_contain(string file_path, string dcm_tag, string content)
{
	string tag = get_dicom_tag_value(file_path, dcm_tag, true);
	int pos = tag.find(content);
//	cout<<file_path<<" "<<dcm_tag<<" "<<tag<<" "<<content<<" "<<pos<<" "<<tag.size()<<endl;
	if( pos>=0 && pos<tag.size() ){
		return true;
	}
	return false;
}


bool is_dicom_file_magnitude_image(string file_path)
{
	return does_dicom_file_tag_contain(file_path, DCM_IMAGE_TYPE, DCM_MAGNITUDE_SUBSTRING);
}

bool is_dicom_file_real_image(string file_path)
{
	return does_dicom_file_tag_contain(file_path, DCM_IMAGE_TYPE, DCM_REAL_SUBSTRING);
}

bool is_dicom_file_imaginary_image(string file_path)
{
	return does_dicom_file_tag_contain(file_path, DCM_IMAGE_TYPE, DCM_IMAGINARY_SUBSTRING);
}

string get_elemtype_in_image_file(string file_path)
{
	string ret="";
	if( is_dicom_file(file_path) ){
		ret = get_elemtype_in_dicom_file(file_path);
	}else if( is_vtk_file(file_path) ){
		ret = get_elemtype_in_vtk_file(file_path);
	}

	return ret;
}

string get_elemtype_in_dicom_file(string file_path)
{
    itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();
	string result = "undefined";

	if( dicomIO->CanReadFile(file_path.c_str()) ){
		dicomIO->SetFileName(file_path.c_str());
		dicomIO->ReadImageInformation();	//get basic DICOM header

//		itk::ImageIOBase::IOPixelType pixelType=dicomIO->GetPixelType(); //scalar
		itk::ImageIOBase::IOComponentType componentType = dicomIO->GetComponentType();

		switch(componentType){	//Enumeration values: UCHAR, CHAR, USHORT, SHORT, UINT, INT, ULONG, LONG, FLOAT, DOUBLE
			case itk::ImageIOBase::UCHAR:
				result = "uchar";
			break;
			case itk::ImageIOBase::CHAR:
				result = "char";
			break;
			case itk::ImageIOBase::USHORT:
				result = "ushort";
			break;
			case itk::ImageIOBase::SHORT:
				result = "short";
			break;
			case itk::ImageIOBase::UINT:
				result = "uint";
			break;
			case itk::ImageIOBase::INT:
				result = "int";
			break;
			case itk::ImageIOBase::ULONG:
				result = "ulong";
			break;
			case itk::ImageIOBase::LONG:
				result = "long";
			break;
			case itk::ImageIOBase::FLOAT:
				result = "float";
			break;
			case itk::ImageIOBase::DOUBLE:
				result = "double";
			break;
		}
	}

	return result;
}

bool is_vtk_file(string file_path)
{
    itk::VTKImageIO::Pointer vtkIO = itk::VTKImageIO::New();
	if(vtkIO->CanReadFile(file_path.c_str())){
		return true;
	}
	return false;
}

string get_elemtype_in_vtk_file(string file_path)
{
    itk::VTKImageIO::Pointer vtkIO = itk::VTKImageIO::New();
	string result = "undefined";

	if(vtkIO->CanReadFile(file_path.c_str())){   //Assumption: File contains image data
		vtkIO->SetFileName(file_path.c_str());
		vtkIO->ReadImageInformation();	//get basic DICOM header
        itk::ImageIOBase::IOComponentType componentType = vtkIO->GetComponentType();
        
		switch(componentType){
			case itk::ImageIOBase::UCHAR:
				result = "uchar";
			break;
			case itk::ImageIOBase::CHAR:
				result = "char";
			break;
			case itk::ImageIOBase::USHORT:
				result = "ushort";
			break;
			case itk::ImageIOBase::SHORT:
				result = "short";
			break;
			case itk::ImageIOBase::UINT:
				result = "uint";
			break;
			case itk::ImageIOBase::INT:
				result = "int";
			break;
			case itk::ImageIOBase::ULONG:
				result = "ulong";
			break;
			case itk::ImageIOBase::LONG:
				result = "long";
			break;
			case itk::ImageIOBase::FLOAT:
				result = "float";
			break;
			case itk::ImageIOBase::DOUBLE:
				result = "double";
			break;
		}
	}
	return result;
}

vector<string> get_dicom_tag_value_combination(string file_path, vector<string> dcm_tags, bool remove_garbage_char)
{
//	cout<<"get_dicom_tag_value_combination..."<<endl;
	itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();

	vector<string> res;
	string dcmdata = "";

	if(dicomIO->CanReadFile(file_path.c_str()))
	{
		dicomIO->SetFileName(file_path.c_str());
//		dicomIO->BreakOnError();
		dicomIO->ReadImageInformation();		//get basic DICOM header

		for(int i=0; i<dcm_tags.size();i++){
			dicomIO->GetValueFromTag(dcm_tags[i],dcmdata);
			if(remove_garbage_char){
				remove_string_ending(dcmdata, " ");
			}

			res.push_back(dcmdata);
		}
	}
	return res;
}

vector<string>	get_dicom_files_with_dcm_tag_value_combos(vector<string> files, vector<string> dcm_tags, vector<string> tag_vals, bool remove_garbage_char)
{
	vector<string> res;

	itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();
	string dcmdata = "";
	bool include_file = true;

	for(int i=0; i<files.size();i++){
		if(dicomIO->CanReadFile(files[i].c_str())){

			dicomIO->SetFileName(files[i].c_str());
			dicomIO->ReadImageInformation();		//get basic DICOM header

			include_file = true;
			for(int j=0; include_file&&(j<dcm_tags.size()); j++){
				dicomIO->GetValueFromTag(dcm_tags[j],dcmdata);
				if(remove_garbage_char){
					remove_string_ending(dcmdata, " ");
				}

				if(dcmdata != tag_vals[j]){
					include_file=false;
				}
			}

			if(include_file){
				res.push_back(files[i]);
			}

		}
	}
	return res;
}

vector<vector<string> >	get_header_combinations_from_these_dicom_files(vector<string> dcm_files, vector<string> tag_combo)
{
	vector<vector<string> > combs;
	vector<string> comb;
	bool add_this_combo=true;

	for(int i=0; i<dcm_files.size();i++){
		comb = get_dicom_tag_value_combination(dcm_files[i],tag_combo,true);

		add_this_combo=true;
		
		for(int j=0; add_this_combo&&(j<combs.size()); j++){
			if( combinations_equal(comb,combs[j]) ){
				add_this_combo=false;
			}
		}
		if(add_this_combo){
			combs.push_back(comb);
		}
	}

	return combs;
}

vector<vector<string> >	get_header_combinations_from_these_dicom_files_sort_files(string dir_path, bool recursive_search, vector<string> tag_combo, vector<vector<string> > &sorted_files)
{
	vector<string> files = get_dicom_files_in_dir(dir_path, true, recursive_search);    //return string vector listing directory contents
	cout<<"files.size()="<<files.size()<<endl;
	return get_header_combinations_from_these_dicom_files_sort_files(files, tag_combo, sorted_files);
}


vector<vector<string> >	get_header_combinations_from_these_dicom_files_sort_files(vector<string> dcm_files, vector<string> tag_combo, vector<vector<string> > &sorted_files)
{
	vector<vector<string> > combs;
	vector<string> comb;
	bool add_this_combo=true;

	for(int i=0; i<dcm_files.size();i++){
		comb = get_dicom_tag_value_combination(dcm_files[i],tag_combo,true);

		add_this_combo=true;
		
		for(int j=0; add_this_combo&&(j<combs.size()); j++){
			if( combinations_equal(comb,combs[j]) ){
				add_this_combo=false;
				sorted_files[j].push_back(dcm_files[i]); // (since the j index will be the same for "combs" and for "sorted_files")
			}
		}
		if(add_this_combo){
			combs.push_back(comb);
			vector<string> new_file_vector;
			new_file_vector.push_back(dcm_files[i]); //add first file...
			sorted_files.push_back(new_file_vector);	// --> the j index will be the same for "combs" and for "sorted_files"
		}
	}
	return combs;
}


//-----------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------
/*

//this is called in the recursion
vector<vector<string> >	get_header_combinations_from_dicom_files_in_dir(string dir_path, vector<string> tag_combo, vector<vector<string> > res2, bool use_folder_recursion)
{
	vector<vector<string> > res = res2;

	if(use_folder_recursion){
		vector<string> dirs = subdirs(dir_path, true);
		for(int i=0; i<dirs.size();i++){
			res = get_header_combinations_from_dicom_files_in_dir(dirs[i], tag_combo, res, use_folder_recursion);
		}
	}

	vector<string> dcm_files = get_dicom_files_in_dir(dir_path, true);
	vector<vector<string> > new_combos = get_header_combinations_from_these_dicom_files(dcm_files, tag_combo);

	bool add_this=true;
	for(int i=0; i<new_combos.size();i++){
		add_this=true;
		for(int j=0; add_this&&(j<res.size()); j++){
			if( combinations_equal(new_combos[i],res[j]) ){
				add_this=false;
			}
		}
		if(add_this){
			res.push_back(new_combos[i]);
		}
	}

	return res;
}

//this is called from "outside"
vector<vector<string> >	get_header_combinations_from_dicom_files_in_dir(string dir_path, vector<string> tag_combo, bool use_folder_recursion=false)
{
	vector<vector<string> > tmp;
	return get_header_combinations_from_dicom_files_in_dir(dir_path, tag_combo, tmp, use_folder_recursion); //this is called in the recursion...
}
*/


vector<string>	get_first_dicom_files_corresponding_to_these_combos(string dir_path, vector<string> dcm_tags, vector<vector<string> > combos, bool recursive_search, bool full_path)
{
	vector<string> dcm_files = get_dicom_files_in_dir(dir_path, true, recursive_search);
	vector<string> this_comb;
	vector<string> res;

	for(int i=0; (combos.size()>0) && (i<dcm_files.size()); i++){
		this_comb = get_dicom_tag_value_combination(dcm_files[i], dcm_tags);

		cout<<dcm_files[i]<<endl;
		for(int j=0; j<combos.size() ; j++){
			for(int k=0; k<combos[j].size() ; k++){
				cout<<combos[j][k]<<" ";
			}
			if( combinations_equal(this_comb,combos[j]) ){
				res.push_back(dcm_files[i]);
				combos.erase(combos.begin()+j);
				cout<<"***";
			}
		}
		cout<<endl;
	}

	return res;
}

vector<string>	get_first_dicom_files_corresponding_to_these_combos2(string dir_path, vector<string> dcm_tags, bool recursive_search, bool full_path)
{
	//jk ????
	vector<string> all_files = get_dir_entries(dir_path,true,true);
	vector<vector<string> > found_combos;
	vector<string> this_combo;
	string dcmdata = "";
	bool combo_exists=true;
	vector<string> first_dcm_files;

	itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();

	for(int i=0; i<all_files.size(); i++){
		cout<<"*i="<<i<<" "<<all_files[i]<<endl;
		if( dicomIO->CanReadFile(all_files[i].c_str()) ){
			dicomIO->SetFileName(all_files[i].c_str());
			dicomIO->ReadImageInformation();		//get basic DICOM header

			for(int j=0; j<dcm_tags.size();j++){
				dicomIO->GetValueFromTag(dcm_tags[j],dcmdata);
				remove_string_ending(dcmdata, " ");
				this_combo.push_back(dcmdata);
			}

			combo_exists=false;
			for(int j=0; (!combo_exists)&&(j<found_combos.size()); j++){
				if(combinations_equal(this_combo, found_combos[j])){
					combo_exists=true;
				}
			}

			if(	!combo_exists ){
				found_combos.push_back(this_combo);
				first_dcm_files.push_back(all_files[i]);
			}

			//you might add files that belong to the same combo to some structure here.....
			this_combo.clear();

		}
	}
	return first_dcm_files;
}


void save_all_dicom_series_to_specific_file_format(string dir_path, vector<string> tag_combo, bool use_recursive_search, string format)
{

	vector<string> dcm_files = get_first_dicom_files_corresponding_to_these_combos2(dir_path, tag_combo, use_recursive_search, true);

	image_base* im;
	string this_dcm_file;
	string save_file_name;

	while(dcm_files.size()>0){
		cout<<"********************"<<endl;
		cout<<dcm_files[0]<<endl;
		this_dcm_file = dcm_files[0]; //this needs to be saved since it is "consumed" when the file is read...
		dicomloader dl = dicomloader(&dcm_files, DCM_LOAD_SERIES_ID_ONLY);
		im = dl.read();

		save_file_name = get_dicom_tag_value(this_dcm_file, tag_combo[0]);
		for(int j=1;j<tag_combo.size();j++){
			save_file_name += "_" + get_dicom_tag_value(this_dcm_file, tag_combo[j]);
		}

		cout<<"save_file_name="<<save_file_name<<endl;
		save_file_name = replace_substrings(save_file_name, " ", "_");
		save_file_name = replace_substrings(save_file_name, ":", "_");
		save_file_name = replace_substrings(save_file_name, "\\", "_"); // changes "single" "\" to "_"
		cout<<"save_file_name="<<save_file_name<<endl;

		if(format == ".dcm"){
			im->save_to_DCM_file(dir_path + save_file_name + ".dcm");
		}else if(format == ".vtk"){
			im->save_to_VTK_file(dir_path + save_file_name + ".vtk");
		}else{
			cout<<"save_all_dicom_series_to_specific_file_format... (format not recognized:"<<format<<")"<<endl;
		}

		delete im;
	}
}

void save_all_dicom_series_to_VTK_files(string dir_path, vector<string> tag_combo, bool use_recursive_search){
	save_all_dicom_series_to_specific_file_format(dir_path, tag_combo, use_recursive_search, ".vtk");
}

void save_all_dicom_series_to_DCM_files(string dir_path, vector<string> tag_combo, bool use_recursive_search){
	save_all_dicom_series_to_specific_file_format(dir_path, tag_combo, use_recursive_search, ".dcm");
}


//------------- String handling functions ----------------------

bool does_string_end_with(string s, string ending)
{
	if(s.find_last_of(ending) == s.size()-1)
	{
		return true;
	}
	return false;
}

bool remove_file_lastname(string &s, int max_no_lastname_chars)
{
	unsigned int last = s.find_last_of(".");
//	cout<<"last="<<s.find_last_of(".")<<endl;
//	cout<<"size="<<s.size()<<endl;
	if( last >= (s.size() -max_no_lastname_chars -1) )
	{
		s = s.substr(0,last);
		return true;
	}
	return false;
}

bool remove_string_ending(string &s, string ending)
{
//	cout<<"remove_string_ending"<<endl;
	if(does_string_end_with(s,ending)){
//		cout<<"s.size()="<<s.size()<<endl;
//		cout<<"ending.size()="<<ending.size()<<endl;
//		cout<<"s="<<s<<endl;
		s = s.substr(0,s.size()-ending.size());
//		cout<<"s="<<s<<endl;
		return true;
	}
	return false;
}

string replace_last_substring(string s, string val, string replacement){
//	cout<<"replace_last_substring..."<<endl;
//	cout<<"s="<<s<<endl;
	int last = s.find_last_of(val);
	string s2 = s;
	string a = s.substr(0,last-val.size()+1);
	string b = replacement;
//	cout<<"s2="<<s2<<endl;
	string c = s2.substr(last+1,s2.size()-1-last);
//	cout<<"last="<<last<<endl;
//	cout<<"a="<<a<<endl;
//	cout<<"b="<<b<<endl;
//	cout<<"c="<<c<<endl;
	return a+b+c;
}

string replace_substrings(string s, string val, string replacement){
	while(s.find_last_of(val)>0 && s.find_last_of(val)<s.size() ){
		s=replace_last_substring(s, val, replacement);
	}
	return s;
}


string get_csv_item(string s, int item_num, string separator)
{
	string word;
	int ind1=0;
	int ind2=0;
	for(int i=0;i<item_num;i++){
		ind2 = s.find_first_of(separator,ind1);
		ind1 = ind2+1;
	}
	return s.substr(ind1,ind2-ind1);
}

bool string_contains(string s, string sample)
{
	if( s.find(sample) < s.size() ){
		return true;
	}
	return false;
}

void get_vector_of_substrings_separated_by(string s, string separator, vector<string> &v){
//	cout<<"s="<<s<<endl;
	int ind = s.find("\t");
	if(ind>=0&&ind<s.size()){
		string substring = s.substr(0,ind);
//		cout<<"substring="<<substring<<endl;
		v.push_back(substring);
		string s2 = s.substr(ind+separator.size(), s.size()-ind);
//		cout<<"s2="<<s2<<endl;
		get_vector_of_substrings_separated_by(s2, separator,v);
	}
}

vector<string> get_vector_of_substrings_separated_by(string s, string separator){
	vector<string> v;
	get_vector_of_substrings_separated_by(s, separator,v);
	return v;
}

vector<string> subdirs(string dir_path, bool fullpath)
{
    ensure_trailing_slash(dir_path);
    vector<string> result = get_dir_entries(dir_path,false);
//	for(int i=0;i<result.size();i++){
//		cout<<"res="<<result[i]<<endl;
//	}

    vector<string>::iterator dirs = result.begin();

    while(result.size() > 0 && dirs != result.end()){
        //sort out items which are not directories or circular references //cout<<"*dirs="<<*dirs<<endl;

	    if(*dirs == "." || *dirs == ".." || !dir_exists(dir_path + *dirs) ){
            dirs=result.erase(dirs);
        }else{
				if(fullpath){
					(*dirs).insert(0,dir_path);
				}
            ensure_trailing_slash(*dirs);
            ++dirs; 
		}
	}
	return result;
}


vector<string> subdirs_where_name_contains(string dir_path, string name_substring)
{
	vector<string> sub = subdirs(dir_path);
	vector<string> sub2;

	for(int i=0;i<sub.size();i++){
//		cout<<"sub[i].find(name_substring)="<<sub[i].find(name_substring)<<endl;
		if( sub[i].find(name_substring) < sub[i].size() ){
			sub2.push_back(sub[i]);
		}
	}
	return sub2;
}

vector<string> get_files_in_dir_where_name_contains(string dir_path, string name_substring, bool fullpath)
{
	vector<string> files = get_dir_entries(dir_path,false);
	vector<string> files2;

	for(int i=0;i<files.size();i++){
//		cout<<"files[i]="<<dir_path+files[i]<<endl;
		if(file_exists(dir_path+files[i])){
//			cout<<"files[i].find(name_substring)="<<files[i].find(name_substring)<<endl;
			if( files[i].find(name_substring) < files[i].size() ){
//				cout<<"***"<<endl;
				if(fullpath){
					files2.push_back(dir_path+"/"+files[i]);
				}else{
					files2.push_back(files[i]);
				}
			}
		}
	}
	return files2;
}

string int2str(int i){
return templ_to_string(i);
}

string float2str(float f){
return templ_to_string(f);
}


void split(const std::string & s, char c, std::vector<std::string> & v)
{
	string::size_type i = 0;
	string::size_type j = s.find(c);
	
	while ( j != string::npos )
	{
		v.push_back(s.substr(i, j - i));
		i = ++j;
		j = s.find(c, j);
		
		if ( j == string::npos )
		{	// there are no more delimiters. get the last part of s
			v.push_back(s.substr(i));
		}
	}
}

