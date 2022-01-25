###########################################################################
############          The TeX Gyre Collection of Fonts         ############
###########################################################################

Font: TeX Gyre DejaVu Math
Authors: Bogus\l{}aw Jackowski, Piotr Strzelczyk and Piotr Pianowski
Version: 1.106
Date: 8 V 2016 / 19 V 2016 (documentation update)

License:
  % Copyright 2016 for TeX Gyre math extensions by B. Jackowski,
  % P. Strzelczyk and P. Pianowski (on behalf of TeX Users Groups).
  %
  % Copyright (c) 2003 by Bitstream, Inc. All Rights Reserved.
  % DejaVu changes are in public domain (see the LICENSE-DejaVu.txt file).
  %
  % This work can be freely used and distributed under
  % the GUST Font License (GFL -- see GUST-FONT-LICENSE.txt)
  % which is actually an instance of the LaTeX Project Public License
  % (LPPL -- see http://www.latex-project.org/lppl.txt).
  %
  % This work has the maintenance status "maintained". The Current Maintainer
  % of this work is Bogus\l{}aw Jackowski, Piotr Strzelczyk and Piotr Pianowski.
  %
  % This work consists of the files listed
  % in the MANIFEST-TeX-Gyre-DejaVu-Math.txt file.


###########################################################################
############          A BRIEF DESCRIPTION OF THE FONT          ############
###########################################################################

TeX Gyre DejaVu Math is a math companion for the DejaVu Serif
family of fonts in the OpenType format;
see the LICENSE-DejaVu.txt file or http://dejavu-fonts.org/wiki/Main_Page
and http://dejavu-fonts.org/wiki/License .

The math OTF fonts should contain a special table, MATH,
described in the Microsoft specification "MATH -- The
mathematical typesetting table", ver. 1.7, March 2015
( http://www.microsoft.com/typography/otspec/math.htm ).
Moreover, they should contain a broad collection of
special characters (see "Technical Report #25.
UNICODE SUPPORT FOR MATHEMATICS" by Barbara Beeton,
Asmus Freytag, and Murray Sargent III, http://unicode.org/reports/tr25/ ).
In particular, math OTF scripts are expected to contain the following
scripts: a basic serif script (regular, bold, italic and
bold italic), a calligraphic script (regular and bold),
a double-struck script, a fraktur script (regular and
bold), a sans-serif script (regular, bold, oblique and
bold oblique), and a monospaced script.

Most of non-math glyphs have been excerpted from the original
DejaVu Serif, Sans, and Mono families of fonts; exception
is the fraktur script which we excerpted from the renowned Euler family
http://en.wikipedia.org/wiki/AMS_Euler (we used the old Type 1
AMS public domain fonts; note that after reshaping in 2009,
the Euler fonts have been released under the Open Font License),
calligraphic script, and Hebrew script (four symbols). The latter
two scripts, as well as math symbols (operators, relational symbols,
braces, arrows, accents etc.) have been designed to match visually
the basic font.

Note that the members of all the mentioned alphabets, except for
the main roman alphabet, should be considered symbols, not letters.
Symbols are not expected to occur in a text stream; instead,
they are expected to appear lonely, perhaps with some embellishments
like subscripts, superscripts, primes, dots above and below, etc.

To produce the font, MetaType1 and the FontForge library were used:
the Type1 PostScript font containing all relevant characters was
generated with the MetaType1 engine, and the result was converted
into the OTF format with all the necessary data structures by
a Python script employing the FontForge library.

                   *    *    *

The GUST e-Foundry Math Fonts Project was launched and is supported by
TeX USERS GROUPS (CS TUG, DANTE eV, GUST, NTG, TUG India, TUG, UK TUG).
Hearty thanks to the representatives of these groups and also
to all people who helped with their work, comments, ideas,
remarks, bug reports, objections, hints, consolations, etc.
