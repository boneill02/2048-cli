#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "engine.h"
#include "highscore.h"
#include <libintl.h>
#include <locale.h>
#include <dirent.h>

const char *hs_dir_name  = HIGHSCOREDIR;

static const char* highscore_retrieve_file(void)
{
    static char buffer[4096];

	if (getenv("USER") != NULL) {
		snprintf(buffer, sizeof(buffer), "%s/%s", hs_dir_name, getenv("USER"));
	} else {
		snprintf(buffer, sizeof(buffer), "highscore");
	}

/*    if (getenv("XDG_DATA_HOME") != NULL) {
        snprintf(buffer, sizeof(buffer), "%s/%s/%s",
                getenv("XDG_DATA_HOME"), hs_dir_name, hs_file_name);
    }
    else if (getenv("HOME")) {
        snprintf(buffer, sizeof(buffer), "%s/.local/share/%s/%s",
                getenv("HOME"), hs_dir_name, hs_file_name);
    }
    else {
        return hs_file_name;
    }*/
	

    /* Create file only if it doesn't exist */
    if (access(buffer, F_OK) != -1)
        return buffer;

    char *sep = strrchr(buffer, '/');
    while (sep != NULL) {
        *sep = '\0';
        if (strlen(buffer) != 0)
            mkdir(buffer, 0777);
        char *tmpsep = sep;
        sep = strrchr(buffer, '/');
        *tmpsep = '/';
    }

    return buffer;
}

static inline void string_to_lower(char *str)
{
    for (; *str; ++str) *str = tolower(*str);
}

void highscore_reset(void)
{
    const char *hsfile = highscore_retrieve_file();
    const size_t resp_length = 16;
    char resp[resp_length];

    printf(gettext("Are you sure you want to reset your scores? Y(es) or N(o)\n"));

    while (1) {
        /* fgets is used to avoid queuing that may occur with getchar */
        if (fgets(resp, resp_length, stdin) == NULL)
            return;

        string_to_lower(resp);

        const size_t sl = strlen(resp);
        if (sl < resp_length)
            resp[sl - 1] = '\0';

        if (!strncmp(resp, gettext("yes"), resp_length) || !strncmp(resp, gettext("y"), resp_length))
            goto reset_scores;
        else if (!strncmp(resp, "no", resp_length) || !strncmp(resp, "n",  resp_length))
            return;

        printf(gettext("Please enter Yes or No\n"));
    }

reset_scores:;
    FILE *fd = fopen(hsfile, "w+");
    fprintf(fd, "%d", 0);
    fclose(fd);
}

int highscore_comp(const void *e1, const void *e2)
{
	int a = ((struct highscore *) e1)->score;
	int b = ((struct highscore *) e2)->score;
	if (a < b) return 1;
	if (a > b) return -1;
	return 0;
}

void highscore_print(void) {
	DIR *d;
	struct dirent *dir;
	d = opendir(hs_dir_name);
	static char buffer[8];
	static char dirbuf[128];
	struct highscore scores[256];
	int scorelen = 0;

	readdir(d); // .
	readdir(d); // ..
	while ((dir = readdir(d)) != NULL) {
		sprintf(dirbuf, "%s/%s", hs_dir_name, dir->d_name);
		FILE *fd = fopen(dirbuf, "r");
		if (!fd) {
			fprintf(stderr,"load: Failed to open highscore file %s\n", dir->d_name);
			continue;
		}
		scores[scorelen].user = malloc(strlen(dir->d_name));
		strcpy(scores[scorelen].user, dir->d_name);

		fgets(buffer, sizeof(buffer), fd);
		scores[scorelen].score = atoi(buffer);
		scorelen++;
    }


	qsort(scores, scorelen, sizeof(*scores), highscore_comp);

	for (int i = 0; i < scorelen; i++) {
		printf("%d: %d - %s\n", i + 1, scores[i].score, scores[i].user);
		free(scores[i].user);
	}
}

long highscore_load(struct gamestate *g)
{
    const char *hsfile = highscore_retrieve_file();
    long result = 0;

    FILE *fd = fopen(hsfile, "r");
    if (fd == NULL)
        fd = fopen(hsfile, "w+");

    if (fd == NULL) {
        fprintf(stderr, gettext("load: Failed to open highscore file\n"));
        return 0;
    }

    if (fscanf(fd, "%ld", &result) != 1) {
        fprintf(stderr, gettext("load: Failed to parse highscore file\n"));
        result = 0;
    }

    fclose(fd);

    g->score_high = result;
    return result;
}

void highscore_save(struct gamestate *g)
{
    /* Someone could make their own merge rules for highscores and this could be meaningless,
     * howeverhighscores are in plaintext, so that isn't that much of a concern */
    if (g->score < g->score_high || g->opts->grid_width != 4 ||
            g->opts->grid_height != 4 || g->opts->ai == true)
        return;

    const char *hsfile = highscore_retrieve_file();

    FILE *fd = fopen(hsfile, "w");
    if (fd == NULL) {
        fprintf(stderr, gettext("save: Failed to open highscore file\n"));
        return;
    }

    if (fprintf(fd, "%ld", g->score) < 0) {
        fprintf(stderr, gettext("save: Failed to write highscore file\n"));
    }
    fclose(fd);
}
