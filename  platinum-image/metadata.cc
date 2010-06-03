// $Id: $

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

#include "metadata.h"
using namespace std;

metadata::metadata()
{
	meta_map = map<string,base*>();
}

metadata::metadata (metadata * const source)
{
//	cout<<"metadata - copy constructor..."<<endl;
	pt_error::error_if_null(source,"Attempting to copyconstruct metadata object from NULL object");

	meta_map = source->meta_map;

//	copy_all_parameters(*source);
//	TR = source->TR;
}

void metadata::set_data_int(string key, int data)
{meta_map[key]= new base_int(data);}

void metadata::set_data_float(string key, float data)
{meta_map[key]= new base_float(data);}

void metadata::set_data_string(string key, string data)
{meta_map[key]= new base_string(data);}


int metadata::get_data_int(string key)
{
	int s=0;
	base* b = meta_map[key];
	if(b!=NULL){
		b->get(s,key);
	}else{
		pt_error::error("No metadata was found for key="+key, pt_error::debug);
	}
	return s;
}

string metadata::get_data_as_string ( std::string key )
{
	base * b = meta_map[key];
	std::string s;
	if ( b != NULL )
		{ s = b->get_data_as_string( key ); }
	else
		{ pt_error::error ( "No metadata was found for key = " + key, pt_error::debug ); }
		
	return s;
}

float metadata::get_data_float(string key)
{
	float s=0;
	base* b = meta_map[key];
	if(b!=NULL){
		b->get(s,key);
	}else{
		pt_error::error("No metadata was found for key="+key, pt_error::debug);
	}
	return s;
}

string metadata::get_data_string(string key)
{
	string s="";
	base* b = meta_map[key];
	if(b!=NULL){
		b->get(s,key);
	}else{
		pt_error::error("No metadata was found for key="+key, pt_error::debug);
	}
	return s;
}


/*
metadata::~metadata()
{
}
*/


void metadata::read_metadata_from_dcm_file(string dcm_file)
{
//	cout<<"read_metadata_from_dcm_file("<<dcm_file<<")"<<endl;
     	//itk::GDCMImageIO::Pointer dcmIO; //Allows simple dicom meta data import
		//dicom might store other data types than image data... 
		//dcmIO->GetFileName()=="" if meta data is not available from dicom file.
		//dcmIO->SetFileName("...filename...");
		//dcmIO->ReadImageInformation();		//Needs to be called before accessing data...
		//dcmIO->GetValueFromTag("0010"+"|"+"0010",string dcmdata);

//	cout<<"load_private_tags="<<itk::GDCMImageIO::GetLoadPrivateTagsDefault()<<endl;

//	itk::GDCMImageIO::SetLoadPrivateTagsDefault(true); 
//	cout<<"load_private_tags="<<itk::GDCMImageIO::GetLoadPrivateTagsDefault()<<endl;

	itk::GDCMImageIO::Pointer dcmIO = itk::GDCMImageIO::New();		//Allows simple dicom meta data import
	dcmIO->SetFileName(dcm_file);
	if( !dcmIO->CanReadFile(dcm_file.c_str()) ){
		pt_error::error("metadata::read_metadata_from_dcm_file...",pt_error::debug);
	}else{
		dcmIO->SetLoadPrivateTagsDefault(true);	//to load private tags like b-value //2001:1003
		dcmIO->GetMetaDataDictionary().Print(cout);
//		dcmIO->GetMetaDataDictionary().Print(cout);
		dcmIO->ReadImageInformation();			//Needs to be called before accessing data...

		add_dcm_data_string(dcmIO,DCM_CREATION_DATE);
		add_dcm_data_string(dcmIO,DCM_CREATION_TIME);
		add_dcm_data_string(dcmIO,DCM_STUDY_DATE);
		add_dcm_data_string(dcmIO,DCM_SERIES_DATE);
		add_dcm_data_string(dcmIO,DCM_SERIES_ID);
		add_dcm_data_string(dcmIO,DCM_MODALITY);
		add_dcm_data_string(dcmIO,DCM_IMAGE_TYPE);
		add_dcm_data_string(dcmIO,DCM_STUDY_DESCRIPTION);
		add_dcm_data_string(dcmIO,DCM_SERIES_DESCRIPTION);

		add_dcm_data_string(dcmIO,DCM_PATIENT_NAME);
		add_dcm_data_string(dcmIO,DCM_PATIENT_ID);
		add_dcm_data_string(dcmIO,DCM_PATIENT_BIRTH_DATE);
		add_dcm_data_string(dcmIO,DCM_PATIENT_SEX);
		add_dcm_data_float(dcmIO,DCM_PATIENT_WEIGHT);

		add_dcm_data_string(dcmIO,DCM_SCANNING_SEQ);
		add_dcm_data_string(dcmIO,DCM_SEQ_VARIANT);
		add_dcm_data_string(dcmIO,DCM_SCAN_OPTION);
		add_dcm_data_string(dcmIO,DCM_MR_ACQ_TYPE);
		add_dcm_data_float(dcmIO,DCM_TR);
		add_dcm_data_float(dcmIO,DCM_TE);
		add_dcm_data_float(dcmIO,DCM_FLIP);
		add_dcm_data_int(dcmIO,DCM_NSA);
		add_dcm_data_float(dcmIO,DCM_IMAGING_FREQUENCY);
		add_dcm_data_float(dcmIO,DCM_SLICE_THICKNESS);
		add_dcm_data_float(dcmIO,DCM_SLICE_SPACING);
		add_dcm_data_float(dcmIO,DCM_FOV);
		add_dcm_data_float(dcmIO,DCM_B0);
		add_dcm_data_int(dcmIO,DCM_NO_PHASE_ENC);
		add_dcm_data_float(dcmIO,DCM_PERCENT_SAMPLING);
		add_dcm_data_float(dcmIO,DCM_RFOV);
		add_dcm_data_string(dcmIO,DCM_PROTOCOL_NAME);
		add_dcm_data_string(dcmIO,DCM_TRANSMIT_COIL);

		add_dcm_data_string(dcmIO,DCM_RECEIVE_COIL);
		add_dcm_data_string(dcmIO,DCM_PHASE_ENCODING_DIRECTION);
		add_dcm_data_string(dcmIO,DCM_PATIENT_POSITION);
		add_dcm_data_string(dcmIO,DCM_PULSE_SEQUENCE_NAME);

		add_dcm_data_int(dcmIO,DCM_SERIES_NUMBER);
		add_dcm_data_int(dcmIO,DCM_ACQ_NUMBER);
		add_dcm_data_string(dcmIO,DCM_IMAGE_POSITION_PATIENT);
		add_dcm_data_string(dcmIO,DCM_IMAGE_ORIENTATION_PATIENT);

		add_dcm_data_int(dcmIO,DCM_ROWS);
		add_dcm_data_int(dcmIO,DCM_COLUMNS);
		add_dcm_data_string(dcmIO,DCM_PIXEL_SPACING);
		add_dcm_data_int(dcmIO,DCM_BITS_ALLOCATED);
		add_dcm_data_int(dcmIO,DCM_BITS_STORED);
		add_dcm_data_int(dcmIO,DCM_HIGH_BIT);
		add_dcm_data_float(dcmIO,DCM_WINDOW_CENTER);
		add_dcm_data_float(dcmIO,DCM_WINDOW_WIDTH);
		add_dcm_data_float(dcmIO,DCM_RESCALE_INTERCEPT);
		add_dcm_data_float(dcmIO,DCM_RESCALE_SLOPE);
		add_dcm_data_string(dcmIO,DCM_RESCALE_TYPE);

		add_dcm_data_float(dcmIO,DCM_SCALE_INTERCEPT);
		add_dcm_data_float(dcmIO,DCM_SCALE_SLOPE);
		add_dcm_data_float(dcmIO,DCM_SCALE_INTERCEPT2);
		add_dcm_data_float(dcmIO,DCM_SCALE_SLOPE2);

		add_dcm_data_string(dcmIO,DCM_SLICE_ORIENTATION); // To make this work ollowing was commented from "itkGDCMImageIO.cxx" 
		add_dcm_data_float(dcmIO,DCM_B_VALUE);				//diffusion b-value

		//if( v->GetName() != gdcm::GDCM_UNKNOWN )	//JK removed this 2009-02-18
		//{EncapsulateMetaData<std::string>(dico, v->GetKey(), v->GetValue() );}
	}
}

int metadata::get_dcm_parameter_as_int(itk::GDCMImageIO::Pointer dcmIO, string DCM_TAG_STRING)
{
	string s = get_dcm_parameter_as_string(dcmIO, DCM_TAG_STRING);
	int i = float(atoi(s.c_str()));
	return i;
}

float metadata::get_dcm_parameter_as_float(itk::GDCMImageIO::Pointer dcmIO, string DCM_TAG_STRING)
{
	string s = get_dcm_parameter_as_string(dcmIO, DCM_TAG_STRING);
	float f = float(atof(s.c_str()));
	return f;
}

string metadata::get_dcm_parameter_as_string(itk::GDCMImageIO::Pointer dcmIO, string DCM_TAG_STRING)
{
	string dcmdata;
	dcmIO->GetValueFromTag(DCM_TAG_STRING,dcmdata);
	return dcmdata;
}

/*
float metadata::get_dx()
{
	return 0;//JK warning...
}
*/

float metadata::get_flip()
{
	return get_data_float(DCM_FLIP);
}

float metadata::get_te()
{
	return get_data_float(DCM_TE);
 }

std::string metadata::get_name()
{
	std::vector<std::string> name;
	
	name.push_back( DCM_SCANNING_SEQ );
	name.push_back( DCM_PROTOCOL_NAME );
	name.push_back( DCM_TR );
	name.push_back( DCM_TE );
	name.push_back( DCM_FLIP );
	name.push_back( DCM_PULSE_SEQUENCE_NAME );
	name.push_back( DCM_STUDY_DESCRIPTION );
	name.push_back( DCM_SERIES_DESCRIPTION );
	name.push_back( DCM_PATIENT_BIRTH_DATE );
		
	stringstream namestring;
	for ( std::vector<std::string>::iterator itr = name.begin(); itr != name.end(); itr++ )
	{
		std::string temp = get_data_as_string ( *itr );
	
		if ( itr != name.begin() )
			{ namestring << "/"; }
	
		if ( temp != "" )
			{ namestring << temp; }
		else
			{ namestring << "#"; }
	}
	
	return namestring.str();
}

std::string metadata::get_slice_orientation()
{
	string s = get_data_as_string(DCM_SLICE_ORIENTATION);

	if( string_contains(s,"SAGITTAL") ){
		return "sagittal";
	}
	else if( string_contains(s,"CORONAL") ){
		return "coronal";
	}
	else if( string_contains(s,"TRANSVERSAL") || string_contains(s,"AXIAL") ){
		return "axial";
	}

	return "undefined";
}

float metadata::get_b_value()
{
//	string s = get_data_string(DCM_B_VALUE);
//	return atof(s.c_str());
	return get_data_float(DCM_B_VALUE);
}

void metadata::set_b_value(float b)
{
	set_data_float(DCM_B_VALUE,b);
}


metadata& metadata::operator=(const metadata& source)
{
//	cout<<"metadata - operator (=) ..."<<endl;
	meta_map = source.meta_map;
	return *this;
}


void metadata::print_all()
{
	cout<<"metadata - print_all()"<<endl;
	print_string(DCM_CREATION_DATE);
	print_string(DCM_CREATION_TIME);
	print_string(DCM_STUDY_DATE);
	print_string(DCM_SERIES_DATE);
	print_string(DCM_SERIES_ID);
	print_string(DCM_MODALITY);
	print_string(DCM_IMAGE_TYPE);
	print_string(DCM_STUDY_DESCRIPTION);
	print_string(DCM_SERIES_DESCRIPTION);

	print_string(DCM_PATIENT_NAME);
	print_string(DCM_PATIENT_ID);
	print_string(DCM_PATIENT_BIRTH_DATE);
	print_string(DCM_PATIENT_SEX);
	print_float(DCM_PATIENT_WEIGHT);

	print_string(DCM_SCANNING_SEQ);
	print_string(DCM_SEQ_VARIANT);
	print_string(DCM_SCAN_OPTION);
	print_string(DCM_MR_ACQ_TYPE);
	print_float(DCM_TR);
	print_float(DCM_TE);
	print_float(DCM_FLIP);
	print_int(DCM_NSA);
	print_float(DCM_IMAGING_FREQUENCY);
	print_float(DCM_SLICE_THICKNESS);
	print_float(DCM_SLICE_SPACING);
	print_float(DCM_FOV);
	print_float(DCM_B0);
	print_int(DCM_NO_PHASE_ENC);
	print_float(DCM_PERCENT_SAMPLING);
	print_float(DCM_RFOV);
	print_string(DCM_PROTOCOL_NAME);
	print_string(DCM_TRANSMIT_COIL);
	print_string(DCM_RECEIVE_COIL);
	print_string(DCM_PHASE_ENCODING_DIRECTION);
	print_string(DCM_PATIENT_POSITION);
	print_string(DCM_PULSE_SEQUENCE_NAME);

	print_int(DCM_SERIES_NUMBER);
	print_int(DCM_ACQ_NUMBER);
	print_string(DCM_IMAGE_POSITION_PATIENT);
	print_string(DCM_IMAGE_ORIENTATION_PATIENT);

	print_int(DCM_ROWS);
	print_int(DCM_COLUMNS);
	print_string(DCM_PIXEL_SPACING);
	print_int(DCM_BITS_ALLOCATED);
	print_int(DCM_BITS_STORED);
	print_int(DCM_HIGH_BIT);
	print_float(DCM_WINDOW_CENTER);
	print_float(DCM_WINDOW_WIDTH);
	print_float(DCM_RESCALE_INTERCEPT);
	print_float(DCM_RESCALE_SLOPE);
	print_string(DCM_RESCALE_TYPE);

	print_float(DCM_SCALE_INTERCEPT);
	print_float(DCM_SCALE_SLOPE);
	print_float(DCM_SCALE_INTERCEPT2);
	print_float(DCM_SCALE_SLOPE2); 

	print_string(DCM_SLICE_ORIENTATION);
	print_float(DCM_B_VALUE);
}

void metadata::add_dcm_data_int(itk::GDCMImageIO::Pointer dcmIO, string DCM_TAG)
{
	set_data_int(DCM_TAG,get_dcm_parameter_as_int(dcmIO,DCM_TAG));
}

void metadata::add_dcm_data_float(itk::GDCMImageIO::Pointer dcmIO, string DCM_TAG)
{
	set_data_float(DCM_TAG,get_dcm_parameter_as_float(dcmIO,DCM_TAG));
}

void metadata::add_dcm_data_string(itk::GDCMImageIO::Pointer dcmIO, string DCM_TAG)
{
	set_data_string(DCM_TAG,get_dcm_parameter_as_string(dcmIO,DCM_TAG));
}


void metadata::print_int(string key)
{
	cout<<key<<" --> "<<get_data_int(key)<<endl;
}

void metadata::print_float(string key)
{
	cout<<key<<" --> "<<get_data_float(key)<<endl;
}
void metadata::print_string(string key)
{
	cout<<key<<" --> "<<get_data_string(key)<<endl;
}

std::string base_int::get_data_as_string ( std::string key )
{
	stringstream ss;
	ss << value;
	return ss.str();
}

std::string base_float::get_data_as_string ( std::string key )
{
	stringstream ss;
	ss << value;
	return ss.str();
}

std::string base_string::get_data_as_string ( std::string key )
{
	return value;
}

