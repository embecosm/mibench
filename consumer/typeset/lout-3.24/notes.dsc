Basser Lout Version 3 and the PostScript Document Structuring Conventions
Jeffrey H. Kingston
24 March 1994
Modified to explain changes in font inclusion, 3 June 2000

The following notes detail all areas known to the author where Lout does
not fully conform to Version 3.0 of the PostScript Document Structuring
Conventions (DSC), as given in Appendix G of the PostScript Language
Reference Manual, second edition (PLRM).  Page numbers refer to PLRM.
These are minor infringements which should not trouble anyone (I hope).
Please let me know of any problems.

Language level and extensions.  Lout does not produce %%LanguageLevel and
%%Extensions comments, implying that its output is Level 1 PostScript with
no extensions (p644).  This is true except for PostScript which user-level
commands choose to add via @SetColour, @Graphic, @IncludeGraphic, and
@PrependGraphic commands.  If this added PostScript is not Level 1 then
the absence of %%LanguageLevel and %%Extensions comments will be in
violation of the DSC.  Lout's standard packages use only Level 1, which
should take care of most of the @SetColour, @Graphic, and @PrependGraphic
commands encountered in practice.  If a %%LanguageLevel or %%Extensions
comment is encountered in an @IncludeGraphic file, Lout prints a warning
message but takes no other action.

Color separation conventions (pp685-688).  At present Lout generates none
of the comments described in this section.

DocumentSuppliedResources.  According to p671, each included document
(%%Begin(End)Document pair in PostScript, @IncludeGraphic file in Lout)
should have a corresponding "%%DocumentSuppliedResources: file" entry.
This contradicts p660, where each %%DocumentSuppliedResources entry is
said to correspond to a %%Begin(End)Resource pair.  Lout does not record
%%Begin(End)Document pairs in the %%DocumentSuppliedResources list.

EOF.  According to p673, the %%EOF comment of an included document (i.e.
one enclosed in %%Begin(End)Document) will be ignored.  However, p658
does not mention this exception, and one Adobe PostScript interpreter at
least in the author's environment terminates at the first %%EOF whether
or not it lies inside %%Begin(End)Document.  Accordingly, Lout strips
out any %%EOF comment within any included document during inclusion.

Font inclusion.  According to p678, "%%IncludeResource: font" comments
relating to commonly available fonts should be placed in the document
setup section.  However no definition of which fonts fall into this
category is offered, so this requirement is not observed by Lout.
Instead, Lout generates one "%%IncludeResource: font" in the page setup
section for each font used on the corresponding page.  (It used to be
the case that fonts used on the first page were an exception to this
rule; they were done once only in the document setup section.  This
exception has been removed from Lout Version 3.22 and above.)  Thus,
fonts used on many pages will generate many "%%IncludeResource: font"
comments, one for each page on which the font is used, which in turn
implies that these fonts might be downloaded many times (Lout itself
never downloads any font, but a document manager might react to an
"%%IncludeResource: font" comment by doing so).  However, most fonts
used by most people are already in the printer, and in these cases
"it is highly likely that the font server or document manager would
ignore the inclusion request, because the font would already be
available on the printer" (p678).  The rationale for putting all
"%%IncludeResource: font" comments into page setups is that it gives
document managers exact information about what fonts are used on
each page, which, hopefully, they can put to good use.

PageResources.  This comment is supposed to list all resources needed
or supplied on the current page except for procsets (p685).  At present
Lout lists fonts but not encoding vectors under this comment.

Definitions in EPS files.  On p736, "it is strongly recommended that an
EPS file make its definitions in its own dictionary or dictionaries".
Lout -EPS does not follow this recommendation, since it is not clear where
to create this dictionary or dictionaries, given that the DSC states (p625)
that the prolog should be a sequence of procset definitions (after the
header section and defaults section) only.  Perhaps each procset should be
in its own dictionary, as is done in lout/include/fig_prepend for example,
but this is not stated anywhere.
