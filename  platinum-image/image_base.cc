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

//#include "image_base.h"
#include "datawidget.h"
#include "bruker.h"
#include "image_label.hxx" //used in... analyze_objloader::read()
#include "image_complex.hxx" //used in... analyze_objloader::read() //????
//#include "image_complex.hxx"
//#include "itkVTKImageIO.h"
//#include "fileutils.h"
//#include "image_integer.hxx"
#include "viewmanager.h"
#include "datamanager.h"
#include "rendermanager.h"

//#include "vtkGESignaReader.h"

using namespace std;

extern datamanager datamanagement;
extern rendermanager rendermanagement;
extern viewmanager viewmanagement;

image_base::image_base():data_base(NULL,get_supported_renderers())
    {set_parameters();}

image_base::image_base(image_base* const s):data_base(s, get_supported_renderers())
    {
    set_parameters();
    //setting copy name at the root would be neat,
    //but is not possible since the widget isn't
    //created yet
    
    origin = s->get_origin();
    orientation = s->get_orientation();
	
    name ("Copy of " + s->name());
    }

vector<RENDERER_TYPE> image_base::get_supported_renderers(){
	vector<RENDERER_TYPE> vec;
	vec.push_back(RENDERER_MPR);
	vec.push_back(RENDERER_MIP);
	return vec;
}
void image_base::set_parameters ()    
    {
    ostringstream namestream;
    //constructor: add "Untitled" name and ID
    namestream << "3D image (" << ID << ")";
	widget=new datawidget<image_base>(this,namestream.str());
    name(namestream.str());
    
    origin.Fill(0);
    orientation.SetIdentity();

    Fl_Group::current(NULL); //evil bugfix: somehwere, sometime, a Group has not been ended
    }



bool image_base::read_origin_from_dicom_file(std::string dcm_file)
{
	bool succeded = false;
	itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();
    
	if (dicomIO->CanReadFile(dcm_file.c_str()))
        {
		dicomIO->SetFileName(dcm_file.c_str());
		dicomIO->ReadImageInformation();		//get basic DICOM header
		this->origin[0] = float(dicomIO->GetOrigin(0));
		this->origin[1] = float(dicomIO->GetOrigin(1));
		this->origin[2] = float(dicomIO->GetOrigin(2));
		succeded = true;
        }
	return succeded;
}

bool image_base::read_orientation_from_dicom_file(std::string dcm_file)
{
	bool succeded = false;
	itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();
    
	if (dicomIO->CanReadFile(dcm_file.c_str()))
        {
		dicomIO->SetFileName(dcm_file.c_str());
		dicomIO->ReadImageInformation();		//get basic DICOM header
		std::vector<double> a = dicomIO->GetDirection(0);
		std::vector<double> b = dicomIO->GetDirection(1);
		std::vector<double> c = dicomIO->GetDirection(2);
		this->orientation[0][0] = a[0];this->orientation[1][0] = a[1];this->orientation[2][0] = a[2];
		this->orientation[0][1] = b[0];this->orientation[1][1] = b[1];this->orientation[2][1] = b[2];
		this->orientation[0][2] = c[0];this->orientation[1][2] = c[1];this->orientation[2][2] = c[2];
		
		succeded = true;
        }
	return succeded;
}

bool image_base::read_slice_orientation_from_dicom_file(std::string dcm_file)
{
	bool succeded = false;
//	itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();
    /*
//	if (dicomIO->CanReadFile(dcm_file.c_str())){
//	}
	typename itk::OrientedImage<ELEMTYPE, IMAGEDIM >::Pointer image = get_image_as_itk_output();
    writer->SetFileName( file_path.c_str() );
    writer->SetInput(image);
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

*/
	return succeded;
}


void image_base::rotate_orientation(int fi_x_deg, int fi_y_deg, int fi_z_deg)
{
//    matrix_generator mg;
//    orientation = mg.get_rot_matrix_3D(fi_z_deg,fi_y_deg,fi_x_deg)*orientation;
    orientation = create_rot_matrix_3D(fi_x_deg*pt_PI/180.0, fi_y_deg*pt_PI/180.0, fi_z_deg*pt_PI/180.0)*orientation;
}

void image_base::rotate_orientation(float fi_x_rad, float fi_y_rad, float fi_z_rad)
{
//    matrix_generator mg;
//    orientation = mg.get_rot_matrix_3D(fi_z_rad,fi_y_rad,fi_x_rad)*orientation;
    orientation = create_rot_matrix_3D(fi_x_rad, fi_y_rad, fi_z_rad)*orientation;
}

void image_base::rotate_origin(int rot_axis, int pos_neg_dir)
{
	int sx = this->get_size_by_dim(0);
	int sy = this->get_size_by_dim(1);
	int sz = this->get_size_by_dim(2);

	if(rot_axis == 0 && pos_neg_dir == +1){
		this->set_origin(this->voxel_to_world(create_Vector3D(0,0,sz)));
	}
	else if(rot_axis == 0 && pos_neg_dir == -1){
		this->set_origin(this->voxel_to_world(create_Vector3D(0,sy,0)));
	}
	else if(rot_axis == 1 && pos_neg_dir == +1){
		this->set_origin(this->voxel_to_world(create_Vector3D(sx,0,0)));
	}
	else if(rot_axis == 1 && pos_neg_dir == -1){
		this->set_origin(this->voxel_to_world(create_Vector3D(0,0,sz)));
	}
	else if(rot_axis == 2 && pos_neg_dir == +1){
		this->set_origin(this->voxel_to_world(create_Vector3D(0,sy,0)));
	}
	else if(rot_axis == 2 && pos_neg_dir == -1){
		this->set_origin(this->voxel_to_world(create_Vector3D(sx,0,0)));
	}
	else{
		pt_error::error("rotate_origin-->parameters...("+int2str(rot_axis)+", "+int2str(pos_neg_dir)+")",pt_error::debug);
	}
}

void image_base::rotate_voxel_size(int rot_axis, int pos_neg_dir)
{
	Vector3D s = this->get_voxel_size();
	cout<<"rotate_voxel_size... s="<<s<<endl;

	if(rot_axis == 0){
		this->set_voxel_size(create_Vector3D(s[0],s[2],s[1]));
	}
	else if(rot_axis == 1){
		this->set_voxel_size(create_Vector3D(s[2],s[1],s[0]));
	}
	else if(rot_axis == 2){
		this->set_voxel_size(create_Vector3D(s[1],s[0],s[2]));
	}
	else{
		pt_error::error("rotate_voxel_size-->parameters...("+int2str(rot_axis)+", "+int2str(pos_neg_dir)+")",pt_error::debug);
	}
}

string image_base::get_slice_orientation()
{
	return this->slice_orientation;
}

void image_base::set_slice_orientation(string s)
{
	//undefined/axial/coronal/sagittal (lower case letters...)
	if(s=="a"){
		this->slice_orientation = "axial";
	}else if(s=="c"){
		this->slice_orientation = "coronal";
	}else if(s=="s"){
		this->slice_orientation = "sagittal";
	}else if(s=="u"){
		this->slice_orientation = "undefined";
	}else{
		this->slice_orientation = s;
	}
}

void image_base::redraw()
    {
    rendermanagement.data_has_changed(ID);
    }

Matrix3D image_base::get_orientation () const
{
    return orientation;
}

string image_base::get_orientation_as_dcm_string()
{
	string s="";
	for(unsigned int c=0;c<2;c++){
		for(unsigned int d=0;d<3;d++){
            s += float2str(this->orientation[d][c]) + "\\";
		}
	}
	return s;
}

Vector3D image_base::get_slice_direction()
{
	return create_Vector3D(orientation[0][2],orientation[1][2],orientation[2][2]);
}
Vector3D image_base::get_voxel_x_dir_in_phys()
{
	return create_Vector3D(orientation[0][0],orientation[1][0],orientation[2][0]);
}
Vector3D image_base::get_voxel_y_dir_in_phys()
{
	return create_Vector3D(orientation[0][1],orientation[1][1],orientation[2][1]);
}
Vector3D image_base::get_voxel_z_dir_in_phys()
{
	return create_Vector3D(orientation[0][2],orientation[1][2],orientation[2][2]);
}

Matrix3D image_base::get_dir_rendering_matrix(preset_direction direction)
{
    Matrix3D dir;

    switch(direction){
		case DEFAULT_DIR:
			dir = orientation;
			break;

		case AXIAL:
			if(this->get_slice_orientation()=="axial"){
				dir = orientation;
			}else if(this->get_slice_orientation()=="sagittal"){
				copy_columns(dir, orientation, 0, 2);
				copy_columns(dir, orientation, 1, 0);
				copy_columns(dir, orientation, 2, 1, true);
			}else if(this->get_slice_orientation()=="coronal"){
				copy_columns(dir, orientation, 0, 0);
				copy_columns(dir, orientation, 1, 2);
				copy_columns(dir, orientation, 2, 1, true);
			}
            break;

		case SAGITTAL:
			if(this->get_slice_orientation()=="axial"){
				copy_columns(dir, orientation, 0, 1);
				copy_columns(dir, orientation, 1, 2, true);
				copy_columns(dir, orientation, 2, 0);
			}else if(this->get_slice_orientation()=="sagittal"){
				dir = orientation;
			}else if(this->get_slice_orientation()=="coronal"){
				copy_columns(dir, orientation, 0, 2);
				copy_columns(dir, orientation, 1, 1);
				copy_columns(dir, orientation, 2, 0, true);
			}
	        break;
		case CORONAL:
			if(this->get_slice_orientation()=="axial"){
				copy_columns(dir, orientation, 0, 0);
				copy_columns(dir, orientation, 1, 2, true);
				copy_columns(dir, orientation, 2, 1);
			}else if(this->get_slice_orientation()=="sagittal"){
				copy_columns(dir, orientation, 0, 2, true);
				copy_columns(dir, orientation, 1, 1);
				copy_columns(dir, orientation, 2, 0);
			}else if(this->get_slice_orientation()=="coronal"){
				dir = orientation;
			}
	        break;


	}
	return dir;
}


void image_base::set_orientation(const Matrix3D m)
{
    orientation = m;
}

void image_base::set_orientation_to_unity()
{
	Matrix3D m;
	m.Fill(0);
	m[0][0]=m[1][1]=m[2][2]=1;
	this->set_orientation(m);
}

Vector3D image_base::get_origin () const
{
    return origin;
}

void image_base::set_origin(const Vector3D v)
{
    origin = v;
}

void image_base::translate_origin(const Vector3D t)
{
    origin += t;
}



void image_base::save_histogram_to_txt_file(const std::string filename, const std::string separator)
    {
        pt_error::pt_error ("Attempt to save_histogram_to_txt_file on a image_base object",pt_error::warning);
    }
void image_base::make_histogram_curve()
    {
        pt_error::pt_error ("Attempt to add histogram on a image_base object",pt_error::warning);
    }

Vector3D image_base::world_to_voxel( const Vector3D & wpos ) const
{
	// (AB)^-1 = B^-1 * A^-1

	Vector3D vpos = wpos - origin;

	Matrix3D inv_orientation;
	inv_orientation = get_orientation().GetInverse();
	
	Matrix3D inv_voxel_resize;
	inv_voxel_resize = get_voxel_resize().GetInverse();

	vpos = inv_voxel_resize * inv_orientation * vpos;	// the operations are done from right to left
	
	return vpos;
}

string image_base::resolve_value_world(Vector3D worldPos)
{
	string s="";
	Vector3D vPos = this->world_to_voxel(worldPos);
	if( this->is_voxelpos_within_image_3D(create_Vector3Dint(vPos[0],vPos[1],vPos[2])) ){
		s = "" + float2str(this->get_number_voxel(vPos[0],vPos[1],vPos[2])); //JK - quick fix...
	}
	return s;
}

string image_base::resolve_tooltip()
{
	return resolve_tooltip_image_base();
}

string image_base::resolve_tooltip_image_base()
{
	return resolve_tooltip_data_base()  +  "origin="+Vector3D2str(origin)+"\n"  +  "slice_orientation="+this->get_slice_orientation()+"\n";  //+"\n"+"orientation="+orientation;
}

string image_base::resolve_elemtype()
{
	return "undefined";
}


imageloader::imageloader(std::vector<std::string> * f)
{
	files = f;
}

imageloader::~imageloader()
{ }


Vector3D image_base::voxel_to_world( const Vector3D & vpos ) const
{
	Vector3D wpos;
	wpos = origin + get_orientation() * get_voxel_resize() * vpos;
	return wpos;
}

/*
class vtkloader: public imageloader
{
private:
    itk::VTKImageIO::Pointer vtkIO;
    
public:
    vtkloader (std::vector<std::string> *);
    image_base * read ();
};
*/

vtkloader::vtkloader(std::vector<std::string> * f): imageloader(f)
{
    vtkIO = itk::VTKImageIO::New();
}


image_base *vtkloader::read()
{    
    image_base *result = NULL;

	for(vector<string>::iterator it = files->begin(); it != files->end() && result == NULL; ){ // Repeat until one image has been read
	string file_path = *it;

	if(vtkIO->CanReadFile(file_path.c_str())){   //Assumption: File contains image data
        vtkIO->SetFileName(file_path);
        vtkIO->ReadImageInformation(); 
        itk::ImageIOBase::IOPixelType pixelType=vtkIO->GetPixelType();
        itk::ImageIOBase::IOComponentType componentType = vtkIO->GetComponentType();
        
        switch(pixelType){
            case itk::ImageIOBase::SCALAR:	//Enumeration values: UCHAR, CHAR, USHORT, SHORT, UINT, INT, ULONG, LONG, FLOAT, DOUBLE
                switch(componentType){
                    case itk::ImageIOBase::UCHAR:
                        result =  new image_integer<unsigned char>();
                        ((image_integer<unsigned char>*)result)->load_dataset_from_VTK_file(file_path);
                        break;
                    case itk::ImageIOBase::USHORT:
                        result = new image_integer<unsigned short>();
                        ((image_integer<unsigned short>*)result)->load_dataset_from_VTK_file(file_path);
                        break;
                    case itk::ImageIOBase::SHORT:
                        result = new image_integer<short>();
                        ((image_integer<short>*)result)->load_dataset_from_VTK_file(file_path);
                        break;
					case itk::ImageIOBase::ULONG:
                        result = new image_integer<unsigned long>();
                        ((image_integer<unsigned long>*)result)->load_dataset_from_VTK_file(file_path);
                        break;
					case itk::ImageIOBase::LONG:
                        result = new image_integer<long>();
                        ((image_integer<long>*)result)->load_dataset_from_VTK_file(file_path);
                        break;
                    case itk::ImageIOBase::FLOAT:
                        result = new image_integer<float>();
                        ((image_integer<float>*)result)->load_dataset_from_VTK_file(file_path);
                        break;
                    case itk::ImageIOBase::DOUBLE:
                        result = new image_integer<double>();
                        ((image_integer<double>*)result)->load_dataset_from_VTK_file(file_path);
                        break;
                    default:
                        pt_error::error("Load scalar VTK: unsupported component type: " + vtkIO->GetComponentTypeAsString (componentType), pt_error::warning);
                    }
                break;


			case itk::ImageIOBase::COMPLEX:
                switch (componentType){
/*					case itk::ImageIOBase::UCHAR:
						result = new image_complex<unsigned char>();
						((image_scalar<unsigned char>*)result)->load_dataset_from_VTK_file(file_path);
						break;
					case itk::ImageIOBase::USHORT:
						result = new image_complex<unsigned short>();
						((image_scalar<unsigned short>*)result)->load_dataset_from_VTK_file(file_path);
						break;
					case itk::ImageIOBase::SHORT:
						result = new image_complex<short>();
						((image_scalar<short>*)result)->load_dataset_from_VTK_file(file_path);
						break;
						*/
					case itk::ImageIOBase::FLOAT:
						result = new image_complex<float>();
						((image_complex<float>*)result)->load_dataset_from_VTK_file(file_path);
						break;
					#ifdef _DEBUG
					default:
						cout << "Load complex VTK: unsupported component type: " << vtkIO->GetComponentTypeAsString (componentType) << endl;
					#endif
					}
                break;

			case itk::ImageIOBase::VECTOR:
                switch (componentType){
					case itk::ImageIOBase::FLOAT:
						result = new image_complex<float>();
						((image_complex<float>*)result)->load_dataset_from_VTK_file(file_path);
						break;
					#ifdef _DEBUG
					default:
						cout << "Load VECTOR VTK: unsupported component type: " << vtkIO->GetComponentTypeAsString (componentType) << endl;
					#endif
					}
                break;


			default:
                pt_error::error("image_base::load(...): unsupported pixel type: " + vtkIO->GetPixelTypeAsString(pixelType), pt_error::warning);
            }

		    //file was read - remove from list
			read_file = *it;
	        it = files->erase(it);
        }//can read
	else{
		it++;
	}
	}//for
    return result;
}


analyze_hdrloader_itk::analyze_hdrloader_itk(std::vector<std::string> *f): imageloader(f)
{
	hdrIO = itk::AnalyzeImageIO::New();
}
    
image_base * analyze_hdrloader_itk::read()
{    
    image_base *result = NULL;

	for(vector<string>::iterator it = files->begin(); it != files->end() && result == NULL; it++)
	{ // Repeat until one image has been read
		string file_path = *it;

		if(hdrIO->CanReadFile(file_path.c_str()))
		{   //Assumption: File contains image data
			hdrIO->SetFileName(file_path);
			hdrIO->ReadImageInformation(); 
			itk::ImageIOBase::IOPixelType pixelType=hdrIO->GetPixelType();
			itk::ImageIOBase::IOComponentType componentType = hdrIO->GetComponentType();
        
			switch(pixelType)
			{
				case itk::ImageIOBase::SCALAR:
					//Enumeration values: UCHAR, CHAR, USHORT, SHORT, UINT, INT, ULONG, LONG, FLOAT, DOUBLE

					switch(componentType)
					{
						case itk::ImageIOBase::UCHAR:
							result =  new image_integer<unsigned char>();
							((image_integer<unsigned char>*)result)->load_dataset_from_hdr_file(file_path);
							break;
						case itk::ImageIOBase::USHORT:
							result = new image_integer<unsigned short>();
							((image_integer<unsigned short>*)result)->load_dataset_from_hdr_file(file_path);
							break;
						case itk::ImageIOBase::SHORT:
							result = new image_integer<short>();
							((image_integer<short>*)result)->load_dataset_from_hdr_file(file_path);
							break;
						case itk::ImageIOBase::FLOAT:
							result = new image_integer<float>();
							((image_integer<float>*)result)->load_dataset_from_hdr_file(file_path);
							break;
						default:
							pt_error::error("Load scalar hdr: unsupported component type: " + hdrIO->GetComponentTypeAsString (componentType), pt_error::warning);
					}
					break;

					break;
				default:
					pt_error::error("image_base::load(...): unsupported pixel type: " + hdrIO->GetPixelTypeAsString(pixelType), pt_error::warning);
			}

		    //file was read - remove from list
			read_file = *it;
	        files->erase(it);
        }//can read
	}//for
	
	if ( result != NULL )
	{
		// std::vector<double> a = hdrIO->GetDirection(0);
		// std::vector<double> b = hdrIO->GetDirection(1);
		// std::vector<double> c = hdrIO->GetDirection(2);
		
		// std::cout << "original orientation:" << std::endl;
		// std::cout << a[0] << "\t" << b[0] << "\t" << c[0] << std::endl;
		// std::cout << a[1] << "\t" << b[1] << "\t" << c[1] << std::endl;
		// std::cout << a[2] << "\t" << b[2] << "\t" << c[2] << std::endl;

		// Not a very nice solution but it works for now...
		Matrix3D m;	
		
		m[0][0] = 1;  m[0][1] = 0;  m[0][2] = 0;
		m[1][0] = 0;  m[1][1] = 0;  m[1][2] = -1;
		m[2][0] = 0;  m[2][1] = -1; m[2][2] = 0;
		result->set_orientation(m);	
	}
	
	return result;
}

ximgloader::ximgloader(std::vector<std::string> * f): imageloader(f)
{
	ximgIO = itk::GE5ImageIO::New();
}

image_base * ximgloader::read()
{    
    image_base * result = NULL;

	for(vector<string>::iterator itr = files->begin(); itr != files->end() && result == NULL; itr++)
	{ // Repeat until one image has been read
		string file_path = *itr;

		if(ximgIO->CanReadFile(file_path.c_str()))
		{   //Assumption: File contains image data
			ximgIO->SetFileName(file_path);
			ximgIO->ReadImageInformation(); 
			itk::ImageIOBase::IOPixelType pixelType = ximgIO->GetPixelType();
	
			itk::ImageIOBase::IOComponentType componentType = ximgIO->GetComponentType();	// returns "UNKNOWNCOMPONENTTYPE"
			// the GetComponentType() returns "UNKNOWNCOMPONENTTYPE" but since the vtkvtkGESignaReader.h says
			// that GE Signa ximg files "...always produces greyscale unsigned short data..."
			// componentType is manual set to unsigned short
			componentType = itk::ImageIOBase::USHORT;
			
			// the same information can also be achived using the vtkGESignaReader			
			// vtkGESignaReader * GESignaReader = vtkGESignaReader::New();
			// GESignaReader->SetFileName(file_path.c_str());

			switch(pixelType)
			{
				case itk::ImageIOBase::SCALAR:
					//Enumeration values: UCHAR, CHAR, USHORT, SHORT, UINT, INT, ULONG, LONG, FLOAT, DOUBLE

					switch(componentType)
					{
						case itk::ImageIOBase::UCHAR:
							result =  new image_integer<unsigned char>();
							((image_integer<unsigned char>*)result)->load_dataset_from_ximg_file(file_path);
							break;
						case itk::ImageIOBase::USHORT:
							result = new image_integer<unsigned short>();
							((image_integer<unsigned short>*)result)->load_dataset_from_ximg_file(file_path);
							break;
						case itk::ImageIOBase::SHORT:
							result = new image_integer<short>();
							((image_integer<short>*)result)->load_dataset_from_ximg_file(file_path);
							break;
						case itk::ImageIOBase::FLOAT:
							result = new image_integer<float>();
							((image_integer<float>*)result)->load_dataset_from_ximg_file(file_path);
							break;
						default:
							pt_error::error("ximgloader::read(): unsupported component type: " + ximgIO->GetComponentTypeAsString (componentType), pt_error::warning);
					}
					break;

				default:
					pt_error::error("image_base::load(...): unsupported pixel type: " + ximgIO->GetPixelTypeAsString(pixelType), pt_error::warning);
			}

		    //file was read - remove from list
			read_file = *itr;
	        files->erase(itr);
        }
	}
		
	return result;
}

/*
dicomloader::dicomloader(std::vector<std::string> *f): imageloader(f)
{
	dicomIO = itk::GDCMImageIO::New();
	this->this_load_type = DCM_LOAD_SERIES_ID_ONLY; //alt. DCM_LOAD_ALL
}
*/

dicomloader::dicomloader(std::vector<std::string> *f, DICOM_LOADER_TYPE type): imageloader(f)
{
   dicomIO = itk::GDCMImageIO::New();
   this_load_type = type;
}


image_base *dicomloader::read()
{
	image_base *result = NULL;
	int i=-1;
	for(std::vector<std::string>::const_iterator file = files->begin();file != files->end() && result == NULL;file++){ // Repeat until one image has been read
//	for(std::vector<std::string>::iterator file = files->begin();file != files->end() && result == NULL;file++){ // Repeat until one image has been read
		i++;
//	for(int i=0; i<files->size() && result == NULL; i++){ // Repeat until one image has been read
		if(dicomIO->CanReadFile(file->c_str())){
			dicomIO->SetFileName(file->c_str());
			dicomIO->ReadImageInformation();	//get basic DICOM header
			std::string seriesIdentifier;		//get series UID, "0020|000e", defined by scanner, multiple series in one directory is possible.
												//note that in a, for example, "dual" MR sequence, both/all echoes are given the same series ID...
			std::string labelId;				//if files with other echo times are found for the same series... add them to the "files"

			if(itk::GDCMImageIO::GetLabelFromTag(DCM_SERIES_ID, labelId) ){
				if(dicomIO->GetValueFromTag(DCM_SERIES_ID, seriesIdentifier) ){
					seriesIdentifier = seriesIdentifier.c_str();	// remove one garbage char at end (this is probably not a very good solution)
					//TODO: find out why this "garbage character" exist. 

					//check if another file in the same series was part of the selection (and loaded)
					std::vector<string>::const_iterator series_itr=loaded_series.begin();
					bool already_loaded=false;

					//if this series ID and TE already has been loaded ---> don't load... 
//					cout<<"*********** check already loaded... *********************"<<endl;
					#ifdef CHECK_ALREADY_LOADED_FILES
						already_loaded = is_file_already_loaded(*file);
					#endif

					if(!already_loaded){

						loaded_series.push_back(seriesIdentifier);
						loaded_TEs.push_back(get_dicom_tag_value(*file,DCM_TE));

						itk::ImageIOBase::IOPixelType pixelType=dicomIO->GetPixelType();
						itk::ImageIOBase::IOComponentType theComponentType = dicomIO->GetComponentType();

						switch(pixelType){ 							//Enumeration values: UCHAR, CHAR, USHORT, SHORT, UINT, INT, ULONG, LONG, FLOAT, DOUBLE
						case itk::ImageIOBase::SCALAR:
							if(this_load_type==DCM_LOAD_SERIES_ID_ONLY){
								switch(theComponentType){
								case itk::ImageIOBase::UCHAR:
									result = new image_integer<unsigned char>();
									((image_integer<unsigned char>*)result)->load_dataset_from_DICOM_fileAF(*file,seriesIdentifier);
									read_file = *file;//save file path
									break;
								case itk::ImageIOBase::CHAR:
									result = new image_integer<char>();
									((image_integer<char>*)result)->load_dataset_from_DICOM_fileAF(*file,seriesIdentifier);
									read_file = *file;//save file path
									break;
								case itk::ImageIOBase::USHORT:
									result = new image_integer<unsigned short>();
									((image_integer<unsigned short>*)result)->load_dataset_from_DICOM_fileAF(*file,seriesIdentifier);
									read_file = *file;//save file path
									break;
								case itk::ImageIOBase::SHORT:
									result = new image_integer<short>();
									((image_integer<short>*)result)->load_dataset_from_DICOM_fileAF(*file,seriesIdentifier);
									read_file = *file;//save file path
									break;
								case itk::ImageIOBase::UINT:
									result = new image_integer<unsigned int>();
									((image_integer<unsigned int>*)result)->load_dataset_from_DICOM_fileAF(*file,seriesIdentifier);
									read_file = *file;//save file path
									break;
								case itk::ImageIOBase::INT:
									result = new image_integer<int>();
									((image_integer<int>*)result)->load_dataset_from_DICOM_fileAF(*file,seriesIdentifier);
									read_file = *file;//save file path
									break;
								case itk::ImageIOBase::FLOAT:
									result = new image_integer<float>();
									((image_integer<float>*)result)->load_dataset_from_DICOM_fileAF(*file,seriesIdentifier);
									read_file = *file;//save file path
									break;
								case itk::ImageIOBase::DOUBLE:
									result = new image_integer<double>();
									((image_integer<double>*)result)->load_dataset_from_DICOM_fileAF(*file,seriesIdentifier);
									break;
								default:
									pt_error::error("dicomloader::read() --> Unsupported component type: " + dicomIO->GetComponentTypeAsString (theComponentType), pt_error::warning);
								}

							}else if(this_load_type==DCM_LOAD_ALL){

							switch (theComponentType){
								case itk::ImageIOBase::UCHAR:
									result = new image_integer<unsigned char>();
									((image_integer<unsigned char>*)result)->load_dataset_from_these_DICOM_files(*files);
									read_file = "";//save file path
									return result;
									break;
								case itk::ImageIOBase::USHORT:
									result = new image_integer<unsigned short>();
									((image_integer<unsigned short>*)result)->load_dataset_from_these_DICOM_files(*files);
									read_file = "";//save file path
									return result;
									break;
								case itk::ImageIOBase::SHORT:
									result = new image_integer<short>();
									((image_integer<short>*)result)->load_dataset_from_these_DICOM_files(*files);
									read_file = "";//save file path
									return result;
									break;
								case itk::ImageIOBase::FLOAT:
									result = new image_integer<float>();
									((image_integer<float>*)result)->load_dataset_from_these_DICOM_files(*files);
									read_file = "";//save file path
									return result;
									break;
								default:
									pt_error::error("dicomloader::read() --> Unsupported component type: " + dicomIO->GetComponentTypeAsString (theComponentType), pt_error::warning);
								}//switch
							}//DCM_LOAD_ALL

						break;
							case itk::ImageIOBase::COMPLEX:
//								switch (theComponentType){
//								case itk::ImageIOBase::FLOAT:
									result = new image_complex<float>();
									((image_complex<float>*)result)->load_complex_dataset_from_these_DICOM_files(*files);
									read_file = "";//save file path
									return result;
//									break;
//								}
//								pt_error::error("image_base::load(...): unsupported pixel type(COMPLEX): " + dicomIO->GetPixelTypeAsString(pixelType),pt_error::warning);
								break;

//							case itk::ImageIOBase::RGB:
//								pt_error::error("image_base::load(...): unsupported pixel type(RGB): " + dicomIO->GetPixelTypeAsString(pixelType),pt_error::warning);
//								break;

							default:
								pt_error::error("image_base::load(...): unsupported pixel type: " + dicomIO->GetPixelTypeAsString(pixelType),pt_error::warning);
					}//switch -scalar-complex
					
					//***JK shouldn't the file be removed from the vector here?

				}//not already loaded
			} //found series tag
		}//series tag exists

		}//can read file
		else{
			//no series identifier, OK if the intention is to just load 1 frame
			//(DICOM files can only contain 1 frame each)
			pt_error::error("(No Value Found in File)",pt_error::notice);
		}

//		files->clear(); //! if at least one file can be read, the rest are assumed to be part of the same series (or otherwise superfluos)
		//???? do not "clear"...

	}//for(file...)
	
	clear_files_vector_from_already_loaded();

//	((image_integer<float>*)result)->print_geometry();

	return result;
}

bool dicomloader::is_file_already_loaded(string file_path)
{
	string id = get_dicom_tag_value(file_path, DCM_SERIES_ID).c_str(); //removes garbage char...
	string te = get_dicom_tag_value(file_path, DCM_TE);
	for(int i=0;i<loaded_series.size(); i++){
		if(id==loaded_series[i] && te ==loaded_TEs[i]){
			return true;
		}
	}
	return false;
}

void dicomloader::clear_files_vector_from_already_loaded()
{
//	cout<<"clear_files_vector_from_already_loaded..."<<endl;
	//remove files from vector with the right (loaded) combinations of series - id....
/*
	for(int i=0; i<files->size(); i++){
		cout<<"files[i]="<<files->at(i)<<endl;
	}
	for(int i=0; i<loaded_series.size(); i++){
		cout<<"loaded_series[i]=("<<loaded_series[i]<<")"<<endl;
		cout<<"loaded_TEs[i]="<<loaded_TEs[i]<<endl;
	}
*/
	bool file_found=false;
	for(vector<string>::iterator it = files->begin(); it!=files->end()&&!file_found;){
		if(is_file_already_loaded(*it)){
			file_found=true;
			it = files->erase(it);
		}else{
			it++;
		}
	}

	if(file_found){
		clear_files_vector_from_already_loaded();
	}else{
//		cout<<"result_files..."<<endl;
//		for(int i=0; i<files->size(); i++){
//			cout<<"result_files[i]="<<files->at(i)<<endl;
//		}
	}
}

/*
void dicomloader::anonymize_single_dcm_file(string load_path, string save_path){
	// #if defined(_MSC_VER)
	// #pragma warning ( disable : 4786 )
	// #endif

	// This example illustrates how to read a single DICOM slice and write it back
	// with some changed header information as another DICOM slice. Header
	// Key/Value pairs can be specified on the command line. The keys are defined
	// in the file  \code{Insight/Utilities/gdcm/Dicts/dicomV3.dic}

	// #include <list>
	// #include <fstream>

	// std::cerr << "Usage: " << argv[0] << " DicomImage OutputDicomImage Entry Value\n";

	typedef unsigned short InputPixelType;
	const unsigned int   Dimension = 2;
	typedef itk::Image< InputPixelType, Dimension > InputImageType;
	typedef itk::ImageFileReader< InputImageType > ReaderType;
	ReaderType::Pointer reader = ReaderType::New();
	reader->SetFileName( load_path.c_str() );
	typedef itk::GDCMImageIO           ImageIOType;
	ImageIOType::Pointer gdcmImageIO = ImageIOType::New();
	reader->SetImageIO( gdcmImageIO );
	try{
		reader->Update();
	}catch (itk::ExceptionObject & e){
		std::cerr << "exception in file reader " << std::endl;
		std::cerr << e.GetDescription() << std::endl;
		std::cerr << e.GetLocation() << std::endl;
	}

	InputImageType::Pointer inputImage = reader->GetOutput();
	typedef itk::MetaDataDictionary   DictionaryType;
	DictionaryType & dictionary = inputImage->GetMetaDataDictionary();

	//Print seriesIDs and make sure it is copied right....
	std::string id = "";
	gdcmImageIO->GetValueFromTag(DCM_SERIES_ID,id);
//	gdcmImageIO->setv

	cout<<"id="<<id<<endl;

	//-----------------------------
	itk::EncapsulateMetaData<std::string>( dictionary, DCM_PATIENT_NAME, "Anonymized" );
	itk::EncapsulateMetaData<std::string>( dictionary, DCM_PATIENT_ID, "Anonymized" );
	itk::EncapsulateMetaData<std::string>( dictionary, DCM_PATIENT_BIRTH_DATE, "Anonymized" );

//	gdcm::FileHelper fh = gdcm::FileHelper();
//	fh.SetKeepMediaStorageSOPClassUID(true);
//	gdcm::FileHelper::SetKeepMediaStorageSOPClassUID(true); //Compile error--> try updating ITK...

	//see: http://www.creatis.insa-lyon.fr/pipermail/dcmlib/2006-January/002730.html
	//??? KeepOriginalUIDOn ???

	// Makes sure the seriesID is copied correctly... 
	// Allowing correct later "Opening" of file series in Platinum
//	itk::EncapsulateMetaData<std::string>( dictionary, DCM_SERIES_ID, id ); 
	//-----------------------------

	typedef itk::ImageFileWriter< InputImageType >  Writer1Type;
	Writer1Type::Pointer writer1 = Writer1Type::New();
	writer1->SetInput( reader->GetOutput() );
	writer1->SetFileName( save_path.c_str() );
	writer1->UseInputMetaDataDictionaryOff(); 
	writer1->SetImageIO( gdcmImageIO );
	try{
		writer1->Update();
	}catch (itk::ExceptionObject & e){
		std::cerr << "exception in file writer " << std::endl;
		std::cerr << e.GetDescription() << std::endl;
		std::cerr << e.GetLocation() << std::endl;
	}

	// Remember again, that modifying the header entries of a DICOM file involves
	// very serious risks for patients and therefore must be done with extreme
	// caution.
}
*/

/*
void dicomloader::anonymize_all_dcm_files_in_folder(string load_folder_path, string save_folder_path){
	vector<string> all = get_dir_entries(load_folder_path);
	vector<string> dcm_files = vector<string>();

	itk::GDCMImageIO::Pointer tmp_dicomIO = itk::GDCMImageIO::New();

	cout<<"anonymize_all_dcm_files_in_folder...."<<endl;
	cout<<all.size()<<" items loaded"<<endl;

	for(int i=0;i<all.size();i++){
		cout<<i<<" "<<all[i]<<endl;

		if( tmp_dicomIO->CanReadFile( (load_folder_path + all[i]).c_str()) ){
			dcm_files.push_back(load_folder_path + all[i]);
		}
	}
	cout<<dcm_files.size()<<" dcm-files found"<<endl;

	for(int i=0;i<dcm_files.size();i++){
		dicomloader::anonymize_single_dcm_file( dcm_files[i],save_folder_path + path_end(dcm_files[i]) );
	}
}

void dicomloader::anonymize_all_dcm_files_in_folder2(string load_folder_path, string save_folder_path){
//JK - Use only data in the metadata class and save one dicom file using platinum
//(alt. save dicom series... remember to change image pos in this case...)


//#include "itkOrientedImage.h"
//#include "itkGDCMImageIO.h"
//#include "itkGDCMSeriesFileNames.h"
//#include "itkImageSeriesReader.h"
//#include "itkImageFileWriter.h"


  typedef signed short    PixelType;
  const unsigned int      Dimension = 3;
  typedef itk::Image< PixelType, Dimension >         ImageType;
//  typedef itk::OrientedImage< PixelType, Dimension >         ImageType;
  typedef itk::ImageSeriesReader< ImageType >        ReaderType;
  ReaderType::Pointer reader = ReaderType::New();

  typedef itk::GDCMImageIO       ImageIOType;
  ImageIOType::Pointer dicomIO = ImageIOType::New();
  reader->SetImageIO( dicomIO );

  typedef itk::GDCMSeriesFileNames NamesGeneratorType;
  NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();
  nameGenerator->SetUseSeriesDetails( true );
  nameGenerator->AddSeriesRestriction("0008|0021" );
  nameGenerator->SetDirectory( load_folder_path.c_str() );
 

  try
    {
    std::cout << std::endl << "The directory: " << std::endl;
    std::cout << std::endl << load_folder_path << std::endl << std::endl;
    std::cout << "Contains the following DICOM Series: ";
    std::cout << std::endl << std::endl;

    typedef std::vector< std::string >    SeriesIdContainer;
    const SeriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();
    SeriesIdContainer::const_iterator seriesItr = seriesUID.begin();
    SeriesIdContainer::const_iterator seriesEnd = seriesUID.end();
    while( seriesItr != seriesEnd )
      {
      std::cout << seriesItr->c_str() << std::endl;
      seriesItr++;
      }
  
    std::string seriesIdentifier;
    seriesIdentifier = seriesUID.begin()->c_str();

    std::cout << std::endl << std::endl;
    std::cout << "Now reading series: " << std::endl << std::endl;
    std::cout << seriesIdentifier << std::endl;
    std::cout << std::endl << std::endl;

    typedef std::vector< std::string >   FileNamesContainer;
    FileNamesContainer fileNames;
    fileNames = nameGenerator->GetFileNames( seriesIdentifier );
    reader->SetFileNames( fileNames );

    try
      {
      reader->Update();
      }
    catch (itk::ExceptionObject &ex)
      {
      std::cout << ex << std::endl;
      }


// We proceed now to save the volumetric image in another file, as specified by
// the user in the command line arguments of this program. Thanks to the
// ImageIO factory mechanism, only the filename extension is needed to identify
// the file format in this case.

	typedef itk::ImageFileWriter< ImageType > WriterType;
    WriterType::Pointer writer = WriterType::New();
    
	writer->SetFileName( "c:/Joel/TMP/joel.vtk" );
//	writer->SetFileName( "c:/Joel/TMP/joel.dcm" );

    writer->SetInput( reader->GetOutput() );

    std::cout  << "Writing the image as " << std::endl << std::endl;
    std::cout  << "c:/Joel/TMP/joel.vtk" << std::endl << std::endl;
//    std::cout  << "c:/Joel/TMP/joel.dcm" << std::endl << std::endl;

    try
      {
      writer->Update();
      }
    catch (itk::ExceptionObject &ex)
      {
      std::cout << ex << std::endl;
      }
    }
  catch (itk::ExceptionObject &ex)
    {
    std::cout << ex << std::endl;
    }
}
*/

analyze_hdrloader::analyze_hdrloader(std::vector<std::string> * files): imageloader(files)
	{
	}

int analyze_hdrloader::buf2int(unsigned char* buf)
	{
	int res = buf[0] << 24|buf[1] << 16|buf[2] << 8|buf[3];
	return res;
	}

short analyze_hdrloader::buf2short(unsigned char* buf)
	{
	short res=buf[0]<<8|buf[1];
	return res;
	}

image_base * analyze_hdrloader::read()
    {
	
    image_base * newImage = NULL;
	for(vector<string>::iterator it = files->begin(); it != files->end() && newImage == NULL; it++){ // Repeat until one image has been read
		string file_path = *it;

	//	std::string hdr_file = files->front();
		std::string hdr_file = file_path;
		std::string img_file = hdr_file;
		std::string img_name;
	    
		unsigned int pos;
	    
		pos=img_file.rfind(".hdr",img_file.length()-1);
	    
		if (pos !=std::string::npos)
			{
			img_file.erase(pos,img_file.length());
			pos=img_file.rfind("/",img_file.length()-1);
			img_name = img_file.substr(pos+1);
			img_file += ".img";

			if (file_exists (img_file) && file_exists (hdr_file))
				{
				short size[3];
				Vector3D voxelsize;
				bool bigEndian;
				bool isSigned;
				bool isFloat;
				int bitDepth;

				std::ifstream hdr (std::string(hdr_file).c_str(),ios::in);
				unsigned char readbuf[100];
				
				hdr.read((char*)readbuf,4);
				int sizeof_hdr=buf2int(readbuf);

				hdr.ignore(36); //Skip
				
				hdr.read((char*)readbuf,2);
				short endian=buf2short(readbuf);
				bigEndian = ((endian >= 0) && (endian <= 15));
				hdr.read((char*)readbuf,2);
				size[0]=buf2short(readbuf);
				hdr.read((char*)readbuf,2);
				size[1]=buf2short(readbuf);
				hdr.read((char*)readbuf,2);
				size[2]=buf2short(readbuf);
				
				hdr.ignore(22); //Skip
				
				hdr.read((char*)readbuf,2);
				short datatype=buf2short(readbuf);

				switch (datatype) {	      
					case 2:
						isSigned=false;
						isFloat=false;
						bitDepth=8;
						break;
					case 4:
						isSigned=true;
						isFloat=false;
						bitDepth=16;
						break;
					case 8:
						isSigned=true;
						isFloat=false;
						bitDepth=32;
						break; 
					case 16:
						isSigned=true;
						isFloat=true;
						bitDepth=32;
						break; 
					case 128:
						isSigned=false;
						isFloat=false;
						bitDepth=24;
						break; 
					default:
						isSigned=false;
						isFloat=false;
						bitDepth=8;					// DT_UNKNOWN
					}

				voxelsize.Fill (1);

				if(isFloat)
					{
					newImage = allocate_image<image_scalar> (
						isFloat,
						isSigned,
						bitDepth,
						std::vector<std::string > (1,img_file), 
						size[0], size[1],
						bigEndian,
						0, 
						voxelsize);
					}
				else
					{
					newImage = allocate_image<image_integer> (
						isFloat,
						isSigned,
						bitDepth,
						std::vector<std::string > (1,img_file), 
						size[0], size[1],
						bigEndian,
						0, 
						voxelsize);
					}
				hdr.close();
				newImage->name(img_name);

				//file was read - remove from list
	//            files->erase (files->begin());
				read_file = *it;//save file path
				files->erase(it);
				}//exists
			}//pos
		}//for
		
    return newImage;
}


analyze_objloader::analyze_objloader(std::vector<std::string> * files): imageloader(files)
	{
	}

image_base *analyze_objloader::read()
    {
    image_base *newImage = NULL;
	for(vector<string>::iterator it = files->begin(); it != files->end() && newImage == NULL; it++){ // Repeat until one image has been read
		string file_path = *it;

	//	std::string obj_file = files->front();
		std::string obj_file = file_path;
		std::string img_name = obj_file;
	    
		unsigned int pos;
	    
		pos=img_name.rfind(".obj",img_name.length()-1);
	    
		if (pos !=std::string::npos)
			{
			img_name.erase(pos,img_name.length());
			pos=img_name.rfind("/",img_name.length()-1);
			img_name = img_name.substr(pos+1);
	    
			if (file_exists (obj_file))
				{
				short size[3];
				
				int nObjects=0;
				int skip=0;

				int corr=4;//3;//-1; ???????????????????????
				std::ifstream fobj (std::string(obj_file).c_str());
				unsigned char  header[20];
				fobj.read((char*)header,20);

				int data[5];
				int i;
				for(i=0; i<5; i++)
					{
					data[i]=header[i*4+0]<<24 | header[i*4+1]<<16 | header[i*4+2]<<8 | header[i*4+3];
					}
				size[0]=data[1];
				//size[1]=data[3];//Analyze6
				//size[2]=data[2];//Analyze6
				size[1]=data[2];//Analyze7
				size[2]=data[3];//Analyze7
				nObjects=data[4];
				skip=nObjects*152+corr;//TODO: This is not correct		
				int n_vox=size[0]*size[1]*size[2];

				unsigned char* buf=new unsigned char[n_vox];
				memset(buf,0,n_vox);

				// Start reading run-lenght encoded data
				fobj.ignore(skip);
				int l,b;
				int j;
				i=0;
				while(fobj.good())     // loop while extraction from file is possible
					{				
					l=fobj.get();	
					if(fobj.good())
						{
						b=fobj.get();
						for(j=0; j<l; j++)
							{
							buf[i]=b;
							i++;
							}
						}
					}

				Vector3D voxelsize;
				voxelsize.Fill(1);
				newImage = new image_label<>(buf, n_vox, size[0], size[1], voxelsize);

				fobj.close();
				newImage->name(img_name);

	//			files->erase (files->begin());
				read_file = *it;//save file path
				files->erase(it);
				}//obj file exists
			}//pos
		}//for

    return newImage;
}

template <class LOADERTYPE>
void image_base::try_loader(std::vector<std::string> *f) //! helper for image_base::load
{
	if(!f->empty()){
		LOADERTYPE loader = LOADERTYPE(f);
		image_base *new_image = NULL; //the eventually loaded image
		do{
			new_image = loader.read();
			if(new_image != NULL){ 
				cout << "About to load ad_data for " << ((imageloader)loader).read_file << endl;
				datamanagement.add(new_image);
				std::string ad_path;
				ad_path = ((imageloader)loader).read_file;
				size_t stop = ad_path.find_last_of(".");
				if(stop != string::npos && ad_path != ""){
					ad_path = ad_path.substr(0,stop).append(".pad");
					std::cout << "Trying to load: " << ad_path << std::endl;
					new_image->read_helper_data_from_file(ad_path);
				}
				//RN ad_insert
			}
		} 
		while(new_image !=NULL && !f->empty());
	}
}




niftiloader::niftiloader(std::vector<std::string> *f): imageloader(f)
{
    niftiIO = itk::NiftiImageIO::New();
}

image_base *niftiloader::read()
{    
	image_base *result = NULL;

	for(vector<string>::iterator it = files->begin(); it != files->end() && result == NULL; it++){ // Repeat until one image has been read
		string file_path = *it;

		if(niftiIO->CanReadFile(file_path.c_str())){	//Assumption: File contains image data
			niftiIO->SetFileName(file_path.c_str());
			niftiIO->ReadImageInformation(); 
			itk::ImageIOBase::IOComponentType componentType = niftiIO->GetComponentType();
			itk::ImageIOBase::IOPixelType pixelType=niftiIO->GetPixelType();
			switch ( pixelType)
			{
			case itk::ImageIOBase::SCALAR:
				//Enumeration values: UCHAR, CHAR, USHORT, SHORT, UINT, INT, ULONG, LONG, FLOAT, DOUBLE

				switch (componentType)
				{
				case itk::ImageIOBase::UCHAR:
					result =  new image_integer<unsigned char>();
					((image_integer<unsigned char>*)result)->load_dataset_from_NIFTI_file(file_path);
					break;
				case itk::ImageIOBase::CHAR:
					result =  new image_integer<char>();
					((image_integer<char>*)result)->load_dataset_from_NIFTI_file(file_path);
					break;
				case itk::ImageIOBase::USHORT:
					result = new image_integer<unsigned short>();
					((image_integer<unsigned short>*)result)->load_dataset_from_NIFTI_file(file_path);
					break;
				case itk::ImageIOBase::SHORT:
					result = new image_integer<short>();
					((image_integer<short>*)result)->load_dataset_from_NIFTI_file(file_path);
					break;
				case itk::ImageIOBase::FLOAT: //used for example in complex dixon data imported from deadface format (.df)
					result = new image_integer<float>();
					((image_integer<float>*)result)->load_dataset_from_NIFTI_file(file_path);
					break;
				default:
					pt_error::error("Load scalar NIFTI: unsupported component type: " + niftiIO->GetComponentTypeAsString (componentType), pt_error::warning);
				}
				break;

				/*case itk::ImageIOBase::COMPLEX:
				switch (componentType)
				{
				case itk::ImageIOBase::USHORT:
				result = new image_complex<unsigned short>();
				((image_scalar<unsigned short>*)result)->load_dataset_from_VTK_file(path_parent(*file));
				break;

				default:
				#ifdef _DEBUG
				cout << "Load complex VTK: unsupported component type: " << vtkIO->GetComponentTypeAsString (componentType) << endl;
				#endif
				}*/
				break;
			default:
				pt_error::error("image_base::load(...): unsupported pixel type: " + niftiIO->GetPixelTypeAsString(pixelType), pt_error::warning);

			}
			//file was read - remove from list
			//files->erase(files->begin());
			read_file = *it;//save file path
			files->erase(it);
		}//can read
	}//for

	return result;
}


void image_base::load( std::vector<std::string> chosen_files)	//loads all files and adds them to datamanagement...
{

	userIOmanagement.progress_update( 1, "Loading image(s)...", 2 );
	
	//Each read function reads the first image it can and removes it...
    //The try_loader calls the read function until NULL is returned...
	try_loader<ximgloader>(&chosen_files);			// try ximg loader
    try_loader<analyze_hdrloader_itk>(&chosen_files);	//try Analyze hdr
    try_loader<analyze_objloader>(&chosen_files);	//try Analyze obj
    //try_loader<analyze_hdrloader>(&chosen_files);	//try Analyze hdr
    try_loader<brukerloader>(&chosen_files);		//try Bruker
    try_loader<vtkloader>(&chosen_files);			//try VTK
    try_loader<niftiloader>(&chosen_files);			//try NIFTI
    try_loader<dicomloader>(&chosen_files);			//try DICOM

    if(!chosen_files.empty()){ //if any files were left, try raw as last resort
        rawimporter::create(chosen_files);
	}

	userIOmanagement.progress_update( 2, "Loading image(s)...", 2 );
	userIOmanagement.progress_update();
}


