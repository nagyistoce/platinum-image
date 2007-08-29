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
        try_allocate<long double,IMGCLASS>(output, voxel_type, files, width, height, bigEndian, headerSize, voxelSize);
        }
    else
        {
        if (signedType)
            {
            try_allocate<signed char,IMGCLASS >(output, voxel_type, files, width, height, bigEndian, headerSize, voxelSize);
            try_allocate<signed short,IMGCLASS >(output, voxel_type, files, width, height, bigEndian, headerSize, voxelSize);
            try_allocate<signed long,IMGCLASS >(output, voxel_type, files, width, height, bigEndian, headerSize, voxelSize);
            try_allocate<signed long long,IMGCLASS >(output, voxel_type, files, width, height, bigEndian, headerSize, voxelSize);
            }
        else
            {
            try_allocate<unsigned char,IMGCLASS >(output, voxel_type, files, width, height, bigEndian, headerSize, voxelSize);
            try_allocate<unsigned short,IMGCLASS >(output, voxel_type, files, width, height, bigEndian, headerSize, voxelSize);
            try_allocate<unsigned long,IMGCLASS >(output, voxel_type, files, width, height, bigEndian, headerSize, voxelSize);
            try_allocate<long long,IMGCLASS >(output, voxel_type, files, width, height, bigEndian, headerSize, voxelSize);
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
        name_from_path (files.front());

        //image_has_changed(true);
        }
    else
        {
        //TODO: if imageptr == NULL at this point, image has not been read and error has
        //to be reported somehow
        }
    }

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::load_dataset_from_DICOM_files(std::string dir_path,std::string seriesIdentifier)
    {  
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

	meta.read_metadata_from_dcm_file(fileNames[0].c_str());	//JK1 - Loads meta data from first dicom file in vector...
    }

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::save_to_VTK_file(const std::string file_path)
    {
    //replicate image to ITK image and save it as VTK file

    typename theImageType::DirectionType itk_orientation;

    for (unsigned int d=0;d<3;d++)
        {
        for (unsigned int c=0;c<3;c++)
            {itk_orientation[d][c]=this->orientation[d][c];}
        }

    make_image_an_itk_reader();
    typename theImagePointer output_image=ITKimportfilter->GetOutput();

    output_image->SetDirection(itk_orientation);

    typename theWriterType::Pointer writer = theWriterType::New();   //default file type is VTK
    writer->SetFileName( file_path.c_str() );
    writer->SetInput(output_image);

    try{
        writer->Update();
        }
    catch (itk::ExceptionObject &ex){
        pt_error::error("Exception thrown saving file (" +file_path + ")", pt_error::warning);
		std::cout<<ex<<std::endl;
        }
    }


template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::save_to_TIF_file_series_3D(const std::string file_path_base)
{
	char buf[10];
	for(int z=0;z<datasize[2];z++)
	{
		image_scalar<ELEMTYPE, IMAGEDIM> *slc = new image_scalar<ELEMTYPE, IMAGEDIM>();
		slc->initialize_dataset(datasize[0],datasize[1],1);
		slc->copy_slice_from_3D(this,z,0,2);
		slc->scale(); //0...256
		slc->data_has_changed(true);

		sprintf(buf,"%04i",z);
		image_scalar<unsigned char,3> *slc2 = new image_scalar<unsigned char,3>(slc);
		slc2->save_uchar2D_to_TIF_file(file_path_base, string(buf));
		delete slc;
		delete slc2;
	}
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::save_uchar2D_to_TIF_file(const std::string file_path_base, const std::string slice)
{
	typedef itk::ImageFileWriter<itk::Image<unsigned char,3> >	theTifWriterType;
    make_image_an_itk_reader();

	string s = file_path_base+"_"+slice+".tif";
	theTifWriterType::Pointer writer = theTifWriterType::New();
	writer->SetFileName(s.c_str());
	writer->SetInput(ITKimportfilter->GetOutput());
	try{
		writer->Update();
	}catch (itk::ExceptionObject &ex){
		cout<<"Exception thrown saving file.....("<<s<<")"<<ex;
	}
}


template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::load_dataset_from_VTK_file(string file_path)
    {
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
    }