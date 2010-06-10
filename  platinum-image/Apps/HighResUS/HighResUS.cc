/////////////////////////////////////////////////////////////////////////////////
//
//  HighResUS - Application for image segmentation using Graphs max flow and min cuts
//
//	For more details...
//
//  BGL - Boost Graph Libary : http://www.boost.org/doc/libs/1_38_0/libs/graph/doc/index.html
//

//   Copyright (c) 2007, Uppsala university
//   All rights reserved.
//
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions are met:
//       * Redistributions of source code must retain the above copyright
//         notice, this list of conditions and the following disclaimer.
//       * Redistributions in binary form must reproduce the above copyright
//         notice, this list of conditions and the following disclaimer in the
//         documentation and/or other materials provided with the distribution.
//       * Neither the name of the Uppsala university nor the
//         names of its contributors may be used to endorse or promote products
//         derived from this software without specific prior written permission.
//
//   THIS SOFTWARE IS PROVIDED BY UPPSALA UNIVERSITY ``AS IS'' AND ANY
//   EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//   DISCLAIMED. IN NO EVENT SHALL UPPSALA UNIVERSITY BE LIABLE FOR ANY
//   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "../../platinumprogram.h"
#include "../../ptconfig.h"

#include "../../ultrasound_importer.h"

#include "../../image_binary.hxx"
#include "../../curve.hxx"
#include <vector>
#include "string.h"
#include "../../ultra1dops.h"


#ifdef _MSC_VER
	//disable warning C4068: "unknown pragma"
	#pragma warning(disable: 4068)
#endif

#define CURVE_CONF_PATH "Apps/HighResUS/curve_conf.inp"

#include <sstream>


void write_raw(ultrasound_importer *ultra, vector<gaussian> gauss){
	string gauss_data = "../PlatinumPrivateApps/HighResUS/output/raw.skv";
	ofstream data(gauss_data.c_str(), ios::app);
	string date, name, ecode,side;
	date = ultra->study_date;
	name = ultra->name;
	ecode = ultra->ecode;
	side = ultra->side;
	double sigma_ratio = 1.1;

	unsigned long begin, end, f_length;
	begin = data.tellp();
	data.seekp (0, ios::end);
	end = data.tellp();
	f_length = end-begin;
	data.seekp (0, ios::beg);
	vector<float> intima, media;
	data << ecode << ";intima;";
	for(int i = 0; i < gauss.size(); i+=2){
		intima.push_back((gauss.at(i).center + gauss.at(i).sigma*sigma_ratio) - (gauss.at(i).center - gauss.at(i).sigma*sigma_ratio));  //intima
		media.push_back((gauss.at(i).center - gauss.at(i).sigma*sigma_ratio) - (gauss.at(i+1).center + gauss.at(i+1).sigma*sigma_ratio)); //media
	}
	for(int i = 0; i < intima.size(); i++){
		if(intima[i] == 0)
			data << ";";
		else
			data << intima[i] << ";";
	}
	data << "\n" << ecode << ";media;";
	for(int i = 0; i < intima.size(); i++){
		if(media[i] == 0)
			data << ";";
		else
			data << media[i] << ";";
	}
	data << "\n";
	data.close();
}

void write_measurements(ultrasound_importer *ultra, vector<gaussian> gauss){
	string gauss_data = "../PlatinumPrivateApps/HighResUS/output/values.skv";
	ofstream data(gauss_data.c_str(), ios::app);
	double intima, media, adventitia, total, ratio;
	double sigma_ratio = 1.1;
	string date, name, ecode,side;
	double media_std;
	double i_m, m_m, a_m, r_m, t_m, imt_m;
	i_m = m_m = a_m = r_m = t_m = imt_m =0;
	date = ultra->study_date;
	name = ultra->name;
	ecode = ultra->ecode;
	side = ultra->side;
	float c = 0;
	for(int i = 0; i < gauss.size(); i+=2){
		intima = (gauss.at(i).center + gauss.at(i).sigma*sigma_ratio) - (gauss.at(i).center - gauss.at(i).sigma*sigma_ratio);  //intima
		media =  (gauss.at(i).center - gauss.at(i).sigma*sigma_ratio) - (gauss.at(i+1).center + gauss.at(i+1).sigma*sigma_ratio); //media
		if(intima > 0.001 && media > 0.001){
			ratio = intima/media;
			adventitia = (gauss.at(i+1).center + gauss.at(i+1).sigma*sigma_ratio) - (gauss.at(i+1).center - gauss.at(i+1).sigma*sigma_ratio); //adventitia
			total =(gauss.at(i).center + gauss.at(i).sigma*sigma_ratio) - (gauss.at(i+1).center - gauss.at(i+1).sigma*sigma_ratio); //total
			i_m += intima;
			m_m += media;
			a_m += adventitia;
			t_m += total;
			imt_m += (intima + media); 
			r_m += ratio;
			c++;
		}
	}

	i_m/=c;
	m_m/=c;
	a_m/=c;
	t_m/=c;
	r_m/= c;
	imt_m/=c;

	//Calc_std for media
	media_std = 0;
	for(int i = 0; i < gauss.size(); i+=2){
		media_std += pow((gauss.at(i).center - gauss.at(i).sigma*sigma_ratio) - (gauss.at(i+1).center + gauss.at(i+1).sigma*sigma_ratio) - m_m,2); //media
	}
	media_std = (sqrt(media_std/(gauss.size()/2))/m_m)*100;

	unsigned long begin, end, f_length;
	begin = data.tellp();
	data.seekp (0, ios::end);
	end = data.tellp();
	f_length = end-begin;
	data.seekp (0, ios::beg);

	if(f_length < 2){
		data << "name;study date;side;intima;media;adventitia;total;IMT;IMR;sucessfull scans;media std\n";
	}
	int successfull = round(((c)/ultra->all_scans.size())*100);
	cout << "success " << successfull  <<" (" << c << ")"<< endl;
	if(successfull >0)
		data << ecode << ";" << date<< ";" << side <<";"<<i_m<< ";" <<m_m<< ";" <<a_m<< ";" <<t_m<<";" <<imt_m<<";" <<r_m<< ";"<< successfull<< "%;" << media_std << "%;\n";
	else
		data << ecode << ";" << date<< ";" << side <<";"<<0<< ";" <<0<< ";" <<0<< ";" <<0<<";" <<0<<";" <<0<< ";"<< 0<< "%;" << 0 << "%;\n";
	data.close();
}

Vector3D find_closest_maxima(vector<Vector3D> keep, int x){
	int before = abs(keep[0][0]-x);
	int last = 0;
	for(int i = 1; i < keep.size(); i++)
		if(keep[i][1] > 0 && abs(keep[i][0]-x) > before)
			return  keep.at(last);
		else if (keep[i][1] > 0){
			before = abs(keep[i][0]-x);
			last = i;
		}
	return keep.at(last);

}

int get_man_peaks_from_file(string path, vector<Vector3D> keep, string name,Vector3D *peaks){
	char line[256];
	path = path + "/man.pt";
	int s = name.length();
	int x1, x2;
	ifstream myfile(path.c_str(),ios::in | ios::binary);
	if(myfile.is_open()){
		while(!myfile.eof()){
			myfile.getline(line,256);
			string l(line);
			if(l.find(name,0)!=string::npos){//name = file + " " + scan nr
				cout << "found " << name << " ";
				int i = myfile.tellg();
				i = i - l.length();
				i = i+s;
				myfile.seekg(i); //located right after name;
				myfile >> x1 >> x2;
				peaks[0] = find_closest_maxima(keep,x1);
				peaks[1] = find_closest_maxima(keep,x2);
				if(peaks[0][0] == peaks[1][0])
					return 1;
				cout << peaks[0][0] << " " << peaks[1][0] << endl;
				myfile.close();
				return 2;
			}
		}
	}
	myfile.close();
	return 1;
}

void manual_marking(int userIO_ID,int par_num){

	/*
	To read a manual peak localization file it needs to be located in the same folder as the DC9 files and be named "man.pt"
	The syntax in the file is "filename Scan XXX intima_xcoordinate adventitia_xcoordinate
	Example file:
	20090817.DC9 Scan 002 80 66
	20090817.DC9 Scan 006 82 66
	20090817.DC9 Scan 007 81 68
	20090603.DC9 Scan 001 61 48
	*/
	if(par_num == USERIO_CB_OK){
		int total_scans = 0;
		string dir_path = userIOmanagement.get_parameter<string>(userIO_ID,0);

		vector<string> all_files = get_dir_entries(dir_path,true);
		if(all_files.empty()){
			size_t found;
			found=dir_path.find_last_of("/\\");
			dir_path = dir_path.substr(0,found+1);
			all_files = get_dir_entries(dir_path, true);

		}
		cout << "found " << all_files.size() << " files in " << dir_path << endl;
		pt_config::initialize(CURVE_CONF_PATH);
 
		for(int iter = 2; iter < all_files.size(); iter++){//The two first are . and ..
			//Uncomment this if you want to look at the scans
			//datamanagement.delete_all();
			cout <<  (iter-1) << "/" << (all_files.size()-2) << endl;
			ultrasound_importer *ultra;
			ultra = new ultrasound_importer(all_files[iter]);
			if(ultra->loaded){
				double av = 0;
				vector<Vector3D> man;
				vector<gaussian> gauss;
				vector<string> names;
				cout << "found " << ultra->all_scans.size() << " scans" << endl;
				total_scans += ultra->all_scans.size();
				for(int i = 0; i < ultra->all_scans.size(); i++){
					Vector3D *peaks = new Vector3D[2];
					curve_scalar<unsigned short> *curve = new curve_scalar<unsigned short>(0,"testing",0,0.04);
					curve->my_data = ultra->all_scans.at(i)->mean_vector;//sets correct scale
					curve->name(ultra->name + " " +ultra->all_scans.at(i)->name);
					datamanagement.add(curve);		
					vector<Vector3D> keep = ultra1dops::simplify_the_curve(curve);

					gauss.push_back(gaussian());//load up dummies
					gauss.push_back(gaussian());
					int peak_count = get_man_peaks_from_file(dir_path, keep, curve->name(), peaks);
					
					if(peak_count ==2){
		
						int res = ultra1dops::straighten_the_peaks(ultra->all_scans.at(i),peaks[0][0] ,peaks[1][0]);
						if(res == 1){
							vector<gaussian> gau;
							gau = ultra1dops::fit_gaussian_curve_and_calculate(curve, peaks[0][0], peaks[1][0]);
							if(gau.size() == 2){
								gauss.pop_back();//remove zero fill;
								gauss.pop_back();
								gauss.push_back(gau[0]);
								gauss.push_back(gau[1]);
							}                  
							names.push_back(curve->name());
						}
					}
					free(peaks);
				}
				write_measurements(ultra, gauss);
				write_raw(ultra, gauss);
			}
		}
		cout << "total scans: " << total_scans << endl;
	}
}

void read_DC9_image(int userIO_ID,int par_num){
	if(par_num == USERIO_CB_OK){
		int total_scans = 0;
		string dir_path = userIOmanagement.get_parameter<string>(userIO_ID,0);

		vector<string> all_files = get_dir_entries(dir_path,true);
		if(all_files.empty()){
			size_t found;
			found=dir_path.find_last_of("/\\");
			dir_path = dir_path.substr(0,found+1);
			all_files = get_dir_entries(dir_path, true);

		}
		cout << "found " << all_files.size() << " files in " << dir_path << endl;

		pt_config::initialize(CURVE_CONF_PATH);
 
		for(int iter = 2; iter < all_files.size(); iter++){//The two first are . and ..
			//Uncomment this if you want to look at the scans
			//datamanagement.delete_all();
			cout <<  (iter-1) << "/" << (all_files.size()-2) << endl;
			ultrasound_importer *ultra;
			ultra = new ultrasound_importer(all_files[iter]);
			if(ultra->loaded){
				double av = 0;
				vector<Vector3D> man;
				vector<gaussian> gauss;
				vector<string> names;
				cout << "found " << ultra->all_scans.size() << " scans" << endl;
				total_scans += ultra->all_scans.size();
				for(int i = 0; i < ultra->all_scans.size(); i++){
					Vector3D *peaks = new Vector3D[2];
					curve_scalar<unsigned short> *curve = new curve_scalar<unsigned short>(0,"testing",0,0.04);
					curve->my_data = ultra->all_scans.at(i)->mean_vector;//sets correct scale
					curve->name(ultra->name + " " +ultra->all_scans.at(i)->name);
					datamanagement.add(curve);

					gauss.push_back(gaussian());//load up dummies
					gauss.push_back(gaussian());
					
					vector<Vector3D> keep = ultra1dops::simplify_the_curve(curve);
					int peak_count = ultra1dops::count_peaks(keep, curve, peaks); //peak[0] = intima, peak[1] = adventitia
					
					if(peak_count ==2 && peaks[0][0] > curve->get_data_size()/2){ //check if only "skin" is found
						int res = ultra1dops::straighten_the_peaks(ultra->all_scans.at(i),peaks[0][0] ,peaks[1][0]);
						if(res == 1){
							Vector3D ma;
							vector<gaussian> gau;
							gau = ultra1dops::fit_gaussian_curve_and_calculate(curve, peaks[0][0], peaks[1][0]);
							if(gau.size() == 2){
								gauss.pop_back();//remove zero fill;
								gauss.pop_back();
								gauss.push_back(gau[0]);
								gauss.push_back(gau[1]);
							}                  
							names.push_back(curve->name());
						}
					}
					free(peaks);
				}
				write_measurements(ultra, gauss);
				write_raw(ultra, gauss);
			}
		}
		cout << "total scans: " << total_scans << endl;
	}
}

int main(int argc, char *argv[])
{
	//start up Platinum
	platinum_init();

	const int w_margin=15*2;
	int windowwidth = 1400-w_margin;
	int windowheight = 900-w_margin;
	//set up main window
	Fl_Window window(windowwidth,windowheight);
	window.fullscreen(); //Maximize the window without maximizing it hack ^^
	window.fullscreen_off(window.x(), window.y(), window.w(), window.h()-30); 

	platinum_setup(window,2,2,300);


	int load_ID=userIOmanagement.add_userIO("Load specified .dc9 file", read_DC9_image, "Load");
	userIOmanagement.add_par_filepath(load_ID,"Input .DC9-file","C:");
	userIOmanagement.finish_userIO(load_ID);


	int cload_ID=userIOmanagement.add_userIO("Load for manual", manual_marking, "Load");
	userIOmanagement.add_par_filepath(cload_ID,"Input .DC9-file","C:");
	userIOmanagement.finish_userIO(cload_ID);

	// *** end userIO control definitions ***

	window.end();

	window.show(argc, argv);
	return Fl::run();
}
