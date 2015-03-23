# 1. Download #

  1. Install a development environment, such as [VS.net](http://www.microsoft.com/express/vc/)
  1. Download the following additional source code:
    * [CMake 2.6.4](http://www.cmake.org/HTML/Download.html)
    * [FLTK 1.1.9](http://www.fltk.org/software.php)
    * [ITK 3.16](http://www.itk.org/HTML/Download.php) - you'll have to fill in the questionnaire
    * [VTK 5.4.2](http://www.vtk.org/get-software.php#latest)
  1. Get yourself an [SVN client](http://subversion.tigris.org/links.html#clients) like for example [TortoiseSVN](http://tortoisesvn.tigris.org/). **Mac:** If you use XCode 3 or later, there is a built-in SVN client. Otherwise, the [base command-line package](http://www.codingmonkeys.de/mbo/articles/tag/subversion) together with Xcode's built in SVN-support (see below) and [svnX](http://www.lachoseinteractive.net/en/community/subversion/svnx/features/) is recommended. (optionally, there is also a plugin for Finder similar to TortoiseSVN called [SCPlugin](http://scplugin.tigris.org/))
# 2. Install #

  * CMake
  * The SVN client

# 3. Build (Windows) #
_Note: Windows instructions only, see below for Unix/Linux/Mac_
## ITK ##
  1. Decompress ”InsightToolkit-3.14.0.zip” _(suggested location:   C:\Lib\InsightToolkit-3.14.0)_
  1. Run CMake
  1. Set source: ”C:\Lib\InsightToolkit-3.14.0”
  1. Set build: ”C:\Lib\Bin\ITK”
    1. Press `Configure…` (may take some time)
  1. Set the following options:
    * Disable BUILD\_EXAMPLES
    * Disable BUILD\_SHARED\_LIBS
    * Disable BUILD\_TESTING
  1. Press `Configure…` again (may take some time)
  1. Press `OK` (build files will be written to ”C:\Lib\Bin\ITK”)
  1. Open `ITK.sln`
  1. Select ”Build All\_Build” in VS.net. wait… (appr. 15 min)

  1. Ensure that the following library files were built (in dir `C:\Lib\Bin\ITK\bin\debug`)

  * ITKAlgorithms
  * ITKBasicFilters
  * ITKCommon
  * ITKFEM
  * ITKIO
  * ITKMetaIO
  * ITKNumerics
  * itkpng
  * ITKStatistics
  * itkzlib

## VTK ##
_VTK will be used in Platinum, however it is not necessary at the moment_
  1. Decompress "VTK-5.0.3.zip” _(suggested location:   C:\Lib\VTK-5.0.3)_
  1. Run CMake
  1. Set source: ”C:\Lib\VTK-5.0.3”
  1. Set build: ”C:\Lib\Bin\VTK”
    1. Press `Configure…` (may take some time)
  1. Set the following options:
    * Disable BUILD\_EXAMPLES
    * Disable BUILD\_SHARED\_LIBS
    * Disable BUILD\_TESTING (You might have to check the "Show Advanced Values" box)
  1. Press `Configure…` again (may take some time)
  1. Press `OK` (build files will be written to ”C:\Lib\Bin\VTK”)
  1. Open `VTK.sln`
  1. Select ”Build All\_Build” in VS.net. Wait…


## FLTK ##
  1. Decompress fltk-1.1.7-source.zip” _(suggested location: C:\Lib\fltk-1.1.7)_
  1. Run CMake
  1. Set source: ”C:\Lib\fltk-1.1.7”
  1. Set build: ”C:\Lib\Bin\FLTK”
  1. Press `Configure…` (may take some time)
  1. Set the following options:
    * Disable BUILD\_EXAMPLES
    * Disable BUILD\_SHARED\_LIBS
    * Disable BUILD\_TESTING
  1. Press `Configure…` again (may take some time)
  1. Press `OK` (build files will be written to ”C:\Lib\Bin\FLTK”)
  1. Open `FLTK.sln`
  1. Select ”Build All\_Build” in VS.net. wait… (appr. 2 min)

## Platinum ##
  1. Check out and build Platinum.
  1. Create a folder for the Platinum source (suggestion: `C:\Lib\Platinum`)
  1. Check out (download) the latest Platinum source by right-clicking in the new source folder and selecting "SVN checkout..."
  1. TortoiseSVN settings:
    * If you don not plan to contribute to the platinum source, you can use this address for SVN:
> > http://platinum-image.googlecode.com/svn/trunk/%20platinum-image
    * If you are going to contribute, you will need to access SVN with your _GMail_ account. Contact one of the project administrators and give them your Google account name to be added as a member.
  1. enter address from this page: http://code.google.com/p/platinum-image/source
  1. Press `OK`, you will now be asked for name and password (also on the above page)
  1. Please wait...
  1. You now have the Platinum source
  1. Run CMake
  1. Set source: ”C:\Lib\Platinum”
  1. Set build: ”C:\Lib\Bin\Platinum”
  1. (no options need to be changed)
  1. Press `Configure…` (may take some time)
  1. Press `OK` (build files will be written to ”C:\Lib\Bin\Platinum”)
  1. Open `Platinum.sln`
  1. Select ”Build All\_Build” in VS.net. wait… (appr. 2 min)
  1. Enjoy!

# 3. Build (UNIX) #
Repeat these steps for ITK, VTK, FLTK and Platinum - respectively, and in that order. _See note below for bulding VTK on Mac OS X._
  1. Set up a "build" directory - make a subdirectory inside where the ITK, FLTK and Platinum source code directories are kept. For example, if `~Myname/Projects/PT/ITK/` is the path to the ITK source, create `~Myname/Projects/PT/bin/`
  1. Create one more directory level for each particular project, e.g. `~Myname/Projects/PT/bin/ITK/`. Enter this dir and run CMake:
> > `cmake -i ~Myname/Projects/PT/ITK/`
    * CMake will now ask for some variables. Most can be left at default values. If you will be developing Platinum, set `CMAKE_BUILD_TYPE` to _Debug_ for all the projects
    * You may have to locate the FLTK and ITK directories manually (asking for `FLTK_DIR` or `ITK_DIR`). In this case, specify the path to the "build" directory, e.g. `~Myname/Projects/PT/bin/ITK/`
  1. When CMake has shown Configuring done, Generating done, Build files have been written to: _(where the build files go)_, the project is ready for build.
  1. Type `make` and press return (this does not apply if you chose Xcode on Mac, see below)
  1. The build process will start, and may take some time. When done, repeat the same steps for the next project or start using Platinum when all three are done.

**NOTE:** the UNIX build process is particular about the compiler used, and that all libraries used were compiled using the same compiler. If the build fails  without a descriptive error message this may be the case. Try setting the compiler manually for all three projects by running `cmake -D CMAKE_CXX_COMPILER:STRING=/usr/bin/c++` (this sets the compiler to `c++`) in each build directory, and then running `make` once again.

# Mac OS X notes #
Since Mac OS X is UNIX, simply follow the general UNIX build procedure above. There are just a few important points to note when building Platinum on the Mac, listed below.

## Building VTK ##
On the Mac, VTK can be built using either the legacy [Carbon](http://en.wikipedia.org/wiki/Carbon_%28API%29) or the current [Cocoa](http://en.wikipedia.org/wiki/Cocoa_%28API%29) classes. Platinum is intended to use Cocoa classes – before running `make` on VTK, use the following commands to make the build a Cocoa one:

`cmake -D VTK_USE_CARBON:BOOL=OFF`

`cmake -D VTK_USE_COCOA:BOOL=ON`

## Building FLTK ##

On the Mac, FLTK can be built using either the legacy [QuickDraw](http://en.wikipedia.org/wiki/QuickDraw) or the current [Quartz](http://en.wikipedia.org/wiki/Quartz_%28graphics_layer%29) graphics layer. Platinum has been designed for work with Quartz, and **will not display images correctly if QuickDraw is used** (if you get stripes instead of the expected image in Platinum, this is the problem). Before running `make` on FLTK, use the following command to set Quartz as the graphics layer:

`cmake -D FLTK_QUARTZ:BOOL=ON`

## Xcode ##
If you are on Mac OS X you may use the `-G Xcode` flag with CMake (all other instructions as above) to generate Platinum as an Xcode project instead of a makefile. Once you have the Xcode project file, the project is built by selecting the appropriate command inside Xcode. NOTE: ITK, VTK or FLTK cannot be built with Xcode, but are easy to build with `make` anyway.

You may also use Xcode for updating/commiting SVN: select "Configure SCM" from the SCM menu, choose Subversion as SCM system and click "Enable SCM". When CMakeLists is changed, the project file will be rewritten on the next Build. Xcode will notice this shortly afterwards and ask You to reload. When this happens, SVN must be re-enabled as described above.

**Note:** for Xcode SVN to work, you have to have SVN previously set up. This can be done by command-line or, preferrably, with a tool like [svnX](http://www.lachoseinteractive.net/en/community/subversion/svnx/features/).