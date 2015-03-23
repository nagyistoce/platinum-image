## Platform-independence ##
Platinum was designed from an early stage to be run and developed while keeping the choice of operating system and computer type open. This creates three concerns for writing code:
  1. Using compiler-independent (standard) **syntax**
> > The Microsoft Visual C++ compiler has numerous non-standard extensions (reviewed in [this article](http://www.tilander.org/aurora/articles/000018.html)) which are not part of the C/C++ standard and will not compile in GCC. Errors from nonstandard code are uncomplicated to fix, but annoying and can pile up if Platinum is developed on MSVC only for an extended period of time. A specific point is include paths: the **backslash** ( \ ) path separator works only on Microsoft systems (it is a regular name character on UNIX) while _a regular slash ( / ) works on all three file systems_ since it isn't allowed in names in Windows.
  1. Using platform-indpendent **toolkits and calls**
> > This is achieved by relying _only_ on the free libraries ITK, FLTK and VTK. Also, the standard UNIX `dirent.h` header is not available in Windows compilers - this has been solved by including a public domain `dirent` substitute in the project, for use on Windows only. Platform-specific calls should be avoided entirely or kept isolated at an absolute minimum, since bug fixes or changes to these parts can be made only on the compiler/OS/hardware in question.
  1. Using **processor-specific** assumptions
> > Some arithmetic, particularly byte-bit manipulations, produces different results on different platforms. In practice, differences are expected only between the x86 (Intel/AMD) and RISC (Sun/IBM/Motorola/Apple) processor architectures. Unlike the above issues, this is not a matter of strategy, and code has to be written and tested specifically for both of the processor families. However, using clever architecture in Platinum this problem can be minimized.

### MSVC to GCC errors ###
When code is written and syntax-checked using only MSVC (vs.net environment), it is easy to unintentionally break the strict C++ syntax used by GCC as described in point 1 above. These are some common error situations and how to resolve them:
| **Error message** | **Condition** | **Solution** |
|:------------------|:--------------|:-------------|
|'_member_' was not declared in this scope |Member of a parent class accessed without this-> prefix| **this->**_member_|
| expected `;' before '_object_', '_object_' was not declared in this scope | _object_ is of a templated class named without "typename" modifier e.g: classname

&lt;params&gt;

 object; | **typename** _classname_

&lt;params&gt;

 object_;_|