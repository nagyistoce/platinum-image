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

/*
 * The Vax conversion routines require that this copyright notice is included.
 *
 Copyright(c) 1982 Association of Universities for Research in Astronomy Inc.

 The FOCAS software is publicly available, but is NOT in the public domain.
 The difference is that copyrights granting rights for unrestricted use and
 redistribution have been placed on all of the software to identify its authors.
 You are allowed and encouraged to take this software and use it as you wish,
 subject to the restrictions outlined below.

 Permission to use, copy, modify, and distribute this software and its
 documentation is hereby granted without fee, provided that the above
 copyright notice appear in all copies and that both that copyright notice
 and this permission notice appear in supporting documentation, and that
 references to the Association of Universities for Research in Astronomy
 Inc. (AURA), the National Optical Astronomy Observatories (NOAO), or the
 Faint Object Classification and Analysis System (FOCAS) not be used in
 advertising or publicity pertaining to distribution of the software without
 specific, written prior permission from NOAO.  NOAO makes no
 representations about the suitability of this software for any purpose.  It
 is provided "as is" without express or implied warranty.

 NOAO DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL NOAO
 BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
inline short int vax2s(short int le_short)
{
	union	{
		short	bs;
		char	bc[2];
	} bsbuf;
	char	chold;

	bsbuf.bs = le_short;
	chold = bsbuf.bc[0];
	bsbuf.bc[0] = bsbuf.bc[1];
	bsbuf.bc[1] = chold;

	return (bsbuf.bs);
}

// Vax float to IEEE 32-bit float
inline float vax2f(unsigned char le_flt[4])
{
	union {
		unsigned char bytes[4];
		float  ff;
		short fs[2];
		struct {				/* VAX F-floating */
			unsigned int sign:1;
			unsigned int exponent:8;
			unsigned int mantissa:23;
		} v;
	} vaxbuf;
	float	ieeebuf;

	vaxbuf.bytes[0] = le_flt[3];
	vaxbuf.bytes[1] = le_flt[2];
	vaxbuf.bytes[2] = le_flt[1];
	vaxbuf.bytes[3] = le_flt[0];

	vaxbuf.fs[0] = vax2s (vaxbuf.fs[0]);
	vaxbuf.fs[1] = vax2s (vaxbuf.fs[1]);

	// this doesn't appear to be doing the right thing - so I took it out - GMR
	//if (vaxbuf.v.exponent < 3) 		/* prevent underflow */ {
	//			ieeebuf = 0.0;
	//			std::cout << std::endl << "underflow detected - result is zero";
	//		}
	//		else
	ieeebuf = vaxbuf.ff / 4.0;

	return (ieeebuf);
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

			float real_ieee, comp_ieee;
			for(int j = 0; j < head->get_value("samples"); j++){
				unsigned char real_part_vax[4] = {0};
				unsigned char imag_part_vax[4] = {0};

				myfile.read((char*)real_part_vax, 4);
				myfile.read((char*)imag_part_vax, 4);
				
				real_ieee = vax2f(real_part_vax);
				comp_ieee = vax2f(imag_part_vax);	
				
				c->push_back(complex<float>(real_ieee,comp_ieee));//In the original code there was -comp_ieee here for some reason
				//v1.push_back(real);
			}
			curves.push_back(c);
		}
		loaded = true;
		//out.close();
		myfile.close();
	}
}

void spectrum_importer::swapWords(float *word)
{

	char	*p = (char *)word;
	char	aux;
	aux = p[0];
	p[0] = p[3];
	p[3] = aux;
	aux = p[1];
	p[1] = p[2];
	p[2] = aux;
	p += 4;
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
