//////////////////////////////////////////////////////////////////////////
//
//  image_generalfile.hxx $Revision $
//
//  File I/O functions implemented in image_general
//
//  $LastChangedBy $
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

#include "fileutils.h"
#include "datamanager.h"
extern datamanager datamanagement;

template <class VOXTYPE, template <class,int=3 > class IMGCLASS>
void try_allocate (image_base* &i,unsigned int voxel_type, std::vector<std::string> files, long width, long height, bool bigEndian, long headerSize, Vector3D voxelSize)
{
    if (i == NULL && sizeof (VOXTYPE) == voxel_type/8 )
        i = new IMGCLASS<VOXTYPE> (files, width, height, bigEndian, headerSize, voxelSize);
}

template <template <class,int=3 > class IMGCLASS>
image_base* allocate_image (bool floatType, bool signedType, unsigned int voxel_type, std::vector<std::string> files, long width, long height, bool bigEndian, long headerSize, Vector3D voxelSize)
{
    image_base* output = NULL;
    
    if (floatType)
        {
        try_allocate<float,IMGCLASS>(output, voxel_type, files, width, height, bigEndian, headerSize, voxelSize);
        try_allocate<double,IMGCLASS>(output, voxel_type, files, width, height, bigEndian, headerSize, voxelSize);
        //try_allocate<long double,IMGCLASS>(output, voxel_type, files, width, height, bigEndian, headerSize, voxelSize);
        }
    else
        {
        if (signedType)
            {
            try_allocate<signed char,IMGCLASS >(output, voxel_type, files, width, height, bigEndian, headerSize, voxelSize);
            try_allocate<signed short,IMGCLASS >(output, voxel_type, files, width, height, bigEndian, headerSize, voxelSize);
            try_allocate<signed long,IMGCLASS >(output, voxel_type, files, width, height, bigEndian, headerSize, voxelSize);
            //try_allocate<signed long long,IMGCLASS >(output, voxel_type, files, width, height, bigEndian, headerSize, voxelSize);
            }
        else
            {
            try_allocate<unsigned char,IMGCLASS >(output, voxel_type, files, width, height, bigEndian, headerSize, voxelSize);
            try_allocate<unsigned short,IMGCLASS >(output, voxel_type, files, width, height, bigEndian, headerSize, voxelSize);
            try_allocate<unsigned long,IMGCLASS >(output, voxel_type, files, width, height, bigEndian, headerSize, voxelSize);
            //try_allocate<long long,IMGCLASS >(output, voxel_type, files, width, height, bigEndian, headerSize, voxelSize);
            }
        }
    
    return output;
}

template <class ELEMTYPE>
ELEMTYPE * load_pixels (unsigned long &num_elements,std::vector<std::string> files, bool bigEndian, long headerSize = 0, unsigned int startFile= 1,unsigned int increment = 1)
    {
    ELEMTYPE * output = NULL;
    num_elements = 0;

    //unsigned long sliceSize = width * height;
    std::string dirname;
    image_load_mode mode = undefined;
    
    //TODO: start & increment values
    
    struct stat fileStats;
    
    if (stat(files.front().c_str(), &fileStats) == 0)
        {
        if (files.size() == 1)
            {
            std::string dirpath = files.front();

            if (S_ISDIR(fileStats.st_mode))
                {
					std::vector<std::string> dir_files = get_dir_entries (dirpath);

                if (dir_files.size() > 0)
                    {
                    dirname = path_end (dirpath);
                    
                    files = dir_files;
                    
                    mode = multifile;
                    }
                }
            else
                {
                mode = single_file;
                dirname = path_parent (files.front());
                }
            }
        }
    else
        {
        pt_error::error ("Opening raw: couldn't get stat for \"" + files.front() + "\"",pt_error::fatal);
        }

    //size of *first* file
    unsigned long dataSize = fileStats.st_size - headerSize; //!header size always in bytes

    if (mode == single_file)
        { num_elements = dataSize/sizeof(ELEMTYPE); }

    if (mode == multifile)
        { num_elements = files.size()*(dataSize/sizeof(ELEMTYPE)); }

    output = new ELEMTYPE [num_elements];
    ELEMTYPE * writepointer = output;

    if (mode == single_file)
        {
        //One multi-slice file
        
        //unsigned long depth = (fileSize - headerSize) / (sliceSize*sizeof (ELEMTYPE));
        
        //initialize_dataset(width, height, depth);
        
        std::ifstream stackFile (files.front().c_str(), std::ios::in | std::ios::binary);
        stackFile.seekg (headerSize);
        
        stackFile.read (reinterpret_cast<FILEPOSTYPE*>(writepointer), sizeof (ELEMTYPE) * num_elements);
        
        adjust_endian (output, num_elements,bigEndian);
        
        stackFile.close();
        }
    
    if (mode == multifile)
        {
        //unsigned long depth = files.size();
        //initialize_dataset(width, height, depth);
        
        std::vector<std::string>::iterator fileItr = files.begin();
        
        while (fileItr != files.end() && writepointer < (output + num_elements))
            {
            //Multiple files, each containing one slice
            std::ifstream imageFile ((*fileItr).c_str(), std::ios::in | std::ios::binary);
            
            if (imageFile.good()) //check that everything is OK for reading
                {
                //header is skipped at the start of each file
                //Note: header size is in bytes (FILEPOSTYPE) regardless of image data type           
                imageFile.seekg (headerSize);
                
                imageFile.read (reinterpret_cast<FILEPOSTYPE*>(writepointer), sizeof (ELEMTYPE) * dataSize);
                writepointer +=dataSize;
                
                imageFile.close();
                }
            }
        adjust_endian (output, num_elements,bigEndian);
        }

    return output;
    }

template <class ELEMTYPE, int IMAGEDIM>
image_general<ELEMTYPE, IMAGEDIM>::image_general(ELEMTYPE * inData, unsigned long numElements, long width, long height, Vector3D voxelSize)
//raw from data constructor
    {
    if (inData != NULL)
        {
        //set voxel size, will be used to calculate
        //unit_to_voxel when initialize_dataset is called

        initialize_dataset (width,height,numElements/(width * height),inData);

        this->voxel_size = voxelSize;
        
        //image_has_changed(true);
        }
    else
        {
        throw ("Image was not created",pt_error::fatal);
        }
    }

template <class ELEMTYPE, int IMAGEDIM>
image_general<ELEMTYPE, IMAGEDIM>::image_general(std::vector<std::string> files, long width, long height, bool bigEndian, long headerSize, Vector3D voxelSize, unsigned int startFile,unsigned int increment)
//raw from file constructor
    {
    unsigned long numElements;
    ELEMTYPE * data = load_pixels<ELEMTYPE>(numElements, files,bigEndian,headerSize,  startFile,increment);

    if (data != NULL)
        {
        //set voxel size, will be used to calculate
        //unit_to_voxel when initialize_dataset is called
        
        initialize_dataset (width,height,numElements/(width * height),data);
        
        this->voxel_size = voxelSize;
        
		delete[] data; //To avoid memory leak !!!!!!!
        this->name_from_path(files.front());

        //image_has_changed(true);
        }
    else
        {
        //TODO: if imageptr == NULL at this point, image has not been read and error has
        //to be reported somehow
        }
    }

/*

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::load_dataset_from_DICOM_files(std::string dir_path,std::string seriesIdentifier)
    {  
	std::cout << "--- load_dataset_from_DICOM_files" << std::endl;



    typedef itk::GDCMSeriesFileNames                NamesGeneratorType;

    itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();


    NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();
    nameGenerator->SetLoadPrivateTags ( true ); 
    nameGenerator->SetUseSeriesDetails( false );    //no details - crucial because existing
    //UID has to match seriesIdentifier
    nameGenerator->SetDirectory(dir_path.c_str());

    typedef std::vector< std::string > FileNamesContainer; 
    FileNamesContainer fileNames; 
    fileNames = nameGenerator->GetFileNames( seriesIdentifier );     

	if(fileNames.size()>0){
		//dcmIO->SetFileName(fileNames[0].c_str());					//JK1
	}
    // *** initialize reader ***

    typename theSeriesReaderType::Pointer reader = theSeriesReaderType::New();
    reader->SetFileNames( fileNames );

    reader->SetImageIO( dicomIO );

    typename theImagePointer image = theImageType::New();

    // *** do reading ***

    try{
        reader->Update();
        }
    catch (itk::ExceptionObject &ex)
        {
        std::cout<<ex<<std::endl;
        }

    image = reader->GetOutput();

//	reader->	//JK1 - Load meta data from dicom reader...

    // *** transfer image data to our platform's data structure ***
    replicate_itk_to_image(image);

    // *** get DICOM metadata ***

    ostringstream namestring;
    const string tagkey []=
        {
        "0018|0020",
        "0018|1030",
        "0018|0080",
        "0018|0081",
        "0018|1314",
        "0018|9005",
        "0008|1030",
        "0008|103E",
        "0010|0030"
        };

    const unsigned int num_name_tags = 9;

    std::string namebase_tag;

    std::string labelId;

    //set name in the form
    //study description-series description-patient's birth date
    //# for empty fields
    for (unsigned int t=0;t <num_name_tags ; t++)
        {
        namebase_tag="";

        if (t >0)
            {namestring<< "/";}

        if( dicomIO->GetValueFromTag(tagkey[t],namebase_tag ) && namebase_tag!="")
            {
            namestring << namebase_tag;}
        else
            {namestring << "#";} 
        }

    name(namestring.str());

    this->from_file(true);

	this->meta.read_metadata_from_dcm_file(fileNames[0].c_str());	//JK1 - Loads meta data from first dicom file in vector...
    }
*/

/*
template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::load_dataset_from_DICOM_files2(std::string dir_path,std::string seriesIdentifier)
{
	std::cout << "--- load_dataset_from_DICOM_files2" << std::endl;


//	 message from ITK/Examples/IO/DicomSeriesReadImageWrite.cxx
//   
//   The DICOM classes illustrated here are OBSOLETE and are
//   scheduled for being removed from the toolkit.
//
//   Please refer to DicomSeriesReadImageWrite2.cxx instead, where the GDCM
//   classes are used. GDCM classes are the ones currently recommended for
//   performing reading and writing of DICOM images.




//  typedef itk::Image<unsigned short,5> ImageNDType;
//  typedef itk::ImageSeriesReader<ImageNDType> ReaderType;

	typedef itk::ImageSeriesReader<theImageType> ReaderType;

	itk::DICOMImageIO2::Pointer io = itk::DICOMImageIO2::New();
	itk::DICOMSeriesFileNames::Pointer names = itk::DICOMSeriesFileNames::New();
	names->SetDirectory(dir_path.c_str());
	  
	typename ReaderType::Pointer reader = ReaderType::New();
//	reader->SetFileNames(names->GetFileNames(seriesIdentifier));
	//  reader->SetFileNames(names->GetFileNames());

    std::vector<std::string> fileNames; 
    fileNames = names->GetFileNames(seriesIdentifier);     
	reader->SetFileNames(fileNames);


	reader->SetImageIO(io);
	std::cout << names;

	//  FilterWatcher watcher(reader);

	try{
	//    if (1){reader->ReverseOrderOn();}
		reader->Update();
		reader->GetOutput()->Print(std::cout);
	}catch (itk::ExceptionObject &ex){
		std::cout << ex;
	}

	typename theImagePointer image = theImageType::New();
	image = reader->GetOutput();
	replicate_itk_to_image(image);

    this->from_file(true);
	this->meta.read_metadata_from_dcm_file(fileNames[0].c_str());	//JK1 - Loads meta data from first dicom file in vector...

	std::cout << fileNames[0].c_str() << std::endl;
	
	this->name( this->meta.get_name() );
	
	this->read_geometry_from_dicom_file( fileNames[0].c_str() );

}
*/

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::load_dataset_from_DICOM_fileAF(std::string file_path, std::string seriesIdentifier)
{ 
	std::cout<< "--- load_dataset_from_DICOM_filesAF" << std::endl;
	std::cout<<"file_path="<<file_path<<std::endl;
	std::cout<<"seriesIdentifier="<<seriesIdentifier<<std::endl;
	string dir_path = path_parent(file_path);
	std::cout<<"dir_path="<<dir_path<<std::endl;
	
    typedef itk::GDCMSeriesFileNames NamesGeneratorType;
    NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();
    nameGenerator->SetUseSeriesDetails(false);		//no details - crucial because existing
    nameGenerator->SetLoadPrivateTags(true);
    nameGenerator->SetDirectory(dir_path.c_str());

    typedef std::vector<std::string> FileNamesContainer;
	FileNamesContainer fileNames = nameGenerator->GetFileNames(seriesIdentifier);
	std::cout<<"Number of files in series..."<<fileNames.size()<<std::endl;

	//********** remove multiple echoes *************
	//JK - This is temporary excluded for speedup --> dual echo iamging sequenses will be loaded in the "same volume"
	vector<string> echotimes = list_dicom_tag_values_for_this_ref_tag_value(fileNames, DCM_SERIES_ID, seriesIdentifier, DCM_TE);
	std::cout<<"Number of TE:s..."<<echotimes.size()<<std::endl;
	if(echotimes.size()>1){//delelect all other echo times but the one int the file clicked...
		fileNames = get_dicom_files_with_dcm_tag_value(fileNames, DCM_TE, get_dicom_tag_value(file_path,DCM_TE));
	}

	fileNames = get_dicom_files_with_dcm_tag_value(fileNames, DCM_TEMP_POS_ID, get_dicom_tag_value(file_path,DCM_TEMP_POS_ID));
	std::cout<<"Number of files in series (efter separation for TEMPORAL_POS_ID..."<<fileNames.size()<<std::endl;
	
	load_dataset_from_these_DICOM_files(fileNames);
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::load_dataset_from_these_DICOM_files(vector<string> fileNames){
	cout<<"load_dataset_from_these_DICOM_files...("<<fileNames.size()<<")"<<endl;
	itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();
	typename theImagePointer image = theImageType::New();

	if(fileNames.size()==1){
		typename theReaderType::Pointer reader = theReaderType::New();
		reader->SetImageIO(dicomIO);
		reader->SetFileName(fileNames[0].c_str());
		try { reader->Update(); }
		catch (itk::ExceptionObject &ex)
			{ std::cout << ex << std::endl; }
		image = reader->GetOutput();

	}else if(fileNames.size()>1){
		typename theSeriesReaderType::Pointer reader = theSeriesReaderType::New();
		reader->SetImageIO ( dicomIO );
		reader->SetFileNames ( fileNames );

		try { reader->Update(); }
		catch (itk::ExceptionObject &ex)
			{ std::cout << ex << std::endl; }
		image = reader->GetOutput();

	}else{
		pt_error::error("load_dataset_from_DICOM_filesAF--> fileNames.size()==0",pt_error::debug);
	}

	replicate_itk_to_image(image);	// *** transfer image data to our platform's data structure ***

    this->from_file(true);
	this->meta.read_metadata_from_dcm_file(fileNames[0].c_str());	//JK1 - Loads meta data from first dicom file in vector...
//	this->meta.print_all();
//	cout<<"..."<<endl;
	this->name( this->meta.get_name() );
	this->set_slice_orientation( this->meta.get_slice_orientation() );

	this->read_geometry_from_dicom_file ( fileNames[0].c_str() );			// use the first file name in the vector
//	this->print_geometry();
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::load_dataset_from_DICOM_file(string fileName) {
	vector<string> fileNames;
	fileNames.push_back(fileName);
	load_dataset_from_these_DICOM_files(fileNames);
}


template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::load_dataset_from_all_DICOM_files_in_dir(std::string dir_path)
    {  
		cout<<"load_dataset_from_all_DICOM_files_in_dir...("<<dir_path<<")"<<endl;
		
		//-- Remove final "\\" of dir_path...
		while(dir_path.find_last_of("\\")==dir_path.size()-1){
			cout<<"substring...ing...(\\)"<<endl;
			dir_path = dir_path.substr(0,dir_path.size()-1);
		}
		while(dir_path.find_last_of("/")==dir_path.size()-1){
			cout<<"substring...ing...(/)"<<endl;
			dir_path = dir_path.substr(0,dir_path.size()-1);
		}
//		cout<<"dir_path="<<dir_path<<endl;

		if(!dir_exists(dir_path)){
			pt_error::error("load_dataset_from_all_DICOM_files_in_dir - No Such Dir ("+dir_path+")",pt_error::debug);
		}else{
			itk::DICOMImageIO2::Pointer dicomIO = itk::DICOMImageIO2::New();
			itk::DICOMSeriesFileNames::Pointer nameGenerator = itk::DICOMSeriesFileNames::New();
			nameGenerator->SetDirectory( dir_path.c_str() );

			typedef vector<string> seriesIdContainer;
			const seriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();
			seriesIdContainer::const_iterator seriesItr = seriesUID.begin();
			seriesIdContainer::const_iterator seriesEnd = seriesUID.end();

			nameGenerator->SetFileNameSortingOrderToSortByImageNumber();//in order...
			typedef vector<string> fileNamesContainer;
			fileNamesContainer fileNames;
			fileNames = nameGenerator->GetFileNames();					//stores all dicom filenames...

			cout<<"fileNames.size()"<<fileNames.size()<<endl;

			//-------------READER-------------
			typename theSeriesReaderType::Pointer reader = theSeriesReaderType::New();
			reader->SetFileNames( fileNames );
			reader->SetImageIO( dicomIO );
			try{
				reader->Update();
			}catch (itk::ExceptionObject &ex){
				cout<<ex<<endl;
			}

			// *** transfer image data to our platform's data structure ***
			typename theImagePointer image = theImageType::New();
			image = reader->GetOutput();
			replicate_itk_to_image(image);

			this->name("Dicomfiles");

			this->from_file(true);

			this->meta.read_metadata_from_dcm_file(fileNames[0].c_str());	//JK1 - Loads meta data from first dicom file in vector...
		}
	}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::load_dataset_from_regular_DICOM_files_in_dir(std::string dir_path, int jump_num_files)
{  
	int a;
}
			


template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::save_to_file(const std::string file_path, const bool useCompression, const bool anonymize)
{
	if( file_path.find_last_of(".vtk") == file_path.size()-1 ){
		this->save_to_VTK_file(file_path, useCompression );
	}else if( file_path.find_last_of(".dcm")==file_path.size()-1 ){
		this->save_to_DCM_file(file_path, useCompression, anonymize );
	}else if( file_path.find_last_of(".nii")==file_path.size()-1 ){
		cout<<"Save NIFTI file using (itk::NiftiImageIO)..."<<endl;
		this->save_to_NIFTI_file(file_path);
	}else{
		pt_error::error("save_to_file - did not recognize the file format (not vtk/dcm)",pt_error::debug);
	}
}


template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::save_to_VTK_file(const std::string file_path, const bool useCompression)
    {
	cout<<"save_to_VTK_file..."<<endl;
    typename theWriterType::Pointer writer = theWriterType::New();   //default file type is VTK
    writer->SetFileName( file_path.c_str() );
    writer->SetInput(get_image_as_itk_output());
	if(useCompression){
		writer->UseCompressionOn();
	}

    try{
        writer->Update();
		/*save helper data if any exist*/
		if(!this->helper_data->data.empty()){
			std::string helper_data_path;
			size_t found = file_path.find_last_of(".");
			if(found != string::npos){
				helper_data_path = file_path.substr(0,found);
				helper_data_path.append(".pad");
				this->save_helper_data_to_file(helper_data_path);
			}
		}
        }
    catch (itk::ExceptionObject &ex){
        pt_error::error("Exception thrown saving file (" +file_path + ")", pt_error::warning);
		std::cout<<ex<<std::endl;
        }
    }

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::save_to_VTK_file_if_true(const bool save_bool_variable, const std::string file_path, const bool useCompression)
{
	if(save_bool_variable){
		this->save_to_VTK_file(file_path,useCompression);
	}
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::save_to_VTK_file_if_non_empty(string save_base, const std::string file_path, const bool useCompression)
{
	if(save_base!=""){
		this->save_to_VTK_file(file_path,useCompression);
	}
}


template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::save_to_DCM_file(const std::string file_path, const bool useCompression, const bool anonymize)
    {
//	cout<<"save_to_DCM_file..."<<endl;    //port image to ITK image and save it as DCM file

	//--------------------------------------------------------
	//--- If dicom image has tag "DCM_IMAGE_ORIENTATION_PATIENT" a/b/c/d/e/f
	//--- This is read as 
	//		a d j
	//		b e	k
	//		c f l
	//--- GDCMImageIO saves the direcional conines transposed compared to the ITK use....
	//--- Idea - Transpose rotation matrix before saving... and again after...

	//--------------------------------------------------------
	this->orientation = this->orientation.GetTranspose();
	//--------------------------------------------------------

    typename theWriterType::Pointer writer = theWriterType::New();
	typename itk::OrientedImage<ELEMTYPE, IMAGEDIM >::Pointer image = get_image_as_itk_output();
    writer->SetFileName( file_path.c_str() );
    writer->SetInput(image);
	writer->UseCompressionOff();
	if(useCompression){
		writer->UseCompressionOn();
	}

	if(anonymize){
		typedef itk::GDCMImageIO ImageIOType;
		ImageIOType::Pointer gdcmImageIO = ImageIOType::New();
		writer->SetImageIO( gdcmImageIO );
		typedef itk::MetaDataDictionary   DictionaryType;
		DictionaryType & dictionary = image->GetMetaDataDictionary();

		//TODO - make sure all interesting data in "metadata-object" is saves....
		itk::EncapsulateMetaData<std::string>( dictionary, DCM_PATIENT_NAME, "Anonymized" );
		itk::EncapsulateMetaData<std::string>( dictionary, DCM_PATIENT_ID, "Anonymized" );
		itk::EncapsulateMetaData<std::string>( dictionary, DCM_PATIENT_BIRTH_DATE, "Anonymized" );
		// The two lines below don't work... 
		// The brutal rotation of the orientation matrix was used before and after saving instead...
		//itk::EncapsulateMetaData<std::string>( dictionary, DCM_IMAGE_POSITION_PATIENT, "1\\2\\3" );
		//itk::EncapsulateMetaData<std::string>( dictionary, DCM_IMAGE_ORIENTATION_PATIENT, this->get_orientation_as_dcm_string() );
	}

    try{
        writer->Update();
		/*save helper data if any exist*/
		if(!this->helper_data->data.empty()){
			std::string helper_data_path;
			size_t found = file_path.find_last_of(".");
			if(found != string::npos){
				helper_data_path = file_path.substr(0,found);
				helper_data_path.append(".pad");
				this->save_helper_data_to_file(helper_data_path);
			}
		}
    }catch (itk::ExceptionObject &ex){
        pt_error::error("Exception thrown saving file (" +file_path + ")", pt_error::warning);
		std::cout<<ex<<std::endl;
    }

	//--------------------------------------------------------
	this->orientation = this->orientation.GetTranspose(); //transpose again  "=back"
	//--------------------------------------------------------
}


template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::save_to_DCM_file_series(const std::string file_path, const bool useCompression, const bool anonymize)
    {
//	cout<<"save_to_DCM_file..."<<endl;    //port image to ITK image and save it as DCM file

	//--------------------------------------------------------
	//--- If dicom image has tag "DCM_IMAGE_ORIENTATION_PATIENT" a/b/c/d/e/f
	//--- This is read as 
	//		a d j
	//		b e	k
	//		c f l
	//--- GDCMImageIO saves the direcional conines transposed compared to the ITK use....
	//--- Idea - Transpose rotation matrix before saving... and again after...

	//--------------------------------------------------------
//	this->orientation = this->orientation.GetTranspose();
	//--------------------------------------------------------


//  typename theSeriesWriterType::Pointer writer = theSeriesWriterType::New();
	typename theImageType::Pointer image = get_image_as_itk_output();

	//--------------------------
/*	cout<<"get_image_as_itk_output..."<<endl;

	typedef itk::ImportImageFilter<ELEMTYPE, IMAGEDIM> filterType;
	typename filterType::Pointer ITKimportfilter = filterType::New();
	ITKimportfilter->SetRegion(this->get_region_itk());
	ITKimportfilter->SetOrigin(this->get_origin_itk());
	ITKimportfilter->SetSpacing(this->get_voxel_size_itk());
	ITKimportfilter->SetDirection(this->get_orientation_itk()); //JK - Very important to remember ;-)

    ITKimportfilter->SetImportPointer(this->imagepointer(), this->num_elements, false);
	ITKimportfilter->Update();
	typename theImageType2::Pointer image = ITKimportfilter->GetOutput();
*/
	//--------------------------
/*
    typename theWriterType::Pointer writer = theWriterType::New();   //default file type is VTK
    writer->SetFileName( file_path + "test.vtk" );
    writer->SetInput(image);

	if(useCompression){
		writer->UseCompressionOn();
	}
    try{
        writer->Update();
        }
    catch (itk::ExceptionObject &ex){
        pt_error::error("Exception thrown saving file (" +file_path + ")", pt_error::warning);
		std::cout<<ex<<std::endl;
        }
*/


	cout<<"dir="<<endl<<image->GetDirection()<<endl;
//---------------------------------------------------
//---------------------------------------------------
//---------------------------------------------------
//---------------------------------------------------


  typedef itk::GDCMImageIO                        ImageIOType;
  ImageIOType::Pointer gdcmIO = ImageIOType::New();

  const char * outputDirectory = file_path.c_str();
  create_dir(outputDirectory);

  itk::MetaDataDictionary & dict = gdcmIO->GetMetaDataDictionary();
  std::string tagkey, value;
  tagkey = "0008|0060"; // Modality
  value = "MR";
  itk::EncapsulateMetaData<std::string>(dict, tagkey, value );
  tagkey = "0008|0008"; // Image Type
  value = "DERIVED\\SECONDARY";
  itk::EncapsulateMetaData<std::string>(dict, tagkey, value);
  tagkey = "0008|0064"; // Conversion Type
  value = "DV";
  itk::EncapsulateMetaData<std::string>(dict, tagkey, value);
  tagkey = DCM_PATIENT_NAME; // Patient name
  value = (this->meta).get_data_string(tagkey);
  itk::EncapsulateMetaData<std::string>(dict, tagkey, value);
  tagkey = DCM_PATIENT_ID; // Patient ID
  value = (this->meta).get_data_string(tagkey);
  itk::EncapsulateMetaData<std::string>(dict, tagkey, value);

  typedef itk::ImageSeriesWriter< theImageType , theImageType2D >  SeriesWriterType;
  typename SeriesWriterType::Pointer seriesWriter = SeriesWriterType::New();
  seriesWriter->SetInput( image );
  seriesWriter->SetImageIO( gdcmIO );

  typename theImageType2::RegionType region = image->GetLargestPossibleRegion();
  typename theImageType2::IndexType start = region.GetIndex();
  typename theImageType2::SizeType  size  = region.GetSize();

  typedef itk::NumericSeriesFileNames             NamesGeneratorType;
  NamesGeneratorType::Pointer namesGenerator = NamesGeneratorType::New();

  std::string format = outputDirectory;
  format += "/image%03d.dcm";
  namesGenerator->SetSeriesFormat( format.c_str() );

  namesGenerator->SetStartIndex( start[2] );
  namesGenerator->SetEndIndex( start[2] + size[2] - 1 );
  namesGenerator->SetIncrementIndex( 1 );

  seriesWriter->SetFileNames( namesGenerator->GetFileNames() );
  
  try
    {
    seriesWriter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown while writing the series " << std::endl;
    std::cerr << excp << std::endl;
//    return EXIT_FAILURE;
    }


//---------------------------------------------------
//---------------------------------------------------
//---------------------------------------------------
//---------------------------------------------------


//    writer->SetFileName( file_path.c_str() );
 //   writer->SetInput(image);

/*
	if(useCompression){
		writer->UseCompressionOn();
	}

	if(anonymize){
		typedef itk::GDCMImageIO ImageIOType;
		ImageIOType::Pointer gdcmImageIO = ImageIOType::New();
		writer->SetImageIO( gdcmImageIO );
		typedef itk::MetaDataDictionary   DictionaryType;
		DictionaryType & dictionary = image->GetMetaDataDictionary();

		//TODO - make sure all interesting data in "metadata-object" is saves....
		itk::EncapsulateMetaData<std::string>( dictionary, DCM_PATIENT_NAME, "Anonymized" );
		itk::EncapsulateMetaData<std::string>( dictionary, DCM_PATIENT_ID, "Anonymized" );
		itk::EncapsulateMetaData<std::string>( dictionary, DCM_PATIENT_BIRTH_DATE, "Anonymized" );
		// The two lines below don't work... 
		// The brutal rotation of the orientation matrix was used before and after saving instead...
		//itk::EncapsulateMetaData<std::string>( dictionary, DCM_IMAGE_POSITION_PATIENT, "1\\2\\3" );
		//itk::EncapsulateMetaData<std::string>( dictionary, DCM_IMAGE_ORIENTATION_PATIENT, this->get_orientation_as_dcm_string() );
	}

    try{
        writer->Update();
    }catch (itk::ExceptionObject &ex){
        pt_error::error("Exception thrown saving file (" +file_path + ")", pt_error::warning);
		std::cout<<ex<<std::endl;
    }
*/
	//--------------------------------------------------------
//	this->orientation = this->orientation.GetTranspose(); //transpose again  "=back"
	//--------------------------------------------------------

}


template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::save_to_raw_file(const std::string file_path, bool save_image_info_txt_file)
{
	typename itk::RawImageIO<ELEMTYPE,IMAGEDIM>::Pointer io = itk::RawImageIO<ELEMTYPE,IMAGEDIM>::New();
		//  io->SetFileTypeToASCII();
//		io->SetByteOrderToBigEndian();
		io->SetByteOrderToLittleEndian();

		// Write out the image
		typename itk::ImageFileWriter<theImageType>::Pointer writer = itk::ImageFileWriter<theImageType>::New();
		writer->SetInput(get_image_as_itk_output());
		writer->SetFileName(file_path.c_str());
		writer->SetImageIO(io);

		try{
			writer->Update();
		}
		catch( itk::ExceptionObject & excp ){
			std::cerr << "Error while writing the raw image "<< std::endl;
			std::cerr << excp << std::endl;
		}

		if(save_image_info_txt_file){
		    ofstream myfile;
			string fname = file_path;
			remove_file_lastname(fname);
			fname = fname + ".info";
			myfile.open(fname.c_str());

			//--------- Special format that is used in segmentation a projects ----------
			//Width 256
			//Height 256
			//Depth 16
			//WidthDim 1.6769
			//HeightDim 1.6769
			//DepthDim 12
			//MinVal 0
			//MaxVal 255
			//BitsUsed 16
			myfile<<"Width "<<this->nx()<<"\n";
			myfile<<"Height "<<this->ny()<<"\n";
			myfile<<"Depth "<<this->nz()<<"\n";
			myfile<<"WidthDim "<<this->get_voxel_size()[0]<<"\n";
			myfile<<"HeightDim "<<this->get_voxel_size()[1]<<"\n";
			myfile<<"DepthDim "<<this->get_voxel_size()[2]<<"\n";
			myfile<<"MinVal "<<this->get_min()<<"\n";
			myfile<<"MaxVal "<<this->get_max()<<"\n";
			myfile<<"BitsUsed 16\n";
			myfile.close();
		}
}


template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::save_to_NIFTI_file(const std::string file_path)
{
	cout<<"save_to_NIFTI_file..."<<endl;    //port image to ITK image and save it as NIFTI (.nii) file

	typename theWriterType::Pointer writer = theWriterType::New();
	typename itk::OrientedImage<ELEMTYPE, IMAGEDIM >::Pointer image = get_image_as_itk_output();
    writer->SetFileName( file_path.c_str() );
    writer->SetInput(image);

  	itk::NiftiImageIO::Pointer io = itk::NiftiImageIO::New();
	writer->SetImageIO( io );

	try{
        writer->Update();
    }catch (itk::ExceptionObject &ex){
        pt_error::error("Exception thrown saving file (" +file_path + ")", pt_error::warning);
		std::cout<<ex<<std::endl;
    }
}


template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::helper_data_to_binary_image(int index){
	image_binary<IMAGEDIM> *binary = new image_binary<IMAGEDIM>(this);
	binary->fill(0);
	write_additional_data(binary, index);
	datamanagement.add(binary,"binary",true);
}
template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::helper_data_to_binary_image(vector<int> indexes){
	image_binary<IMAGEDIM> *binary = new image_binary<IMAGEDIM>(this);
	binary->fill(0);
	cout << "Adding index:";
	for(int i = 0; i < indexes.size(); i++){
		cout << " " << indexes.at(i) << endl;
		write_additional_data(binary, indexes.at(i));
	}
	cout << endl;
	datamanagement.add(binary,"binary",true);
}
template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::helper_data_to_binary_image(ADDITIONAL_TYPE type){
	image_binary<IMAGEDIM> *binary = new image_binary<IMAGEDIM>(this);
	binary->fill(0);
	for(int i = 0; i < this->helper_data->data.size(); i++){
		if((this->helper_data)->data.at(i)->type == type)
			write_additional_data(binary, i);
	}
	datamanagement.add(binary,"binary",true);
}
template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::write_additional_data(image_binary<IMAGEDIM> *bin_image, int i){
	bool fill = i<0;
	i = abs(i)-1; //added 1 so that 0 can have fill to
	additional_data_base* base = this->helper_data->data.at(i);
	ADDITIONAL_TYPE type;
	type = base->type;
	vector<Vector3D> vec, free, temp;
	Vector3D p1, p2, p3,p4;
	float radius;
	switch(type){
		case AT_POINT:
			vec.push_back(this->world_to_voxel((dynamic_cast<point_data*>(base))->p));
			break;
		case AT_LINE:
			p1 = this->world_to_voxel((dynamic_cast<line_data*>(base))->start);
			p2 = this->world_to_voxel((dynamic_cast<line_data*>(base))->stop);
			vec = shape_calc::calc_line_3d(p1,p2);
			break;
		case AT_RECT:
			p1 = this->world_to_voxel((dynamic_cast<rect_data*>(base))->c1);
			p2 = this->world_to_voxel((dynamic_cast<rect_data*>(base))->c2);
			p3 = this->world_to_voxel((dynamic_cast<rect_data*>(base))->c3);
			p4 = this->world_to_voxel((dynamic_cast<rect_data*>(base))->c4);
			vec = shape_calc::calc_rect_3d(p1,p2,p3,p4);
			break;
		case AT_CIRCLE://Different from the others due to the radius calculation
			p1 = (dynamic_cast<circle_data*>(base))->c;
			p2 = (dynamic_cast<circle_data*>(base))->n;
			radius = (dynamic_cast<circle_data*>(base))->radius;
			cout << "radius:" << radius << endl;
			free = shape_calc::calc_cirlce_3d(p1,p2,radius);
			if(free.size() >=2){
				p1 = world_to_voxel(free.at(0));
				for(int q = 1; q < free.size(); q++){//connect the dots
					p2 = world_to_voxel(free.at(q));
					temp = shape_calc::calc_line_3d(p1,p2);
					vec.insert(vec.end(),temp.begin(), temp.end());
					p1 = p2;
				}	
			}
			break;
		case AT_FREEHAND:
			free = (dynamic_cast<freehand_data*>(base))->p;
			if(free.size() >=2){
				p1 = world_to_voxel(free.at(0));
				for(int q = 1; q < free.size(); q++){//connect the dots
					p2 = world_to_voxel(free.at(q));
					temp = shape_calc::calc_line_3d(p1,p2);
					vec.insert(vec.end(),temp.begin(), temp.end());
					p1 = p2;
				}
				//temp = shape_calc::calc_line_3d(p1,world_to_voxel(free.at(0))); //connect end points
				//vec.insert(vec.end(),temp.begin(), temp.end());
			}
			break;
		default:
			break;

	}
	// = helper_data->data.at(i)->get_all_points();
	int count = 0;
	int x = datasize [0];
	int y = datasize[1];
	int z = datasize[2];
	cout << "vec size: " << vec.size() << endl;
	if(!fill){
		for(int i = 0; i < vec.size(); i++){
			Vector3D point = vec.at(i);//world_to_voxel(vec.at(i));
			if(point[0]>=0 && point[0] < x && point[1]>=0 && 
				point[1] < y && point[2]>=0 && point[2] < z ){
				//add voxel to image
				bin_image->set_voxel(point,1);
				count++;
			}
		}
	}else if(fill){
		bin_image->fill_region_2d(vec, 1);
	}
	cout << "written nr: " << count << endl;
		//binary->set_voxel(vec.at(i)[0], vec.at(i)[1], vec.at(i)[2]);
}
template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::fill_region_2d(vector<Vector3D> border, ELEMTYPE fill_val){
	
	image_binary<3> *bin_image = new image_binary<3>(datasize[0],datasize[1],1);
	bin_image->fill(0);
	//additional_data_base* base = helper_data->data.at(i);
	//base->calc_data();
	//vector<Vector3D> border = base->points_to_draw;
	for(int i = 0; i <border.size(); i++){
		bin_image->set_voxel(border.at(i)[0],border.at(i)[1],0,1);
	}
	int z = border.front()[2]; //requires the object to be in the xy plane
	bin_image->fill_holes_2D(2,1);

	for(int x = 0; x <datasize[0]; x++){
		for(int y = 0; y<datasize[1]; y++){
			if(bin_image->get_voxel(x,y) == 1){
				this->set_voxel(x,y,z,fill_val);
			}
		}
	}
	/*vector<Vector3D> queue;
	queue.push_back(start_seed);
	vector<vector<bool> > visit;
	for(int i = 0; i < datasize[0];i++){
		vector<bool> v;
		v.assign(datasize[1],false);
		visit.push_back(v);
	}
	while(!queue.empty()){
		Vector3D p = queue.front();
		this->set_voxel(p,value);
		if(this->get_voxel(p[0]+1,p[1],p[2]) != value && !visit.at(p[0]+1).at(p[1]))
			queue.push_back(create_Vector3D(p[0]+1,p[1],p[2]));

		if(this->get_voxel(p[0]-1,p[1],p[2]) != value && !visit.at(p[0]-1).at(p[1]))
			queue.push_back(create_Vector3D(p[0]-1,p[1],p[2]));

		if(this->get_voxel(p[0],p[1]-1,p[2]) != value && !visit.at(p[0]).at(p[1]-1))
			queue.push_back(create_Vector3D(p[0],p[1]-1,p[2]));

		if(this->get_voxel(p[0],p[1]+1,p[2]) != value && !visit.at(p[0]).at(p[1]+1)){
			queue.push_back(create_Vector3D(p[0],p[1]+1,p[2]));
		}
		visit.at(p[0]+1).at(p[1]) = true;
		visit.at(p[0]-1).at(p[1]) = true;
		visit.at(p[0]).at(p[1]+1) = true;
		visit.at(p[0]).at(p[1]-1) = true;

		queue.erase(queue.begin(),queue.begin()+1);
		//cout <<"queue size: " << queue.size() << endl;
	}*/
}
/*
template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::load_dataset_from_VTK_file(string file_path)
    {
	if(file_exists(file_path)){
		typename theReaderType::Pointer r = theReaderType::New();
		itk::VTKImageIO::Pointer VTKIO = itk::VTKImageIO::New();

		r->SetFileName(file_path.c_str());

		r->SetImageIO( VTKIO );

		typename theImagePointer image = theImageType::New();
		image = r->GetOutput();
		r->Update();
		typename theSizeType s = image->GetBufferedRegion().GetSize();

		replicate_itk_to_image(image);

		this->name_from_path (file_path);
	}else{
		pt_error::error("image_general::load_dataset_from_VTK_file()--> file does not exist...",pt_error::debug);
	}
    }
*/

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::save_regular_slices_to_VTK_file(const std::string file_path, int start_slc, int jump_num_slc, int dir, const bool useCompression)
{
	cout<<"save_regular_slices_to_VTK_file"<<dir<<endl;
	cout<<"file_path="<<file_path<<endl;
	cout<<"start="<<start_slc<<endl;
	cout<<"every="<<jump_num_slc<<endl;
	cout<<"dir="<<dir<<endl;
	cout<<"useCompression="<<useCompression<<endl;

	if(file_path != ""){
		image_general<ELEMTYPE,IMAGEDIM> *im_tmp = this->get_subvolume_from_slices_3D(start_slc,jump_num_slc,dir);
		im_tmp->name("im_tmp");
		im_tmp->data_has_changed(true);
		im_tmp->save_to_VTK_file(file_path,useCompression);
		delete im_tmp;
	}
}


template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::load_dataset_from_hdr_file(string file_path)
    {
	if(file_exists(file_path)){
		typename theReaderType::Pointer r = theReaderType::New();
		itk::AnalyzeImageIO::Pointer hdrIO = itk::AnalyzeImageIO::New();
		r->SetFileName(file_path.c_str());
		r->SetImageIO( hdrIO );
		typename theImagePointer image = theImageType::New();
		image = r->GetOutput();
		r->Update();
		typename theSizeType s = image->GetBufferedRegion().GetSize();
		replicate_itk_to_image(image);
		this->name_from_path (file_path);
	}else{
		pt_error::error("image_general::load_dataset_from_hdr_file()--> file does not exist...",pt_error::debug);
	}
    }
	
template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::load_dataset_from_ximg_file(string file_path)
{
	if(file_exists(file_path))
	{
		typename theReaderType::Pointer r = theReaderType::New();
		itk::GE5ImageIO::Pointer ximgIO = itk::GE5ImageIO::New();
		r->SetFileName(file_path.c_str());
		r->SetImageIO(ximgIO);
		typename theImagePointer image = theImageType::New();
		image = r->GetOutput();
		r->Update();
		typename theSizeType s = image->GetBufferedRegion().GetSize();
		replicate_itk_to_image(image);
		this->name_from_path (file_path);
	}
	else
	{
		pt_error::error("image_general::load_dataset_from_ximg_file()--> file does not exist...",pt_error::debug);
	}
}




template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::load_dataset_from_NIFTI_file(string file_path)
{
	if(file_exists(file_path)){
		typename theReaderType::Pointer r = theReaderType::New();
	//	itk::VTKImageIO::Pointer VTKIO = itk::VTKImageIO::New();
	    itk::NiftiImageIO::Pointer niftiIO = itk::NiftiImageIO::New();
		r->SetFileName(file_path.c_str());
		r->SetImageIO( niftiIO );

		typename theImagePointer image = theImageType::New();
		image = r->GetOutput();
		r->Update();
		typename theSizeType s = image->GetBufferedRegion().GetSize();

		replicate_itk_to_image(image);

		this->name_from_path (file_path);
	}else{
		pt_error::error("image_general::load_dataset_from_NIFTI_file()--> file does not exist...",pt_error::debug);
	}

}