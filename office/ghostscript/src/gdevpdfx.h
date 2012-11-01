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

/* Internal definitions for PDF-writing driver. */
#include "gsparam.h"
#include "gxdevice.h"
#include "gxline.h"
#include "stream.h"
#include "gdevpstr.h"
#include "gdevpsdf.h"

/* ---------------- Statically allocated sizes ---------------- */
/* These should all really be dynamic.... */

/* Define the maximum size of the output file name. */
#define fname_size 80

/* Define the maximum number of contents fragments on a page. */
#define max_contents_ids 300

/* Define the maximum depth of an outline tree. */
/* Note that there is no limit on the breadth of the tree. */
#define max_outline_depth 8

/* Define the maximum size of a destination array string. */
#define max_dest_string 80

/* ================ Types and structures ================ */

/* ---------------- Resources ---------------- */

typedef enum {
  resourceFont,
  resourceEncoding,
  resourceFontDescriptor,
  resourceColorSpace,
  resourceXObject,
  num_resource_types
} pdf_resource_type;
#define pdf_resource_type_names\
  "Font", "Encoding", "FontDescriptor", "ColorSpace", "XObject"
#define pdf_resource_type_structs\
  &st_pdf_font, &st_pdf_resource, &st_pdf_resource, &st_pdf_resource,\
  &st_pdf_resource

#define pdf_resource_common(typ)\
	typ *next;			/* next resource of this type */\
	pdf_resource *prev;		/* previously allocated resource */\
	gs_id rid;			/* optional key */\
	long id
typedef struct pdf_resource_s pdf_resource;
struct pdf_resource_s {
	pdf_resource_common(pdf_resource);
};
#define private_st_pdf_resource()\
  gs_private_st_ptrs2(st_pdf_resource, pdf_resource, "pdf_resource",\
    pdf_resource_enum_ptrs, pdf_resource_reloc_ptrs, next, prev)

/* Font resources */
typedef struct pdf_font_s pdf_font;
struct pdf_font_s {
	pdf_resource_common(pdf_font);
	gs_const_string fname;
};
/****** Doesn't handle the string ******/
#define private_st_pdf_font()\
  gs_private_st_suffix_add0(st_pdf_font, pdf_font, "pdf_font",\
    pdf_font_enum_ptrs, pdf_font_reloc_ptrs, st_pdf_resource)

/* ---------------- Other auxiliary structures ---------------- */

/* Outline nodes and levels */
typedef struct pdf_outline_node_s {
  long id, parent_id, prev_id, first_id, last_id;
  int count;
  gs_string action_string;
} pdf_outline_node;
typedef struct pdf_outline_level_s {
  pdf_outline_node first;
  pdf_outline_node last;
  int left;
} pdf_outline_level;

/* Articles */
typedef struct pdf_bead_s {
  long id, article_id, prev_id, next_id;
  char dest[max_dest_string];
  gs_rect rect;
} pdf_bead;
typedef struct pdf_article_s pdf_article;
struct pdf_article_s {
  pdf_article *next;
  gs_string title;
  long id;
  pdf_bead first;
  pdf_bead last;
};
/****** Doesn't handle the strings ******/
#define private_st_pdf_article()\
  gs_private_st_ptrs1(st_pdf_article, pdf_article, "pdf_article",\
    pdf_article_enum_ptrs, pdf_article_reloc_ptrs, next)

/* Named destinations */
typedef struct pdf_named_dest_s pdf_named_dest;
struct pdf_named_dest_s {
  pdf_named_dest *next;
  gs_string key;
  char dest[max_dest_string];
};
/****** Doesn't handle the string ******/
#define private_st_pdf_named_dest()\
  gs_private_st_ptrs1(st_pdf_named_dest, pdf_named_dest, "pdf_named_dest",\
    pdf_named_dest_enum_ptrs, pdf_named_dest_reloc_ptrs, next)

/* ---------------- The device structure ---------------- */

/* Text state */
typedef struct pdf_text_state_s {
	float character_spacing;
	pdf_font *font;
	float size;
	float word_spacing;
	float horizontal_scaling;
} pdf_text_state;
#define pdf_text_state_default\
  0, NULL, 0, 0, 100

/* Define the device structure. */
typedef enum {
  NoMarks = 0,
  ImageB = 1,
  ImageC = 2,
  ImageI = 4,
  Text = 8
} pdf_procset;
typedef enum {
  pdf_in_none,
  pdf_in_stream,
  pdf_in_text
} pdf_context;
typedef struct gx_device_pdf_s {
	gx_device_psdf_common;
	  /* PDF-specific distiller parameters */
	float CompatibilityLevel;
	bool DoThumbnails;		/* ****** OBSOLETE ****** */
	  /* End of distiller parameters */
	  /* Other parameters */
	long FirstObjectNumber;
	  /* End of parameters */
	bool binary_ok;			/* if true, OK to output binary info */
		/* End of settable parameters. */
		/* Following are set when device is opened. */
#define pdf_memory v_memory
	char tfname[fname_size + 1];
	FILE *tfile;
		/* ................ */
	long next_id;
		/* The following 2 IDs, and only these, are allocated */
		/* when the file is opened. */
	long root_id;
	long info_id;
#define pdf_num_initial_ids 2
	long pages_id;
	long outlines_id;
	int next_page;
	long contents_ids[max_contents_ids];
	int next_contents_id;
	pdf_context context;
	long contents_length_id;
	long contents_pos;
	pdf_procset procsets;		/* used on this page */
	float flatness;		/****** SHOULD USE state ******/
	/* The line width, dash offset, and dash pattern */
	/* are in default user space units. */
	gx_line_params line_params;	/* current values */
				/****** SHOULD USE state ******/
#define initial_num_page_ids 50
	long *page_ids;
	int num_page_ids;
	int pages_referenced;
	pdf_resource *resources[num_resource_types];
	pdf_resource *annots;		/* rid = page # */
	pdf_resource *last_resource;
	gs_string catalog_string;
	gs_string pages_string;
	gs_string page_string;
	pdf_outline_level outline_levels[max_outline_depth];
	int outline_depth;
	int closed_outline_depth;
	int outlines_open;
	pdf_article *articles;
	pdf_named_dest *named_dests;
	pdf_text_state text_state;
} gx_device_pdf;
#define is_in_document(pdev)\
  ((pdev)->next_contents_id != 0 || (pdev)->last_resource != 0)
#define is_in_page(pdev)\
  ((pdev)->next_contents_id != 0)

/* ================ Utility procedures ================ */

/* ---------------- Exported by gdevpdf.c ---------------- */

/* ------ Document ------ */

/* Initialize the IDs allocated at startup. */
void pdf_initialize_ids(P1(gx_device_pdf *pdev));

/* Open the document if necessary. */
void pdf_open_document(P1(gx_device_pdf *pdev));

/* ------ Objects ------ */

/* Allocate an ID for a future object. */
long pdf_obj_ref(P1(gx_device_pdf *pdev));

/* Begin an object, optionally allocating an ID. */
long pdf_open_obj(P2(gx_device_pdf *pdev, long id));

/* Begin an object, allocating an ID. */
#define pdf_begin_obj(pdev) pdf_open_obj(pdev, 0)

/* End an object. */
int pdf_end_obj(P1(gx_device_pdf *pdev));

/* ------ Graphics ------ */

/* Set the fill or stroke color. */
int pdf_set_color(P4(gx_device_pdf *pdev, gx_color_index color,
		     gx_drawing_color *pdcolor, const char *rgs));

/* Set the scale for coordinates according to the current resolution. */
void pdf_set_scale(P1(gx_device_pdf *pdev));

/* ------ Page contents ------ */

/* Open a page contents part. */
/* Return an error if the page has too many contents parts. */
int pdf_open_contents(P2(gx_device_pdf *pdev, pdf_context context));

/* Close the current contents part if we are in one. */
int pdf_close_contents(P2(gx_device_pdf *pdev, bool last));

/* ------ Resources et al ------ */

/* Begin an aside (resource, annotation, ...). */
int pdf_begin_aside(P4(gx_device_pdf *pdev, pdf_resource **plist,
		       const gs_memory_struct_type_t *pst,
		       pdf_resource **ppres));

/* Begin a resource of a given type. */
int pdf_begin_resource(P3(gx_device_pdf *pdev, pdf_resource_type type,
			  pdf_resource **ppres));

/* Find a resource of a given type by gs_id. */
pdf_resource *pdf_find_resource_by_gs_id(P3(gx_device_pdf *pdev,
					    pdf_resource_type type,
					    gs_id rid));

/* End an aside. */
int pdf_end_aside(P1(gx_device_pdf *pdev));

/* End a resource. */
int pdf_end_resource(P1(gx_device_pdf *pdev));

/* ------ Pages ------ */

/* Reset the state of the current page. */
void pdf_reset_page(P1(gx_device_pdf *pdev));

/* Get or assign the ID for a page. */
/* Returns 0 if the page number is out of range. */
long pdf_page_id(P2(gx_device_pdf *pdev, int page_num));

/* Open a page for writing. */
int pdf_open_page(P2(gx_device_pdf *pdev, pdf_context context));

/* Write saved page- or document-level information. */
int pdf_write_saved_string(P2(gx_device_pdf *pdev, gs_string *pstr));

/* Write the default entries of the Info dictionary. */
int pdf_write_default_info(P1(gx_device_pdf *pdev));

/* ------ Path drawing ------ */

int pdf_put_clip_path(P2(gx_device_pdf *pdev, const gx_clip_path *pcpath));

/* ---------------- Exported by gdevpdfm.c ---------------- */

/* Compare a C string and a gs_param_string. */
bool pdf_key_eq(P2(const gs_param_string *pcs, const char *str));

/* Process a pdfmark (called from pdf_put_params). */
int pdfmark_process(P2(gx_device_pdf *pdev, const gs_param_string_array *pma));

/* Close the current level of the outline tree. */
int pdfmark_close_outline(P1(gx_device_pdf *pdev));

/* Write an article bead. */
int pdfmark_write_article(P2(gx_device_pdf *pdev, const pdf_bead *pbead));

/* ---------------- Exported by gdevpdft.c ---------------- */

/* Process a show operation (called from pdf_put_params). */
int pdfshow_process(P2(gx_device_pdf *pdev, const gs_param_dict *ptd));
