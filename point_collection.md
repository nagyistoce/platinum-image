# Plans #
The inheritance is planned as follows
  * [data\_base](data_base.md)
    * [point\_collection](point_collection.md) base class of multiple points, no structure
      * [point](point.md) a single point accepts all operations (=a subclass) of points
      * [points\_seq](points_seq.md)
        * [points\_seq\_closed](points_seq_closed.md) loop of points
        * [points\_seq\_func](points_seq_func.md) parametric = returned data `const`
      * [points\_mesh](points_mesh.md) interconnected points

## [userIO](userIO.md) parameter ##
There is a userIO parameter, [FLTKuserIOpar\_points](FLTKuserIOpar_points.md), that allows selection of a point\_collection (or subclass, in the same way as [FLTKuserIOpar\_image](FLTKuserIOpar_image.md)). The parameter can return either the [ID](ID.md) of the point\_collection or, if the derived object is a [point](point.md), its physical coordinates as a Vector3D.

The following example code demonstrates this:

### Function definition ###
```
void pointInputDemoFcn (int userIO_ID,int par_num)
{
    if (par_num == USERIO_CB_OK)
        {
        //you can get either ID of the point_collection object:
        int theID = userIOmanagement.get_parameter<pointIDtype>(userIO_ID,0);
        
        //or a Vector3D - which only works for the point class, because it's the
        //only one with just 1 value:
        Vector3D thePoint = userIOmanagement.get_parameter<Vector3D>(userIO_ID,0);
        
        //if you are interested in voxel coordinates of a point, call
        //image->world_to_voxel(const Vector3D)
        
        std::ostringstream numbers;
        numbers << "Global coordinates: (" << thePoint[0] << "," << thePoint[1] << "," << thePoint[2] << ")"; 
        
        userIOmanagement.show_message("Point parameter value",numbers.str(),userIOmanager::block);
        }
}
```
### Parameters ###
```
    int pointInputDemoID=userIOmanagement.add_userIO("Point input demo",pointInputDemoFcn,"OK");
    userIOmanagement.add_par_points(pointInputDemoID,"What's the point?");
```