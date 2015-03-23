# Guidelines for coding syntax and structure #

(Try to follow these guidelines where applicable. See also [PlatformIndependence](PlatformIndependence.md) for coding guidelines to ease the cross-platform divide)

## Declaration file structure (.h) ##

**private:**

**protected:**

**public:**

-Class constructors/destructors

-Access functions (get_/set_)

-Fuctions of more internal caracter (Reordering/Resampling and draw/handle in case of GUI classes)

-Processing function

## Implementation file structure (.cc .hxx) ##
For simplicity, implement functions in the same order as in header files.

