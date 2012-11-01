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

/* gdevpdf.c */
/* PDF-writing driver */
#include "math_.h"
#include "string_.h"
#include "time_.h"
#include "gx.h"
#include "gp.h"
#include "gscdefs.h"
#include "gserrors.h"
#include "gxdevice.h"
#include "gxfixed.h"
#include "gxistate.h"
#include "gxpaint.h"
#include "gzpath.h"
#include "gzcpath.h"
#include "gdevpdfx.h"
#include "strimpl.h"		/* for short-sighted compilers */
#include "scfx.h"		/* s_CFE_template is default */

/* Define the size of the internal stream buffer. */
/* (This is not a limitation, it only affects performance.) */
#define sbuf_size 512

/* GC descriptors */
private_st_pdf_resource();
private_st_pdf_font();

/* Device procedures */
private dev_proc_open_device(pdf_open);
private dev_proc_output_page(pdf_output_page);
private dev_proc_close_device(pdf_close);
private dev_proc_fill_rectangle(pdf_fill_rectangle);
extern dev_proc_copy_mono(gdev_pdf_copy_mono);		/* in gdevpdfi.c */
extern dev_proc_copy_color(gdev_pdf_copy_color);	/* in gdevpdfi.c */
extern dev_proc_get_params(gdev_pdf_get_params);	/* in gdevpdfp.c */
extern dev_proc_put_params(gdev_pdf_put_params);	/* in gdevpdfp.c */
private dev_proc_fill_path(pdf_fill_path);
private dev_proc_stroke_path(pdf_stroke_path);
extern dev_proc_fill_mask(gdev_pdf_fill_mask);		/* in gdevpdfi.c */
extern dev_proc_begin_image(gdev_pdf_begin_image);	/* in gdevpdfi.c */
extern dev_proc_image_data(gdev_pdf_image_data);	/* in gdevpdfi.c */
extern dev_proc_end_image(gdev_pdf_end_image);		/* in gdevpdfi.c */

#ifndef X_DPI
#  define X_DPI 720
#endif
#ifndef Y_DPI
#  define Y_DPI 720
#endif

gx_device_pdf far_data gs_pdfwrite_device =
{	std_device_color_body(gx_device_pdf, 0, "pdfwrite",
	  85*X_DPI/10, 110*Y_DPI/10, X_DPI, Y_DPI, 24, 255, 255),
	{	pdf_open,
		gx_upright_get_initial_matrix,
		NULL,			/* sync_output */
		pdf_output_page,
		pdf_close,
		gx_default_rgb_map_rgb_color,
		gx_default_rgb_map_color_rgb,
		pdf_fill_rectangle,
		NULL,			/* tile_rectangle */
		gdev_pdf_copy_mono,
		gdev_pdf_copy_color,
		NULL,			/* draw_line */
		NULL,			/* get_bits */
		gdev_pdf_get_params,
		gdev_pdf_put_params,
		NULL,			/* map_cmyk_color */
		NULL,			/* get_xfont_procs */
		NULL,			/* get_xfont_device */
		NULL,			/* map_rgb_alpha_color */
		gx_page_device_get_page_device,
		NULL,			/* get_alpha_bits */
		NULL,			/* copy_alpha */
		NULL,			/* get_band */
		NULL,			/* copy_rop */
		pdf_fill_path,
		pdf_stroke_path,
		gdev_pdf_fill_mask,
		NULL,			/* fill_trapezoid */
		NULL,			/* fill_parallelogram */
		NULL,			/* fill_triangle */
		NULL,			/* draw_thin_line */
		gdev_pdf_begin_image,
		gdev_pdf_image_data,
		gdev_pdf_end_image
	},
	psdf_initial_values,
	1.2,		/* CompatibilityLevel */
	0/*false*/,	/* DoThumbnails */
	1,		/* FirstObjectNumber */
	0/*false*/,	/* binary_ok */
	 { 0 },		/* tfname */
	0,		/* tfile */
	0,		/* next_id */
	0,		/* root_id */
	0,		/* info_id */
	0,		/* pages_id */
	0,		/* outlines_id */
	0,		/* next_page */
	 { 0 },		/* contents_ids */
	0,		/* next_contents_id */
	pdf_in_none,	/* context */
	0,		/* contents_length_id */
	0,		/* contents_pos */
	NoMarks,	/* procsets */
	-1,		/* flatness */
	 { gx_line_params_initial },	/* line_params */
	0,		/* page_ids */
	0,		/* num_page_ids */
	0,		/* pages_referenced */
	 { 0 },		/* resources */
	0,		/* annots */
	0,		/* last_resource */
	 { 0, 0 },	/* catalog_string */
	 { 0, 0 },	/* pages_string */
	 { 0, 0 },	/* page_string */
	 { { { 0 } } },	/* outline_levels */
	0,		/* outline_depth */
	0,		/* closed_outline_depth */
	0,		/* outlines_open */
	0,		/* articles */
	0,		/* named_dests */
	 { pdf_text_state_default }	/* text_state */
};

/* ---------------- Utilities ---------------- */

/* ------ Document ------ */

/* Initialize the IDs allocated at startup. */
void
pdf_initialize_ids(gx_device_pdf *pdev)
{	pdev->next_id = pdev->FirstObjectNumber;
	pdev->root_id = pdf_obj_ref(pdev);
	pdev->pages_id = pdf_obj_ref(pdev);
}

/* Open the document if necessary. */
void
pdf_open_document(gx_device_pdf *pdev)
{	stream *s = pdev->strm;

	if ( !is_in_document(pdev) && stell(s) == 0 )
	  { pputs(s, "%PDF-1.1\n");
	    pdev->binary_ok = !pdev->params.ASCII85EncodePages;
	    if ( pdev->binary_ok )
	      pputs(s, "%\307\354\217\242\n");
	  }
}

/* ------ Objects ------ */

/* Allocate an object ID. */
private long
pdf_next_id(gx_device_pdf *pdev)
{	return (pdev->next_id)++;
}

/* Allocate an ID for a future object. */
long
pdf_obj_ref(gx_device_pdf *pdev)
{	long id = pdf_next_id(pdev);
	stream *s = pdev->strm;
	long pos = stell(s);
	FILE *tfile = pdev->tfile;

	fwrite(&pos, sizeof(pos), 1, tfile);
	return id;
}

/* Begin an object, optionally allocating an ID. */
long
pdf_open_obj(gx_device_pdf *pdev, long id)
{	stream *s = pdev->strm;
	FILE *tfile = pdev->tfile;

	if ( id <= 0 )
	  { id = pdf_obj_ref(pdev);
	  }
	else
	  { long pos = stell(s);
	    long tpos = ftell(tfile);

	    fseek(tfile, (id - pdev->FirstObjectNumber) * sizeof(pos),
		  SEEK_SET);
	    fwrite(&pos, sizeof(pos), 1, tfile);
	    fseek(tfile, tpos, SEEK_SET);
	  }
	pprintld1(s, "%ld 0 obj\n", id);
	return id;
}

/* End an object. */
int
pdf_end_obj(gx_device_pdf *pdev)
{	pputs(pdev->strm, "endobj\n");
	return 0;
}

/* ------ Graphics ------ */

/* Set the fill or stroke color. */
int
pdf_set_color(gx_device_pdf *pdev, gx_color_index color,
  gx_drawing_color *pdcolor, const char *rgs)
{	if ( gx_dc_pure_color(pdcolor) != color )
	  { color_set_pure(pdcolor, color);
	    psdf_set_color((gx_device_vector *)pdev, pdcolor, rgs);
	  }
	return 0;
}

/* Reset the graphics state parameters to initial values. */
private void
pdf_reset_graphics(gx_device_pdf *pdev)
{	color_set_pure(&pdev->fill_color, 0);	/* black */
	color_set_pure(&pdev->stroke_color, 0);	/* ditto */
	pdev->flatness = -1;
	{ static const gx_line_params lp_initial = { gx_line_params_initial };
	  pdev->line_params = lp_initial;
	}
}

/* Set the scale for coordinates according to the current resolution. */
void
pdf_set_scale(gx_device_pdf *pdev)
{	pdev->scale.x = pdev->HWResolution[0] / 72.0;
	pdev->scale.y = pdev->HWResolution[1] / 72.0;
}

/* ------ Page contents ------ */

/* Begin a page contents part. */
/* Return an error if the page has too many contents parts. */
int
pdf_open_contents(gx_device_pdf *pdev, pdf_context context)
{	stream *s = pdev->strm;

	if ( pdev->context == pdf_in_none )
	  { int next = pdev->next_contents_id;

	    if ( next == max_contents_ids )
	      return_error(gs_error_limitcheck);
	    pdev->contents_ids[next] = pdf_begin_obj(pdev);
	    pdev->next_contents_id = next + 1;
	    pdev->contents_length_id = pdf_obj_ref(pdev);
	    pprintld1(s, "<< /Length %ld 0 R >>\n", pdev->contents_length_id);
	    pputs(s, "stream\n");
	    pdev->context = pdf_in_stream;
	    pdev->contents_pos = stell(s);
	    if ( next == 0 )
	      { /* Do a level of gsave for the clipping path. */
		pputs(s, "q\n");
	      }
	  }
	if ( pdev->context == context )
	  return 0;
	switch ( context )
	{
	case pdf_in_text:
	  /* pdev->context must be pdf_in_stream */
	  pputs(s, "BT\n");
	  pdev->procsets |= Text;
	  break;
	case pdf_in_stream:
	  /* pdev->context must be pdf_in_text */
	  pputs(s, "ET\n");
	  break;
	/*case pdf_in_none:*/	/* not possible */
	}
	pdev->context = context;
	return 0;
}

/* Close the current contents part if we are in one. */
int
pdf_close_contents(gx_device_pdf *pdev, bool last)
{	stream *s = pdev->strm;
	long length;

	switch ( pdev->context )
	{
	case pdf_in_none:
		return 0;
	case pdf_in_text:
		pputs(s, "ET\n");
	case pdf_in_stream:
		;
	}
	if ( last )
	  { /* Exit from the clipping path gsave. */
	    pputs(s, "Q\n");
	    pdev->text_state.font = 0;
	  }
	length = stell(s) - pdev->contents_pos;
	pputs(s, "endstream\n");
	pdf_end_obj(pdev);
	pdf_open_obj(pdev, pdev->contents_length_id);
	pprintld1(s, "%ld\n", length);
	pdf_end_obj(pdev);
	pdev->context = pdf_in_none;
	return 0;
}

/* ------ Resources et al ------ */

/* Define the names of the resource types. */
private const char *resource_names[] =
 { pdf_resource_type_names };

/* Define the allocator descriptors for the resource types. */
private const gs_memory_struct_type_t *resource_structs[] =
 { pdf_resource_type_structs };

/* Find a resource of a given type by gs_id. */
pdf_resource *
pdf_find_resource_by_gs_id(gx_device_pdf *pdev, pdf_resource_type type,
  gs_id rid)
{	pdf_resource **pprev = &pdev->resources[type];
	pdf_resource *pres;

	for ( ; (pres = *pprev) != 0; pprev = &pres->next )
	  if ( pres->rid == rid )
	    { *pprev = pres->next;
	      pres->next = pdev->resources[type];
	      pdev->resources[type] = pres;
	      break;
	    }
	return pres;
}

/* Begin an aside (resource, annotation, ...). */
int
pdf_begin_aside(gx_device_pdf *pdev, pdf_resource **plist,
  const gs_memory_struct_type_t *pst, pdf_resource **ppres)
{	pdf_resource *pres;

	if ( pdev->context != pdf_in_none &&
	     pdev->next_contents_id == max_contents_ids
	   )
	  return_error(gs_error_limitcheck);
	if ( pst == NULL )
	  pst = &st_pdf_resource;
	pres =
	  gs_alloc_struct(pdev->pdf_memory, pdf_resource, pst, "begin_aside");
	if ( pres == 0 )
	  return_error(gs_error_VMerror);
	pdf_close_contents(pdev, false);
	pdf_open_document(pdev);
	pres->next = *plist;
	*plist = pres;
	pres->prev = pdev->last_resource;
	pdev->last_resource = pres;
	pres->id = pdf_begin_obj(pdev);
	*ppres = pres;
	return 0;
}
/* Begin a resource of a given type. */
int
pdf_begin_resource(gx_device_pdf *pdev, pdf_resource_type type,
  pdf_resource **ppres)
{	stream *s = pdev->strm;
	int code = pdf_begin_aside(pdev, &pdev->resources[type],
				   resource_structs[type], ppres);

	if ( code < 0 )
	  return code;
	pprints1(s, "<< /Type /%s", resource_names[type]);
	pprintld1(s, " /Name /R%ld", (*ppres)->id);
	return code;
}

/* End an aside. */
int
pdf_end_aside(gx_device_pdf *pdev)
{	return pdf_end_obj(pdev);
}
/* End a resource. */
int
pdf_end_resource(gx_device_pdf *pdev)
{	return pdf_end_aside(pdev);
}

/* ------ Pages ------ */

/* Reset the state of the current page. */
void
pdf_reset_page(gx_device_pdf *pdev)
{	pdev->next_contents_id = 0;
	pdf_reset_graphics(pdev);
	pdev->procsets = NoMarks;
	{ int i;
	  for ( i = 0; i < num_resource_types; ++i )
	    pdev->resources[i] = 0;
	}
	pdev->page_string.data = 0;
	{ static const pdf_text_state text_default =
	   { pdf_text_state_default };
	  pdev->text_state = text_default;
	}
}

/* Get or assign the ID for a page. */
/* Returns 0 if the page number is out of range. */
long
pdf_page_id(gx_device_pdf *pdev, int page_num)
{	long page_id;

	if ( page_num >= pdev->num_page_ids )
	  { /* Grow the page_ids array. */
	    uint new_num_ids =
	      max(page_num + 10, pdev->num_page_ids << 1);
	    /* resize_object for a byte array takes a new object size */
	    /* in bytes.  This is a quirk of the API that we probably */
	    /* won't ever be able to fix.... */
	    long *new_ids = gs_resize_object(pdev->pdf_memory, pdev->page_ids,
					     new_num_ids * sizeof(long),
					     "pdf_page_id(resize page_ids)");

	    if ( new_ids == 0 )
	      return 0;
	    pdev->page_ids = new_ids;
	    pdev->num_page_ids = new_num_ids;
	  }
	if ( page_num < 1 )
	  return 0;
	while ( page_num > pdev->pages_referenced )
	  pdev->page_ids[pdev->pages_referenced++] = 0;
	if ( (page_id = pdev->page_ids[page_num - 1]) == 0 )
	  pdev->page_ids[page_num - 1] = page_id = pdf_obj_ref(pdev);
	return page_id;
}

/* Write saved page- or document-level information. */
int
pdf_write_saved_string(gx_device_pdf *pdev, gs_string *pstr)
{	if ( pstr->data != 0 )
	  { pwrite(pdev->strm, pstr->data, pstr->size);
	    gs_free_string(pdev->pdf_memory, pstr->data, pstr->size,
			   "pdf_write_saved_string");
	    pstr->data = 0;
	  }
	return 0;
}

/* Open a page for writing. */
int
pdf_open_page(gx_device_pdf *pdev, pdf_context context)
{	if ( !is_in_page(pdev) )
	  { if ( pdf_page_id(pdev, pdev->next_page + 1) == 0 )
	      return_error(gs_error_VMerror);
	    pdf_open_document(pdev);
	  }
	return (context != pdf_in_none ? pdf_open_contents(pdev, context) :
		pdf_close_contents(pdev, false));
}

/* Close the current page. */
private int
pdf_close_page(gx_device_pdf *pdev)
{	stream *s = pdev->strm;
	int page_num = ++(pdev->next_page);
	long page_id;

	/* If the very first page is blank, we need to open the document */
	/* before doing anything else. */
	pdf_open_document(pdev);
	pdf_close_contents(pdev, true);
	page_id = pdf_page_id(pdev, page_num);
	pdf_open_obj(pdev, page_id);
	pprintd2(s, "<<\n/Type /Page\n/MediaBox [0 0 %d %d]\n",
		 (int)(pdev->MediaSize[0]), (int)(pdev->MediaSize[1]));
	pprintld1(s, "/Parent %ld 0 R\n", pdev->pages_id);
	pputs(s, "/Resources << /ProcSet [/PDF");
	if ( pdev->procsets & ImageB )
	  pputs(s, " /ImageB");
	if ( pdev->procsets & ImageC )
	  pputs(s, " /ImageC");
	if ( pdev->procsets & ImageI )
	  pputs(s, " /ImageI");
	if ( pdev->procsets & Text )
	  pputs(s, " /Text");
	pputs(s, "]\n");
	{ int i;
	  for ( i = 0; i < num_resource_types; ++i )
	    { const pdf_resource *pres = pdev->resources[i];
	      if ( pres != 0 )
		{ pprints1(s, "/%s <<\n", resource_names[i]);
		  for ( ; pres; pres = pres->next )
		    pprintld2(s, "/R%ld %ld 0 R\n", pres->id, pres->id);
		  pputs(s, ">>\n");
		}
	      pdev->resources[i] = 0;
	    }
	}
	pputs(s, ">>\n");
	if ( pdev->next_contents_id == 1 )
	  pprintld1(s, "/Contents %ld 0 R\n", pdev->contents_ids[0]);
	else
	  { int i;
	    pputs(s, "/Contents [\n");
	    for ( i = 0; i < pdev->next_contents_id; ++i )
	      pprintld1(s, "%ld 0 R\n", pdev->contents_ids[i]);
	    pputs(s, "]\n");
	  }
	pdf_write_saved_string(pdev, &pdev->page_string);
	{ const pdf_resource *pres = pdev->annots;
	  bool any = false;
	  for ( ; pres != 0; pres = pres->next )
	    if ( pres->rid == page_num - 1 )
	      { if ( !any )
		  { pputs(s, "/Annots [\n");
		    any = true;
		  }
		pprintld1(s, "%ld 0 R\n", pres->id);
	      }
	  if ( any )
	    pputs(s, "]\n");
	}
	pputs(s, ">>\n");
	pdf_end_obj(pdev);
	pdf_reset_page(pdev);
	return 0;
}

/* Write the default entries of the Info dictionary. */
int
pdf_write_default_info(gx_device_pdf *pdev)
{	stream *s = pdev->strm;
	/* Reading the time without using time_t is a challenge.... */
	long t[2];	/* time_t can't be longer than 2 longs. */
	struct tm ltime;
	char buf[20];

	time((void *)t);
	ltime = *localtime((void *)t);
	sprintf(buf, "%04d%02d%02d%02d%02d%02d",
		ltime.tm_year + 1900, ltime.tm_mon + 1, ltime.tm_mday,
		ltime.tm_hour, ltime.tm_min, ltime.tm_sec);
	pprints1(s, "/CreationDate (D:%s)\n", buf);
	sprintf(buf, "%1.2f", gs_revision / 100.0);
	pprints2(s, "/Producer (%s %s)\n", gs_product, buf);
	return 0;
}

/* ---------------- Device open/close ---------------- */

/* Open the device. */
private int
pdf_open(gx_device *dev)
{	gx_device_pdf *pdev = (gx_device_pdf *)dev;
	char fmode[4];
	int code;

	pdev->pdf_memory = &gs_memory_default;		/* as good as any */
	strcpy(fmode, "w+");
	strcat(fmode, gp_fmode_binary_suffix);
	pdev->tfile =
	  gp_open_scratch_file(gp_scratch_file_name_prefix,
			       pdev->tfname, fmode);
	if ( pdev->tfile == 0 )
	  return_error(gs_error_invalidfileaccess);
	code = gdev_vector_open_file((gx_device_vector *)pdev, sbuf_size);
	if ( code < 0 )
	  goto fail;
	gdev_vector_init((gx_device_vector *)pdev);
	/* Set in_page so the vector routines won't try to call */
	/* any vector implementation procedures. */
	pdev->in_page = true;
	pdf_set_scale(pdev);
	pdf_initialize_ids(pdev);
	pdev->outlines_id = 0;
	pdev->next_page = 0;
	pdev->page_ids = (void *)
	  gs_alloc_byte_array(pdev->pdf_memory, initial_num_page_ids,
			      sizeof(*pdev->page_ids), "pdf_open(page_ids)");
	if ( pdev->page_ids == 0 )
	  { code = gs_error_VMerror;
	    goto fail;
	  }
	pdev->num_page_ids = initial_num_page_ids;
	pdev->pages_referenced = 0;
	pdev->catalog_string.data = 0;
	pdev->pages_string.data = 0;
	pdev->outline_levels[0].first.id = 0;
	pdev->outline_levels[0].left = max_int;
	pdev->outline_depth = 0;
	pdev->closed_outline_depth = 0;
	pdev->outlines_open = 0;
	pdev->articles = 0;
	pdev->named_dests = 0;
	pdf_reset_page(pdev);

	return 0;
fail:
	fclose(pdev->tfile);
	unlink(pdev->tfname);
	pdev->tfile = 0;
	return code;
}

/* Wrap up ("output") a page. */
private int
pdf_output_page(gx_device *dev, int num_copies, int flush)
{	gx_device_pdf *pdev = (gx_device_pdf *)dev;
	return pdf_close_page(pdev);
}

/* Close the device. */
private int
pdf_close(gx_device *dev)
{	gx_device_pdf *pdev = (gx_device_pdf *)dev;
	stream *s = pdev->strm;
	FILE *tfile = pdev->tfile;
	long xref;
	long named_dests_id = 0;

	/* Tidy up a little.  This shouldn't be necessary, */
	/* but we don't want an EPS file with no showpage to produce */
	/* illegal PDF. */
	pdf_close_contents(pdev, true);

	/* Create the root (Catalog). */
	pdf_open_obj(pdev, pdev->pages_id);
	pputs(s, "<< /Type /Pages /Kids [\n");
	{ int i;
	  for ( i = 0; i < pdev->next_page; ++i )
	    pprintld1(s, "%ld 0 R\n", pdev->page_ids[i]);
	}
	pprintd1(s, "] /Count %d\n", pdev->next_page);
	pdf_write_saved_string(pdev, &pdev->pages_string);
	pputs(s, ">>\n");
	pdf_end_obj(pdev);
	if ( pdev->outlines_id != 0 )
	  { pdfmark_close_outline(pdev);	/* depth must be zero! */
	    pdf_open_obj(pdev, pdev->outlines_id);
	    pprintd1(s, "<< /Count %d", pdev->outlines_open);
	    pprintld2(s, " /First %ld 0 R /Last %ld 0 R >>\n",
		      pdev->outline_levels[0].first.id,
		      pdev->outline_levels[0].last.id);
	    pdf_end_obj(pdev);
	  }
	if ( pdev->articles != 0 )
	  { pdf_article *part;
	    /* Write the first and last beads of each article. */
	    for ( part = pdev->articles; part != 0; part = part->next )
	      { if ( part->last.id == 0 )
		  { /* Only one bead in the article. */
		    part->first.prev_id = part->first.next_id = part->first.id;
		  }
		else
		  { /* More than one bead in the article. */
		    part->first.prev_id = part->last.id;
		    part->last.next_id = part->first.id;
		    pdfmark_write_article(pdev, &part->last);
		  }
		pdfmark_write_article(pdev, &part->first);
	      }
	  }
	if ( pdev->named_dests != 0 )
	  { pdf_named_dest *pnd;
	    named_dests_id = pdf_begin_obj(pdev);
	    pputs(s, "<<\n");
	    while ( (pnd = pdev->named_dests) != 0 )
	      { pdev->named_dests = pnd->next;
		pwrite(s, pnd->key.data, pnd->key.size);
		pprints1(s, " %s\n", pnd->dest);
		gs_free_string(pdev->pdf_memory, pnd->key.data, pnd->key.size,
			       "pdf_close(named_dest key)");
		gs_free_object(pdev->pdf_memory, pnd, "pdf_close(named_dest)");
	      }
	    pputs(s, ">>\n");
	    pdf_end_obj(pdev);
	  }
	pdf_open_obj(pdev, pdev->root_id);
	pprintld1(s, "<< /Type /Catalog /Pages %ld 0 R\n", pdev->pages_id);
	if ( pdev->outlines_id != 0 )
	  pprintld1(s, "/Outlines %ld 0 R\n", pdev->outlines_id);
	if ( pdev->articles != 0 )
	  { pdf_article *part;
	    pputs(s, "/Threads [ ");
	    while ( (part = pdev->articles) != 0 )
	      { pdev->articles = part->next;
		pprintld1(s, "%ld 0 R\n", part->id);
		gs_free_string(pdev->pdf_memory, part->title.data,
			       part->title.size, "pdf_close(article title)");
		gs_free_object(pdev->pdf_memory, part, "pdf_close(article)");
	      }
	    pputs(s, "]\n");
	  }
	if ( named_dests_id != 0 )
	  pprintld1(s, "/Dests %ld 0 R\n", named_dests_id);
	pdf_write_saved_string(pdev, &pdev->catalog_string);
	pputs(s, ">>\n");
	pdf_end_obj(pdev);

	/* Create the Info directory. */
	/* This is supposedly optional, but some readers may require it. */
	if ( pdev->info_id == 0 )
	  { pdev->info_id = pdf_begin_obj(pdev);
	    pputs(s, "<< ");
	    pdf_write_default_info(pdev);
	    pputs(s, ">>\n");
	    pdf_end_obj(pdev);
	}

	/* Write the cross-reference section. */
	xref = stell(s);
	if ( pdev->FirstObjectNumber == 1 )
	  pprintld1(s, "xref\n0 %ld\n0000000000 65535 f \n",
		    pdev->next_id);
	else
	  pprintld2(s, "xref\n0 1\n0000000000 65535 f \n%ld %ld\n",
		    pdev->FirstObjectNumber,
		    pdev->next_id - pdev->FirstObjectNumber);
	fseek(tfile, 0L, SEEK_SET);
	{ long i;
	  for ( i = pdev->FirstObjectNumber; i < pdev->next_id; ++i )
	    { ulong pos;
	      char str[21];

	      fread(&pos, sizeof(pos), 1, tfile);
	      sprintf(str, "%010ld 00000 n \n", pos);
	      pputs(s, str);
	    }
	}

	/* Write the trailer. */
	pputs(s, "trailer\n");
	pprintld3(s, "<< /Size %ld /Root %ld 0 R /Info %ld 0 R\n",
		  pdev->next_id, pdev->root_id, pdev->info_id);
	pputs(s, ">>\n");
	pprintld1(s, "startxref\n%ld\n%%%%EOF\n", xref);

	/* Release the resource records. */
	{ pdf_resource *pres;
	  pdf_resource *prev;
	  for ( prev = pdev->last_resource; (pres = prev) != 0; )
	    { prev = pres->prev;
	      gs_free_object(pdev->pdf_memory, pres, "pdf_resource");
	    }
	  pdev->last_resource = 0;
	}

	gs_free_object(pdev->pdf_memory, pdev->page_ids, "page_ids");
	pdev->page_ids = 0;
	pdev->num_page_ids = 0;

	gdev_vector_close_file((gx_device_vector *)pdev);
	fclose(pdev->tfile);
	pdev->tfile = 0;
	unlink(pdev->tfname);
	return 0;
}

/* ---------------- Drawing ---------------- */

/* Fill a rectangle. */
private int
pdf_fill_rectangle(gx_device *dev, int x, int y, int w, int h,
  gx_color_index color)
{	gx_device_pdf *pdev = (gx_device_pdf *)dev;
	stream *s = pdev->strm;
	int code;

	/* Make a special check for the initial fill with white, */
	/* which shouldn't cause the page to be opened. */
	if ( color == 0xffffff && !is_in_page(pdev) )
	  return 0;
	code = pdf_open_page(pdev, pdf_in_stream);
	if ( code < 0 )
	  return code;
	/* Make sure we aren't being clipped. */
	pdf_put_clip_path(pdev, NULL);
	pdf_set_color(pdev, color, &pdev->fill_color, "rg");
	pprintg4(s, "%g %g %g %g re\nf\n",
		 x / pdev->scale.x, y / pdev->scale.y,
		 w / pdev->scale.x, h / pdev->scale.y);
	return 0;
}

/* ---------------- Path drawing ---------------- */

/* ------ Utilities ------ */

/* Put a path on the output file.  If do_close is false and the last */
/* path component is a closepath, omit it and return 1. */
private int
pdf_put_path(gx_device_pdf *pdev, const gx_path *ppath, bool do_close,
  const gs_matrix *pmat)
{	stream *s = pdev->strm;
	gs_fixed_rect rbox;
	const subpath *next;
	gs_path_enum cenum;

	/* If do_close is false, we recognize rectangles specially. */
	if ( !do_close && ppath->subpath_count == 1 &&
	     ppath->curve_count == 0 &&
	     gx_subpath_is_rectangle(ppath->first_subpath, &rbox, &next)
	   )
	  {	gs_point p, q;
		p.x = fixed2float(rbox.p.x), p.y = fixed2float(rbox.p.y);
		q.x = fixed2float(rbox.q.x), q.y = fixed2float(rbox.q.y);
		if ( pmat )
		  { gs_point_transform_inverse(p.x, p.y, pmat, &p);
		    gs_point_transform_inverse(q.x, q.y, pmat, &q);
		  }
		pprintg4(s, "%g %g %g %g re\n",
			 p.x / pdev->scale.x, p.y / pdev->scale.y,
			 (q.x - p.x) / pdev->scale.x,
			 (q.y - p.y) / pdev->scale.y);
		return 0;
	  }
	gx_path_enum_init(&cenum, ppath);
	for ( ; ; )
	  { fixed vs[6];
	    gs_point vp[3];
	    const char *format;
	    int pe_op = gx_path_enum_next(&cenum, (gs_fixed_point *)vs);

sw:	    switch ( pe_op )
	      {
	      case 0:		/* done */
		return 0;
	      case gs_pe_moveto:
		format = "%g %g m\n";
		goto do1;
	      case gs_pe_lineto:
		format = "%g %g l\n";
do1:		vp[0].x = fixed2float(vs[0]), vp[0].y = fixed2float(vs[1]);
		if ( pmat )
		  gs_point_transform_inverse(vp[0].x, vp[0].y, pmat, &vp[0]);
		pprintg2(s, format,
			 vp[0].x / pdev->scale.x, vp[0].y / pdev->scale.y);
		break;
	      case gs_pe_curveto:
		vp[0].x = fixed2float(vs[0]), vp[0].y = fixed2float(vs[1]);
		vp[1].x = fixed2float(vs[2]), vp[1].y = fixed2float(vs[3]);
		vp[2].x = fixed2float(vs[4]), vp[2].y = fixed2float(vs[5]);
		if ( pmat )
		  { gs_point_transform_inverse(vp[0].x, vp[0].y, pmat, &vp[0]);
		    gs_point_transform_inverse(vp[1].x, vp[1].y, pmat, &vp[1]);
		    gs_point_transform_inverse(vp[2].x, vp[2].y, pmat, &vp[2]);
		  }
		pprintg6(s, "%g %g %g %g %g %g c\n",
			 vp[0].x / pdev->scale.x, vp[0].y / pdev->scale.y,
			 vp[1].x / pdev->scale.x, vp[1].y / pdev->scale.y,
			 vp[2].x / pdev->scale.x, vp[2].y / pdev->scale.y);
		break;
	      case gs_pe_closepath:
		if ( do_close )
		  { pputs(s, "h\n");
		    break;
		  }
		pe_op = gx_path_enum_next(&cenum, (gs_fixed_point *)vs);
		if ( pe_op != 0 )
		  { pputs(s, "h\n");
		    goto sw;
		  }
		return 1;
	      }
	  }
}

/* Test whether we will need to put the clipping path. */
private bool
pdf_must_put_clip_path(gx_device_pdf *pdev, const gx_clip_path *pcpath)
{	if ( pcpath == NULL )
	  return pdev->clip_path_id != pdev->no_clip_path_id;
	if ( pdev->clip_path_id == pcpath->id )
	  return false;
	if ( gx_cpath_includes_rectangle(pcpath, fixed_0, fixed_0,
					 int2fixed(pdev->width),
					 int2fixed(pdev->height))
	   )
	  return pdev->clip_path_id != pdev->no_clip_path_id;
	return true;
}

/* Put a clipping path on the output file. */
int
pdf_put_clip_path(gx_device_pdf *pdev, const gx_clip_path *pcpath)
{	stream *s = pdev->strm;

	if ( pcpath == NULL ) {
	  if ( pdev->clip_path_id == pdev->no_clip_path_id )
	    return 0;
	  pputs(s, "Q\nq\n");
	  pdev->clip_path_id = pdev->no_clip_path_id;
	} else {
	  if ( pdev->clip_path_id == pcpath->id )
	    return 0;
	  if ( gx_cpath_includes_rectangle(pcpath, fixed_0, fixed_0,
					   int2fixed(pdev->width),
					   int2fixed(pdev->height))
	     ) {
	    if ( pdev->clip_path_id == pdev->no_clip_path_id )
	      return 0;
	    pputs(s, "Q\nq\n");
	    pdev->clip_path_id = pdev->no_clip_path_id;
	  } else {
	    pputs(s, "Q\nq\nW\n");
	    if ( pcpath->segments_valid )
	      pdf_put_path(pdev, &pcpath->path, true, NULL);
	    else
	      { /* Write out the rectangles. */
		const gx_clip_rect *prect = pcpath->list.head;

		if ( prect == 0 )
		  prect = &pcpath->list.single;
		for ( ; prect != 0; prect = prect->next )
		  if ( prect->xmax > prect->xmin && prect->ymax > prect->ymin )
		    pprintg4(s, "%g %g %g %g re\n",
			     prect->xmin / pdev->scale.x,
			     prect->ymin / pdev->scale.y,
			     (prect->xmax - prect->xmin) / pdev->scale.x,
			     (prect->ymax - prect->ymin) / pdev->scale.y);
	      }
	    pputs(s, "n\n");
	    pdev->clip_path_id = pcpath->id;
	  }
	}
	pdev->text_state.font = 0;
	if ( pdev->context == pdf_in_text )
	  pdev->context = pdf_in_stream;
	pdf_reset_graphics(pdev);
	return 0;
}

/* ------ Driver procedures ------ */

/* Fill a path. */
private int
pdf_fill_path(gx_device *dev, const gs_imager_state *pis, gx_path *ppath,
  const gx_fill_params *params,
  const gx_drawing_color *pdcolor, const gx_clip_path *pcpath)
{	gx_device_pdf *pdev = (gx_device_pdf *)dev;
	stream *s = pdev->strm;
	int code;
	/*
	 * HACK: we fill an empty path in order to set the clipping path
	 * and the color for writing text.  If it weren't for this, we
	 * could detect and skip empty paths before putting out the clip
	 * path or the color.
	 */
	bool have_path = !gx_path_is_void(ppath);

	if ( !gx_dc_is_pure(pdcolor) )
	  return gx_default_fill_path(dev, pis, ppath, params, pdcolor,
				      pcpath);
	/*
	 * Make a special check for the initial fill with white,
	 * which shouldn't cause the page to be opened.
	 */
	if ( gx_dc_pure_color(pdcolor) == 0xffffff && !is_in_page(pdev) )
	  return 0;
	if ( have_path || pdev->context == pdf_in_none ||
	     pdf_must_put_clip_path(pdev, pcpath)
	   )
	  { code = pdf_open_page(pdev, pdf_in_stream);
	    if ( code < 0 )
	      return code;
	  }
	pdf_put_clip_path(pdev, pcpath);
	pdf_set_color(pdev, gx_dc_pure_color(pdcolor), &pdev->fill_color, "rg");
	if ( have_path )
	  { if ( params->flatness != pdev->flatness )
	      { pprintg1(s, "%g i\n", params->flatness);
	        pdev->flatness = params->flatness;
	      }
	    pdf_put_path(pdev, ppath, false, NULL);
	    pputs(s, (params->rule < 0 ? "f\n" : "f*\n"));
	  }
	return 0;
}

/* Compare two dash patterns. */
private bool
dash_pattern_eq(const float *stored, const gx_dash_params *set, float scale)
{	int i;
	for ( i = 0; i < set->pattern_size; ++i )
	  if ( stored[i] != (float)(set->pattern[i] * scale) )
	    return false;
	return true;
}

/* Stroke a path. */
private int
pdf_stroke_path(gx_device *dev, const gs_imager_state *pis, gx_path *ppath,
  const gx_stroke_params *params,
  const gx_drawing_color *pdcolor, const gx_clip_path *pcpath)
{	gx_device_pdf *pdev = (gx_device_pdf *)dev;
	stream *s = pdev->strm;
	int code;
	int pattern_size = pis->line_params.dash.pattern_size;
	double scale;
	int i;

	if ( !gx_dc_is_pure(pdcolor) )
	  return gx_default_stroke_path(dev, pis, ppath, params, pdcolor,
					pcpath);
	code = pdf_open_page(pdev, pdf_in_stream);
	if ( code < 0 )
	  return code;
	/* Check for a CTM we can handle. */
	if ( pdev->scale.x != pdev->scale.y )
	  return gx_default_stroke_path(dev, pis, ppath, params, pdcolor,
					pcpath);
	if ( pis->ctm.xy == 0 && pis->ctm.yx == 0 )
	  { scale = fabs(pis->ctm.xx);
	    if ( fabs(pis->ctm.yy) != scale )
	      return gx_default_stroke_path(dev, pis, ppath, params, pdcolor,
					    pcpath);
	  }
	else if ( pis->ctm.xx == 0 && pis->ctm.yy == 0 )
	  { scale = fabs(pis->ctm.xy);
	    if ( fabs(pis->ctm.yx) != scale )
	      return gx_default_stroke_path(dev, pis, ppath, params, pdcolor,
					    pcpath);
	  }
	else if ( (pis->ctm.xx == pis->ctm.yy && pis->ctm.xy == -pis->ctm.yx)||
		  (pis->ctm.xx == -pis->ctm.yy && pis->ctm.xy == pis->ctm.yx)
		)
	  scale = sqrt(pis->ctm.xx * pis->ctm.xx + pis->ctm.xy * pis->ctm.xy);
	else
	  return gx_default_stroke_path(dev, pis, ppath, params, pdcolor,
					pcpath);
	scale /= pdev->scale.x;
	pdf_put_clip_path(pdev, pcpath);
	pdf_set_color(pdev, gx_dc_pure_color(pdcolor), &pdev->stroke_color, "RG");
	if ( (float)(pis->line_params.dash.offset * scale) != pdev->line_params.dash.offset ||
	     pattern_size != pdev->line_params.dash.pattern_size ||
	     pattern_size > max_dash ||
	     (pattern_size != 0 &&
	      !dash_pattern_eq(pdev->dash_pattern, &pis->line_params.dash,
			       scale))
	   )
	  { pputs(s, "[ ");
	    pdev->line_params.dash.pattern_size = pattern_size;
	    for ( i = 0; i < pattern_size; ++i )
	      { float element = pis->line_params.dash.pattern[i] * scale;
		if ( i < max_dash )
		  pdev->dash_pattern[i] = element;
		pprintg1(s, "%g ", element);
	      }
	    pdev->line_params.dash.offset =
	      pis->line_params.dash.offset * scale;
	    pprintg1(s, "] %g d\n", pdev->line_params.dash.offset);
	  }
	if ( params->flatness != pdev->flatness )
	  { pprintg1(s, "%g i\n", params->flatness);
	    pdev->flatness = params->flatness;
	  }
	if ( (float)(pis->line_params.half_width * scale) != pdev->line_params.half_width )
	  { pdev->line_params.half_width = pis->line_params.half_width * scale;
	    pprintg1(s, "%g w\n", pdev->line_params.half_width * 2);
	  }
	if ( pis->line_params.miter_limit != pdev->line_params.miter_limit )
	  { pprintg1(s, "%g M\n", pis->line_params.miter_limit);
	    gx_set_miter_limit(&pdev->line_params,
			       pis->line_params.miter_limit);
	  }
	if ( pis->line_params.cap != pdev->line_params.cap )
	  { pprintd1(s, "%d J\n", pis->line_params.cap);
	    pdev->line_params.cap = pis->line_params.cap;
	  }
	if ( pis->line_params.join != pdev->line_params.join )
	  { pprintd1(s, "%d j\n", pis->line_params.join);
	    pdev->line_params.join = pis->line_params.join;
	  }
	code = pdf_put_path(pdev, ppath, false, NULL);
	if ( code < 0 )
	  return code;
	pputs(s, (code ? "s\n" : "S\n"));
	return 0;
}
