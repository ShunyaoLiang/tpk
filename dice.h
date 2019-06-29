/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef DICE_H
#define DICE_H

struct dice {
	int amount;
	int faces;
	int mod;
};

struct dice dice_scan(char *src);

int dice_roll(struct dice dice);

void dice_print(struct dice dice);

#endif
