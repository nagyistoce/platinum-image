### rendererMPR.h ###

```
    //// create image to view for the volume/viewport interface
    // MUST THINK A BIT UPON THIS!!!

    /*
    uchar *rendererMRP::make_gradientimage(int width, int height)
    {
    animage = new uchar[4*width*height];
    uchar *p = animage;
    for (int y = 0; y < height; y++) {
    double Y = double(y)/(height-1);
    for (int x = 0; x < width; x++) {
    double X = double(x)/(width-1);
    *p++ = uchar(255*((1-X)*(1-Y))); // red in upper-left
    *p++ = uchar(255*((1-X)*Y));	// green in lower-left
    *p++ = uchar(255*(X*Y));	// blue in lower-right
    X -= 0.5;
    Y -= 0.5;
    int alpha = (int)(255 * sqrt(X * X + Y * Y));
    //      if (alpha < 255) *p++ = uchar(alpha);	// alpha transparency
    //else *p++ = 255;
    *p++ = 255;

    Y += 0.5;
    }
    }
    }
    */
```
### rendererMPR.cc ###

```
/*//Scale (multiply by elements) two vectors

//as a quick substitute for vector->unit matrix->second vector operation



//Deprecated, still used in arbitrary MPR



Vector3D mult_elems( const Vector3D & one, const Vector3D & other ) {

Vector3D result;

for( unsigned int i=0; i<3; i++)

{

result[i] = one[i] * other [i];

}

return result;

}



//Scale one vector by the inverse of the argument



Vector3D div_elems( const Vector3D & numer, const Vector3D & denom ) {

Vector3D result;

for( unsigned int i=0; i<3; i++)

{

result[i] = numer[i] / denom [i];

}

return result;

}



//"True" MPR renderer allowing arbitrary slices

//should be combined with the above using a suitable condition

//eg. render normal x*y*z == 0



void rendererMPR::render_XYplane(unsigned char *rgb, int rgb_sx, int rgb_sy, float pan_x, float pan_y, float pan_z, float zoom){



cout<<"rendererMPR::render_XYplane pos: (" << pan_x << ", " << pan_y << ", " << pan_z << ") at " << zoom << " times magnification." <<endl;



volumehandler_base *volumestorender->rendervolume_pointers[MAXRENDERVOLUMES];

for (int i=0; i < noofrenderedvolumes; i++) {

volumestorender->rendervolume_pointers[i] = get_volumepointer(volumehandlerVID[i]); // get pointers to all volumes we will use for this render

}



//unified scaling:

//assume square screen pixels

//do not assume cube voxels



//Variables

//for unified scaling



Vector3D voxel_size; //only sets proportions, hence should be normalized

//bright idea: divide datasize by voxelsize,

//then normalize to get instant scaling

//max norm in that case



Vector3D data_size; //size of volume data in voxels

//voxel_size mult. elementwise with data_size yields

//the proportions of the volume



float scale; //voxel to pixel ratio, determined so that dataset's longest edge

//fits into renderer's unit space (this includes z-direction of course)

//multiplied with zoom it gives the V/P ratio to render with

//also taking into account current zoom level



float data_max_norm; //max norm of volume data size



Vector3D center; //center viewpoint, this will be the middle of the

//rendered image and the pivot point when altering slice direction.

//Unit coordinates



Vector3D slice_normal; //normal to the slice plane



Vector3D unit_center; //vector representing offset from cartesian to centered

//unit coordinate system



// Initialization



for (int d=0;d<3;d++) {

data_size[d]=volumestorender->rendervolume_pointers[0]->get_size_by_dim(d);

}

cout<<"Volumesize "<<data_size[0]<<" "<<data_size[1]<<" "<<data_size[2]<<endl;



data_max_norm=max(data_size[0],max(data_size[1],data_size[2]));



scale = min(float(rgb_sx),float(rgb_sy))/data_max_norm;



voxel_size[0]=1; //this should eventually be set in volumehandler

voxel_size[1]=1; //along with slice-by-slice metadata

voxel_size[2]=data_max_norm/data_size[2]; //quick fix for typical voxel size variations

//this should be moved to volumehandler

//and ultimately determined from metadata



slice_normal[0]=0;

slice_normal[1]=0;

slice_normal[2]=1;



center[0]=pan_x;

center[1]=pan_y;

center[2]=pan_z;



//slope should be determined by slice normal vector (+ screen Y or an up vector)

//currently hardcoded to x=data(x), y=data(y)

Vector3D slope_x, slope_y;

slope_x[0]=1;

slope_x[1]=0;

slope_x[2]=0;



slope_y[0]=0;

slope_y[1]=1;

slope_y[2]=0;



//set slope to size of render plane in unit coordinates

slope_x/=zoom;

slope_y/=zoom;



//compensate step length for irregular voxel size

slope_x=div_elems(slope_x,voxel_size);

slope_y=div_elems(slope_y,voxel_size);



//center of data in unit coordinates where longest edge = 1

unit_center=data_size;

unit_center/=data_max_norm*2;



//determine start position in unit space

Vector3D start=div_elems (center,voxel_size)+unit_center-(slope_x*0.5)-(slope_y*0.5);



//transform start to data space

start*=data_max_norm;

//start=mult_elems(start,data_size); //version using unit (each side 1 long) coordinates

//start=div_elems(start,voxel_size);



//transform scale to data space (scale = pix/data ratio)

slope_x/=scale;

slope_y/=scale;



//center slice onscreen

int nonsquare_offset=(rgb_sx-rgb_sy)/2;

if (nonsquare_offset > 0) {

//if positive, image is portrait orientation

start-=slope_x*nonsquare_offset;

}

else {

start+=slope_y*nonsquare_offset;

}



// Render loop



//1. iterate Y and determine new position of horizontal scanline

//2. check that position is within data bounds

//3. render pixel

//4. move along scanline (iterate x)

//5. repeat



//position in voxel data grid

Vector3D data_pos;



//voxel value in loop

unsigned char value=0;



for (int rgb_y=0; rgb_y < rgb_sy; rgb_y++){

data_pos=start+slope_y*rgb_y;



for (int rgb_x=0; rgb_x < rgb_sx; rgb_x++){

if ((data_pos[0]>0) && (data_pos[0]<data_size[0])

&& (data_pos[1]>0) && (data_pos[1]<data_size[1])

&& (data_pos[2]>0) && (data_pos[2]<data_size[2]))

{

value = volumestorender->rendervolume_pointers[0]->get_voxel(data_pos[0], data_pos[1], data_pos[2]);



rgb[RGBpixmap_bytesperpixel * (rgb_x+rgb_sx*rgb_y)] = value;

rgb[RGBpixmap_bytesperpixel * (rgb_x+rgb_sx*rgb_y) + 1] = value;

rgb[RGBpixmap_bytesperpixel * (rgb_x+rgb_sx*rgb_y) + 2] = value;

}

else {

//Outside data bounds, render black

rgb[RGBpixmap_bytesperpixel * (rgb_x+rgb_sx*rgb_y)] = 0;

rgb[RGBpixmap_bytesperpixel * (rgb_x+rgb_sx*rgb_y) + 1] = 0;

rgb[RGBpixmap_bytesperpixel * (rgb_x+rgb_sx*rgb_y) + 2] = 0;

}

data_pos+=slope_x;

}

}

}*/
```
### datamanager.cc ###
```
//Mac drag & drop:
//fl_open_callback() 

//debug method to identify proper tag for certain information
void print_DICOM_tag (itk::GDCMImageIO::Pointer gdcm_ptr, string tagkey)
{
    std::string value;
    std::string labelId;
    if( itk::GDCMImageIO::GetLabelFromTag( tagkey, labelId ) )
    {
        std::cout << labelId << " (" << tagkey << "): ";
        if( gdcm_ptr->GetValueFromTag(tagkey, value) )
        {
            std::cout << value << endl;
        }
        else
        {
            std::cout << "(No Value Found in File)";
        }
        std::cout << std::endl;
    }
    else
    {
        std::cerr << "Trying to access nonexistant DICOM tag." << std::endl;
    }
}

//Enumeration values: UNKNOWNPIXELTYPE,SCALAR, RGB, OFFSET, VECTOR, POINT, COVARIANTVECTOR, SYMMETRICSECONDRANKTENSOR, DIFFUSIONTENSOR3D, COMPLEX, FIXEDARRAY 

/*
void loadvolume_callback_wrapper(XXXX *argument) // wrapper for single pointer compatibility
{
datamanager::loadvolume_callback(argument)
}
*/

/*void datamanager::redraw_datawidgets ()
    {
    data_widget_box->interior->parent()->redraw();
    }*/
    
    /*

Princip:
Håller i volymerna:
vector volymer*[] har unikt ID

SKIPPA FOLLOW i detta gränssnitt!

*/


    /*switch (unit)
    {
    case VOLDATA_CHAR: avolume  =((volumehandler3D<char> *)blueprint)->alike(); break;
    case VOLDATA_UCHAR: avolume =((volumehandler3D<unsigned char>*)blueprint)->alike(); break;
    case VOLDATA_SHORT: avolume =((volumehandler3D<short>*)blueprint)->alike(); break;
    case VOLDATA_USHORT: avolume=((volumehandler3D<unsigned short>*)blueprint)->alike(); break;
    case VOLDATA_DOUBLE: avolume=((volumehandler3D<double>*)blueprint)->alike(); break;
    default: cout << "Unsupported data type\n" << endl;
    }*/
    
    //
// Här definieras arrayerna av volumehandler_base och vectorhandler_base
// Vi sköter även det grafiska gränssnittet för att ladda volymer mm. Interaktionen
// för ALLA volymer sker samtidigt, genom att vi skapar en kompositwidget med information
// från alla ingående volymer
//

volumehandler3D.h:
   // void raytrace(vector3D direction, int threshold, vector3D &resultpos, vector3D startpos = 0); // mostly a demo app.

volumehandler3D.cc:
/*string filename_from_filepath(string name)
{
unsigned int pos;

pos=name.rfind("/",name.length()-1);
if (pos !=string::npos)
{
name.erase(0,pos+1);
}
return name;
}*/

/*template<class VOLUMETYPE>
volumehandler_base * volumehandler3D<VOLUMETYPE>::alike (int unit)
    {
    volumehandler_base * new_volume = NULL;

        //TODO: this is super-stupid code. Should be templated, that may not be possible however
        switch (unit)
            {
            case VOLDATA_CHAR:      new_volume = new volumehandler3D<char> ();            
                ((volumehandler3D<char> *)new_volume)->set_parameters(this);
                ((volumehandler3D<char> *)new_volume)->volumeptr = new char[((volumehandler3D<char> *)new_volume)->datasize[0]*((volumehandler3D<char> *)new_volume)->datasize[1]*((volumehandler3D<char> *)new_volume)->datasize[2]];
                break;
        case VOLDATA_UCHAR:     new_volume = new volumehandler3D<unsigned char> ();            
                ((volumehandler3D<unsigned char> *)new_volume)->set_parameters(this);
                ((volumehandler3D<unsigned char> *)new_volume)->volumeptr = new unsigned char[((volumehandler3D<unsigned char> *)new_volume)->datasize[0]*((volumehandler3D<unsigned char> *)new_volume)->datasize[1]*((volumehandler3D<unsigned char> *)new_volume)->datasize[2]];
                break;
        case VOLDATA_SHORT:     new_volume = new volumehandler3D<short>();            break;
        case VOLDATA_USHORT:    new_volume = new volumehandler3D<unsigned short>();   break;
        case VOLDATA_DOUBLE:    new_volume = new volumehandler3D<double>();           break;
        }

    return new_volume;
    }*/


// *** planned custom file format ***

// An identifier might look like:
// Version: 5
// ...or (the parameter gives the amount of bytes to read)...
// Volumedata: 15748
// ...or...
// Volumedata: "kalle.via"

// removes comments (# or // -lines), reads identifier and identifies data type (either size or variable type)

    void datamanager::parse_identifier(ifstream &in, int &identifierindex) // index follows the IDENTIFIERS array
        {
        char aline[1000];

        do
            { in.getline(aline,1000); }
        while (aline[0]=='#' || (aline[0]=='/' && aline[1]=='/'));


        identifierindex = -1;

        for (int i=0; i < NOOFIDENTIFIERS; i++)
            { 
            if (strncmp(aline, IDENTIFIERS[i].name, strlen(IDENTIFIERS[i].name)) == 0) // we found the target
                {
                identifierindex = i; return;
                }
            }
        return;
        }


////////
// loads spatial info and imagedata
//

void datamanager::parse_and_load_file(char filename[])
    {
    ifstream in(filename);

    int identindex;

    while (!in.eof())
        {
        parse_identifier(in, identindex);

        for (int param=0; param < IDENTIFIERS[identindex].noofparams; param++) // how many params to read
            {

            }
        }
    }


```

### CMakelists.txt ###
```
    /*
PROJECT( myProject )

INCLUDE (${CMAKE_ROOT}/Modules/FindITK.cmake)
IF ( USE_ITK_FILE )
INCLUDE(${USE_ITK_FILE})
ENDIF( USE_ITK_FILE )

ADD_EXECUTABLE( myProject   myProject.cxx )

TARGET_LINK_LIBRARIES ( myProject  
VXLNumerics   ITKCommon   ITKIO   ITKMetaIO   itkpng   itkzlib )
*/

```
### image3D.cc ###
```
    /*itk::ImageBase< 3 >::DirectionType itk_orientation;*/
    
    // typename itk::ImportImageFilter< VOLUMETYPE, 3 >::Pointer importfilter; // = new itk::ImportImageFilter< VOLUMETYPE, 3 >;

//  typedef itk::ImportImageFilter< VOLUMETYPE, 3 >   ImportFilterType;
//  itk::ImportImageFilter< VOLUMETYPE, 3 > importFilter;
//importFilter::Pointer = ImportFilterType::New();
//importFilterRawpointer = new itk::ImportImageFilter< VOLUMETYPE, 3 >;
/*
ImportFilterType::SizeType  size;
size[0]  = datasize[0];  // size along X
size[1]  = datasize[1];  // size along Y
size[2]  = datasize[2];  // size along Z

ImportFilterType::IndexType start;
start.Fill( 0 );

ImportFilterType::RegionType region;
region.SetIndex( start );
region.SetSize(  size  );
importFilter->SetRegion( region );

double origin[ 3 ];
origin[0] = 0.0;    // X coordinate 
origin[1] = 0.0;    // Y coordinate
origin[2] = 0.0;    // Z coordinate
importFilter->SetOrigin( origin );

double spacing[ 3 ];
spacing[0] = 1.0;    // along X direction 
spacing[1] = 1.0;    // along Y direction
spacing[2] = 1.0;    // along Z direction
importFilter->SetSpacing( spacing );

importFilter->SetImportPointer( localBuffer, numberOfPixels, false);
return &importFilter;

*/

//}

////
// This computes how many pixels NOT to render at beginning of line,
// how many to render at line before stop,
// how many lines to skip before start,
// how many lines to render before stop. This is basically the offset info, if any
//
// IN: area we want to render in global coordinates (offset 0,0)
//
template<class VOLUMETYPE>
void volumehandler3D<VOLUMETYPE>::give_parametersXYplane(int renderstartX, int renderstartY, int renderwidth, int renderheight, int &startoffset, int &patchXoffset )
    {
    // WE DONT USE OFFSET - ALL VOLUMES EQUAL. MUST FIX!!!
    startoffset = renderstartX + datasize[0]*renderstartY;
    patchXoffset = datasize[0] - renderwidth; // we already rendered "renderwidth" voxels
    }

/*template<class VOLUMETYPE>
void volumehandler3D<VOLUMETYPE>::give_volumesize(int &x, int &y, int &z) // REMOVE THIS LATER!!!
{
x = datasize[0];
y = datasize[1];
z = datasize[2];
}*/

            //banded test pattern
            /*for (int z=0; z < datasize[2]; z++)
            for (int y=0; y < datasize[1]; y++)
            for (int x=0; x < datasize[0]; x++)
            {
            //   if ((x+y+z)%2 == 0) { set_voxel(x,y,z,255); }
            //                 else  { set_voxel(x,y,z,0); }
            set_voxel(x,y,z,(float(x+y+z)/3 > float(datasize[2]/2)) && (y % 2) == 0 ? 255: 0 );
            }*/
            
  //The DICOM standard is online at http://medical.nema.org/ .  Most of the
    //tags are listed in part 6: Data Dictionary, with more information for
    //specific tags in part 3: Information Object Definitions. The "Image
    //Orientation Patient" tag gives the direction cosines for the rows and
    //columns for the three axes defined above. Your typical axial slices will
    //have a value 1/0/0/0/1/0: rows increase from left to right, columns
    //increase from posterior to anterior. This is your everyday "looking up
    //from the bottom of the head with the eyeballs up" image.
    //
    //In your images, IOP =
    //0.999794\0.000000\-0.020289\-0.020127\-0.126071\-0.991817. So this is a
    //slightly-rotated coronal acquisition: rows increase from left to right,
    //and columns increase from head to foot.
    //
    //The "Image Position Patient" tag gives the coordinates of the first
    //voxel in the image in the "RAH" coordinate system, relative to some
    //origin. On our scanner it's the magnet isocenter, but I don't know if
    //that's always true.
    //
    //The steps I take when reconstructing a volume are these: First,
    //calculate the slice normal from IOP:
    //
    //normal[0] = cosines[1]*cosines[5] - cosines[2]*cosines[4];
    //normal[1] = cosines[2]*cosines[3] - cosines[0]*cosines[5];
    //normal[2] = cosines[0]*cosines[4] - cosines[1]*cosines[3];
    //
    //You only have to do this once for all slices in the volume. Next, for
    //each slice, calculate the distance along the slice normal using the IPP
    //tag ("dist" is initialized to zero before reading the first slice) :
    //
    //for (int i = 0; i < 3; ++i) dist += normal[i]*ipp[i];
    //
    //Then I order the slices according to the value "dist". Finally, once
    //I've read in all the slices, I calculate the z-spacing as the difference
    //between the "dist" values for the first two slices.
    
/*
    // *** TEST: find image series in directory ***
    
    typedef std::vector< std::string > SeriesIdContainer;
    const SeriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();
    SeriesIdContainer::const_iterator seriesItr = seriesUID.begin();
    SeriesIdContainer::const_iterator seriesEnd = seriesUID.end();
    
#ifdef _DEBUG
    std::cout << "DICOM series:" << endl;
    while( seriesItr != seriesEnd )
    {
        std::cout << seriesItr->c_str() << std::endl;
        seriesItr++;
    }
    
    //we might provide an option for the user to set the series identifier
    //this just takes the first series, if multiple
    //seriesIdentifier = seriesUID.begin()->c_str();
    
    //Found series: "1.2.840.113674.1242.214.184.3001072.000000256256"
    //Selected series: "1.2.840.113674.1242.214.184.300"
        
    std::cout << "Found series: \"" << seriesUID.begin()->c_str() << "\"" << endl;
    std::cout << "Selected series: \"" << seriesIdentifier << "\"" << endl;
#endif
    
    //end of TEST
     */
     
         /*std::string entryId1("0028|0010"); //Rows
        std::string entryId2("0028|0011"); //Columns
    std::string entryId3("0028|0030"); //Pixel Spacing
    std::string entryId4("0018|0050"); //Slice Thickness
    std::string entryId5("0018|0088"); //Spacing between Slices
    std::string entryId6("0020|0032"); //Image Position Patient
    std::string entryId7("0020|1041"); //Slice Location
    */
```
### viewmanager.cc ###
```
/*
//joel: Saves the original to avoid messing up everything...

void viewmanager::demo_setup_tiles()
{
const int demonoofhorizontaltiles[MAXVIRTUALVIEWS] = { 3,0,0 };
for (int vv=0; vv < MAXVIRTUALVIEWS; vv++) { noofhorizontaltiles[vv] = demonoofhorizontaltiles[vv]; }

const int demonoofverticaltiles[MAXVIRTUALVIEWS][MAXHORIZONTALGRID] = { { 2,3,2,1  } };
for (int vv=0; vv < MAXVIRTUALVIEWS; vv++)
 for (int h=0; h < MAXHORIZONTALGRID ; h++)
   { noofverticaltiles[vv][h] = demonoofverticaltiles[vv][h];  }

const float demotilewidthpercent[MAXVIRTUALVIEWS][MAXHORIZONTALGRID] = { { 0.5, 0.3, 0.1, 0.1 } }; // last will be the reminder; i.e. not used
for (int vv=0; vv < MAXVIRTUALVIEWS; vv++)
 for (int h=0; h < MAXHORIZONTALGRID ; h++)
   { tilewidthpercent[vv][h] =  demotilewidthpercent[vv][h]; }

const float demotileheightpercent[MAXVIRTUALVIEWS][MAXHORIZONTALGRID][MAXVERTICALGRID] = { { { 0.3, 0.3, 0.3, 0.1 }, { 0.2, 0.2, 0.2, 0.1 }, { 0.5, 0.2, 0.2, 0.1 }, { 0.3, 0.1, 0.3, 0.1 }, { 0.2, 0.5, 0.1, 0.1 } } }; // last will be the reminder; i.e. not used
for (int vv=0; vv < MAXVIRTUALVIEWS; vv++)
 for (int h=0; h < MAXHORIZONTALGRID ; h++)
    for (int v=0; v < MAXVERTICALGRID; v++)
       { tileheightpercent[vv][h][v] = demotileheightpercent[vv][h][v]; }
}
*/

/*
int viewmanager::find_viewport_no_renderer()
    {
    // returns -1 if none found

    //since the only usage scenario for this method is that
    //the calling method creates the viewport otherwise,
    //we may want to create a viewport in that case,
    //so that this method is guaranteed to return a viewport
    //OR: make a int viewmanager::get_unused_viewport()
    //that behaves that way

    int free_vp_id = NOT_FOUND_ID;
    for (int i=0; i < viewports.size(); i++) 
        {


        for (int vv=0; vv < MAXVIRTUALVIEWS && free_vp_id==NOT_FOUND_ID; vv++) // samma mönster för alla virtuella vyer - distribuera viewports
            {
            for (int v=0; v < MAXVERTICALGRID && free_vp_id==NOT_FOUND_ID; v++)
                for (int h=0; h < MAXHORIZONTALGRID && free_vp_id==NOT_FOUND_ID ; h++)
                    {
                    if (viewports[i].get_renderer_id()==NO_RENDERER_ID)
                        {
                        //found viewport without renderer
                        free_vp_id = viewports[i].get_id();
                        }
                    }
            }
        }
    return free_vp_id;
    }*/
    
 /*void viewmanager::renderers_have_changed ()
    {
    for (int v=0;v < viewports.size();v++)
        {
        viewports[v].renderers_have_changed();
        }
    }*/

/*void viewmanager::volumehandler_vector_has_changed()
    {
    for (int v=0;v < viewports.size();v++)
        {
        viewports[v].volumehandler_vector_has_changed();
        }
    }*/
    
viewport.h:
	// FISHY viewport(const viewport &k) { cout << "viewport copy" << this->ID << " " << k.ID << endl;  *this=k;   this->ID = k.ID;  viewport(); }
	
viewport.cc:
    //blank viewport with a striped pattern that totally says, like, "no renderer here"

    /*for (long p=0; p < rgbpixmapwidth*rgbpixmapheight*RGBpixmap_bytesperpixel; p +=RGBpixmap_bytesperpixel )
    {
    if (((p/RGBpixmap_bytesperpixel) % rgbpixmapwidth + ((int)(p/RGBpixmap_bytesperpixel)/(int)rgbpixmapwidth) )% 40 < 20 )
    {
    rgbpixmap[p] = 0;
    rgbpixmap[p + 1] = 0;
    rgbpixmap[p + 2] = 0;
    }
    else
    {
    rgbpixmap[p] = 255;
    rgbpixmap[p + 1] = 0;
    rgbpixmap[p + 2] = 0;
    }
    }*/

    //blank viewport with a nice, green test color

    /*for (long p=0; p < rgbpixmapwidth*rgbpixmapheight*RGBpixmap_bytesperpixel; p +=RGBpixmap_bytesperpixel )
    {
    rgbpixmap[p] = 0;
    rgbpixmap[p + 1] = 255;
    rgbpixmap[p + 2] = 0;
    }*/

        /*if(f->callback_action != CB_ACTION_NONE &&
            rendererIndex>=0 &&
            f->needs_re_rendering)
            {
            rendermanagement.render(rendererIndex, rgbpixmap, rgbpixmapwidth, rgbpixmapheight);
            f->needs_re_rendering = false;
            }

        if (f->callback_action != CB_ACTION_NONE && f->callback_action != CB_ACTION_RESIZE)
            {
            f->draw(rgbpixmap);
            f->damage(FL_DAMAGE_ALL);
            }

         if (f->callback_action == CB_ && f->callback_action != CB_ACTION_RESIZE)
            {
            f->draw(rgbpixmap);
            f->damage(FL_DAMAGE_ALL);
            }*/
            
//// relative scaling only valid for the image, not for buttons et.c.
    //
    //Fl_Widget *flexiblesizewidget;
    //flexiblesizewidget = new Fl_Box(xpos,ypos+buttonheight,width,height-buttonheight,"");
    //containerwidget->resizable(flexiblesizewidget);
    
    
    //FLTKrgbimage = new Fl_RGB_Image(rgbpixmap, width, drawheight, 3); // RGB ingen transparens//joel
    
    histogram.cc:
      /*for (unsigned int j=0;j < h; j++)
    {
    for (unsigned int i=0;i < w; i++)
    {
    //TEST: dotted pattern
    if ((i + j) % 2 == 0)
    {image[(i+j*w)*RGBpixmap_bytesperpixel]=image[(i+j*w)*RGBpixmap_bytesperpixel+1]=image[(i+j*w)*RGBpixmap_bytesperpixel+2]=255;}
    }
    }*/
```
### datawidget.cc ###
```
        // DEMO 
        /*if (prioritet == 2)
        {
        ((Fl_Button *)minbutton)->hide();((Fl_Button *)maxbutton)->hide();
        }
        else {
        ((Fl_Button *)priobutton)->hide();
        }*/
        
/*datawidget::datawidget (int id) {
    ostringstream namestream;

    //no name: add "Untitled" and ID
    namestream << "Untitled (" << id << ")";

    datawidget(id, namestream.str());
    }*/
```
### userIO.cc ###
```
/*userIO::userIO(int x, int y, int w, int h, const char * l) : Fl_Pack(x, y, w, h, l)
    {
    ID=new_uIO_ID++;

    box(FL_DOWN_BOX);
    align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE);

    strcpy(name,l);
    //label(name);  //done by overloaded cosntructor

    close_button = new Fl_Button(NA,NA,NA,NA, "Close");
    close_button->box(FL_THIN_UP_BOX);

    OK_button = new Fl_Button(NA,NA,NA,NA, "OK");
    OK_button->box(FL_THIN_UP_BOX);
    }*/
```
### FLTK2Dregionofinterest.cc ###
```
/*zoom*= std::min (frame->w(), frame->h())/std::min (canvas_size_x,canvas_size_y);*/

     /*region_start_x = (region_start_x-canvas_size_x/2.0)*zoom + frame->w()/2.0;
     region_end_x = (region_end_x-canvas_size_x/2.0)*zoom + frame->w()/2.0;
     region_start_y = (region_start_y-canvas_size_y/2.0)*zoom + frame->h()/2.0;
     region_end_y = (region_end_y-canvas_size_y/2.0)*zoom + frame->h()/2.0;*/
```

### mainwindow.cc ###
```
 /*char aproject[100] = "";
    char avolume[100] = "";*/

    //std::cout << "Welcome to Platinum v. " << PLATINUM_MAJOR_VER << "." << PLATINUM_MINOR_VER << std::endl;
    
    std::cout << "You are going Platinum, please stand by!" << std::endl;

    /*const int MAXBUFFER=100;*/
    /*
    #define ever (;;)
    char c;
    for ever

    {
    char *optspec = "hp:v:"; // the options. x: = x with single argument
    if ((c = getopt(argc,argv,optspec)) == -1) break;
    if (strlen(optarg) >= MAXBUFFER) { cout << "Input overflow ERROR!\n"; exit(0); } switch (c)
    {
    case -99:// end of options list - break the while loop
    break;
    // options will follow
    case 'h':
    cout  << "Options: "  << &optspec  << endl;
    exit(1);
    case 'p':
    strcpy(aproject, optarg);
    break;
    case 'v':
    strcpy(avolume, optarg);
    break;
    }
    }
    */
    ////
    // Ladda in (eventuellt) projekt, volymsdata eller annat
    // INTE interaktivt - filnamn som inparametrar
    //
    //if (avolume[0] != 0) { datamanaging.load_volume(avolume); }
    //if (aproject[0] != 0) { datamanaging.load_project(aproject); }


/*int platinum_run (int argc, char *argv[])
{
////
// ...finish the window creation
window.end();

////
//  start the main loop

window.show(argc, argv);

return Fl::run();
}*/
```