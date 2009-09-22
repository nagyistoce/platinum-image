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

#include "spectrum_importer.h"

spectrum_importer::spectrum_importer(string file){
	loaded = false;
	head = new header();
	if(read_header(file)){
		read_data(file);
	}
	file.substr(0, 2);
	name = file.substr(file.find_last_of('/')+1);
	cout << "File name: " << name;

	

}

spectrum_importer::~spectrum_importer(){
	delete head;
}

bool spectrum_importer::read_header(string filepath){
	
	//string keywords[] = {"samples", "rows", "sample_frequency"};
	//int nr_keywords = 3;
	cout << "trying to read" << endl;
	string line;
	ifstream myfile (filepath.c_str());
	if (myfile.is_open()){
		while (! myfile.eof()){
			getline (myfile,line);
			if(line.size() > 1){
				head->add_info(line);
			}
		}
		myfile.close();
	}else{
		cout << "IT'S CLOSED..." << endl;
		return false;
	}

	cout << "reading done" << endl;
	return true;
}

void spectrum_importer::read_data(string filepath){
	string::size_type i = filepath.find_last_of(".");
	string end = filepath.substr(i);
	string path = filepath.substr(0,i);
	string new_end;
	if(end.compare(".SPAR") == 0)
		path = path + ".SDAT";
	else
		path = path + ".sdat";
	cout << "Reading dat file: " << path << endl;
	string out_path = path + "out";
	//ofstream out(out_path.c_str());
	ifstream myfile(path.c_str(),ios::binary); //BINARY SOLVED EVERYTHING!!!
	cout << "rows: " << head->get_value("rows") << endl;
	cout << "samples: " << head->get_value("samples") << endl;
	if(myfile.is_open()){

		for(int i = 0; i < head->get_value("rows"); i++){
			ptc_vector<float> *c = new ptc_vector<float>(head->get_value("samples"));
			c->clear();
			float real, comp;
			for(int j = 0; j < head->get_value("samples"); j++){
				myfile.read( (char *)(&real), sizeof(real) );
				myfile.read( (char *)(&comp), sizeof(comp) );
				//myfile >> real >> comp;
			//	out << real << " " << comp << "\n";
				if(my_isnan(real)){
					real = 0;
					cout << "NAN!!!!" << endl;
				}
				if(my_isnan(comp)){
					comp = 0;
					cout << "NAN!!!!" << endl;
				}
				c->push_back(complex<float>(real,comp));
			}
			curves.push_back(c);
		}
		loaded = true;
		//out.close();
		myfile.close();
	}
}

header::header(){
	keys.push_back(keywords("samples", 0));
	keys.push_back(keywords("rows", 0));
	keys.push_back(keywords("sample_frequency", 0));
	start_to_look = 0;
}

header::~header(){
	
}
void header::add_info(string s){
	//Divide string and store as keyword
	string::size_type i;
	if((i = s.find_first_of(":")) != string::npos){
		string key = s.substr(0, i-1); //Skip the whitespace
		for(int a = start_to_look; a < keys.size(); a++){
			if(keys.at(a).key.compare(key) == 0){
				keys.at(a).val = string_to_val<double>(s.substr(i+1));
				cout << "adding: *" << key << "*  "  << keys.at(a).val << endl;
				if(a == start_to_look)
					start_to_look++;
				return;
			}
		}
	}
}
double header::get_value(string s){
	for(int i = 0; i < keys.size(); i++){
		if(keys.at(i).key.compare(s) == 0){
			return keys.at(i).val;
		}
	}
	return -1; //May not work if a value can be -1
}

keywords::keywords(std::string k, double v){
	key = k;
	val = v;
}
