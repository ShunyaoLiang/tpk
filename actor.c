/* SPDX-License-Identifier: BSD-2-Clause */
#include "actor.h"
#include "list.h"

#include <curses.h>
#include <string.h>

#define swap(a, b) do { \
	typeof(*a) tmp = *a; \
	*a = *b; \
	*b = tmp; \
} while (0);

struct list_head actor_list = LIST_HEAD_INIT(actor_list);

struct actor *current_actor = NULL;

struct actor *actor_init(void)
{
	struct actor *new = malloc(sizeof(*new));
	new->colour = COLOR_PAIR(0);
	INIT_LIST_HEAD(&new->list);

	return new;
}

void actor_insert(struct actor *insert)
{
	if (!list_empty(&actor_list)) {
		struct list_head *pos;
		list_for_each(pos, &actor_list) {
			if (insert->init > list_entry(pos, struct actor, list)->init) {
				list_add_tail(&insert->list, pos);
				return;
			}
		}
	}

	list_add_tail(&insert->list, &actor_list);
	if (current_actor == NULL) {
		current_actor = insert;
	}
}

void actor_delete(struct actor *delete)
{
	list_del_init(&delete->list);
	if (list_empty(&actor_list)) {
		current_actor = NULL;
	}
}

void actor_swap(struct actor *a, struct actor *b)
{
	swap(a, b);
	swap(&a->list, &b->list);
}

void actor_scan(const char *ask, struct actor *dest)
{
	nocbreak();
	echo();
	curs_set(1);

	mvaddstr(LINES - 1, 0, ask);

	int to_scan = 5;
	while (to_scan > 0) {
		switch (to_scan) {
		case 5:
			to_scan -= mvscanw(LINES - 1, strlen(ask), "%15s %d %d %d %d",
				dest->name, &dest->init, &dest->ac, &dest->hp, &dest->hp_max);
			continue;
		case 4:
			to_scan -= mvscanw(LINES - 1, strlen(ask), "%d %d %d %d",
				&dest->init, &dest->ac, &dest->hp, &dest->hp_max);
			continue;
		case 3:
			to_scan -= mvscanw(LINES - 1, strlen(ask), "%d %d %d", &dest->ac,
				&dest->hp, &dest->hp_max);
			continue;
		case 2:
			to_scan -= mvscanw(LINES - 1, strlen(ask), "%d %d", &dest->hp,
				&dest->hp_max);
			continue;
		case 1:
			dest->hp_max = dest->hp;
			to_scan = 0;
		}
	}

	cbreak();
	noecho();
	curs_set(0);
}

void actor_print(struct actor *src)
{
	if (src == current_actor) {
		attron(A_STANDOUT);
	}
	attron(COLOR_PAIR(src->colour));

	attron(A_BOLD);
	printw("%2d ", src->init);
	attroff(A_BOLD);

	printw("%16s ", src->name);

	attron(A_BOLD);
	printw("%2d ", src->ac);
	attroff(A_BOLD);

	printw("%3d/%-3d\n", src->hp, src->hp_max);

	attroff(A_STANDOUT | COLOR_PAIR(src->colour));
}

#ifdef TEST_INSERT

#include <stdlib.h>
#include <stdio.h>

struct actor *new_actor(int n)
{
	struct actor *new = malloc(sizeof(*new));
	new->name = NULL;
	new->init = new->ac = new->hp = new->hp_max = n;

	return new;
}

int main(void)
{
	actor_insert(new_actor(1));
	actor_insert(new_actor(10));
	actor_insert(new_actor(100));
	actor_insert(new_actor(1000));
	actor_insert(new_actor(18));

	struct list_head *pos;
	list_for_each(pos, &actor_list) {
		printf("%d->", list_entry(pos, struct actor, list)->init);
	}
	puts("X");
}

#endif /* TEST_INSERT */

#ifdef TEST_SWAP

#include <stdlib.h>
#include <assert.h>

struct actor *new_actor(int n)
{
	struct actor *new = malloc(sizeof(*new));
	new->name = NULL;
	new->init = new->ac = new->hp = new->hp_max = n;

	return new;
}

int main(void)
{
	struct actor *a = new_actor(2);
	struct actor *b = new_actor(4);

	actor_swap(a, b);

	assert(a->init == 4);
	assert(b->init == 2);
}

#endif
