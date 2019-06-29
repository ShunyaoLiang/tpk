/* SPDX-License-Identifier: BSD-2-Clause */
#include "actor.h"
#include "dice.h"

#include <locale.h>
#include <curses.h>
#include <stdlib.h>

void int_scan(const char *ask, int *dest)
{
	nocbreak();
	echo();
	curs_set(1);

	mvaddstr(LINES - 1, 0, ask);
	scanw("%d", dest);

	cbreak();
	noecho();
	curs_set(0);
}

void str_scan(const char *ask, char **dest)
{
	nocbreak();
	echo();
	curs_set(1);

	mvaddstr(LINES - 1, 0, ask);
	scanw("%ms", dest);

	cbreak();
	noecho();
	curs_set(0);
}

void init_colours(void)
{
	start_color();

	init_pair(0, COLOR_BLACK, COLOR_BLACK);
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);
	init_pair(4, COLOR_BLUE, COLOR_BLACK);
	init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(6, COLOR_CYAN, COLOR_BLACK);
	init_pair(7, COLOR_WHITE, COLOR_BLACK);

}

enum key {
	KEY_NEXT_ACTOR = 'j',
	KEY_PREV_ACTOR = 'k',

	KEY_INSERT_ACTOR = 'i',
	KEY_DELETE_ACTOR = 'd',
	KEY_EDIT_ACTOR = 'e',

	KEY_DSHIFT_ACTOR = ']',
	KEY_USHIFT_ACTOR = '[',

	KEY_COLOUR_ACTOR = 'c',

	KEY_HEAL_ACTOR = '+',
	KEY_HURT_ACTOR = '-',
	KEY_SET_HP_ACTOR = '=',

	KEY_ROLL_DICE = '/' /* 'r' */
};

void key_next_actor(void)
{
	if (!current_actor)
		return;

	current_actor = actor_next(current_actor);
	/* Due to how actor.c was designed, there perpetually exists a list_head
	   object that is part of no struct actor, causing the list to always
	   contain an "actor" with garbage values. This is my shitty workaround. */
	if (&current_actor->list == &actor_list) {
		current_actor = actor_next(current_actor);
	}
}

void key_prev_actor(void)
{
	if (!current_actor)
		return;

	current_actor = actor_prev(current_actor);
	/* See key_next_actor(). */
	if (&current_actor->list == &actor_list) {
		current_actor = actor_prev(current_actor);
	}
}

void key_insert_actor(void)
{
	struct actor *new = actor_init();
	actor_scan(": ", new);
	actor_insert(new);
}

void key_delete_actor(void)
{
	key_next_actor();
	actor_delete(actor_prev(current_actor));
}

void key_edit_actor(void)
{
	key_delete_actor();
	key_insert_actor();
}

void key_dshift_actor(void)
{
	if (current_actor->init == actor_next(current_actor)->init) {
		actor_swap(current_actor, actor_next(current_actor));
	}
}

void key_ushift_actor(void)
{
	if (current_actor->init == actor_prev(current_actor)->init) {
		actor_swap(current_actor, actor_prev(current_actor));
	}
}

void key_colour_actor(void)
{
	int_scan("colour: ", &current_actor->colour);
}

void key_heal_actor(void)
{
	int heal;
	int_scan("heal: ", &heal);
	current_actor->hp += heal;
}

void key_hurt_actor(void)
{
	int hurt;
	int_scan("hurt: ", &hurt);
	current_actor->hp -= hurt;
}

void key_set_hp_actor(void)
{
	int hp;
	int_scan("hp: ", &hp);
	current_actor->hp = hp;
}

void key_roll_dice(void)
{
	char *buf;
	str_scan("/", &buf);
	struct dice dice = dice_scan(buf);
	free(buf);
	int result = dice_roll(dice);
	move(LINES - 2, 0);
	dice_print(dice);
	printw("(%d)", result);
	getch();
}

void (*handle[])(void) = {
	[KEY_NEXT_ACTOR]   = key_next_actor,
	[KEY_PREV_ACTOR]   = key_prev_actor,

	[KEY_INSERT_ACTOR] = key_insert_actor,
	[KEY_DELETE_ACTOR] = key_delete_actor,
	[KEY_EDIT_ACTOR]   = key_edit_actor,

	[KEY_DSHIFT_ACTOR] = key_dshift_actor,
	[KEY_USHIFT_ACTOR] = key_ushift_actor,

	[KEY_COLOUR_ACTOR] = key_colour_actor,

	[KEY_HEAL_ACTOR] = key_heal_actor,
	[KEY_HURT_ACTOR] = key_hurt_actor,
	[KEY_SET_HP_ACTOR] = key_set_hp_actor,

	/* TODO: Improve dice rolling. */
	[KEY_ROLL_DICE] = key_roll_dice,

	/* temporary workaround. */
	['o'] = NULL
};

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "");

	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);

	if (has_colors()) {
		init_colours();
	}

	while (1) {
		clear();
		struct actor *pos;
		list_for_each_entry(pos, &actor_list, list) {
			actor_print(pos);
		}
		refresh();

		int key = getch();
		if (handle[key])
			handle[key]();
	}

	endwin();

	return EXIT_SUCCESS;
}
