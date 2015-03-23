# Purpose #
Labeled images depict a number of classes, with consecutive integer numbers depicting several classes of voxels e.g. tissue types. They serve the same purpose as binary images but more than two possible values per voxel. The default transfer function is [transfer\_mapcolor](transfer_mapcolor.md).

To impose consistent use, values are always expected to run in a straight range from 0 to the number of classes. This allows for simple and consistent conversion from binary images, consistent display and a convention for algorithms.