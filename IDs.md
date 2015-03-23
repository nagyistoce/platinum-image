The ID system is used to keep references to objects which are not dependent on addressing, and also range- and existance checked when dereferenced, increasing code robustness. The ID will survive storage to files and provide the basis for future workspace storage.

# Concepts #

  * ID is stored as a signed 'short' integer
  * Negative values should be allowed at all times IDs are used, (-1) means not found and is available in the constant 'NOT\_FOUND\_ID'
  * There are some legacy code that uses list indices (NOT ID!) to reference objects. These have been renamed to reflect this, any new functionality referencing images, points, viewports, renderers et.c. should be implemented to use ID.