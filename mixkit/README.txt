------------------------------------------------------------------------
		       MixKit Software Library
			     Version 1.0
			    March 17, 1999

		 Michael Garland <garland@cs.cmu.edu>


 Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.


 For the latest version and up-to-date information, please visit
 http://www.cs.cmu.edu/~garland/quadrics/qslim.html


 Michael Garland
 Computer Science Department
 Carnegie Mellon University
 5000 Forbes Avenue
 Pittsburgh, PA 15213

------------------------------------------------------------------------



OVERVIEW
--------

This is the first public release of the MixKit software library.  It
contains the bulk of the code I have developed in the course of my
doctoral research at Carnegie Mellon University.  Since I developed it
for my own research purposes, the code may be a little ugly in places,
and the selection of features has been largely driven by what I needed
to get my work done.  It's also been a place to do a little
experimenting.  For instance, the MxAsp module is an experimental
super-minimal scripting interface.

I am releasing this library in the hope that you'll find it useful.
If you use my software for anything more substantial than toying
around, I'd be interested to hear about it.  Any other comments you'd
like to make are also welcome.  I'd also like to encourage you to
contribute bug reports, patches, and whatever improvements you might
come up with to this library.



BUILDING & INSTALLATION
-----------------------

To build on a Unix machine:

  - In order to use the GUI, you will need to have the XForms library
    <http://bragg.phys.uwm.edu/xforms> installed.

  - Support for TIFF image files requires the libtiff library.

  - Running ./configure --help will tell you how to customize the
    compilation of the library.  For some examples, you might try
    consulting the 'build/mkconf*' scripts which I use on my SGI
    machine.

  - To build the library, just perform the following steps:

	(1) cd build
	(2) ./configure
	(3) cd ../src
	(4) make

    NOTE: If you do not have XForms installed, you will need to edit
    mixkit/src/Makefile and remove the files listed under GUI_SRCS.


To build using Visual C++ 5.0 under Windows 9x/NT:

        (1a) Use the SlimKit workspace
     or
        (1b) Use the MixKit.dsp project in your own workspace

No further installation is required.  The SlimKit build will
automatically grab the MixKit library from this directory.



DOCUMENTATION
-------------

I'm sorry to say that there is currently only minimal documentation
available for this library (see the 'doc' subdirectory).  You can also
glean some information from the comments in the source files.

Fortunately, this situation will be improving.  By April 1999, my
Ph.D. dissertation will be completed and publicly available.  This is
the most authoritative description of the simplification algorithms
implemented by the MxQSlim classes.  It also contains an appendix
which describes the use of some of the underlying code.  Along with my
dissertation, I will also be releasing more detailed documentation for
the primary classes in the library.



FUTURE PLANS
------------

Further Documentation -- As outlined above, I'm currently working on
expanded documentation both for my simplification algorithm and the
code contained in this library.

Improved GUI -- The current GUI code is functional and compiles under
both Win32 and X11.  However, it's very minimal and doesn't allow for
any kind of sophisticated user interface.  I'm currently developing a
much improved interface based on FLTK <http://www.fltk.org>.  Look for
this to be available in Summer 1999.

Scripting -- I've been working on using SWIG <http://www.swig.org> to
provide the appropriate glue for using the MixKit library in scripting
languages, specifically Perl and Python.

Restructuring -- For my personal work, it's been advantageous to
structure the code base as a single monolithic library.
Unfortunately, this makes MixKit shared libraries cumbersome.
