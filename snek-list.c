/*
 * Copyright © 2018 Keith Packard <keithp@keithp.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#include "snek.h"

static snek_offset_t
snek_list_alloc(snek_offset_t size)
{
	return size + (size >> 3) + (size < 9 ? 3 : 6);
}

static snek_list_t *
snek_list_resize(snek_list_t *list, snek_offset_t size)
{
	if (list->alloc >= size) {
		list->size = size;
		return list;
	}

	snek_offset_t alloc = snek_list_readonly(list) ? size : snek_list_alloc(size);

	snek_stack_push_list(list);
	snek_poly_t *data = snek_alloc(alloc * sizeof (snek_poly_t));
	list = snek_stack_pop_list();

	if (!data)
		return false;
	snek_offset_t to_copy = size;
	if (to_copy > list->size)
		to_copy = list->size;
	memcpy(data, snek_pool_addr(list->data), to_copy * sizeof (snek_poly_t));
	list->data = snek_pool_offset(data);
	list->size = size;
	list->alloc = alloc;
	return list;
}

snek_list_t	*snek_empty_tuple;

static snek_list_t *
snek_list_head_alloc(bool readonly)
{
	snek_list_t *list = snek_alloc(sizeof(snek_list_t));
	if (list)
		snek_list_set_readonly(list, readonly);
	return list;
}

snek_list_t *
snek_list_make(snek_offset_t size, bool readonly)
{
	if (size == 0 && readonly) {
		if (!snek_empty_tuple)
			return snek_empty_tuple = snek_list_head_alloc(true);
		return snek_empty_tuple;
	}
	snek_list_t *list = snek_list_head_alloc(readonly);
	if (list)
		list = snek_list_resize(list, size);

	return list;
}

snek_list_t *
snek_list_append(snek_list_t *list, snek_list_t *append)
{
	snek_offset_t oldsize = list->size;

	if (snek_list_readonly(list))
		return NULL;

	snek_stack_push_list(append);
	list = snek_list_resize(list, list->size + append->size);
	append = snek_stack_pop_list();

	if (list)
		memcpy((snek_poly_t *) snek_pool_addr(list->data) + oldsize,
		       snek_pool_addr(append->data),
		       append->size * sizeof(snek_poly_t));
	return list;
}

snek_list_t *
snek_list_plus(snek_list_t *a, snek_list_t *b)
{
	snek_stack_push_list(a);
	snek_stack_push_list(b);
	snek_list_t *n = snek_list_make(a->size + b->size, snek_list_readonly(a));
	b = snek_stack_pop_list();
	a = snek_stack_pop_list();
	if (!n)
		return NULL;
	memcpy(snek_pool_addr(n->data),
	       snek_pool_addr(a->data),
	       a->size * sizeof(snek_poly_t));
	memcpy((snek_poly_t *) snek_pool_addr(n->data) + a->size,
	       snek_pool_addr(b->data),
	       b->size * sizeof(snek_poly_t));
	return n;
}

snek_list_t *
snek_list_times(snek_list_t *a, snek_soffset_t count)
{
	if (count < 0)
		count = 0;
	snek_stack_push_list(a);
	snek_offset_t size = a->size;
	snek_list_t *n = snek_list_make(size * count, snek_list_readonly(a));
	a = snek_stack_pop_list();
	if (!n)
		return NULL;
	snek_poly_t *src = snek_pool_addr(a->data);
	snek_poly_t *dst = snek_pool_addr(n->data);
	while (count--) {
		memcpy(dst, src, size * sizeof (snek_poly_t));
		dst += size;
	}
	return n;
}

snek_poly_t *
snek_list_ref(snek_list_t *list, snek_soffset_t o, bool report_error)
{
	if (o < 0)
		o = list->size + o;
	if (list->size <= (snek_offset_t) o) {
		if (report_error)
			snek_error_range(o);
		return NULL;
	}
	return &snek_list_data(list)[o];
}

snek_poly_t
snek_list_get(snek_list_t *list, snek_soffset_t o, bool report_error)
{
	snek_poly_t *p = snek_list_ref(list, o, report_error);
	if (p)
		return *p;
	return SNEK_NULL;
}

bool
snek_list_equal(snek_list_t *a, snek_list_t *b)
{
	if (snek_list_readonly(a) != snek_list_readonly(b))
		return false;
	if (a->size != b->size)
		return false;
	snek_poly_t *adata = snek_pool_addr(a->data);
	snek_poly_t *bdata = snek_pool_addr(b->data);
	for (snek_offset_t o = 0; o < a->size; o++)
		if (!snek_poly_equal(adata[o], bdata[o], false))
			return false;
	return true;
}

snek_poly_t
snek_list_imm(snek_offset_t size, bool readonly)
{
	snek_list_t	*list = snek_list_make(size, readonly);

	if (!list) {
		snek_stack_drop(size);
		return SNEK_NULL;
	}

	snek_poly_t	*data = snek_pool_addr(list->data);
	while (size--)
		data[size] = snek_stack_pop();
	return snek_list_to_poly(list);
}

snek_list_t *
snek_list_slice(snek_list_t *list, snek_slice_t *slice)
{
	bool readonly = snek_list_readonly(list);
	if (readonly && slice->identity)
	    return list;

	snek_stack_push_list(list);
	snek_list_t *n = snek_list_make(slice->count, readonly);
	list = snek_stack_pop_list();
	if (!n)
		return NULL;
	snek_offset_t i = 0;
	snek_poly_t *data = snek_pool_addr(list->data);
	snek_poly_t *ndata = snek_pool_addr(n->data);
	for (; snek_slice_test(slice); snek_slice_step(slice))
		ndata[i++] = data[slice->pos];
	return n;
}

void
snek_stack_push_list(snek_list_t *l)
{
	snek_stack_push(snek_list_to_poly(l));
}

snek_list_t *
snek_stack_pop_list(void)
{
	return snek_poly_to_list(snek_stack_pop());
}

snek_offset_t
snek_list_size(void *addr)
{
	(void) addr;
	return sizeof (snek_list_t);
}

void
snek_list_mark(void *addr)
{
	snek_list_t *list = addr;
	debug_memory("\t\tmark list size %d alloc %d data %d\n", list->size, list->alloc, list->data);
	if (list->alloc) {
		snek_poly_t *data = snek_pool_addr(list->data);
		snek_mark_blob(data, list->alloc * sizeof (snek_poly_t));
		for (snek_offset_t i = 0; i < list->size; i++)
			snek_poly_mark_ref(&data[i]);
	}
}

void
snek_list_move(void *addr)
{
	snek_list_t *list = addr;
	debug_memory("\t\tmove list size %d alloc %d data %d\n", list->size, list->alloc, list->data);
	if (list->alloc) {
		snek_move_block_offset(&list->data);
		snek_poly_t *data = snek_pool_addr(list->data);
		for (snek_offset_t i = 0; i < list->size; i++)
			snek_poly_move(&data[i]);
	}
}
