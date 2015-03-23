Classes in _italics_ are planned (some prototyped), but not implemented.
# View & rendering #
**Viewport**
Abstraction of widget containing controls and image pane (implemented in FLTKviewport) for each viewpoint of data.

**FLTKviewport**
FLTK implementation of the central pane of each viewport that displays the rendered image and can be clicked/dragged in.

**[viewmanager](viewmanager.md)**
Viewmanager lists the available viewports and allows access through viewport ID and various ways of requesting updates.

**Renderer\_base**
Renderers produce a 2D viewport pixmap out of images and point sets. The base class has virtual functions for coordinate storage conversion, common to different renderer types.

**Renderer\_VTK**
Abstract base for VTK-based renderers.

**RendererMPR**
MPR renderer subclass which also renders thumbnails via a static function. Like other possible renderers it also converts between local and global coordinate systems.

**Rendercombination**
Stores list of volumes and blend mode(s) for renderers.

**Rendergeometry**
Rendergeometry stores rendering coordinates (look at, orientation and zoom) for renderers.

**Rendermanager**
Keeps a list of renderer objects and handles communication between viewport and renderer.

## Transfer functions ##

_See TransferFunctions_

**[transfer\_mapcolor](transfer_mapcolor.md)**
Assigns unique colors to few (16 or less) consecutive integer values, like on a map. If it turns out to be applicable only to the [image\_label](image_label.md) class, this class should be renamed transfer\_label.

_**[transfer\_CT](transfer_CT.md)**_
Special _computed tomography_ transfer function with several avaiable presets for the Hounsfield values of various tissue types, including a possible multi-color scheme for muscle, bone, fat etc.

_**[transfer\_brightnesscontrast](transfer_brightnesscontrast.md)**_
Simple brightness and contrast sliders.

_**[transfer\_linear](transfer_linear.md)**_
As above, but implemented as a line graph on a histogram.

_**[transfer\_spline](transfer_spline.md)**_
Spline interpolated transfer curve.

**[transfer\_default](transfer_default.md)**
The default transfer function, maps values into grayscales with lowest value in image being black and highest white.

**[transfer\_base](transfer_base.md)**
Abstract base for all transfer functions

_**[transfer\_interpolated](transfer_interpolated.md)**_
Abstract base for transfer functions using curves/lines drawn on a histogram, with lookup table (transfer\_spline, transfer\_linear)

# Data handling #
**Datamanager**
The datamanager maintains a list of work data loaded or created at runtime. It has functions for loading and saving data (that invoke functions in the relevant classes).

## Pointwise storage ##
_**Point\_base**_
The untemplated base class for point collections.

_**Points**_

_**Point**_
A single point is a special case of points

_**PointMesh**_
Unstructures poit mesh

_**PointSequence**_
Linear sequence of points

## Image storage ##
**Image\_base**
Untemplated base class for image storage, including volumes, contains 3D position and orientation information.

**Image\_storage**
Base class templated only by voxel type, which allows operations where the image size and dimensionality is unknown (e.g. voxel-by-voxel operations)

**Image\_general**
Image class templated over voxel data type and dimension.

**Image\_scalar**
Image type with scalar voxel/pixel values. Mostly the same as image\_general.

**Image\_integer**
Image type with integer values, in itself and as base for more task-specific subclasses.

**[Image\_binary](Image_binary.md)**
Binary image type, subclass of image\_integer.

**Image\_label**
Image type with integer values signifying distinct classes or "labels" (i.e. not continuous values). Can be described as multiple binary volumes rolled into one. 0 (zero) carries special meaning as empty/background.


**Image\_multi**
Base class for images with multispectral data, such as vectors or complex MRI data.

_**Image\_complex**_
Image class with complex valued voxels.

# User interaction #
**Datawidget**
The datawidget is the GUI representation of a dataset (image/point). It provides controls for saving, deleting and shows the thumbnail.

**UserIO**
Class defining an operation on data presented to the user in a list.
**FLTKuserIOparameter\_base**
Base class for numerous parameter widgets populating userIO blocks. Handles value retrieval, updates and creation/deletion.

**UserIOmanager**
Maintains a list of userIO blocks, and relays all operations on blocks and their parameters. The interface to this class is independent of the FL toolkit.

# Helper classes #
_**Color**_
Color storage class.

_**Colormap**_
Interpolated color map class.

_**Cursor3D**_
3D cursor that could be implemented as a point3D.

**Histogram\_base**
Base class for processing of histograms in 1-3D (separated from user interface).

**Histogram\_2D**
Displays and does area higlights of a 2D histogram.

**Thresholdparvalue**
N-dimensional threshold values with optional shape variations. Includes a function for creating a new volume through thresholding.

## 2D histogram segmentation specials ##
**Threshold\_overlay**
Overlay to viewports that displays a segmentation preview for the 2D histogram segmentation project.

**FLTK2Dregionofinterest**
Overlay to viewports that displays region of interest information, including a rectangular selection and threshold\_overlay segmentation previews.
