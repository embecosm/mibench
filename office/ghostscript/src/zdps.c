/* Copyright (C) 1997 Aladdin Enterprises.  All rights reserved.
  
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

/* zdps.c */
/* Display PostScript extensions */
#include "ghost.h"
#include "errors.h"
#include "oper.h"
#include "gsstate.h"
#include "igstate.h"
#include "iname.h"
#include "store.h"

/* Import the user name table. */
extern ref user_names;

/* ------ Graphics state ------ */

/* <screen_index> <x> <y> .setscreenphase - */
private int
zsetscreenphase(register os_ptr op)
{	int code;
	long x, y;

	check_type(op[-2], t_integer);
	check_type(op[-1], t_integer);
	check_type(*op, t_integer);
	x = op[-1].value.intval;
	y = op->value.intval;
	if ( x != (int)x || y != (int)y ||
	     op[-2].value.intval < -1 ||
	     op[-2].value.intval >= gs_color_select_count
	   )
	  return_error(e_rangecheck);
	code = gs_setscreenphase(igs, (int)x, (int)y,
				 (gs_color_select_t)op[-2].value.intval);
	if ( code >= 0 )
	  pop(3);
	return code;
}

/* <screen_index> .currentscreenphase <x> <y> */
private int near
zcurrentscreenphase(register os_ptr op)
{	gs_int_point phase;
	int code;

	check_type(*op, t_integer);
	if ( op->value.intval < -1 ||
	     op->value.intval >= gs_color_select_count
	   )
	  return_error(e_rangecheck);
	code = gs_currentscreenphase(igs, &phase,
				     (gs_color_select_t)op->value.intval);
	if ( code < 0 )
	  return code;
	push(1);
	make_int(op - 1, phase.x);
	make_int(op, phase.y);
	return 0;
}

/* ------ View clipping ------ */

/* - viewclip - */
private int
zviewclip(register os_ptr op)
{	/****** NYI ******/
	return_error(e_undefined);
}

/* - eoviewclip - */
private int
zeoviewclip(register os_ptr op)
{	/****** NYI ******/
	return_error(e_undefined);
}

/* - initviewclip - */
private int
zinitviewclip(register os_ptr op)
{	/****** NYI ******/
	return_error(e_undefined);
}

/* - viewclippath - */
private int
zviewclippath(register os_ptr op)
{	/****** NYI ******/
	return_error(e_undefined);
}

/* ------ User names ------ */

/* <index> <name> defineusername - */
private int
zdefineusername(register os_ptr op)
{	ref uname;

	check_type(op[-1], t_integer);
	check_type(*op, t_name);
	if ( array_get(&user_names, op[-1].value.intval, &uname) >= 0 )
	  { switch ( r_type(&uname) )
	      {
	      case t_null:
		break;
	      case t_name:
		if ( name_eq(&uname, op) )
		  goto ret;
		/* falls through */
	      default:
		return_error(e_invalidaccess);
	      }
	  }
	else
	  { /* Expand the array. */
	    /****** NOT IMPLEMENTED YET ******/
	    return_error(e_rangecheck);
	  }
	ref_assign(user_names.value.refs + op[-1].value.intval, op);
ret:	pop(2);
	return 0;
}

/* ------ Initialization procedure ------ */

BEGIN_OP_DEFS(zdps_op_defs) {
		/* Graphics state */
	{"1.currentscreenphase", zcurrentscreenphase},
	{"3.setscreenphase", zsetscreenphase},
		/* View clipping */
	{"0eoviewclip", zeoviewclip},
	{"0initviewclip", zinitviewclip},
	{"0viewclip", zviewclip},
	{"0viewclippath", zviewclippath},
		/* User names */
	{"2defineusername", zdefineusername},
END_OP_DEFS(0) }

