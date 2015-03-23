## Code comments ##
In the future Platinum may adapt [Doxygen](http://doxygen.org) for generating automatic documentation. To support this - and to get a common style - comments should be made in a [Doxygen-compatible syntax](http://www.stack.nl/~dimitri/doxygen/docblocks.html):

### Header files (.h) ###
The header file should start with a brief description like this example:
```
/////////////////////////////////////////////////
//
//  The_class $Revision $
///
/// Class that does its thing. Used with The_other_class.
///
//  $LastChangedBy $
//
```
The tags starting with "$" will be filled in by SVN after commit.
Functions and their parameters can be briefly described in their forward (header) declaration:
```
void SomeFunction (); ///This is some function
```
Note the _three_ slashes on consecutive lines that trigger Doxygen. For all other minor comments, use only two slashes.
### Implementation (.cc/.hxx) files ###
The first line of implementation files should have a
```
// $Id $
```
which provides a summary including who, when and at which revision the file was last edited. If necessary, implementation files may instead use the .h header as described above.

**Note:** all source files must contain the [LGPL](LGPL.md) header. For some files (at the time of writing: only example applications) the license should be [BSD](BSD.md) rather than LGPL.
## SVN commit comments ##
When committing changes with [SVN](SVN.md), try to make the following information available:
  * **Which classes/functions** were changed (which file(s) were affected is automatically recorded in the log)
  * **Why** - bug fix/new feature/code cleanup?
## Google Code Wiki ##
A more thorough description of a class, plans et.c. should be put in this wiki. Use these tags:
|For|Tag|Page name|
|:--|:--|:--------|
|Description of an entire class, including info on functions|ClassDescription|Class name|
|Page about a single function|FunctionDescription|Function name|

When mentioning a class name, use `[ ]` to link to its page like this: `[viewmanager]` becomes [viewmanager](viewmanager.md).










