/* Copyright (C) 1991, 1996, 1997 Aladdin Enterprises.  All rights reserved.
  
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

/* gxclist.c */
/* Command list writing for Ghostscript. */
#include "memory_.h"
#include "gx.h"
#include "gp.h"
#include "gpcheck.h"
#include "gserrors.h"
#include "gxdevice.h"
#include "gxdevmem.h"			/* must precede gxcldev.h */
#include "gxcldev.h"

#define cdev cwdev

/* Forward declarations of procedures */
private dev_proc_open_device(clist_open);
private dev_proc_output_page(clist_output_page);
private dev_proc_open_device(clist_close);
private dev_proc_get_band(clist_get_band);
/* In gxclrect.c */
extern dev_proc_fill_rectangle(clist_fill_rectangle);
extern dev_proc_copy_mono(clist_copy_mono);
extern dev_proc_copy_color(clist_copy_color);
extern dev_proc_copy_alpha(clist_copy_alpha);
extern dev_proc_strip_tile_rectangle(clist_strip_tile_rectangle);
extern dev_proc_strip_copy_rop(clist_strip_copy_rop);
/* In gxclread.c */
extern dev_proc_get_bits(clist_get_bits);

/* The device procedures */
gx_device_procs gs_clist_device_procs =
{	clist_open,
	gx_forward_get_initial_matrix,
	gx_default_sync_output,
	clist_output_page,
	clist_close,
	gx_forward_map_rgb_color,
	gx_forward_map_color_rgb,
	clist_fill_rectangle,
	gx_default_tile_rectangle,
	clist_copy_mono,
	clist_copy_color,
	gx_default_draw_line,
	clist_get_bits,
	gx_forward_get_params,
	gx_forward_put_params,
	gx_forward_map_cmyk_color,
	gx_forward_get_xfont_procs,
	gx_forward_get_xfont_device,
	gx_forward_map_rgb_alpha_color,
	gx_forward_get_page_device,
	gx_forward_get_alpha_bits,
	clist_copy_alpha,
	clist_get_band,
	gx_default_copy_rop,
	gx_default_fill_path,
	gx_default_stroke_path,
	gx_default_fill_mask,
	gx_default_fill_trapezoid,
	gx_default_fill_parallelogram,
	gx_default_fill_triangle,
	gx_default_draw_thin_line,
	gx_default_begin_image,
	gx_default_image_data,
	gx_default_end_image,
	clist_strip_tile_rectangle,
	clist_strip_copy_rop,
	gx_forward_get_clipping_box
};

/* ------ Define the command set and syntax ------ */

/* Define the clipping enable/disable opcodes. */
/* The path extensions initialize these to their proper values. */
byte cmd_opvar_disable_clip = 0xff;
byte cmd_opvar_enable_clip = 0xff;

#ifdef DEBUG
const char *cmd_op_names[16] = { cmd_op_name_strings };
private const char *cmd_misc_op_names[16] = { cmd_misc_op_name_strings };
const char **cmd_sub_op_names[16] =
{	cmd_misc_op_names, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};
private ulong far_data cmd_op_counts[256];
private ulong far_data cmd_op_sizes[256];
private ulong cmd_tile_reset, cmd_tile_found, cmd_tile_added;
extern ulong cmd_diffs[5];		/* in gxclpath.c */
private ulong cmd_same_band, cmd_other_band;
int
cmd_count_op(int op, uint size)
{	cmd_op_counts[op]++;
	cmd_op_sizes[op] += size;
	if ( gs_debug_c('L') )
	  { const char **sub = cmd_sub_op_names[op >> 4];
	    if ( sub )
	      dprintf2(", %s(%u)\n", sub[op & 0xf], size);
	    else
	      dprintf3(", %s %d(%u)\n", cmd_op_names[op >> 4], op & 0xf, size);
	    fflush(dstderr);
	  }
	return op;
}
void
cmd_uncount_op(int op, uint size)
{	cmd_op_counts[op]--;
	cmd_op_sizes[op] -= size;
}
#endif

/* Initialization for imager state. */
/* The initial scale is arbitrary. */
const gs_imager_state clist_imager_state_initial =
 { gs_imager_state_initial(300.0 / 72.0) };

/*
 * The buffer area (data, data_size) holds a bitmap cache when both writing
 * and reading.  The rest of the space is used for the command buffer and
 * band state bookkeeping when writing, and for the rendering buffer (image
 * device) when reading.  For the moment, we divide the space up
 * arbitrarily, except that we allocate less space for the bitmap cache if
 * the device doesn't need halftoning.
 *
 * All the routines for allocating tables in the buffer are idempotent, so
 * they can be used to check whether a given-size buffer is large enough.
 */

/*
 * Calculate the desired size for the tile cache.
 */
private uint
clist_tile_cache_size(const gx_device *target, uint data_size)
{	uint bits_size =
	  (data_size / 5) & -align_cached_bits_mod;  /* arbitrary */

	if ( (gx_device_has_color(target) ? target->color_info.max_color :
	      target->color_info.max_gray) >= 31
	   )
	  { /* No halftones -- cache holds only Patterns & characters. */
	    bits_size -= bits_size >> 2;
	  }
#define min_bits_size 1024
	  if ( bits_size < min_bits_size )
	    bits_size = min_bits_size;
#undef min_bits_size
	return bits_size;
}

/*
 * Initialize the allocation for the tile cache.  Sets: tile_hash_mask,
 * tile_max_count, tile_table, chunk (structure), bits (structure).
 */
private int
clist_init_tile_cache(gx_device *dev, byte *init_data, ulong data_size)
{	byte *data = init_data;
	uint bits_size = data_size;
	/*
	 * Partition the bits area between the hash table and the actual
	 * bitmaps.  The per-bitmap overhead is about 24 bytes; if the
	 * average character size is 10 points, its bitmap takes about 24 +
	 * 0.5 * 10/72 * xdpi * 10/72 * ydpi / 8 bytes (the 0.5 being a
	 * fudge factor to account for characters being narrower than they
	 * are tall), which gives us a guideline for the size of the hash
	 * table.
	 */
	uint avg_char_size =
	  (uint)(dev->x_pixels_per_inch * dev->y_pixels_per_inch *
		 (0.5 * 10/72 * 10/72 / 8)) + 24;
	uint hc = bits_size / avg_char_size;
	uint hsize;

	while ( (hc + 1) & hc )
	  hc |= hc >> 1;	/* make mask (power of 2 - 1) */
	if ( hc < 0xff )
	  hc = 0xff;		/* make allowance for halftone tiles */
	else if ( hc > 0xfff )
	  hc = 0xfff;		/* cmd_op_set_tile_index has 12-bit operand */
	/* Make sure the tables will fit. */
	while ( hc >= 3 && (hsize = (hc + 1) * sizeof(tile_hash)) >= bits_size )
	  hc >>= 1;
	if ( hc < 3 )
	  return_error(gs_error_rangecheck);
	cdev->tile_hash_mask = hc;
	cdev->tile_max_count = hc - (hc >> 2);
	cdev->tile_table = (tile_hash *)data;
	data += hsize;
	bits_size -= hsize;
	gx_bits_cache_chunk_init(&cdev->chunk, data, bits_size);
	gx_bits_cache_init(&cdev->bits, &cdev->chunk);
	return 0;
}

/*
 * Initialize the allocation for the bands.  Requires: target.  Sets:
 * page_band_height (=page_info.band_params.BandHeight), nbands.
 */
private int
clist_init_bands(gx_device *dev, uint data_size, int band_width,
  int band_height)
{	gx_device *target = cdev->target;
	int nbands;

	if ( gdev_mem_data_size((gx_device_memory *)target, band_width,
				band_height) > data_size
	   )
	  return_error(gs_error_rangecheck);
	cdev->page_band_height = band_height;
	nbands = (target->height + band_height - 1) / band_height;
	cdev->nbands = nbands;
#ifdef DEBUG
	if ( gs_debug_c('l') | gs_debug_c(':') )
	  dprintf4("[l]width=%d, band_width=%d, band_height=%d, nbands=%d\n",
		   target->width, band_width, band_height, nbands);
#endif
	return 0;
}

/*
 * Initialize the allocation for the band states, which are used only
 * when writing.  Requires: nbands.  Sets: states, cbuf, cend.
 */
private int
clist_init_states(gx_device *dev, byte *init_data, uint data_size)
{	ulong state_size = cdev->nbands * (ulong)sizeof(gx_clist_state);

	/*
	 * The +100 in the next line is bogus, but we don't know what the
	 * real check should be.
	 */
	if ( state_size + sizeof(cmd_prefix) + cmd_largest_size + 100 > data_size )
	  return_error(gs_error_rangecheck);
	cdev->states = (gx_clist_state *)init_data;
	cdev->cbuf = init_data + state_size;
	cdev->cend = init_data + data_size;
	return 0;
}

/*
 * Initialize all the data allocations.  Requires: target.  Sets:
 * page_tile_cache_size, page_info.band_params.BandWidth,
 * page_info.band_params.BandBufferSpace, + see above.
 */
private int
clist_init_data(gx_device *dev, byte *init_data, uint data_size)
{	gx_device *target = cdev->target;
	const int band_width =
	  cdev->page_info.band_params.BandWidth =
	    (cdev->band_params.BandWidth ? cdev->band_params.BandWidth :
	     target->width);
	int band_height = cdev->band_params.BandHeight;
	const uint band_space =
	  cdev->page_info.band_params.BandBufferSpace =
	    (cdev->band_params.BandBufferSpace ?
	     cdev->band_params.BandBufferSpace : data_size);
	byte *data = init_data;
	uint size = band_space;
	uint bits_size;
	int code;
	  
	if ( band_height )
	  { /*
	     * The band height is fixed, so the band buffer requirement
	     * is completely determined.
	     */
	    uint band_data_size =
	      gdev_mem_data_size((gx_device_memory *)target,
				 band_width, band_height);

	    if ( band_data_size >= band_space )
	      return_error(gs_error_rangecheck);
	    bits_size = min(band_space - band_data_size, data_size >> 1);
	  }
	else
	  { /*
	     * Choose the largest band height that will fit in the
	     * rendering-time buffer.
	     */
	    bits_size = clist_tile_cache_size(target, band_space);
	    bits_size = min(bits_size, data_size >> 1);
	    band_height = gdev_mem_max_height((gx_device_memory *)target,
					      band_width,
					      band_space - bits_size);
	    if ( band_height == 0 )
	      return_error(gs_error_rangecheck);
	  }
	code = clist_init_tile_cache(dev, data, bits_size);
	if ( code < 0 )
	  return code;
	cdev->page_tile_cache_size = bits_size;
	data += bits_size;
	size -= bits_size;
	code = clist_init_bands(dev, size, band_width, band_height);
	if ( code < 0 )
	  return code;
	return clist_init_states(dev, data, data_size - bits_size);
}
/*
 * Initialize the device state (for writing).  This routine requires only
 * data, data_size, and target to be set, and is idempotent.
 */
private int
clist_init(gx_device *dev)
{	int code = clist_init_data(dev, cdev->data, cdev->data_size);
	int nbands = cdev->nbands;

	if ( code < 0 )
	  return code;
	/* Now initialize the rest of the state. */
	cdev->ymin = cdev->ymax = -1;	/* render_init not done yet */
	memset(cdev->tile_table, 0, (cdev->tile_hash_mask + 1) *
	       sizeof(*cdev->tile_table));
	cdev->cnext = cdev->cbuf;
	cdev->ccl = 0;
	cdev->band_range_list.head = cdev->band_range_list.tail = 0;
	cdev->band_range_min = 0;
	cdev->band_range_max = nbands - 1;
	{ int band;
	  gx_clist_state *states = cdev->states;

	  for ( band = 0; band < nbands; band++, states++ )
	    { static const gx_clist_state cls_initial =
	        { cls_initial_values };
	      *states = cls_initial;
	    }
	}
	/*
	 * Round up the size of the per-tile band mask so that the bits,
	 * which follow it, stay aligned.
	 */
	cdev->tile_band_mask_size =
	  ((nbands + (align_bitmap_mod * 8 - 1)) >> 3) &
	    ~(align_bitmap_mod - 1);
	/*
	 * Initialize the all-band parameters to impossible values,
	 * to force them to be written the first time they are used.
	 */
	memset(&cdev->tile_params, 0, sizeof(cdev->tile_params));
	cdev->tile_depth = 0;
	cdev->tile_known_min = nbands;
	cdev->tile_known_max = -1;
	cdev->imager_state = clist_imager_state_initial;
	cdev->clip_path = NULL;
	cdev->clip_path_id = gs_no_id;
	cdev->color_space = 0;
	{ int i;
	  for ( i = 0; i < countof(cdev->transfer_ids); ++i )
	    cdev->transfer_ids[i] = gs_no_id;
	}
	cdev->black_generation_id = gs_no_id;
	cdev->undercolor_removal_id = gs_no_id;
	cdev->device_halftone_id = gs_no_id;
	cdev->in_image = false;
	return 0;
}
/* Open the device by initializing the device state and opening the */
/* scratch files. */
private int
clist_open(gx_device *dev)
{	char fmode[4];
	int code;

	cdev->page_cfile = 0;	/* in case of failure */
	cdev->page_bfile = 0;	/* ditto */
	code = clist_init(dev);
	if ( code < 0 )
	  return code;
	strcpy(fmode, "w+");
	strcat(fmode, gp_fmode_binary_suffix);
	cdev->page_cfname[0] = 0;	/* create a new file */
	cdev->page_bfname[0] = 0;	/* ditto */
	cdev->page_bfile_end_pos = 0;
	if ( (code = clist_fopen(cdev->page_cfname, fmode, &cdev->page_cfile,
				 &gs_memory_default, true)) < 0 ||
	     (code = clist_fopen(cdev->page_bfname, fmode, &cdev->page_bfile,
				 &gs_memory_default, true)) < 0
	   )
	  clist_close(dev);
	return code;
}

/* The output_page procedure should never be called! */
private int
clist_output_page(gx_device *dev, int num_copies, int flush)
{	return_error(gs_error_Fatal);
}

/* Reset (or prepare to append to) the command list after printing a page. */
int
clist_finish_page(gx_device *dev, bool flush)
{	if ( flush )
	  { clist_rewind(cdev->page_cfile, true, cdev->page_cfname);
	    clist_rewind(cdev->page_bfile, true, cdev->page_bfname);
	    cdev->page_bfile_end_pos = 0;
	  }
	else
	  { clist_fseek(cdev->page_cfile, 0L, SEEK_END, cdev->page_cfname);
	    clist_fseek(cdev->page_bfile, 0L, SEEK_END, cdev->page_bfname);
	  }
	return clist_init(dev);		/* reinitialize */
}

/* Close the device by freeing the temporary files. */
/* Note that this does not deallocate the buffer. */
private int
clist_close(gx_device *dev)
{	if ( cdev->page_cfile != NULL )
	  {	clist_fclose(cdev->page_cfile, cdev->page_cfname, true);
		cdev->page_cfile = NULL;
	  }
	if ( cdev->page_bfile != NULL )
	  {	clist_fclose(cdev->page_bfile, cdev->page_bfname, true);
		cdev->page_bfile = NULL;
	  }
	return 0;
}

/* Print statistics. */
#ifdef DEBUG
void
cmd_print_stats(void)
{	int ci, cj;
	dprintf3("[l]counts: reset = %lu, found = %lu, added = %lu\n",
	         cmd_tile_reset, cmd_tile_found, cmd_tile_added);
	dprintf5("     diff 2.5 = %lu, 3 = %lu, 4 = %lu, 2 = %lu, >4 = %lu\n",
		 cmd_diffs[0], cmd_diffs[1], cmd_diffs[2], cmd_diffs[3],
		 cmd_diffs[4]);
	dprintf2("     same_band = %lu, other_band = %lu\n",
		 cmd_same_band, cmd_other_band);
	for ( ci = 0; ci < 0x100; ci += 0x10 )
	   {	const char **sub = cmd_sub_op_names[ci >> 4];
		if ( sub != 0 )
		  { dprintf1("[l]  %s =", cmd_op_names[ci >> 4]);
		    for ( cj = ci; cj < ci + 0x10; cj += 2 )
		      dprintf6("\n\t%s = %lu(%lu), %s = %lu(%lu)",
			       sub[cj-ci],
			       cmd_op_counts[cj], cmd_op_sizes[cj],
			       sub[cj-ci+1],
			       cmd_op_counts[cj+1], cmd_op_sizes[cj+1]);
		  }
		else
		  { ulong tcounts = 0, tsizes = 0;
		    for ( cj = ci; cj < ci + 0x10; cj++ )
		      tcounts += cmd_op_counts[cj],
		      tsizes += cmd_op_sizes[cj];
		    dprintf3("[l]  %s (%lu,%lu) =\n\t",
			     cmd_op_names[ci >> 4], tcounts, tsizes);
		    for ( cj = ci; cj < ci + 0x10; cj++ )
		      if ( cmd_op_counts[cj] == 0 )
			dputs(" -");
		      else
			dprintf2(" %lu(%lu)", cmd_op_counts[cj],
				 cmd_op_sizes[cj]);
		  }
		dputs("\n");
	   }
}
#endif				/* DEBUG */

/* ------ Writing ------ */

/* Utilities */

/* Write the commands for one band or band range. */
private int
cmd_write_band(gx_device_clist_writer *cldev, int band_min, int band_max,
  cmd_list *pcl, byte cmd_end)
{	const cmd_prefix *cp = pcl->head;

	if ( cp != 0 || cmd_end != cmd_opv_end_run ) {
	  clist_file_ptr cfile = cldev->page_cfile;
	  clist_file_ptr bfile = cldev->page_bfile;
	  cmd_block cb;
	  byte end = cmd_count_op(cmd_end, 1);
	  int code;

	  cb.band_min = band_min;
	  cb.band_max = band_max;
	  cb.pos = clist_ftell(cfile);
	  if_debug3('l', "[l]writing for bands (%d,%d) at %ld\n",
		    band_min, band_max, cb.pos);
	  clist_fwrite_chars(&cb, sizeof(cb), bfile);
	  if ( cp != 0 ) {
	    pcl->tail->next = 0;	/* terminate the list */
	    for ( ; cp != 0; cp = cp->next ) {
#ifdef DEBUG
	      if ( (const byte *)cp < cldev->cbuf ||
		   (const byte *)cp >= cldev->cend ||
		   cp->size > cldev->cend - (const byte *)cp
		 ) {
		lprintf1("cmd_write_band error at 0x%lx\n", (ulong)cp);
		return_error(gs_error_Fatal);
	      }
#endif
	      clist_fwrite_chars(cp + 1, cp->size, cfile);
	    }
	    pcl->head = pcl->tail = 0;
	  }

	  clist_fwrite_chars(&end, 1, cfile);
	  process_interrupts();
	  if ( (code = clist_ferror_code(bfile)) < 0 ||
	       (code = clist_ferror_code(cfile)) < 0
	     )
	    return_error(code);
	}
	return 0;
}

/* Write out the buffered commands, and reset the buffer. */
private int
cmd_write_buffer(gx_device_clist_writer *cldev, byte cmd_end)
{	int nbands = cldev->nbands;
	gx_clist_state *pcls;
	int band;
	int code = cmd_write_band(cldev, cldev->band_range_min,
				  cldev->band_range_max,
				  &cldev->band_range_list, cmd_opv_end_run);

	for ( band = 0, pcls = cldev->states;
	      code >= 0 && band < nbands; band++, pcls++
	    )
	  code = cmd_write_band(cldev, band, band, &pcls->list, cmd_end);
	cldev->cnext = cldev->cbuf;
	cldev->ccl = 0;
	cldev->band_range_list.head = cldev->band_range_list.tail = 0;
#ifdef DEBUG
	if ( gs_debug_c('l') )
	  cmd_print_stats();
#endif
	return_check_interrupt(code);
}
/* End a page by flushing the buffer and terminating the command list. */
int
clist_end_page(gx_device_clist_writer *cldev)
{	int code = cmd_write_buffer(cldev, cmd_opv_end_page);
	cmd_block cb;

	if ( code < 0 )
	  return code;
	/* Write the terminating entry in the block file. */
	/* Note that because of copypage, there may be many such entries. */
	cb.band_min = cb.band_max = cmd_band_end;
	cb.pos = clist_ftell(cldev->page_cfile);
	clist_fwrite_chars(&cb, sizeof(cb), cldev->page_bfile);
	cldev->page_bfile_end_pos = clist_ftell(cldev->page_bfile);
#ifdef DEBUG
	if ( gs_debug_c('l') | gs_debug_c(':') )
	  dprintf2("[l]clist_render_init at cfile=%ld, bfile=%ld\n",
		   cb.pos, cldev->page_bfile_end_pos);
#endif
	return 0;
}

/* Add a command to the appropriate band list, */
/* and allocate space for its data. */
/* Return the pointer to the data area. */
/* If an error occurs, set cldev->error_code and return 0. */
#define cmd_headroom (sizeof(cmd_prefix) + arch_align_ptr_mod)
byte *
cmd_put_list_op(gx_device_clist_writer *cldev, cmd_list *pcl, uint size)
{	byte *dp = cldev->cnext;
	if ( size + cmd_headroom > cldev->cend - dp )
	  { int code = cldev->error_code =
	      cmd_write_buffer(cldev, cmd_opv_end_run);
	    if ( code < 0 )
	      return 0;
	    return cmd_put_list_op(cldev, pcl, size);
	  }
	if ( cldev->ccl == pcl )
	  { /* We're adding another command for the same band. */
	    /* Tack it onto the end of the previous one. */
	    cmd_count_add1(cmd_same_band);
#ifdef DEBUG
	    if ( pcl->tail->size > dp - (byte *)(pcl->tail + 1) )
	      { lprintf1("cmd_put_list_op error at 0x%lx\n", (ulong)pcl->tail);
	      }
#endif
	    pcl->tail->size += size;
	  }
	else
	  { /* Skip to an appropriate alignment boundary. */
	    /* (We assume the command buffer itself is aligned.) */
	    cmd_prefix *cp =
	      (cmd_prefix *)(dp +
			     ((cldev->cbuf - dp) & (arch_align_ptr_mod - 1)));
	    cmd_count_add1(cmd_other_band);
	    dp = (byte *)(cp + 1);
	    if ( pcl->tail != 0 )
	      {
#ifdef DEBUG
		if ( pcl->tail < pcl->head ||
		     pcl->tail->size > dp - (byte *)(pcl->tail + 1)
		   )
		  { lprintf1("cmd_put_list_op error at 0x%lx\n",
			     (ulong)pcl->tail);
		  }
#endif
		pcl->tail->next = cp;
	      }
	    else
	      pcl->head = cp;
	    pcl->tail = cp;
	    cldev->ccl = pcl;
	    cp->size = size;
	  }
	cldev->cnext = dp + size;
	return dp;
}
#ifdef DEBUG
byte *
cmd_put_op(gx_device_clist_writer *cldev, gx_clist_state *pcls, uint size)
{	if_debug3('L', "[L]band %d: size=%u, left=%u",
		  (int)(pcls - cldev->states),
		  size, (uint)(cldev->cend - cldev->cnext));
	return cmd_put_list_op(cldev, &pcls->list, size);
}
#endif

/* Add a command for a range of bands. */
byte *
cmd_put_range_op(gx_device_clist_writer *cldev, int band_min, int band_max,
  uint size)
{	cmd_prefix *tail;
	if_debug4('L', "[L]band range(%d,%d): size=%u, left=%u",
		  band_min, band_max, size,
		  (uint)(cldev->cend - cldev->cnext));
	if ( cldev->band_range_list.head == 0 ||
	     band_min != cldev->band_range_min ||
	     band_max != cldev->band_range_max ||
	     (tail = cldev->band_range_list.tail,
	      cldev->cnext != (byte *)(tail + 1) + tail->size)
	   )
	  { if ( (cldev->error_code = cmd_write_buffer(cldev, cmd_opv_end_run)) < 0 )
	      return 0;
	    cldev->band_range_min = band_min;
	    cldev->band_range_max = band_max;
	  }
	return cmd_put_list_op(cldev, &cldev->band_range_list, size);
}

/* Write a variable-size positive integer. */
int
cmd_size_w(register uint w)
{	register int size = 1;
	while ( w > 0x7f ) w >>= 7, size++;
	return size;
}
byte *
cmd_put_w(register uint w, register byte *dp)
{	while ( w > 0x7f ) *dp++ = w | 0x80, w >>= 7;
	*dp = w;
	return dp + 1;
}

/* Define the encodings of the different settable colors. */
const clist_select_color_t
  clist_select_color0 = {cmd_op_set_color0, cmd_opv_delta2_color0, 0},
  clist_select_color1 = {cmd_op_set_color1, cmd_opv_delta2_color1, 0},
  clist_select_tile_color0 = {cmd_op_set_color0, cmd_opv_delta2_color0, 1},
  clist_select_tile_color1 = {cmd_op_set_color1, cmd_opv_delta2_color1, 1};
int
cmd_put_color(gx_device_clist_writer *cldev, gx_clist_state *pcls,
  const clist_select_color_t *select,
  gx_color_index color, gx_color_index *pcolor)
{	byte *dp;
	long diff = (long)color - (long)(*pcolor);
	byte op, op_delta2;

	if ( diff == 0 )
	  return 0;
	if ( select->tile_color )
	  set_cmd_put_op(dp, cldev, pcls, cmd_opv_set_tile_color, 1);
	op = select->set_op;
	op_delta2 = select->delta2_op;
	if ( color == gx_no_color_index )
	  {	/*
		 * We must handle this specially, because it may take more
		 * bytes than the color depth.
		 */
		set_cmd_put_op(dp, cldev, pcls, op + 15, 1);
	  }
	else
	   {	long delta;
		byte operand;

		switch ( (cldev->color_info.depth + 15) >> 3 )
		  {
		  case 5:
			if ( !((delta = diff + cmd_delta1_32_bias) &
			      ~cmd_delta1_32_mask) &&
			     (operand =
			      (byte)((delta >> 23) + ((delta >> 18) & 1))) != 0 &&
			     operand != 15
			   )
			  { set_cmd_put_op(dp, cldev, pcls,
					   (byte)(op + operand), 2);
			    dp[1] = (byte)(((delta >> 10) & 0300) +
					   (delta >> 5) + delta);
			    break;
			  }
		  	if ( !((delta = diff + cmd_delta2_32_bias) &
			       ~cmd_delta2_32_mask)
			   )
			  { set_cmd_put_op(dp, cldev, pcls, op_delta2, 3);
			    dp[1] = (byte)((delta >> 20) + (delta >> 16));
			    dp[2] = (byte)((delta >> 4) + delta);
			    break;
			  }
			set_cmd_put_op(dp, cldev, pcls, op, 5);
			*++dp = (byte)(color >> 24);
			goto b3;
		  case 4:
			if ( !((delta = diff + cmd_delta1_24_bias) &
			       ~cmd_delta1_24_mask) &&
			     (operand = (byte)(delta >> 16)) != 0 &&
			     operand != 15
			   )
			  { set_cmd_put_op(dp, cldev, pcls,
					   (byte)(op + operand), 2);
			    dp[1] = (byte)((delta >> 4) + delta);
			    break;
			  }
		  	if ( !((delta = diff + cmd_delta2_24_bias) &
			       ~cmd_delta2_24_mask)
			   )
			  { set_cmd_put_op(dp, cldev, pcls, op_delta2, 3);
			    dp[1] = ((byte)(delta >> 13) & 0xf8) +
			      ((byte)(delta >> 11) & 7);
			    dp[2] = (byte)(((delta >> 3) & 0xe0) + delta);
			    break;
			  }
			set_cmd_put_op(dp, cldev, pcls, op, 4);
b3:			*++dp = (byte)(color >> 16);
			goto b2;
		  case 3:
			set_cmd_put_op(dp, cldev, pcls, op, 3);
b2:			*++dp = (byte)(color >> 8);
			goto b1;
		  case 2:
			if ( diff >= -7 && diff < 7 )
			  { set_cmd_put_op(dp, cldev, pcls,
					   op + (int)diff + 8, 1);
			    break;
			  }
			set_cmd_put_op(dp, cldev, pcls, op, 2);
b1:			dp[1] = (byte)color;
		  }
	   }
	*pcolor = color;
	return 0;
}

/* Put out a command to set the tile colors. */
int
cmd_set_tile_colors(gx_device_clist_writer *cldev, gx_clist_state *pcls,
  gx_color_index color0, gx_color_index color1)
{	if ( color0 != pcls->tile_colors[0] )
	   {	int code = cmd_put_color(cldev, pcls,
					 &clist_select_tile_color0,
					 color0, &pcls->tile_colors[0]);
		if ( code < 0 )
		  return code;
	   }
	if ( color1 != pcls->tile_colors[1] )
	   {	int code = cmd_put_color(cldev, pcls,
					 &clist_select_tile_color1,
					 color1, &pcls->tile_colors[1]);
		if ( code < 0 )
		  return code;
	   }
	return 0;
}

/* Put out a command to set the tile phase. */
int
cmd_set_tile_phase(gx_device_clist_writer *cldev, gx_clist_state *pcls,
  int px, int py)
{	int pcsize;
	byte *dp;

	pcls->tile_phase.x = px;
	pcls->tile_phase.y = py;
	pcsize = 1 + cmd_sizexy(pcls->tile_phase);
	set_cmd_put_op(dp, cldev, pcls, (byte)cmd_opv_set_tile_phase, pcsize);
	++dp;
	cmd_putxy(pcls->tile_phase, dp);
	return 0;
}

/* Write a command to enable or disable the logical operation. */
int
cmd_put_enable_lop(gx_device_clist_writer *cldev, gx_clist_state *pcls,
  int enable)
{	byte *dp;
	set_cmd_put_op(dp, cldev, pcls,
		       (byte)(enable ? cmd_opv_enable_lop :
			      cmd_opv_disable_lop),
		       1);
	pcls->lop_enabled = enable;
	return 0;
}

/* Write a command to enable or disable clipping. */
/* This routine is only called if the path extensions are included. */
int
cmd_put_enable_clip(gx_device_clist_writer *cldev, gx_clist_state *pcls,
  int enable)
{	byte *dp;
	set_cmd_put_op(dp, cldev, pcls,
		       (byte)(enable ? cmd_opvar_enable_clip :
			      cmd_opvar_disable_clip),
		       1);
	pcls->clip_enabled = enable;
	return 0;
}

/* Write a command to set the logical operation. */
int
cmd_set_lop(gx_device_clist_writer *cldev, gx_clist_state *pcls,
  gs_logical_operation_t lop)
{	byte *dp;
	uint lop_msb = lop >> 6;

	set_cmd_put_op(dp, cldev, pcls,
		       cmd_opv_set_misc, 2 + cmd_size_w(lop_msb));
	dp[1] = cmd_set_misc_lop + (lop & 0x3f);
	cmd_put_w(lop_msb, dp + 2);
	pcls->lop = lop;
	return 0;
}

/* ---------------- Driver interface ---------------- */

private int
clist_get_band(gx_device *dev, int y, int *band_start)
{	int band_height = cdev->page_band_height;
	int start;

	if ( y < 0 )
	  y = 0;
	else if ( y >= dev->height )
	  y = dev->height;
	*band_start = start = y - y % band_height;
	return min(dev->height - start, band_height);
}
