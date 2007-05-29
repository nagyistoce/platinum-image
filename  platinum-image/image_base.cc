// $Id$

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

#include "image_base.h"

#include <sstream>
using namespace std;

//#include "fileutils.h"
#include "image_integer.h"
#include "image_complex.h"
#include "viewmanager.h"
#include "datamanager.h"
#include "rendermanager.h"

extern datamanager datamanagement;
extern rendermanager rendermanagement;
extern viewmanager viewmanagement;

static int imagemaxID = 1;

image_base::image_base():data_base()
    {set_parameters ();}
image_base::image_base(image_base* s):data_base(s)
    {
    set_parameters ();
    //setting copy name at the root would be neat,
    //but is not possible since the widget isn't
    //created yet

    name ("Copy of " + s->name());
    }

void image_base::set_parameters ()    
    {
    ostringstream namestream;

    ID = imagemaxID++;

    //constructor: add "Untitled" name and ID
    namestream << "3D image (" << ID << ")";

    widget=new datawidget(DATAHANDLER_VOLUME_3D,ID,namestream.str());
    name(namestream.str());
    }

Vector3D image_base::transform_unit_to_voxel(Vector3D pos)
    {
    Vector3D vox;

    vox=pos+unit_center_;
    vox=unit_to_voxel_*vox;

    return vox;
    }

//enum fileFormatType {FILE_FORMAT_DICOM,FILE_FORMAT_VTK};

// //meta-helper to load templated classes from file format of choice
//template <class imageClass>
//void load_image (imageClass img,fileFormatType format)
//    {
//    if (format == FILE_FORMAT_VTK)
//        {img->load_dataset_from_VTK_file(*file);}
//    if (format == FILE_FORMAT_DICOM)
//        {img->load_dataset_from_DICOM_files(*file);}
//    }

//
// //helper that creates subclass objects from ITK voxel/data type constants
//template <class imageClass>
//imageClass * new_image (fileFormatType format, itk::ImageIOBase::IOPixelType dataType)
//    {
//    imageClass *  newImage = NULL;
//
//    switch (dataType)
//        {
//        case itk::ImageIOBase::UCHAR:
//            newImage = new imageClass<unsigned char>();
//            if (format == FILE_FORMAT_VTK)
//                {((imageClass<unsigned char>*)newImage)->load_dataset_from_VTK_file(*file);}
//            if (format == FILE_FORMAT_DICOM)
//                {((imageClass<unsigned char>*)newImage)->load_dataset_from_DICOM_files(*file);}
//            break;
//        case itk::ImageIOBase::USHORT:
//            newImage = new imageClass<unsigned short>();
//            if (format == FILE_FORMAT_VTK)
//                {((imageClass<unsigned short>*)newImage)->load_dataset_from_VTK_file(*file);}
//            if (format == FILE_FORMAT_DICOM)
//                {((imageClass<unsigned short>*)newImage)->load_dataset_from_DICOM_files(*file);}
//            break;
//        case itk::ImageIOBase::SHORT:
//            newImage = new imageClass<signed short>();
//            if (format == FILE_FORMAT_VTK)
//                {((imageClass<signed short>*)newImage)->load_dataset_from_VTK_file(*file);}
//            if (format == FILE_FORMAT_DICOM)
//                {((imageClass<signed short>*)newImage)->load_dataset_from_DICOM_files(*file);}
//            break;
//        default:
//            cout << "image_base::new_image: unsupported voxel data type (" << vtkIO->GetComponentTypeAsString (componentType) << ")" << endl;
//        }
//
//    return newImage;
//    }

class imageloader
{
protected:
    vector<string> files;
    vector<string>::iterator file;
    vector<string> rejected_files;
    
public:
        imageloader(std::vector<std::string> filesarg)
        {
            files = filesarg;
            file = files.begin();
        }
    
    vector<string> rejected()
        { return rejected_files; }
        
//load mode
//    enum  {
//        LOMO_NOTSET,
//        LOMO_ATOMICFILES, //loading individual image files, such as VTK
//        LOMO_DICOM,       //loading DICOM series
//        LOMO_BRUKER,      //loading Bruker series
//        LOMO_RAW          //loading raw series
//    }
};

class vtkloader: public imageloader
{
private:
    itk::VTKImageIO::Pointer vtkIO;
    
public:
    vtkloader (std::vector<std::string> files);
    image_base * read ();
};

class dicomloader: public imageloader
{
private:
    itk::GDCMImageIO::Pointer dicomIO;
    
    vector<string> loaded_series; //! UIDs of the DICOM series loaded during this call
                                  //! to prevent multiple selected frames
                                  //! from loading the same series multiple times
public:
    dicomloader (std::vector<std::string> files);
    image_base * read ();
};

class brukerloader: public imageloader
//! Bruker import based on the description at
//! http://imaging.mrc-cbu.cam.ac.uk/imaging/FormatBruker?highlight=%28bruker%29
{
private:
    string                     session;
    vector<string>             runs;
    vector<string>             reconstructions;
    vector<string>::iterator   reconstruction;
    
    void get_reconstructions(std::string run_dir_path);
    
public:
    brukerloader (std::vector<std::string> files);
    image_base * read ();
};


brukerloader::brukerloader(std::vector<std::string> files): imageloader(files)
{
    //determine level (session, run, reconstruction)
    string parent = path_parent (*(files.begin()));
    
    if (file_exists (parent + "subject"))
        {
        //session level
        
        session = parent;
        
        runs = subdirs (session);
     
        std::vector<std::string>::iterator run = runs.begin();
        
        while (run != runs.end())
            {
            get_reconstructions (*run);
            ++run;
            }
        }
    
    else if (file_exists (parent + "acqp"))
        {
        //run level
        
        runs.push_back(parent);
        get_reconstructions(parent);
        }
    
    else if (file_exists (parent + "2dseq"))
        {
        //reconstruction level
        
        reconstructions.push_back(parent);
        }
    
    if (!reconstructions.empty())
        {
        reconstruction = reconstructions.begin();
        }
    else
        {
        //guess it wasn't Bruker
        
        //bruker selection = selection of directory, so
        //there should be either all files
        //or none in the rejected result
        
        rejected_files  =  files;
        }
}

void brukerloader::get_reconstructions(std::string run_dir_path)
{
    std::vector<std::string> A = subdirs (run_dir_path + "pdata");
    
    reconstructions.insert(reconstructions.end(),A.begin(),A.end());
    //
//    std::vector<std::string>::iterator B = A.begin();
//        
//    while (B != A.end())
//        {
//        reconstructions.push_back (*B);
//        }
}

image_base * brukerloader::read()
{
    //1. get metadata from  (*reconstruction + "d3proc") and (*reconstruction + "reco")
    
    //2. call image_general<ELEMTYPE, IMAGEDIM> (std::vector<std::string>, long width, long height, bool bigEndian = false, long headerSize = 0, Vector3D voxelSize = Vector3D (1,1,4), unsigned int startFile = 1,unsigned int increment = 1);
    //image data is in (*reconstruction + "2dseq")
    
    ++reconstruction;

    return NULL;
}

vtkloader::vtkloader(std::vector<std::string> files): imageloader(files)
{
    vtkIO = itk::VTKImageIO::New();
}

image_base *vtkloader::read()
{    
    image_base * result = NULL;
    
    while (file != files.end() && result == NULL)
        {
        if (vtkIO->CanReadFile (file->c_str()))
            {
            //assumption:
            //File contains image data
            
            vtkIO->SetFileName(file->c_str());
            
            vtkIO->ReadImageInformation(); 
            
            itk::ImageIOBase::IOComponentType componentType = vtkIO->GetComponentType();
            
            //get voxel type
            itk::ImageIOBase::IOPixelType pixelType=vtkIO->GetPixelType();
            
            switch ( pixelType)
                {
                case itk::ImageIOBase::SCALAR:
                    //Enumeration values: UCHAR, CHAR, USHORT, SHORT, UINT, INT, ULONG, LONG, FLOAT, DOUBLE
                    
                    switch (componentType)
                        {
                        case itk::ImageIOBase::UCHAR:
                            result =  new image_integer<unsigned char>();
                            ((image_integer<unsigned char>*)result)->load_dataset_from_VTK_file(*file);
                            break;
                        case itk::ImageIOBase::USHORT:
                            result = new image_integer<unsigned short>();
                            ((image_integer<unsigned short>*)result)->load_dataset_from_VTK_file(*file);
                            break;
                            
                        case itk::ImageIOBase::SHORT:
                            result = new image_integer<short>();
                            ((image_integer<short>*)result)->load_dataset_from_VTK_file(*file);
                            break;
                        default:
#ifdef _DEBUG
                            cout << "Load scalar VTK: unsupported component type: " << vtkIO->GetComponentTypeAsString (componentType) << endl;
#endif
                        }
                    break;
                    
                    /*case itk::ImageIOBase::COMPLEX:
                    switch (componentType)
                    {
                        case itk::ImageIOBase::UCHAR:
                            result = new image_complex<unsigned char>();
                            ((image_scalar<unsigned char>*)result)->load_dataset_from_VTK_file(path_parent(*file));
                            break;
                        case itk::ImageIOBase::USHORT:
                            result = new image_complex<unsigned short>();
                            ((image_scalar<unsigned short>*)result)->load_dataset_from_VTK_file(path_parent(*file));
                            break;
                            
                        case itk::ImageIOBase::SHORT:
                            result = new image_complex<short>();
                            ((image_scalar<short>*)result)->load_dataset_from_VTK_file(path_parent(*file));
                            break;
                        default:
#ifdef _DEBUG
                            cout << "Load complex VTK: unsupported component type: " << vtkIO->GetComponentTypeAsString (componentType) << endl;
#endif
                    }*/
                    break;
                default:
#ifdef _DEBUG
                    std::cout << "image_base::load(...): unsupported pixel type: " << vtkIO->GetPixelTypeAsString(pixelType) << endl;
#endif
                    
                }
            }
        else
            { rejected_files.push_back ((*file)); }
        
        //advance files iterator
        ++file;
        }
    
    return result;
}

dicomloader::dicomloader (std::vector<std::string> files): imageloader(files)
{
    dicomIO = itk::GDCMImageIO::New();
}

image_base *dicomloader::read()
{    
    image_base * result = NULL;
    
    while (file != files.end() && result == NULL) // Repeat until one image has been read
        {
        if (dicomIO->CanReadFile (file->c_str()))
            {
            dicomIO->SetFileName(file->c_str());
            
            //get basic DICOM header
            dicomIO->ReadImageInformation();
            
            //get series UID
            std::string seriesIdentifier;
            
            //"0020|000e" - Series Instance UID (series defined by the scanner)
            //series ID identifies the series (out of possibly multiple series in
            //one directory)
            std::string tagkey = "0020|000e";
            
            std::string labelId;
            if( itk::GDCMImageIO::GetLabelFromTag( tagkey, labelId ) )
                {
                std::cout << labelId << " (" << tagkey << "): ";
                if( dicomIO->GetValueFromTag(tagkey, seriesIdentifier) )
                    {
                    //remove one garbage char at end
                    seriesIdentifier.erase(seriesIdentifier.length()-1,seriesIdentifier.length());
                    //check if another file in the same series was part of the
                    //selection (and loaded)
                    vector<string>::const_iterator series_itr=loaded_series.begin();
                    bool already_loaded=false;
                    
                    if (find(loaded_series.begin(),loaded_series.end(),seriesIdentifier)
                        == loaded_series.end())
                        {loaded_series.push_back(seriesIdentifier);}
                    else
                        {already_loaded = true; }
#ifdef _DEBUG
                    std::cout << seriesIdentifier << endl;
#endif      
                    //get voxel type
                    itk::ImageIOBase::IOPixelType pixelType=dicomIO->GetPixelType();
                    
                    if (!already_loaded) 
                        {
                        itk::ImageIOBase::IOComponentType componentType = dicomIO->GetComponentType();
                        switch ( pixelType)
                            {
                            case itk::ImageIOBase::SCALAR:
                                
                                
                                //Enumeration values: UCHAR, CHAR, USHORT, SHORT, UINT, INT, ULONG, LONG, FLOAT, DOUBLE
                                
                                switch (componentType)
                                    {
                                    case itk::ImageIOBase::UCHAR:
                                        result = new image_integer<unsigned char>();
                                        ((image_integer<unsigned char>*)result)->load_dataset_from_DICOM_files(path_parent(*file),seriesIdentifier);
                                        break;
                                    case itk::ImageIOBase::USHORT:
                                        result = new image_integer<unsigned short>();
                                        ((image_integer<unsigned short>*)result)->load_dataset_from_DICOM_files(path_parent(*file),seriesIdentifier);
                                        break;
                                        
                                    case itk::ImageIOBase::SHORT:
                                        result = new image_integer<short>();
                                        ((image_integer<short>*)result)->load_dataset_from_DICOM_files(path_parent(*file),seriesIdentifier);
                                        break;
                                    default:
#ifdef _DEBUG
                                        cout << "Unsupported component type: " << dicomIO->GetComponentTypeAsString (componentType) << endl;
#endif
                                    }
                                break;
                            case itk::ImageIOBase::COMPLEX:
                                break;
                            default:
#ifdef _DEBUG
                                std::cout << "image_base::load(...): unsupported pixel type: " << dicomIO->GetPixelTypeAsString(pixelType) << endl;
#endif
                                
                            }
                        
                        }//not already loaded
                    
                    } //found series tag
                
                }//series tag exists
            else
                {
                //no series identifier, OK if the intention is to just load 1 frame
                //(DICOM files can only contain 1 frame each)
#ifdef _DEBUG
                std::cout << "(No Value Found in File)";
#endif
                }
            }
        else
            { rejected_files.push_back ((*file)); }
        
        //advance files iterator
        ++file;
        }
    
    return result;
}

void image_base::load(std::vector<std::string> flist)
{
    std::vector<std::string> files = flist;
    
    image_base *new_image = NULL; //the eventually loaded image

    {//try Bruker
        brukerloader loader = brukerloader(files);
        
        do {
            new_image = loader.read();
            if (new_image != NULL)
                { datamanagement.add(new_image); }
        }
        while (new_image != NULL);
        
        files = loader.rejected();
    }
    
    {//try VTK
        vtkloader loader = vtkloader(files);
        
        do {
            new_image = loader.read();
            if (new_image != NULL)
                { datamanagement.add(new_image); }
        }
        while (new_image != NULL);
        
        files = loader.rejected();
    }
    
    //try DICOM
    {
        dicomloader loader = dicomloader(files);
        
        do {
            new_image = loader.read();
            if (new_image != NULL)
                { datamanagement.add(new_image); }
        }
        while (new_image != NULL);
        
        files = loader.rejected();
    }
    
    
    if ( !files.empty() )
        {
        //if any files were left, try raw as last resort
        
        new rawimporter (files);
        }
}
