------------------------------------------------------------------------
		    SlimKit Surface Modeling Tools
			     Version 1.0
			    March 17, 1999

		 Michael Garland <garland@cs.cmu.edu>


 Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.


 For the latest version and up-to-date information, please visit
 http://www.cs.cmu.edu/~garland/quadrics/


 Michael Garland
 Computer Science Department
 Carnegie Mellon University
 5000 Forbes Avenue
 Pittsburgh, PA 15213

------------------------------------------------------------------------

OVERVIEW
--------

This is the SlimKit collection of surface modeling tools.  Most of the
programs in this collection simply provide an interface to the
underlying functionality of the MixKit library (see mixkit/README.txt).

The primary component of this package is the QSlim 2.0 surface
simplification tool located in the tools/qslim directory.  Please
consult tools/qslim/QSlim.txt for further information.  There are also
several useful programs in tools/filters/ for manipulating SMF model
files.



BUILDING & INSTALLATION
-----------------------

To build on a Unix machine:

	(0) Read mixkit/README.txt

	(1) cd mixkit/build
	(2) ./configure
	(3) cd ../src
	(4) make
	(5) cd ../../tools/qslim
	(6) make qslim

  If XForms is available:
	(7) make qvis	

To build using Visual C++ under Windows 9x/NT:

        (1) Use the SlimKit workspace in the "win32" directory

Finally:

	 o  Read tools/qslim/QSlim.txt



FUTURE PLANS
------------

Further Documentation -- Along with further documentation for the
underlying MixKit functionality, I'm also working on documenting and
improving the utility of the programs in the ./tools/filters/
directory.

Improved GUI -- The current GUI code is functional and compiles under
both Win32 and X11.  However, it's very minimal and doesn't allow for
any kind of sophisticated user interface.  I'm currently developing a
much improved interface based on FLTK <http://www.fltk.org>.  Look for
this to be available in Summer 1999.

Integrated Attribute Simplification -- I plan on integrating the
functionality of the examples/propslim program into the main QSlim
program.
