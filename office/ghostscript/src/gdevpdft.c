/* Copyright (C) 1996, 1997 Aladdin Enterprises.  All rights reserved.
  
  This file is part of Aladdin Ghostscript.
  
  Aladdin Ghostscript is distributed with NO WARRANTY OF ANY KIND.  No author
  or distributor accepts any responsibility for the consequences of using it,
  or for whether it serves any particular purpose or works at all, unless he
  or she says so in writing.  Refer to the Aladdin Ghostscript Free Public
  License (the "License") for full details.
  
  Every copy of Aladdin Ghostscript must include a copy of the License,
  normally in a plain ASCII text file named PUBLIC.  The License grants you
  the right to copy, modify and redistribute Aladdin Ghostscript, but only
  under certain conditions described in the License.  Among other things, the
  License requires that the copyright notice and this notice be preserved on
  all copies.
*/

/* gdevpdft.c */
/* Text handling for PDF-writing driver. */
#include "string_.h"
#include "gx.h"
#include "gserrors.h"
#include "gsutil.h"			/* for bytes_compare */
#include "gdevpdfx.h"
#include "strimpl.h"
#include "sstring.h"			/* for PSStringEncode */

/*
 * Define whether to re-encode characters in order to find them within base
 * font encodings.  This can greatly reduce the number of characters
 * represented as bitmaps, but it may cause the text in the PDF file to
 * differ from the text in the PostScript input.
 */
#define RE_ENCODE_OK

/*
 * The show pseudo-parameter is currently the way that the PostScript code
 * passes show operations to the PDF writer.  It is a hack!  Its "value"
 * is a dictionary with the following keys and values:
 *	/String (str)
 *	/Values [cx cy char ax ay px py]
 *	/Matrix [xx xy yx yy tx ty]
 *	/FontName /fontname
 *	/Encoding [e0 .. e255]
 *	/BaseEncoding [e0 ... e255]
 * Note that px/y and tx/y are floating point values in device space;
 * cx/y and ax/y are in user space.  The matrix is the concatenation of
 *	FontMatrix
 *	inverse of base FontMatrix
 *	CTM
 * This represents the transformation from a 1-unit-based character space
 * to device space.  The base encoding is StandardEncoding for all fonts
 * except Symbol and ZapfDingbats.
 */

/* Define the 14 standard built-in fonts. */
private const char *standard_font_names[] = {
  "Courier", "Courier-Bold", "Courier-Oblique", "Courier-BoldOblique",
  "Helvetica", "Helvetica-Bold", "Helvetica-Oblique", "Helvetica-BoldOblique",
  "Symbol",
  "Times-Roman", "Times-Bold", "Times-Italic", "Times-BoldItalic",
  "ZapfDingbats",
  0
};

/* Process a show operation. */
int
pdfshow_process(gx_device_pdf *pdev, const gs_param_dict *ptd)
{
#define plist (ptd->list)
	gs_param_string str, fnstr;
	gs_param_float_array va;
#define v_cx va.data[0]
#define v_cy va.data[1]
#define v_cch (int)va.data[2]
#define v_ax va.data[3]
#define v_ay va.data[4]
#define v_px va.data[5]
#define v_py va.data[6]
	gs_param_float_array ma;
#define cmat (*(const gs_matrix *)ma.data)
	gs_param_string_array ea, bea;
	int code;
	pdf_font *ppf;
	stream *s = pdev->strm;
	double sx = pdev->scale.x, sy = pdev->scale.y;
	bool re_encode = false;
	float size;
	byte strbuf[200];

	if ( (code = param_read_string(plist, "String", &str)) ||
	     (code = param_read_float_array(plist, "Values", &va)) ||
	     va.size != 7 ||
	     (code = param_read_float_array(plist, "Matrix", &ma)) ||
	     ma.size != 6 ||
	     (code = param_read_string(plist, "FontName", &fnstr)) ||
	     (code = param_read_name_array(plist, "Encoding", &ea)) ||
	     ea.size != 256 ||
	     (code = param_read_name_array(plist, "BaseEncoding", &bea)) ||
	     bea.size != 256
	   )
	  return_error(gs_error_rangecheck);
	if ( v_cy != 0 || (v_cch != 32 && v_cx != 0) || v_ay != 0 )
	  return_error(gs_error_undefined);
	/* Check that all characters match the base encoding. */
	{ uint i;
	  for ( i = 0; i < str.size; ++i )
	    { byte chr = str.data[i];
	      if ( ea.data[chr].data != bea.data[chr].data )
		{
#ifdef RE_ENCODE_OK
		  /* Since the penalty for converting text to a bitmap */
		  /* is so severe, see if the character is present */
		  /* at some other position in the base encoding. */
		  int ei;
		  for ( ei = 0; ei < 256; ++ei )
		    if ( ea.data[chr].data == bea.data[ei].data )
		      break;
		  if ( ei == 256 )
		    return_error(gs_error_undefined);
		  /* It really simplifies things if we can buffer */
		  /* the entire string locally in one piece.... */
		  if ( !re_encode )
		    { if ( str.size > sizeof(strbuf) )
		        return_error(gs_error_limitcheck);
		      memcpy(strbuf, str.data, str.size);
		      re_encode = true;
		    }
		  strbuf[i] = (byte)ei;
#else
		  return_error(gs_error_undefined);
#endif
		}
	    }
	}
	/* Find or create the font resource. */
	for ( ppf = (pdf_font *)pdev->resources[resourceFont]; ppf != 0;
	      ppf = ppf->next
	    )
	  if ( !bytes_compare(ppf->fname.data, ppf->fname.size,
			      fnstr.data, fnstr.size)
	     )
	    break;
	size = (cmat.xx != 0 ? cmat.xx / sx : 1);
	if ( ppf == 0 )
	{	/* Currently, we only handle the built-in fonts. */
		const char **ppfn;
		for ( ppfn = standard_font_names; *ppfn; ++ppfn )
		  if ( strlen(*ppfn) == fnstr.size &&
		       !strncmp(*ppfn, (const char *)fnstr.data, fnstr.size)
		     )
		    break;
		if ( !*ppfn )
		  return_error(gs_error_undefined);
		code = pdf_begin_resource(pdev, resourceFont,
					  (pdf_resource **)&ppf);
		if ( code < 0 )
		  return_error(gs_error_undefined);
		pprints1(s, " /Subtype /Type1 /BaseFont /%s >>\n",
			 *ppfn);
		ppf->fname.data = fnstr.data, ppf->fname.size = fnstr.size;
		pdf_end_resource(pdev);
	}
	code = pdf_open_contents(pdev, pdf_in_text);
	if ( code < 0 )
	  return code;
	/* We attempt to eliminate redundant parameter settings. */
	if ( ppf != pdev->text_state.font || size != pdev->text_state.size )
	  { pprintld1(s, "/R%ld ", ppf->id);
	    pprintg1(s, "%g Tf\n", size);
	    pdev->text_state.font = ppf;
	    pdev->text_state.size = size;
	  }
	sx *= size;
	sy *= size;
	/* The spacing values are in user space; we want them in text space. */
	{ gs_matrix tmat;
	  gs_point spacing;

	  tmat.xx = cmat.xx / sx;
	  tmat.xy = cmat.xy / sy;
	  tmat.yx = cmat.yx / sx;
	  tmat.yy = cmat.yy / sy;
	  tmat.tx = (v_px + cmat.tx) / pdev->scale.x;
	  tmat.ty = (v_py + cmat.ty) / pdev->scale.y;
	  pprintg6(s, "%g %g %g %g %g %g Tm\n",
		   tmat.xx, tmat.xy, tmat.yx, tmat.yy, tmat.tx, tmat.ty);
	  gs_distance_transform_inverse(v_ax, v_ay, &tmat, &spacing);
	  { float chars = spacing.x;
	    if ( pdev->text_state.character_spacing != chars )
	      { pprintg1(s, "%g Tc\n", chars);
	        pdev->text_state.character_spacing = chars;
	      }
	  }
	  gs_distance_transform_inverse(v_cx, v_cy, &tmat, &spacing);
	  { float words = spacing.x;
	    if ( pdev->text_state.word_spacing != words )
	      { pprintg1(s, "%g Tw\n", words);
	        pdev->text_state.word_spacing = words;
	      }
	  }
	}
	/* Write the string.  Make sure it gets any necessary \s. */
	pputc(s, '(');
	{ byte buf[100];		/* size is arbitrary */
	  stream_cursor_read r;
	  stream_cursor_write w;
	  int status;

	  r.ptr = (re_encode ? strbuf : str.data) - 1;
	  r.limit = r.ptr + str.size;
	  w.limit = buf + sizeof(buf) - 1;
	  do
	    { w.ptr = buf - 1;
	      status = (*s_PSSE_template.process)(NULL, &r, &w, true);
	      pwrite(s, buf, (uint)(w.ptr + 1 - buf));
	    }
	  while ( status == 1 );
	}
	pputs(s, " Tj\n");

	return 0;
}
