== Changes by Andrew Bainbridge ==

Removed support for non-zero filling. There's no difference between even-odd
and non-zero so long as no polygon edges cross. I'm happy to make that
assumption.

Removed support for subpixel sizes less than 1/8th of a pixel. I don't need
higher quality than that.

Removed the concept of polygons and subpolygons. All I wanted was the
subpolygon and the polygon filler. I've renamed subpolygon to PolygonData. Also
removed the VectorGraphic class. Basically my aim was to create an anti-aliased
polygon renderer that could be used to render fat lines and beziers and other
geometric primitives and these extra classes weren't ideal for that.

Moved all the source files into one directory and sorted out the #includes
so that all files include everything they need to be self-sufficient.

Made PolygonFiller keep a pointer to the bitmap that it is currently setup to
render on. Then modified PolygonFiller to assume that this bitmap is a 
Deadfrog lib BitmapRGBA.

Merged the PolygonFiller and PolygonData code into a single file so that
various things that they shared could be kept private. Also it means that
a user of the code now only needs to include a single header, namely
polygon.h.

Removed the Matrix2d class. It was partly used to allow you to provide a
transform to the poly before rendering it. That wasn't useful for my purposes.
Instead, you can now get direct access of the vertex data and set it how you
like. The only other use of the Matrix2d class was to transform the vertex data
from pixel space to "subpixel" space. This is now done more efficiently by not
using a fully-general matrix transform.

Removed all of the versions of the polygon filler other than 'F'. Obviously
only one is needed and F is the fastest. I'd like to have kept 'A' to make
it easier to understand the algorithm, however, that version seemed buggy
in the code released by Kiia, so I decided not to keep it either.

Made the PolygonData constructor allocate the vertex data itself, rather 
than it needing to passed in.

Made the PolygonFiller construction always create space for 60 edges, since
that should be enough for anyone. If not, it will grow itself anyway.

Removed support for clip rectangles other than the bitmap size, since I didn't
need that.

Removed the macros for accessing Vector2d. I don't understand what they were
for.

Changed the coding style:
 * Remove the 'a' at the start of all arguments.
 * Use m_ on member variables. 
 * Capitalize first char of function names.
 * Always put spaces after commas.
 * Change filenames to be lower case with underscores.
 * Remove doxygen mark-up.
 * Replace C style comments with C++ style ones.
 * Use "for(int i =" instead of "int i; for(i =" where possible.

Removed lots of duplicated comments and comments that only repeated the name of
the function/class/file.
