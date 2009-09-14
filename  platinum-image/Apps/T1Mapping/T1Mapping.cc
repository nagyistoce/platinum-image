/////////////////////////////////////////////////////////////////////////////////
//
//  T1 Mapping application using Platinum
//	See description in: Kullberg2006 - J Magn Reson Imaging. 2006 Aug;24(2):394-401.
//	Whole-body T1 mapping improves the definition of adipose tissue: consequences for automated image analysis.
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

#include <cstdio>
#include <iostream>
#include <limits>	//for example...  numeric_limits<float>
#include <vector>


#include "string.h"
#include "../../global.h"
#include "../../image_binary.hxx"

//JK-Remove pragma warnings in VS...
#ifdef _MSC_VER
	//disable warning C4068: "unknown pragma"
	#pragma warning(disable: 4068)
#endif

//retrieve command-line arguments
int visid = -1;
int arg(int argc, char **argv, int &i) {
	if (argv[i][1] == 'v') {
		if (i+1 >= argc) return 0;
		visid = atoi(argv[i+1]);
		i += 2;
		return 2;
	}
	return 0;
}

// *** begin userIO demo functions ***

//string streams for parameter demo function
#include <sstream>
using namespace std;

void slc_reorg_function (int userIO_ID,int par_num)
{
	if (par_num == USERIO_CB_OK)
	{
		image_integer<unsigned short,3> *vol = dynamic_cast<image_integer<unsigned short,3>*>(datamanagement.get_image<image_base>(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,0)));
		long int nc = userIOmanagement.get_parameter<long int>(userIO_ID,1);
		long int nd = userIOmanagement.get_parameter<long int>(userIO_ID,2);
		if(vol!=NULL && nc >0 && nd>0)
		{
			vol->slice_reorganization_multicontrast( int(nd), int(nc) );
		}
	}
}


void reslice_function(int userIO_ID,int par_num)
{
	if (par_num == USERIO_CB_OK)
	{
		image_integer<unsigned short,3> *im = dynamic_cast<image_integer<unsigned short,3>*>(datamanagement.get_image<image_base>(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,0)));
		long int dir = userIOmanagement.get_parameter<long int>(userIO_ID,1);
		long int start = userIOmanagement.get_parameter<long int>(userIO_ID,2);
		long int every = userIOmanagement.get_parameter<long int>(userIO_ID,3);
		std::string file = userIOmanagement.get_parameter<std::string>(userIO_ID,4);

		cout<<"dir="<<dir<<endl;
		cout<<"start="<<start<<endl;
		cout<<"every="<<every<<endl;
		cout<<"file="<<file<<endl;

		if(im != NULL && file != ""){
			image_general<unsigned short,3> *im2 = im->get_subvolume_from_slices_3D(int(start),int(every),int(dir));
			im2->name("im2..");
			im2->data_has_changed(true);
			datamanagement.add(im2);
			im2->save_to_VTK_file(file);
		}
	}
}

void t1_mapping_function (int userIO_ID,int par_num)
{
	if (par_num == USERIO_CB_OK)
	{
		image_integer<unsigned short,3> *large = dynamic_cast<image_integer<unsigned short,3>*>(datamanagement.get_image<image_base>(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,0)));
		float tr = userIOmanagement.get_parameter<float>(userIO_ID,1);
		float te = userIOmanagement.get_parameter<float>(userIO_ID,2);
		long int large_flip = userIOmanagement.get_parameter<long int>(userIO_ID,3);
		std::string large_dcm = userIOmanagement.get_parameter<std::string>(userIO_ID,4);

		image_integer<unsigned short,3> *small = dynamic_cast<image_integer<unsigned short,3>*>(datamanagement.get_image<image_base>(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,5)));
		long int small_flip = userIOmanagement.get_parameter<long int>(userIO_ID,6);
		std::string small_dcm = userIOmanagement.get_parameter<std::string>(userIO_ID,7);

		if(large == NULL){
			cout<<"large == NULL"<<endl;
		}else if(small == NULL){
			cout<<"small == NULL"<<endl;
		}else if(small == large){
			cout<<"small == large"<<endl;
		}else{
			cout<<"Files successfully loaded..."<<endl;

			if(large_dcm!=""){large->meta.read_metadata_from_dcm_file(large_dcm);}
			if(tr>0){large->meta.set_data_float(DCM_TR, float(tr));}
			if(te>0){large->meta.set_data_float(DCM_TE, float(te));}
			large->meta.set_data_float(DCM_FLIP, float(large_flip));

			if(small_dcm!=""){small->meta.read_metadata_from_dcm_file(small_dcm);}
			small->meta.set_data_float(DCM_FLIP, float(small_flip));

			image_scalar<unsigned short,3> *t1map = large->calculate_T1Map_from_two_flip_angle_MRvolumes_3D(small,20,0,2000);
			t1map->name(large->name()+" - T1Map");
			datamanagement.add(t1map);
		}

	}
}

void tif_save_function (int userIO_ID,int par_num)
{
	if (par_num == USERIO_CB_OK)
	{
		string path = "c:/Joel/TMP/M/_F5/";
		//		string path = "c:/Joel/TMP/M/_FRANCISCO3fix_plus_rest/";
		//	string path = "G:/Joel/Data-Backup/_MALTIDSTUDIEN/_FRANCISCO_Liver_Segm/";
		//		vector<string> files = get_dir_entries_ending_with(path,".vtk");
		vector<string> files;
		//		files.push_back("10472f_15slc.vtk");
		files.push_back("10472f.vtk");
		files.push_back("10845f.vtk");
		files.push_back("10884f.vtk");
		files.push_back("10933f.vtk");
		string s;
		for(int i=0;i<files.size();i++)
		{
			image_scalar<unsigned short,3> *im = new image_scalar<unsigned short,3>();
			im->load_dataset_from_VTK_file(path+files[i]);
			//		s = path + "Tif/" + files[i].substr(0,files[i].size()-4);
			//		im->save_to_TIF_file_series_3D(s);
			im->flip_voxel_data_3D(2);
			im->save_to_VTK_file(path+"_"+files[i]);
			delete im;
		}
	}
}




void smooth_function(int userIO_ID,int par_num) 
{
	if (par_num == USERIO_CB_OK)
	{
		image_scalar<short,3> *psi = new image_scalar<short,3>();
		psi->load_dataset_from_VTK_file("c:/Joel/TMP/psi100slc.vtk");
		Vector3D radius;
		radius.Fill(2);
		//		psi->smooth_ITK(radius);
		psi->smooth_3D(radius);
		psi->save_to_VTK_file("c:/Joel/TMP/psi100slc2_3D.vtk");
	}
}


void t1_multiflip_function(int userIO_ID,int par_num) 
{
	if (par_num == USERIO_CB_OK)
	{
		float TR;
		int body_int_min;
		float flip;

		TR = userIOmanagement.get_parameter<float>(userIO_ID,0);
		body_int_min = int(userIOmanagement.get_parameter<long int>(userIO_ID,1));
		cout<<"TR="<<TR<<endl;

		vector<image_scalar<unsigned short,3> *> v;

		image_scalar<unsigned short,3> *f1 = dynamic_cast<image_scalar<unsigned short,3>* >(datamanagement.get_image<image_base>(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,2)));
		flip = userIOmanagement.get_parameter<long int>(userIO_ID,3);
		cout<<"flip="<<flip<<endl;
		f1->meta.set_data_float(DCM_FLIP,flip);
		f1->meta.set_data_float(DCM_TR,TR);
		//		f1->meta.print_all();
		v.push_back(f1);

		image_scalar<unsigned short,3> *f2 = dynamic_cast<image_scalar<unsigned short,3>* >(datamanagement.get_image<image_base>(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,4)));
		flip = userIOmanagement.get_parameter<long int>(userIO_ID,5);
		cout<<"flip="<<flip<<endl;
		f2->meta.set_data_float(DCM_FLIP,flip);
		v.push_back(f2);

		image_scalar<unsigned short,3> *f3 = dynamic_cast<image_scalar<unsigned short,3>* >(datamanagement.get_image<image_base>(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,6)));
		if(f3!=NULL){
			flip = userIOmanagement.get_parameter<long int>(userIO_ID,7);
			cout<<"flip="<<flip<<endl;
			f3->meta.set_data_float(DCM_FLIP,flip);
			v.push_back(f3);
		}

		image_scalar<unsigned short,3> *f4 = dynamic_cast<image_scalar<unsigned short,3>* >(datamanagement.get_image<image_base>(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,8)));
		if(f4!=NULL){
			flip = userIOmanagement.get_parameter<long int>(userIO_ID,9);
			cout<<"flip="<<flip<<endl;
			f4->meta.set_data_float(DCM_FLIP,flip);
			v.push_back(f4);
		}


		image_scalar<unsigned short,3> *t1map = f1->calculate_T1Map_3D(v,body_int_min);

		datamanagement.add(t1map);
		//do not delete images that are handled by the datamanagement...
	}
}


void hist_function(int userIO_ID,int par_num) 
{
	if (par_num == USERIO_CB_OK)
	{
		image_scalar<unsigned short,3> *f1 = dynamic_cast<image_scalar<unsigned short,3>* >(datamanagement.get_image<image_base>(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,0)));
		f1->save_histogram_to_txt_file("C:/Joel/TMP/hist/hist01.txt", ";"); //Richard la till ett semikolon sa att det gick att kompilera. Hoppas det inte sabbar till nat
	}
}



void test_function(int userIO_ID,int par_num) 
{
	if (par_num == USERIO_CB_OK)
	{

		string path = "C:/Joel/TMP/";
		image_scalar<unsigned short,3> *f = new image_scalar<unsigned short,3>(path + "femur_model.vtk");
		f->name("f");
		datamanagement.add(f);

		path = "C:/Joel/TMP/Pivus75/PD/";
		image_scalar<unsigned short,3> *t1 = new image_scalar<unsigned short,3>(path + "00000001");
		t1->name("t1");
		datamanagement.add(t1);

		//Calculation of T1map from QMED concentrations...
/*
		string path = "C:/Joel/Data/QMed-Rat/QMEDTEST_T1Map_tests_of_diff_consentrations/20071107/crop/";
		image_scalar<unsigned short,3> *q1 = new image_scalar<unsigned short,3>();
		image_scalar<unsigned short,3> *q2 = new image_scalar<unsigned short,3>();
		image_scalar<unsigned short,3> *q3 = new image_scalar<unsigned short,3>();
		image_scalar<unsigned short,3> *q4 = new image_scalar<unsigned short,3>();
		image_scalar<unsigned short,3> *q5 = new image_scalar<unsigned short,3>();
		image_scalar<unsigned short,3> *q6 = new image_scalar<unsigned short,3>();
		image_scalar<unsigned short,3> *q7 = new image_scalar<unsigned short,3>();
		image_scalar<unsigned short,3> *q8 = new image_scalar<unsigned short,3>();
		image_scalar<unsigned short,3> *q9 = new image_scalar<unsigned short,3>();
		image_scalar<unsigned short,3> *q10 = new image_scalar<unsigned short,3>();
		image_scalar<unsigned short,3> *q11 = new image_scalar<unsigned short,3>();
		image_scalar<unsigned short,3> *q12 = new image_scalar<unsigned short,3>();
		image_scalar<unsigned short,3> *q13 = new image_scalar<unsigned short,3>();
		image_scalar<unsigned short,3> *q14 = new image_scalar<unsigned short,3>();
		
		q1->load_dataset_from_VTK_file(path+"flip80.vtk");q1->meta.set_data_float(DCM_FLIP,80);q1->meta.set_data_float(DCM_TR,100);
		q2->load_dataset_from_VTK_file(path+"flip75.vtk");q2->meta.set_data_float(DCM_FLIP,75);q1->meta.set_data_float(DCM_TR,100);
		q3->load_dataset_from_VTK_file(path+"flip70.vtk");q3->meta.set_data_float(DCM_FLIP,70);q1->meta.set_data_float(DCM_TR,100);
		q4->load_dataset_from_VTK_file(path+"flip65.vtk");q4->meta.set_data_float(DCM_FLIP,65);q1->meta.set_data_float(DCM_TR,100);
		q5->load_dataset_from_VTK_file(path+"flip60.vtk");q5->meta.set_data_float(DCM_FLIP,60);q1->meta.set_data_float(DCM_TR,100);
		q6->load_dataset_from_VTK_file(path+"flip55.vtk");q6->meta.set_data_float(DCM_FLIP,55);q1->meta.set_data_float(DCM_TR,100);
		q7->load_dataset_from_VTK_file(path+"flip50.vtk");q7->meta.set_data_float(DCM_FLIP,50);q1->meta.set_data_float(DCM_TR,100);
		q8->load_dataset_from_VTK_file(path+"flip45.vtk");q8->meta.set_data_float(DCM_FLIP,45);q1->meta.set_data_float(DCM_TR,100);
		q9->load_dataset_from_VTK_file(path+"flip40.vtk");q9->meta.set_data_float(DCM_FLIP,40);q1->meta.set_data_float(DCM_TR,100);
		q10->load_dataset_from_VTK_file(path+"flip35.vtk");q10->meta.set_data_float(DCM_FLIP,35);q1->meta.set_data_float(DCM_TR,100);
		q11->load_dataset_from_VTK_file(path+"flip30.vtk");q11->meta.set_data_float(DCM_FLIP,30);q1->meta.set_data_float(DCM_TR,100);
		q12->load_dataset_from_VTK_file(path+"flip25.vtk");q12->meta.set_data_float(DCM_FLIP,25);q1->meta.set_data_float(DCM_TR,100);
		q13->load_dataset_from_VTK_file(path+"flip20.vtk");q13->meta.set_data_float(DCM_FLIP,20);q1->meta.set_data_float(DCM_TR,100);
		q14->load_dataset_from_VTK_file(path+"flip15.vtk");q14->meta.set_data_float(DCM_FLIP,15);q1->meta.set_data_float(DCM_TR,100);

		vector<image_scalar<unsigned short,3> *> v;
		v.push_back(q1);
		v.push_back(q2);
		v.push_back(q3);
		v.push_back(q4);
		v.push_back(q5);
		v.push_back(q6);
		v.push_back(q7);
		v.push_back(q8);
		v.push_back(q9);
		v.push_back(q10);
		v.push_back(q11);
		v.push_back(q12);
		v.push_back(q13);
		v.push_back(q14);

		image_scalar<unsigned short,3> *t1map = q1->calculate_T1Map_3D(v,0);
		t1map->save_to_VTK_file(path+"t1map.vtk");

		*/

/*


		string path = "C:/Joel/TMP/COMBI_30/";
		image_scalar<float,3> *f_re = new image_scalar<float,3>();
		image_scalar<float,3> *f_im = new image_scalar<float,3>();
		image_scalar<float,3> *w_re = new image_scalar<float,3>();
		image_scalar<float,3> *w_im = new image_scalar<float,3>();



		f_re->load_dataset_from_VTK_file(path + "COMBI_30b_Fat_Re.vtk");
		f_im->load_dataset_from_VTK_file(path + "COMBI_30b_Fat_Im.vtk");
		w_re->load_dataset_from_VTK_file(path + "COMBI_30b_Water_Re.vtk");
		w_im->load_dataset_from_VTK_file(path + "COMBI_30b_Water_Im.vtk");

		image_scalar<float,3> *ff = new image_scalar<float,3>(f_re,0);
//		ff->initialize_dataset(f_re->get_size_by_dim(0),f_re->get_size_by_dim(1),f_re->get_size_by_dim(2));
		ff->fill(0);

		complex<float> f;
		complex<float> w;
		complex<float> sum;
		complex<float> res;
		float f_magn=0;
		float w_magn=0;
		float sum_magn;

		for(int m=0;m<f_im->get_size_by_dim(2);m++){
			for(int l=0;l<f_im->get_size_by_dim(1);l++){
				for(int k=0;k<f_im->get_size_by_dim(0);k++){
					f = complex<float>(f_re->get_voxel(k,l,m),f_im->get_voxel(k,l,m));
					w = complex<float>(w_re->get_voxel(k,l,m),w_im->get_voxel(k,l,m));
					sum = f+w;
					sum_magn = sqrt(real(sum)*real(sum)+imag(sum)*imag(sum));

					if(f_magn>=w_magn){
						f_magn = sqrt(real(f)*real(f)+imag(f)*imag(f));
						ff->set_voxel(k,l,m,f_magn/sum_magn );
					}else{
						w_magn = sqrt(real(w)*real(w)+imag(w)*imag(w));
						ff->set_voxel(k,l,m,1-w_magn/sum_magn );
					}

//					res = f/(f+w);
//					ff->set_voxel(k,l,m,sqrt(real(res)*real(res) + imag(res)*imag(res)));
//					ff->set_voxel(k,l,m,sqrt( real(f)/(real(f)+real(w)) ));
				}
			}
		}

//		ff->save_to_VTK_file(path + "COMBI_30b_FF_real.vtk");
		ff->save_to_VTK_file(path + "COMBI_30b_FF_idea.vtk");
*/

		//		string path = "C:/Joel/Data/COMBI/COMBI - All_DIXON_DATA_2007_10_15/";

//		string path = "C:/Joel/TMP/";
/*
		image_scalar<unsigned short,3> *water = new image_scalar<unsigned short,3>();
		water->load_dataset_from_VTK_file(path + "ME10liver.vtk");
		//		water->map_values(250,1000000,250);
		//		water->data_has_changed(true);
		Vector3D seed;
		//		seed[0]=63; seed[1]=123; seed[2]=48;
		seed[0]=50; seed[1]=99; seed[2]=16;
		water->region_grow_robust_3D(seed,220,330,107,2);
		//		water->region_grow_3D(seed,220,330);
		delete water;
*/

/*
		image_scalar<unsigned short,3> *f2 = new image_scalar<unsigned short,3>();
		//		f2->load_dataset_from_VTK_file(path + "COMBI18_8F_percent.vtk");
		f2->load_dataset_from_VTK_file(path + "ME10liver.vtk");

		Vector3D body_seed;
		//		body_seed[0]=68;
		//		body_seed[1]=108;
		//		body_seed[2]=134;
		body_seed[0]=50;
		body_seed[1]=100;
		body_seed[2]=15;

		cout<<"1"<<endl;
		image_binary<3> *f3 = f2->region_grow_3D(body_seed,240,320);
		//		f2->region_grow_3D(body_seed,760);

		cout<<"2"<<endl;
		f3->save_to_VTK_file(path + "ME10liver22_bin.vtk");
		//		f3->save_to_VTK_file(path + "COMBI18_8F_percent_region_grow.vtk");

		cout<<"3"<<endl;

*/

/*

		image_scalar<float,3> *f = new image_scalar<float,3>();
		f->load_dataset_from_VTK_file(path + "COMBI18_8F.vtk");

		image_scalar<float,3> *w = new image_scalar<float,3>();
		w->load_dataset_from_VTK_file(path + "COMBI18_8W.vtk");

		image_scalar<float,3> *sum = new image_scalar<float,3>(f); //data is copied...

		sum->combine(w,COMB_ADD);
		//		sum->save_to_VTK_file(path + "COMBI18_8SUM.vtk");

		image_scalar<unsigned short,3> *body = scalar_copycast<image_scalar, unsigned short, 3>(sum);

		Vector3D body_seed;
		body_seed[0]=68;
		body_seed[1]=108;
		body_seed[2]=134;
		body->region_grow_3D(body_seed,40);
		cout<<"ska save-a..."<<endl;
		body->save_to_VTK_file(path + "COMBI18_8BODY.vtk");
		cout<<"har save-at..."<<endl;

		// ------ FAT and WATER % ---------
		f->combine(sum,COMB_DIV);
		f->scale_by_factor(1000);
		f->data_has_changed();
		w->combine(sum,COMB_DIV);
		w->scale_by_factor(1000);
		w->data_has_changed();

		image_scalar<unsigned short,3> *f2 = scalar_copycast<image_scalar, unsigned short, 3>(f);
		image_scalar<unsigned short,3> *w2 = scalar_copycast<image_scalar, unsigned short, 3>(w);
		f2->save_to_VTK_file(path + "COMBI18_8F_percent.vtk");
		w2->save_to_VTK_file(path + "COMBI18_8W_percent.vtk");
		delete f;
		delete w;
		delete sum;
		datamanagement.add(body);
		datamanagement.add(f2);
		datamanagement.add(w2);

		*/
		/*
		image_scalar<unsigned short,3> *f2 = scalar_copycast<image_scalar, unsigned short, 3>(f);
		f2->save_to_VTK_file(path + "COMBI9_5F_ushort.vtk");

		//		image_scalar<float,3> *w = new image_scalar<float,3>();
		//		w->load_dataset_from_VTK_file(path + "COMBI18_8W.vtk");

		//		image_scalar<float,3> *sum = new image_scalar<float,3>(f); //data is copied...


		Vector3D sat_seed;

		//		sat_seed[0]=212;
		//		sat_seed[1]=138;
		//		sat_seed[2]=138;
		//		f->region_grow_3D(sat_seed,85);
		//		f->save_to_VTK_file(path + "COMBI18_8F_SAT.vtk");

		sat_seed[0]=35;
		sat_seed[1]=112;
		sat_seed[2]=109;
		//		f->region_grow_3D(sat_seed,184);
		//		f->save_to_VTK_file(path + "COMBI9_5F_SAT.vtk");
		*/
		//35 112 109
		//184

		//-------------------------
		//212 // 138 //138..

		/*
		image_scalar<unsigned short,3> *slice2Dhist = fat->create_slicewise_2Dhistograms_3D(water,0,true,500,250);

		fat->name("fat");
		water->name("water");
		slice2Dhist->name("slice2Dhist");

		datamanagement.add(fat);
		datamanagement.add(water);
		datamanagement.add(slice2Dhist);
		*/





		/*
		string path = "C:/Joel/TMP/COMBI_EL/";
		image_integer<unsigned short,3> *f1 = new image_integer<unsigned short,3>();
		f1->load_dataset_from_VTK_file("C:/Joel/TMP/COMBI_EL/7_EL_Flip20.vtk");
		datamanagement.add(f1);
		*/
		/*
		string path = "C:/Joel/Data/COMBI/COMBI - All_DIXON_DATA_2007_09_19/";

		image_scalar<unsigned short,3> *f1 = new image_scalar<unsigned short,3>();
		//		image_scalar<unsigned short,3> *f2 = new image_scalar<unsigned short,3>();
		f1->load_dataset_from_VTK_file(path + "COMBI0_6_1716_96_380_76_32bit_Fat.vtk");
		//		f2->load_dataset_from_VTK_file(path + "COMBI0_6_1716_96_380_76_32bit_Water.vtk");
		image_scalar<unsigned short,3> *f2 = new image_scalar<unsigned short,3>(f1);
		f2->save_histogram_to_txt_file("c:/Joel/TMP/COMBI0_6_constructor_copied_hist.txt");
		*/

		//		point_collection p();
		//		datamanagement.add(p);

		/*


		//Multistack...


		//		string path = "C:/Joel/TMP/\COMBI_11/";
		//7_T1Map_Multistack_Flip10_05_TR205_TE095_1.vtk

		//		string path = "C:/Joel/TMP/T1Map3DMultistack/";
		//		string path = "C:/Joel/TMP/COMBI_EL/";

		//		string path = "C:/Joel/TMP/COMBI - 3 T1Map Test EL BE/VTK_DATA/";
		//		string path = "C:/Joel/TMP/COMBI_13_ACS_T1maptest/T1MAPTES/20071008/";
		//		string path = "C:/Joel/TMP/COMBI_16/VTK/";
		string path = "C:/Joel/TMP/COMBI_17_COMBI_Mst_comparison_MT/VTK/";

		image_scalar<unsigned short,3> *f1 = new image_scalar<unsigned short,3>();
		image_scalar<unsigned short,3> *f2 = new image_scalar<unsigned short,3>();
		image_scalar<unsigned short,3> *f3 = new image_scalar<unsigned short,3>();
		image_scalar<unsigned short,3> *f4 = new image_scalar<unsigned short,3>();

		f1->load_dataset_from_VTK_file(path + "9_T1Map_Multistation_TR5_Flip7_4_Stack1.vtk");
		f2->load_dataset_from_VTK_file(path + "9_T1Map_Multistation_TR5_Flip7_4_Stack2.vtk");
		f3->load_dataset_from_VTK_file(path + "9_T1Map_Multistation_TR5_Flip7_4_Stack3.vtk");
		f4->load_dataset_from_VTK_file(path + "9_T1Map_Multistation_TR5_Flip7_4_Stack4.vtk");

		//		f1->load_dataset_from_all_DICOM_files_in_dir(path + "MR801/");

		//		f1->save_to_VTK_file(path+"201.vtk");

		image_general<unsigned short,3> *f1a = f1->get_subvolume_from_slices_3D(0,2);
		image_general<unsigned short,3> *f2a = f2->get_subvolume_from_slices_3D(0,2);
		image_general<unsigned short,3> *f3a = f3->get_subvolume_from_slices_3D(0,2);
		image_general<unsigned short,3> *f4a = f4->get_subvolume_from_slices_3D(0,2);

		image_general<unsigned short,3> *f1b = f1->get_subvolume_from_slices_3D(1,2);
		image_general<unsigned short,3> *f2b = f2->get_subvolume_from_slices_3D(1,2);
		image_general<unsigned short,3> *f3b = f3->get_subvolume_from_slices_3D(1,2);
		image_general<unsigned short,3> *f4b = f4->get_subvolume_from_slices_3D(1,2);

		f4a->add_volume_3D(f3a);
		f4a->add_volume_3D(f2a);
		f4a->add_volume_3D(f1a);
		f4a->save_to_VTK_file(path+"9_T1Map_Flip7.vtk");

		f4b->add_volume_3D(f3b);
		f4b->add_volume_3D(f2b);
		f4b->add_volume_3D(f1b);
		f4b->save_to_VTK_file(path+"9_T1Map_Flip4.vtk");

		datamanagement.add(f4a);
		datamanagement.add(f4b);
		delete f1; delete f1a; delete f1b; //delete f1c;
		delete f2; delete f2a; delete f2b; //delete f2c;
		delete f3; delete f3a; delete f3b; //delete f3c;
		delete f4; //delete f4a; delete f4b; //delete f4c;
		//		delete f5; delete f5a; delete f5b; //delete f5c;
		//		delete f6; delete f6a; delete f6b; //delete f6c;
		//		delete f7; //delete f7a; delete f7b;
		//		delete f8; //delete f8a; delete f8b;
		//		delete f9; 
		*/
	}
}


//radius_at_bottom = 
//Dark region...
//slice 1 (top)	1pixels 2mm	23
//slice 2		16		32	14
//slice 3		21		43	4
//slice 4		23		47	0
//x		y
//0		0
//4		8
//14	16
//23	24
//32	16
//42	8
//46	0
//excel --> y=-0,039x^2 + 1,818x + 0,3768


// *** end of userIO functions ***

int main(int argc, char *argv[])
{
	//start up Platinum
	platinum_init();

	//set up main window

	const int w_margin=15*2;
	int windowwidth = 1100-w_margin;
	int windowheight = 800-w_margin;

	Fl_Window window(1100,800);

	//prepare Platinum for userIO creation
	platinum_setup(window,3,2,300);


	//	image_scalar<unsigned short,3> *tmp;
	//	tmp->load_dataset_from_VTK_file("C:/Joel/TMP/Pivus75/750077_MR301_TE100.vtk");
	//	datamanagement.add(tmp);

	// *** begin userIO control definitions ***


	int test_ID=userIOmanagement.add_userIO("Test..",test_function,"Test..");
	userIOmanagement.finish_userIO(test_ID);

	//	int rg_ID=userIOmanagement.add_userIO("RegionGrow..",rg_function,"Grow..");
	//	userIOmanagement.add_par_voxelseed(rg_ID,"Seed");
	//	userIOmanagement.finish_userIO(rg_ID);

	/*
	int test_ID=userIOmanagement.add_userIO("Test..",test_function,"Test..");
	userIOmanagement.finish_userIO(test_ID);

	int hist_ID=userIOmanagement.add_userIO("Histogram saving..",hist_function,"Save..");
	userIOmanagement.add_par_image(hist_ID,"Image");
	userIOmanagement.finish_userIO(hist_ID);
	*/

	/*

	// --------------------------
	int smooth_ID=userIOmanagement.add_userIO("Smooth ITK",smooth_function,"Smooth");
	userIOmanagement.finish_userIO(smooth_ID);






	// --------------------------
	int tif_save_ID=userIOmanagement.add_userIO("Tif saving",tif_save_function,"Save");
	userIOmanagement.finish_userIO(tif_save_ID);
	*/
	/*
	// --------------------------
	int slc_reorg_ID=userIOmanagement.add_userIO("Slice Reorganization",slc_reorg_function,"Start");
	userIOmanagement.add_par_image(slc_reorg_ID,"Image volume");
	userIOmanagement.add_par_longint_box(slc_reorg_ID,"No Contrasts");
	userIOmanagement.add_par_longint_box(slc_reorg_ID,"No Dynamics");
	//No slices per "stack" is calculated from param above + nz...

	//userIOmanagement.add_par_message(slc_reorg_ID,"par_message...","hej...");
	Vector3D v;
	v.Fill(123);
	userIOmanagement.add_par_coordinate3Ddisplay(slc_reorg_ID,"NOSE","brightest...", v);
	userIOmanagement.add_par_coordinate3Ddisplay(slc_reorg_ID,"EAR","darkest...", v);
	userIOmanagement.finish_userIO(slc_reorg_ID);

	//-------------

	int reslice_ID=userIOmanagement.add_userIO("SubVolume Reslice Tool...",reslice_function,"Reslice");
	userIOmanagement.add_par_image(reslice_ID,"Volume...");
	userIOmanagement.add_par_longint_box(reslice_ID,"Slice direction (0,1,2)",2,0);
	userIOmanagement.add_par_longint_box(reslice_ID,"Start slice",1000,0);
	userIOmanagement.add_par_longint_box(reslice_ID,"Every 'X' slice",1000,1);
	userIOmanagement.add_par_string(reslice_ID,"Save path_and_file","Test_save.vtk");
	userIOmanagement.finish_userIO(reslice_ID);

	//-------------

	int t1_mapping_ID=userIOmanagement.add_userIO("T1-Mapping calculator",t1_mapping_function,"Map T1");
	userIOmanagement.add_par_image(t1_mapping_ID,"Large flip volume");
	userIOmanagement.add_par_float_box(t1_mapping_ID,"TR (Overrides dcm info, if not 0)",5000,0);
	userIOmanagement.add_par_float_box(t1_mapping_ID,"TE (Overrides dcm info, if not 0)",5000,0);
	userIOmanagement.add_par_longint_box(t1_mapping_ID,"Flip angle (0-180)",180,0);
	userIOmanagement.add_par_string(t1_mapping_ID,"Large - Dcm file (leave empty if not needed)","C:/Joel/flip80.dcm");

	userIOmanagement.add_par_image(t1_mapping_ID,"Small flip volume");
	userIOmanagement.add_par_longint_box(t1_mapping_ID,"Flip angle (0-180)",180,0);
	userIOmanagement.add_par_string(t1_mapping_ID,"Small - Dcm file (leave empty if not needed)","C:/Joel/flip30.dcm");
	userIOmanagement.finish_userIO(t1_mapping_ID);



	//-------------

	int t1_multi_ID=userIOmanagement.add_userIO("T1 multiflip...",t1_multiflip_function,"Multi..");
	userIOmanagement.add_par_float_box(t1_multi_ID,"TR",5000,0);
	userIOmanagement.add_par_longint_box(t1_multi_ID,"Body Int Thres",1000,0);

	userIOmanagement.add_par_image(t1_multi_ID,"Flip1 (Largest)");
	userIOmanagement.add_par_longint_box(t1_multi_ID,"Flip angle (0-180)",180,0);

	userIOmanagement.add_par_image(t1_multi_ID,"Flip2");
	userIOmanagement.add_par_longint_box(t1_multi_ID,"Flip angle (0-180)",180,0);

	userIOmanagement.add_par_image(t1_multi_ID,"Flip3");
	userIOmanagement.add_par_longint_box(t1_multi_ID,"Flip angle (0-180)",180,0);

	userIOmanagement.add_par_image(t1_multi_ID,"Flip4");
	userIOmanagement.add_par_longint_box(t1_multi_ID,"Flip angle (0-180)",180,0);

	userIOmanagement.add_par_image(t1_multi_ID,"Flip5");
	userIOmanagement.add_par_longint_box(t1_multi_ID,"Flip angle (0-180)",180,0);
	userIOmanagement.finish_userIO(t1_multi_ID);
*/
	// *** end userIO control definitions ***



	//finish the window creation
	window.end();

	window.show(argc, argv);

	return Fl::run();
}
