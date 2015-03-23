This page contains tips on C++ template usage. These may be considered trivial by seasoned programmers.

### Specifying type ###
Some member functions, notably `get_parameter` in the userIOmanager, require template parameters by themselves. Typically, the template parameter(s) are automatically determined by the type of the arguments. For return values, this is not allowed in C++.

Example: to specify the return type `float` for a get\_parameter call:

`userIOmanagement.get_parameter<float>(1,0);`

## image\_base resolution ##
When using images not generated in Platinum (i.e. loaded from file, by user or in code) the template parameters of the image are not determined at compile time. For example, an algorithm doing processing on binary images would need a binary image for input. However, when aquiring an image through 'get\_image(id)' the returned type will be [image\_base](image_base.md). The solution is to use one of the functions described below.

| **Function** | **Return type** | **Pointing to** | **Expects input** |
|:-------------|:----------------|:----------------|:------------------|
|`dynamic_cast<requestedClass<type, dimension>>` |requestedClass<type, dimension>| **original**|requestedClass<type, dimension> or subclass|
|`scalar_copycast<requestedClass, type, dimension >`|requestedClass<type, dimension> (must be scalar or scalar subclass)|copy|Any scalar type|
|`binary_copycast<dimension >`|image\_binary

&lt;dimension &gt;

|copy|Binary, unsigned char|
|`label_copycast<dimension >`|image\_label

&lt;dimension&gt;

|copy|image\_integer and subclasses|

**NOTE:** all functions return `NULL` if the cast attempt fails. In most situations this has to be accounted for, or the program will crash.

### Requesting type ###

`image_binary * my_binary_pointer = dynamic_cast<image_binary<3> >(datamanagement.get_image(id));`

If the loaded image did not match, the cast will fail and return 0 (`NULL`) which is how it differs from an ordinary casting operation. _Note: remember to put spaces between the ">" signs or they will be interpreted as the stream operator (`>>`) and cause trouble._

The above code also has us assuming the image is 3-dimensional. The following gets a dimension-independent pointer to the same image, which you can use an iterator on:

`image_storage * my_pointer_to_a_bunch_of_voxels = dynamic_cast<image_storage<bool> >(datamanagement.get_image(id));`

### Forcing type ###
`scalar_copycast <requestedClass, type, dimension >(image_base* input)` does the same basic thing as `dynamic_cast` (but unlike the latter, it's not a built-in feature of C++). The difference is that, rather than casting the same image instance into its true data type, it will **make a copy** with the pixel type changed as requested. This only works for scalar-based data types (i.e. NOT binary, complex, RGB et.c.). **Note** also, that due to limitations in C++, the template parameters have to be written differently.

`binary_copycast<dimension>` behaves as scalar\_copycast, it similarly allows casting of binary data loaded as integer types into an [image\_binary](image_binary.md). Values > 0 will be set to true, 0 becomes false.

`label_copycast<dimension>` yields an [image\_label](image_label.md) object the same way.

Several examples of specifying type and requesting using the casting functions can be found in the [Histo2D](Histo2D.md) source code.

## Include files ##
In Platinum, implementations of templated classes/functions are usually put in .hxx file, with accompanying declarations in a .h file. In a similar pattern to .h/.cc file, the .h file is included in the .hxx and not the other way around. The purpose of separate .h/.hxx files is to allow a declaration of a templated class when classes are interdependent i.e. - the .h files of templated classes are to be included in other header files only.

**NOTE:** to use a templated class defined with .h + .hxx files, you must include _only the .hxx file_. For example, to use the class [image\_label](image_label.md), put `#include "image_label.hxx"` in the Platinum application code. Using the .h file for this purpose will result in undefined symbols when linking.