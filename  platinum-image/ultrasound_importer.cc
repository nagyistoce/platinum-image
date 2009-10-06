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

#include "ultrasound_importer.h"

ultrasound_importer::ultrasound_importer(string file){
	side = "unknown";
	loaded = false;
	scan_index = 0;
	row_index = 0;
	read_file(file);
	file.substr(0, 2);
	name = file.substr(file.find_last_of('/')+1);
	ecode = name;
	/*if(name.at(0) == '0'){
		ecode = "E" + name.substr(0,8);
	}else{
		ecode = "E000" + name.substr(0,5);
	}*/
	//cout << "File name: " << name << endl;

}

ultrasound_importer::~ultrasound_importer(){
}

/*void ultrasound_importer::read_study(ifstream &myfile, long length){
	char input;
	char buff[6000];
	int id = 0;
	long size;
	myfile.getline(buff,6000); //useless short row
	//myfile.getline(buff,6000);
	size = myfile.gcount();
	int i = size-2;
	
	while(buff[i] != 0){
		i--;
	}
	cout << "from: " << i << " to" << size << endl;
	char* sub = &buff[i+1];
	cout << "found study: " << sub << endl;
}*/

bool ultrasound_importer::match(string text){
	const char *l;
	l = text.c_str();
	int high, low;
	int dot = 46;
	high = 57;
	low = 48;
	for(int i = 0; i < 10; i++){
		if(l[i] > high || l[i] < dot)
			return false;
	}
	return l[2] == dot && l[5] == dot;
}
bool ultrasound_importer::is_new_version(ifstream &myfile){
	char buff[7];

	myfile.read(buff,7);
	string line(buff);
	return line.find("3.00",0) == string::npos && study_date.find("199",0) == string::npos; //not old version. Want all newer versions to be supported
}
void ultrasound_importer::set_side(ifstream &myfile, long length){
	char buff[6];
	long pos_ = 0;
	char input;
	long new_pos;
	while(pos_ < length){// && (pos_ > -1)){
		new_pos = myfile.tellg();
		input = myfile.get();
		if(input == 'c'){
			myfile.read(buff,6);
			string line(buff);
			//cout << line << endl;
			if(line.find("aroti",0)!=string::npos){
				new_pos -= 5; //f or h;
				myfile.seekg(new_pos);
				myfile.read(buff,6);
				string line(buff);
				//There are 1 or 2 spaces between right(left) and carotis
				if(line.find("ht",0)!=string::npos){
					side = "dx";
					return;
				}else if(line.find("ft",0)!=string::npos){
					side = "sin";
					return;
				}
			}
		}
		pos_ = myfile.tellg();
	}
}
void ultrasound_importer::set_date(ifstream &myfile, long length){
	char buff[11];
	long pos_ = 0;
	char input;
	while(pos_ < length){// && (pos_ > -1)){
		input = myfile.get();
		//myfile.read( (char *)(&val), sizeof(val) );
		if(input == '.'){
			long new_pos = myfile.tellg();
			new_pos -=3; //move to start of date
			myfile.seekg(new_pos);
			myfile.read(buff,11);
			string line(buff);
			//cout << "line: " << line << endl;
			if(match(line)){ //Supports from 20:th century. Not 21:st
				//cout << "date is: " << res[1] << "."<< res[2] << "." << res[3] << endl;
				string day = line.substr(0,2);
				string month = line.substr(3,2);
				string year = line.substr(6,4);
				
				study_date = year +"-"+ month +"-"+ day;
				cout << "study date: " << study_date << endl;
				return;
			}
		}
		pos_ = myfile.tellg();
	}
}

void ultrasound_importer::read_file(string filepath){
	string line = "";
	int header_size = 0; //To be determined or ignored ;)
	int linesize = 176;//Verifyed; 
	int nr_of_lines = 208;//Verifyed but is determined by a setting on the scanner...
	int line_offset = 102; //verifyed

	//string filepath = userIOmanagement.get_parameter<string>(userIO_ID,0);
	if(filepath.empty()){
		return;
	}
	long pos_ = 0;
	unsigned long f_length, begin, end;
	ifstream myfile(filepath.c_str(),ios::in | ios::binary); //BINARY SOLVED EVERYTHING!!!



	if(myfile.is_open()){

		//Get length of file
		begin = myfile.tellg();
		myfile.seekg (0, ios::end);
		end = myfile.tellg();
		f_length = end-begin;
		myfile.seekg (0, ios::beg);

		

		//myfile.seekg (0, ios::beg); no need
		
		set_side(myfile, f_length);

		cout << "side is set to: " << side << endl;
		
		myfile.seekg (0, ios::beg); //if side is not found

		set_date(myfile, f_length);

		myfile.seekg (0, ios::beg); //should not be needed
		//read_study(myfile, f_length);

		//This last so side and date is set
		if(!is_new_version(myfile)){
			myfile.close();
			cout << "reading old file" << endl;
			read_old_file(filepath);
			return;
		}

		unsigned short val;
		char input;
		char buff[8];
		int id = 0;
		while(pos_ < f_length){// && (pos_ > -1)){
			input = myfile.get();
			//myfile.read( (char *)(&val), sizeof(val) );
			if(input == 'S'){
				myfile.read(buff,8);
				string line(buff);
				if(line.find("can ",0)!=string::npos){ //It says "Scan XXX" in the file, where XXX is a number
					string name = "Scan ";
					name.append(line, 4, 3);
					us_scan *scan = new us_scan(name, linesize, nr_of_lines);

					int olle = myfile.tellg();
					myfile.seekg(olle-100);
					myfile.read( (char *)(&val), sizeof(val) );
					nr_of_lines = val;
					//cout << "lines for " << name << ": " << nr_of_lines << endl;
					/*myfile.seekg(olle-108);
					for(int i = 0; i < 100; i+=2){
						myfile.read( (char *)(&val), sizeof(val) );
						//if(i == 8){
						//	cout << "8 = "<< val << " ";
						//}else if(i == 38){
						//	cout << "38 = " << val << " ";
						//}
						//if(val == 16){
						//	cout << i << " ";
						//}
					}
					cout << endl;*/

					myfile.seekg(olle+line_offset);
					for(int j = 0; j<nr_of_lines; j++){
						pts_vector<unsigned short>* row = new pts_vector<unsigned short>(0);
						for(int i = 0; i < linesize; i++){
							myfile.read( (char *)(&val), sizeof(val) );
							unsigned short sum = numeric_limits<unsigned short>::max() - val;
							scan->mean_vector->at(i)+= ((sum - scan->mean_vector->at(i))/(j+1));
							row->push_back(sum);
						}
						row->config_x_axis(0.04,0);
						scan->rows.push_back(row);
					}
					scan->mean_vector->config_x_axis(0.04,0);
					all_scans.push_back(scan);

				}
			}
			pos_ = myfile.tellg();
		}
		//cout << "bytes read: " << pos_  << "/" << f_length << endl;
		myfile.close();
		if(all_scans.empty()){
			cout << "reading old file" << endl;
			read_old_file(filepath);
		}else{
			loaded = true;
		}
	}
}

void ultrasound_importer::read_old_file(string filepath){
	string line = "";
	int header_size = 0; //To be determined or ignored ;)
	int linesize = 82*2;//176/2;
	int nr_of_lines = 128;//Verifyed but is determined by a setting on the scanner...
	int line_offset = -2;//102; //verifyed

	//string filepath = userIOmanagement.get_parameter<string>(userIO_ID,0);
	if(filepath.empty()){
		return;
	}
	long pos_ = 0;
	unsigned long f_length, begin, end;
	ifstream myfile(filepath.c_str(),ios::in | ios::binary); //BINARY SOLVED EVERYTHING!!!



	if(myfile.is_open()){

		//Get length of file
		begin = myfile.tellg();
		myfile.seekg (0, ios::end);
		end = myfile.tellg();
		f_length = end-begin;
		myfile.seekg (0, ios::beg);

		if(is_new_version(myfile)){
			myfile.close();
			cout << "Unknown version" << endl;
			return;
		}

		unsigned short val;
		char input;
		char buff[7];
		int id = 0;
		while(pos_ < f_length){// && (pos_ > -1)){
			input = myfile.get();
			//myfile.read( (char *)(&val), sizeof(val) );
			if(input == '.'){
				myfile.read(buff,7);
				string line(buff);
				if(line.find(".19",0)!=string::npos || line.find(".20",0)!=string::npos){ //Supports from 20:th century. Not 21:st
					string name = "OldScan ";
					myfile.read(buff, 1);
					myfile.read(buff,2);
					string t(buff);
					name.append(t, 0, 2);
					us_scan *scan = new us_scan(name, linesize, nr_of_lines);

					int olle = myfile.tellg();
					//myfile.seekg(olle-100);
					//myfile.read( (char *)(&val), sizeof(val) );
					//nr_of_lines = val;
					//cout << "lines for " << name << ": " << nr_of_lines << endl;
					/*myfile.seekg(olle-108);
					for(int i = 0; i < 100; i+=2){
						myfile.read( (char *)(&val), sizeof(val) );
						//if(i == 8){
						//	cout << "8 = "<< val << " ";
						//}else if(i == 38){
						//	cout << "38 = " << val << " ";
						//}
						//if(val == 16){
						//	cout << i << " ";
						//}
					}
					cout << endl;*/

					myfile.seekg(olle+line_offset);
					for(int j = 0; j<nr_of_lines; j++){
						pts_vector<unsigned short>* row = new pts_vector<unsigned short>(0);
						for(int i = 0; i < linesize; i++){
							myfile.read( (char *)(&val), sizeof(val) );
							unsigned short sum = numeric_limits<unsigned short>::max() - val;
							scan->mean_vector->at(i)+= ((sum - scan->mean_vector->at(i))/(j+1));
							row->push_back(sum);
						}
						row->config_x_axis(0.07,0);
						scan->rows.push_back(row);
					}
					scan->mean_vector->config_x_axis(0.07,0);
					all_scans.push_back(scan);

				}
			}
			pos_ = myfile.tellg();
		}
		//cout << "bytes read: " << pos_  << "/" << f_length << endl;
		myfile.close();
		if(!all_scans.empty()){
			loaded = true;
		}
	}
}

string ultrasound_importer::set_current_scan(int i){
	scan_index = (scan_index+i)%all_scans.size(); //tror detta kommer funka
	//curve->my_data = all_scans.at(scan_index)->rows.at(row_index);
	return all_scans.at(scan_index)->name;
}

int ultrasound_importer::set_current_row(int i){
	row_index = (row_index+i)% all_scans.at(scan_index)->rows.size(); //tror detta kommer funka
	//curve->my_data = all_scans.at(scan_index)->rows.at(row_index);
	return row_index;
}


us_scan::us_scan(string n, int init_size, int lines){
	name = n;
	mean_vector = new pts_vector<unsigned short>(init_size);
	mean_vector->assign(init_size,0);
}

us_scan::~us_scan(){
//	mean_vector->clear();
	//Add more later
}
