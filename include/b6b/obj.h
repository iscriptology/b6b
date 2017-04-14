/*
 * This file is part of b6b.
 *
 * Copyright 2017 Dima Krasner
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <inttypes.h>
#include <sys/types.h>
#include <stddef.h>
#include <sys/queue.h>
#include <assert.h>

typedef double b6b_num;
#define B6B_NUM_FMT "%.12f"

struct b6b_obj;

TAILQ_HEAD(b6b_lhead, b6b_litem);
struct b6b_litem {
	struct b6b_obj *o;
	TAILQ_ENTRY(b6b_litem) ents;
};

struct b6b_interp;

enum b6b_obj_flags {
	B6B_OBJ_LIST    = 1,
	B6B_OBJ_STR     = 1 << 1,
	B6B_OBJ_NUM     = 1 << 2,
	B6B_OBJ_HASHED  = 1 << 3
};

typedef enum b6b_res (*b6b_procf)(struct b6b_interp *, struct b6b_obj *);
typedef void (*b6b_delf)(void *);

struct b6b_obj {
	struct b6b_lhead l;
	char *s;
	b6b_num n;
	size_t slen;
	b6b_procf proc;
	b6b_delf del;
	void *priv;
	int refc;
	uint32_t hash;
	uint8_t flags;
};

#define b6b_list_first(o) TAILQ_FIRST(&o->l)
#define b6b_list_next(li) TAILQ_NEXT(li, ents)
#define b6b_list_foreach_safe(o, li, tli) \
	TAILQ_FOREACH_SAFE(li, &o->l, ents, tli)
#define b6b_list_foreach(o, li) TAILQ_FOREACH(li, &o->l, ents)
#define b6b_list_remove(o, li) TAILQ_REMOVE(&o->l, li, ents)

__attribute__((nonnull(1)))
static inline struct b6b_obj *b6b_ref(struct b6b_obj *o)
{
	assert(o->refc >= 0);

	++o->refc;
	return o;
}

struct b6b_obj *b6b_new(void);
void b6b_destroy_l(struct b6b_obj *o);
void b6b_destroy(struct b6b_obj *o);

__attribute__((nonnull(1)))
static inline void b6b_unref(struct b6b_obj *o)
{
	assert(o->refc > 0);

	if (b6b_unlikely(!--o->refc))
		b6b_destroy(o);
}

int b6b_obj_hash(struct b6b_obj *o);
int b6b_obj_eq(struct b6b_obj *a, struct b6b_obj *b, int *eq);

static inline enum b6b_res b6b_obj_isnull(const struct b6b_obj *o)
{
	return (((o->flags & B6B_OBJ_STR) && !o->slen) ||
	        ((o->flags & B6B_OBJ_LIST) && !b6b_list_first(o)));
}

static inline enum b6b_res b6b_obj_istrue(const struct b6b_obj *o)
{
	return (((o->flags & B6B_OBJ_NUM) && o->n) ||
	        ((o->flags & B6B_OBJ_LIST) && b6b_list_first(o)) ||
	        o->slen);
}
