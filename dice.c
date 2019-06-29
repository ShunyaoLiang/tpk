/* SPDX-License-Identifier: BSD-2-Clause */
#include "dice.h"

#include <curses.h>
#include <time.h>
#include <limits.h>
#include <stdio.h>

#define do_once(thing) do { \
	static int done = 0; \
	if (!done) { \
		thing; \
		done = !0; \
	} \
} while (0);

/* Marsiglia xorshift */
static unsigned int xorshift(unsigned int state)
{
	state ^= state << 13;
	state ^= state >> 17;
	state ^= state << 5;
	return state;
}

static int rand(void)
{
	static unsigned int state;
	do_once(state = time(NULL));

	return (state = xorshift(state)) % INT_MAX;
}

struct dice dice_scan(char *src)
{
	struct dice dice = { 0 };
	sscanf(src, "%dd%d%d", &dice.amount, &dice.faces, &dice.mod);
	return dice;
}


int dice_roll(struct dice dice)
{
	int res = 0;
	while (dice.amount-- > 0) {
		res += rand() % dice.faces + 1;
	}

	return res + dice.mod;
}

void dice_print(struct dice dice)
{
	attron(A_BOLD);
	printw("%dd%d%+d ", dice.amount, dice.faces, dice.mod);
	attroff(A_BOLD);
}
