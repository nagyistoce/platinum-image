# Integration #
The transfer objects's main function operates on an [image\_general](image_general.md) iterator. It may also need access to min/max values, which can be done via its pointer to the associated image. (as [image\_storage](image_storage.md)). Transfer functions are implemented as objects attached to image objects. Their UI is a subclass to Fl\_group, displayed through the image's dataWidget.

When selected by the user, a new transfer function object of the chosen type is created by the static [transferfactory](transferfactory.md) object in the [transfer\_manufactured](transfer_manufactured.md) base class.

## Classes ##

**[transfer\_mapcolor](transfer_mapcolor.md)**
Assigns unique colors to few (20 or less) consecutive integer values, like on a map. If it turns out to be applicable only to the [image\_label](image_label.md) class, this class should be renamed transfer\_label.

_**[transfer\_CT](transfer_CT.md)**_
Special _computed tomography_ transfer function with several avaiable presets for the Hounsfield values of various tissue types, including a possible multi-color scheme for muscle, bone, fat etc.

**[transfer\_brightnesscontrast](transfer_brightnesscontrast.md)**
Simple brightness and contrast sliders.

**[transfer\_linear](transfer_linear.md)**
As above, but implemented as a line graph on a histogram.

**[transfer\_spline](transfer_spline.md)**
Spline interpolated transfer curve.

**[transfer\_default](transfer_default.md)**
The default transfer function, maps values into grayscales with lowest value in image being black and highest white.

### Helpers ###
**[transfer\_manufactured](transfer_manufactured.md)**
Untemplated base class to allow a static factory object

**[transfer\_base](transfer_base.md)**
Abstract base for all transfer functions

**[transfer\_interpolated](transfer_interpolated.md)**
Abstract base for transfer functions using curves/lines drawn on a histogram, with lookup table (transfer\_spline, transfer\_linear)