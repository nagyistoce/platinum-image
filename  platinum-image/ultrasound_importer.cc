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
	loaded = false;
	scan_index = 0;
	row_index = 0;
	read_file(file);

}

ultrasound_importer::~ultrasound_importer(){
}

void ultrasound_importer::read_file(string filepath){
	string line = "";
		int header_size = 0; //To be determined or ignored ;)
		int linesize = 176;//Verifyed; 
		int nr_of_lines = 208;//Verifyed

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

						myfile.seekg(olle+102);
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
			cout << "bytes read: " << pos_  << "/" << f_length << endl;
			myfile.close();
			loaded = true;
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
