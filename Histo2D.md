# Introduction #
Histo2D (abbreviation for "2D histogram segmentation") is an application that uses Platinum to perform semi-automatic segmentation using bi-spectral images. It was developed as part of a master thesis project involving development of Platinum.

# Purpose #
The Histo2D program is a way to explore multispectral image segmentation, and serves as an example of application development with Platinum, including [templates](Templates.md), [image objects](image_general.md), [ITK](ITK.md) calls and the use of [userIO](userIO.md) controls.

Users can load and select two images for which a 2D histogram will be produced. Rectangular or circular regions in the histogram can be selected and voxels with corresponding values  in the affected images will be highlighted. This highlight can be finalized into a labeled volume for further processing.

Histo2D also contains a dilation-erosion algorithm implemented in ITK for refining the segmentation result.

# Build and run #
The directory `Apps/Histo2D/` contains a CMake file that will build the [Histo2D](Histo2D.md) application.