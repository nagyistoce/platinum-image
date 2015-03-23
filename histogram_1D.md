# Purpose #
**histogram\_1D** calculates the following information from an image:
  * A frequency histogram with an adjustable number of buckets
  * The number of discrete values (only when # of buckets is >= number of possible discrete values)
  * Minimum and maximum values

**NOTE:** The bucket\_max value (used for display) _ignores values 0 and 1_ because these are the most common by far in many images. This is intentional in order to get a sensible scaling when displaying the histogram.

# Input #
The input can be either an image\_storage (or derived class) or a pointer to image data (including a pointer to the end of this data). In the latter case it is important to match the template argument for histogram\_1D

&lt;type&gt;

 with the type of the image data.

A histogram\_1D is attached to each [image\_storage](image_storage.md) object to provide stastics for use when displaying and processing the image.