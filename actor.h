/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef ACTOR_H
#define ACTOR_H

#include "list.h"
#include <stdlib.h>

struct actor {
	char name[16];
	int init;
	int ac;
	int hp, hp_max;
	int colour;

	struct list_head list;
};

extern struct list_head actor_list;

extern struct actor *current_actor;

struct actor *actor_init(void);

void actor_insert(struct actor *insert);
void actor_delete(struct actor *delete);
void actor_swap(struct actor *a, struct actor *b);
void actor_scan(const char *ask, struct actor *dest);
void actor_print(struct actor *src);

#define actor_next(actor) \
	list_next_entry(actor, list)

#define actor_prev(actor) \
	list_prev_entry(actor, list)

#endif
