#ifndef HIGHSCORE_H
#define HIGHSCORE_H

#include "engine.h"

struct highscore {
	char *user;
	int score;
};

void highscore_reset(void);
long highscore_load(struct gamestate *g);
void highscore_save(struct gamestate *g);
void highscore_print(void);

#endif
