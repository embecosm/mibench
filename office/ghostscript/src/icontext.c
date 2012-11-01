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

/* icontext.c */
/* Context state operations */
#include "ghost.h"
#include "gsstruct.h"		/* for gxalloc.h */
#include "gxalloc.h"
#include "igstate.h"
#include "icontext.h"
#include "dstack.h"
#include "estack.h"
#include "ostack.h"
#include "store.h"

/*
 * Eventually this will replace some of interp.c for single-context
 * systems; right now it is used only in multi-context (DPS) systems.
 ****** DO NOT TRY TO USE THIS FILE. ******
 */

/* Define the initial stack sizes. */
#define DSTACK_INITIAL 20
#define ESTACK_INITIAL 250
#define OSTACK_INITIAL 200

/* Per-context state stored in statics */
extern ref ref_array_packing;
extern ref ref_binary_object_format;
extern ref user_names;
extern long zrand_state;

/* Initialization procedures */
void zrand_state_init(P1(long *));

/* Allocate the state of a context. */
int
context_state_alloc(gs_context_state_t *pcst, gs_ref_memory_t *mem)
{	/****** NOT COMPLETE OR FUNCTIONAL ******/
	/****** ALLOCATE & ref_stack_init EACH STACK ******/
	/****** ALLOCATE GSTATE ******/
	/****** WHAT TO DO ABOUT memory? ******/
	make_false(&pcst->array_packing);
	make_int(&pcst->binary_object_format, 0);
	zrand_state_init(&pcst->rand_state);
	pcst->usertime_total = 0;
	pcst->keep_usertime = false;
	/****** view clipping ******/
	/****** user parameters ******/
	/****** %stdin, %stdout ******/
	return 0;
}

/* Load the interpreter state from a context. */
void
context_state_load(const gs_context_state_t *pcst)
{	d_stack = pcst->dstack;
	e_stack = pcst->estack;
	o_stack = pcst->ostack;
	igs = pcst->pgs;
	gs_imemory = pcst->memory;
	ref_array_packing = pcst->array_packing;
	ref_binary_object_format = pcst->binary_object_format;
	zrand_state = pcst->rand_state;
}

/* Store the interpreter state in a context. */
void
context_state_store(gs_context_state_t *pcst)
{	pcst->dstack = d_stack;
	pcst->estack = e_stack;
	pcst->ostack = o_stack;
	pcst->pgs = igs;
	pcst->memory = gs_imemory;
	pcst->array_packing = ref_array_packing;
	pcst->binary_object_format = ref_binary_object_format;
	pcst->rand_state = zrand_state;
}

/* Free the state of a context. */
void
context_state_free(gs_context_state_t *pcst, gs_ref_memory_t *mem)
{	/****** SEE alloc ABOVE ******/
	{ int i;
	  for ( i = 0; i < countof(pcst->memory.spaces.indexed); ++i )
	    if ( pcst->memory.spaces.indexed[i] != 0 &&
		 !--(pcst->memory.spaces.indexed[i]->num_contexts)
	       )
	      { /****** FREE MEMORY ******/
	      }
	}
}
