# Plans #

Viewporttool objects are selectable in a "palette" in the main Platinum window. Once selected, a tool will somehow be able to catch mouse events, to alter the viewport and any (typically the topmost or all at once) images/points in that viewport. The same tool is selected for all viewports simultaneously.

Together with the tool palette there will be a StatusArea that shows either a text string (writable in an algorithm to provide progress feedback) or any FLTK widgets depending on the tool selected.

## Events ##
A viewporttool can receive five types of mouse events corresponding to three mouse buttons, scroll wheel and hovering (mouse up):
  1. Hover
  1. Adjust
  1. Create
  1. Browse
  1. Scroll
By using the concepts above instead of mouse button names, and selecting the most appropriate feature to each name, some consistency in mouse button usage can be achieved. Also, it becomes considerably easier to adapt to use with a stylus, laptop trackpad or standard Mac mouse by using modifier keys in addition.

Event types that the viewporttool should also handle eventually:
  * Draw
  * Keypress
  * Drag and drop

Events that should cause the same behavior across all tools should simply be ignored by the selectable tools and caught last by a master tool (which does not show onscreen, only in code)

# Implementation #
The viewporttool icons (buttons) for selecting tool are created once by a static method in the viewporttool. A viewporttool object will be created when the tool type is first used in a particular viewport.

Through the static `taste` function, an instance of the selected tool is created when a viewport receives its first event. This in turn calls the appropriate constructor: if the tool grabs the event, the tool will be returned to the calling viewport. Otherwise it's deleted.

Once a viewport has a tool object it receives all events. The tool instance remains in memory until another tool is selected, or the viewport is deleted. The viewport may respond to some events, but will also pass them along to the selected tool.

The viewporttool instance can store things like past drag coordinates or a selection ROI of some kind. Static variables in a viewporttool subclass may keep more persistent information like brush radii, "modes" and such. This information will be retained until the Platinum program is exited.

## Members ##
...

## Some classes that inherits from viewporttool ##
  * viewporttool
    * nav\_tool - zoom, pan, scroll, shows the pixel value
      * cursor\_tool - set point, shows the view coordinates