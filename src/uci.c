#include <string.h>
#include <assert.h>
#include <time.h>

#include <unistd.h>
#include <pthread.h>

#include "headers/board.h"
#include "headers/play.h"
#include "headers/search.h"
#include "headers/eval.h"
#include "headers/hashtables.h"
#include "headers/uci.h"
#include "headers/draw.h"


static void isready(void);
static void position(Board *board, char *s);
static void go(Board *board, Settings *settings, char *s);

pthread_t worker;
int working = 0;

void uci(void) {

	fprintf(stdout, "id name %s\n", ENGINE_NAME);
	fprintf(stdout, "id author %s\n", ENGINE_AUTHOR);
	fprintf(stdout, "uciok\n");
	fflush(stdout);

	Board *board = malloc(sizeof(Board));
	char *msg = malloc(4096);

	while (1) {
		char *r;
		r = fgets(msg, 4096, stdin);

		if (r == NULL)
			break;

		if (strncmp(msg, "isready", 7) == 0)
			isready();
		else if (strncmp(msg, "ucinewgame", 10) == 0)
			initialBoard(board);
		else if (strncmp(msg, "position", 8) == 0)
			position(board, msg + 9);
		else if (strncmp(msg, "eval", 4) == 0)
			evaluate(board);
		else if (strncmp(msg, "go", 2) == 0)
			go(board, &settings, msg + 2);
		else if (strncmp(msg, "stop", 4) == 0) {
			settings.stop = 1;

			if (working)
				stopSearchThread();
		} else if (strncmp(msg, "quit", 4) == 0)
			break;
	}

	if (working)
		stopSearchThread();

	free(board);
	free(msg);
}

// Lets the GUI know the engine is ready. Serves as a ping.
static void isready(void) {
	fprintf(stdout, "readyok\n");
	fflush(stdout);
}

/*
 * Sets the board to a certain position
 * position (startpos | fen) (moves e2e4 c7c5)?
 */
static void position(Board *board, char *s) {
	if (strncmp(s, "startpos", 8) == 0) {
		s += 9;
		initialBoard(board);
	} else {
		s += parseFen(board, s) + 1;
	}

	if (strncmp(s, "moves", 5) == 0)
		moves(board, s + 6);
}

static void go(Board *board, Settings *settings, char *s) {
	defaultSettings(settings);

	while (s[1] != '\0' && s[1] != '\n') {
		if (strncmp(s, "infinite", 8) == 0) {
			settings->depth = MAX_DEPTH;
			break;
		} else if (strncmp(s, "wtime", 5) == 0) {
			s += 6;
			settings->wtime = atoi(s);
		} else if (strncmp(s, "btime", 5) == 0) {
			s += 6;
			settings->btime = atoi(s);
		} else if (strncmp(s, "winc", 4) == 0) {
			s += 5;
			settings->winc = atoi(s);
		} else if (strncmp(s, "binc", 4) == 0) {
			s += 5;
			settings->binc = atoi(s);
		} else if (strncmp(s, "movestogo", 9) == 0) {
			s += 10;
			settings->movestogo = atoi(s);
		} else if (strncmp(s, "depth", 5) == 0) {
			s += 6;
			settings->depth = atoi(s);
		} else if (strncmp(s, "movetime", 8) == 0) {
			s += 9;
			settings->movetime = atoi(s);
		}

		++s;
	}

	createSearchThread(board);
}

void *bestmove(void *args) {
	Board *board = (Board *) args;
	char pv[6];

	const Move move = search(board);
	moveToText(move, pv);

	fprintf(stdout, "bestmove %s\n", pv);
	fflush(stdout);

	return NULL;
}

void infoString(const Board *board, PV *pv, const int score, const int depth, const long duration, const uint64_t nodes) {
	const int relativeScore = (board->turn == WHITE) ? score : -score;

	fprintf(stdout, "info score cp %d depth %d time %ld nps %d nodes %ld pv ", relativeScore, depth, duration, 1000 * nodes / (duration+1), nodes);

	for (int i = 0; i < pv->count; ++i) {
		char moveText[6];
		moveToText(pv->moves[i], moveText);
		fprintf(stdout, "%s ", moveText);
	}

	fprintf(stdout, "\n");
	fflush(stdout);
}

void createSearchThread(Board *board) {
	if (working)
		pthread_join(worker, NULL);

	pthread_create(&worker, NULL, bestmove, (void *) board);
	working = 1;
}

void stopSearchThread(void) {
	settings.stop = 1;
	pthread_join(worker, NULL);
	working = 0;
}
