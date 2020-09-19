#include <ncurses.h>
#include <time.h>
#include <stdlib.h>
#include "cmap.h"
#include "slater.h"
#include <list>
#include <iostream>
using namespace std;

void updatewin(WINDOW * local_win);
void initcolors();
void togglegrab(WINDOW * status_win);
void togglepush(WINDOW * status_win);
void move_ant(int y, int x);
//int index_rock(int y, int x);
void modwin(WINDOW * local_win);
void modchwin(WINDOW * local_win, int y, int x, char ch);

#define T_WIDTH 50
#define T_HEIGHT 20
#define STATUS_HEIGHT 1
#define STATUS_WIDTH COLS
#define ANT_START_Y (T_HEIGHT / 2)
#define ANT_START_X (T_WIDTH / 2)
#define ANT_FACE '@'
#define ROCK_SYMBOL '#'
#define ROCK_NUMBER 600
//#define IS_ROCK(A, B) (index_rock(A, B) != -1)
//#define SLATER_NUMBER 10
#define DESTINATION_SYMBOL '>'
#define CORRIDOR_NUMBER 100

int ant_y = ANT_START_Y;
int ant_x = ANT_START_X;

int grabbing = 0;

//int rocks[ROCK_NUMBER][2];

cmap * mp;
int level;

int main() {
	int result = 1;
	srand(time(NULL));
	int winx, winy;
	int wwidth = T_WIDTH + 2;
	int wheight = T_HEIGHT + 2;

	WINDOW *win;
	WINDOW *statuswin;
	WINDOW *topwin;

	initscr();
	start_color();
	initcolors();
	cbreak();
	noecho();
	curs_set(0);
	winy = (LINES - wheight) / 2;
	winx = (COLS - wwidth) / 2;

	win = newwin(wheight, wwidth, winy, winx);
	statuswin = newwin(STATUS_HEIGHT, STATUS_WIDTH, LINES - 1, 0);
	topwin = newwin(1, COLS, 0, 0);

	mvwprintw(topwin, 0, 0, "MOVE: hjklyubn   GRAB: g   PUSH: p   REDRAW: r   QUIT: q");


	box(win, 0, 0);

	for (level = 1; result == 1; level++) {
		mp = new cmap(T_HEIGHT, T_WIDTH);

		for (int my = 0; my < T_HEIGHT; my++)
			for (int mx = 0; mx < T_WIDTH; mx++)
				mp->setcmch(my, mx, ROCK_SYMBOL);

		for (int corridors = 0; corridors < CORRIDOR_NUMBER; corridors++) {
			int csegments = rand() % 10 + 1;
			int segy = rand() % T_HEIGHT;
			int segx = rand() % T_WIDTH;
			for (int i = 0; i < csegments; i++) {
				int ydelta = rand() % 3 - 1;
				int xdelta = rand() % 3 - 1;
				int seglength = rand() % 4 + 1;
				for (int q = 0; q < seglength; q++)
					mp->setcmch(segy + q * ydelta, segx + q * xdelta, ' ');
			}
		}
		

	/* 	for (int i = 0; i < ROCK_NUMBER; i++) {
	 * 		int xr, yr;
	 * 		do {
	 * 			yr = rand() % T_HEIGHT;
	 * 			xr = rand() % T_WIDTH;
	 * 		} while (mp->getcmch(yr, xr) != ' ');
	 * //		rocks[i][0] = yr;
	 * //		rocks[i][1] = xr;
	 * 		mp->setcmch(yr, xr, ROCK_SYMBOL);
	 * 	}
	 */
		Slater ** slist = new Slater*[level];
		for (int i = 0; i < level; i++) {
			int slater_y;
			int slater_x;


			do {
				slater_y = rand() % T_HEIGHT;
				slater_x = rand() % T_WIDTH;
			} while (mp->getcmch(slater_y, slater_x) != ' ');
			Slater * slater = new Slater(mp, slater_y, slater_x);
			slist[i] = slater;
			mp->setcmch(slist[i]->y, slist[i]->x, SLATER_SYMBOL);
		}

		int destination_y;
		int destination_x;

		do {
			destination_y = rand() % T_HEIGHT;
			destination_x = rand() % T_WIDTH;
		} while(mp->getcmch(destination_y, destination_x) != ' ');

		mp->setcmch(destination_y, destination_x, DESTINATION_SYMBOL);

		do {
			ant_y = rand() % T_HEIGHT;
			ant_x = rand() % T_WIDTH;
		} while (mp->getcmch(ant_y, ant_x) != ' ');
		mp->setcmch(ant_y, ant_x, ANT_FACE);

		wrefresh(win);
		wrefresh(statuswin);
		wrefresh(topwin);

		updatewin(win);


		int ch;
		int turn = 0;
		result = 0;
		while ((ch = wgetch(win)) != 'q' && !result) {
			int mv = 1;
			switch (ch) {
				case 'j':
				case '2':
					move_ant(1, 0);
					break;
				case 'k':
				case '8':
					move_ant(-1, 0);
					break;
				case 'h':
				case '4':
					move_ant(0, -1);
					break;
				case 'l':
				case '6':
					move_ant(0, 1);
					break;
				case 'y':
				case '7':
					move_ant(-1, -1);
					break;
				case 'u':
				case '9':
					move_ant(-1, 1);
					break;
				case 'b':
				case '1':
					move_ant(1, -1);
					break;
				case 'n':
				case '3':
					move_ant(1, 1);
					break;
				case 'g':
					togglegrab(statuswin);
					mv = 0;
					break;
				case 'p':
					togglepush(statuswin);
					mv = 0;
					break;
				case 'r':
					updatewin(win);
					mv = 0;
					break;
				default:
					mv = 0;
					break;
			}
			ant_y = (ant_y > T_HEIGHT - 1) ? T_HEIGHT - 1 : ((ant_y < 0) ? 0 : ant_y);
			ant_x = (ant_x > T_WIDTH - 1) ? T_WIDTH - 1 : ((ant_x < 0) ? 0 : ant_x);
			if (mv) {
				for (int i = 0; i < level; i++) {
					mvwaddch(topwin, 0, COLS - 9, '0' + i);
					Slater * slater = slist[i];
					mvwaddch(topwin, 0, COLS - 2, '0');
					mvwaddch(topwin, 0, COLS - 1, '0');
					wrefresh(topwin);
					int asy = -1;
					int asx = -1;
					int ydelta = ant_y - slater->y;
					int xdelta = ant_x - slater->x;

					if (((ydelta * ydelta + xdelta * xdelta) < 400) && mp->LOS(ant_y, ant_x, slater->y, slater->x)) {
	//				if (1) {
						mvwaddch(topwin, 0, COLS - 2, '1');
						mvwaddch(topwin, 0, COLS - 1, '1');
						wrefresh(topwin);
						asy = ant_y;
						asx = ant_x;
					} else {
						mvwaddch(topwin, 0, COLS - 2, '2');
						mvwaddch(topwin, 0, COLS - 1, '2');
						wrefresh(topwin);
					}
	//				std::cout << "  " << asy << ":" << asx << std::endl;
					slater->act(asy, asx); 
				}
				mvwaddch(topwin, 0, COLS - 9, ' ');
				turn++;
				mvwaddch(topwin, 0, COLS - 2, '3');
				mvwaddch(topwin, 0, COLS - 1, '3');
				wrefresh(topwin);

			}
			modwin(win);
	/* 		if (slater->state) {
	 * 			mvwprintw(statuswin, 0, 0, "CHASING  ");
	 * 		} else {
	 * 			mvwprintw(statuswin, 0, 0, "SEARCHING");
	 * 		}
	 */
	//		wrefresh(statuswin);
			for (int i = 0; i < level; i++)
				if ((ABS(ant_y - slist[i]->y) < 2) && (ABS(ant_x - slist[i]->x) < 2)) result = -1;
			if ((ant_y == destination_y) && (ant_x == destination_x)) result = 1;
			mvwaddch(topwin, 0, COLS - 2, ' ');
			mvwaddch(topwin, 0, COLS - 1, ' ');
			wrefresh(topwin);
			if (result) break;
		}
		for (int i = 0; i < level; i++) {
			delete slist[i];
		}
		delete mp;
		delete[] slist;
	}

	if (result == -1) {
		wattron(win, COLOR_PAIR(2));
		mvwprintw(win, 0, 1, "GAME OVER\t SCORE %d", level - 1);
		wattroff(win, COLOR_PAIR(2));
	} else if (result == 1) {
		wattron(win, COLOR_PAIR(1));
		mvwprintw(win, 0, 1, "VICTORY!");
		wattroff(win, COLOR_PAIR(1));
	} else {
		wattron(win, COLOR_PAIR(4));
		mvwprintw(win, 0, 1, "GAME OVER\t SCORE %d", level - 1);
		wattroff(win, COLOR_PAIR(4));
	}
	wrefresh(win);
	wgetch(win);
	endwin();
	return 0;
}

void updatewin(WINDOW * local_win) {
	werase(local_win);
	box(local_win, 0, 0);
	mvwprintw(local_win, 0, 1, "Level: %d", level);


	for (int y = 0; y < T_HEIGHT; y++)
		for (int x = 0; x < T_WIDTH; x++)
			modchwin(local_win, y, x, mp->getcmch(y, x));
	mp->clearmq();

	/*
	wattron(local_win, COLOR_PAIR((grabbing) ? 2 : 1) | A_BOLD);
	mvwaddch(local_win, ant_y + 1, ant_x + 1, ANT_FACE);
	wattroff(local_win, COLOR_PAIR((grabbing) ? 2 : 1) | A_BOLD);

	wattron(local_win, COLOR_PAIR(4) | A_BOLD);
	mvwaddch(local_win, slater_y + 1, slater_x + 1, SLATER_SYMBOL);
	wattroff(local_win, COLOR_PAIR(4) | A_BOLD);

	wattron(local_win, COLOR_PAIR(3));
	for (int i = 0; i < ROCK_NUMBER; i++) {
		mvwaddch(local_win, rocks[i][0] + 1, rocks[i][1] + 1, ROCK_SYMBOL);
	}
	wattroff(local_win, COLOR_PAIR(3));
	*/
	wrefresh(local_win);
}

void initcolors() {
	init_pair(1, COLOR_BLUE, COLOR_BLACK); //1: Blue on black background
	init_pair(2, COLOR_RED, COLOR_BLACK); //2: Red on black background
	init_pair(3, COLOR_GREEN, COLOR_BLACK); //3: Green on black background
	init_pair(4, COLOR_YELLOW, COLOR_BLACK); //4: Yellow on black background
	init_pair(5, COLOR_WHITE, COLOR_BLACK); //5: White on black
}

void togglegrab(WINDOW * status_win) {
	mvwprintw(status_win, 0, STATUS_WIDTH - 8, "        ");
	if (grabbing != 1) {
		mvwprintw(status_win, 0, STATUS_WIDTH - 8, "GRABBING");
		grabbing = 1;
	} else {
		grabbing = 0;
	}
	mp->setcmch(ant_y, ant_x, ANT_FACE);
	wrefresh(status_win);


}

void togglepush(WINDOW * status_win) {
	mvwprintw(status_win, 0, STATUS_WIDTH - 8, "        ");
	if (grabbing != 2) {
		mvwprintw(status_win, 0, STATUS_WIDTH - 7, "PUSHING");
		grabbing = 2;
	} else {
		grabbing = 0;
	}
	mp->setcmch(ant_y, ant_x, ANT_FACE);
	wrefresh(status_win);
}

void move_ant(int y, int x) {
//	int ri;

	if ((grabbing == 2) && (mp->getcmch(ant_y + y, ant_x + x) == ROCK_SYMBOL)) {
		if (mp->getcmch(ant_y + 2 * y, ant_x + 2 * x) != ' ') return;
		mp->setcmch(ant_y + 2 * y, ant_x + 2 * x, ROCK_SYMBOL);
		mp->setcmch(ant_y, ant_x, ' ');

	}  else {

		if (mp->getcmch(ant_y + y, ant_x + x) != ' ' && mp->getcmch(ant_y + y, ant_x + x) != DESTINATION_SYMBOL) return;
		

		mp->setcmch(ant_y, ant_x, ' ');

		if ((grabbing == 1) && (mp->getcmch(ant_y - y, ant_x - x) == ROCK_SYMBOL)) {
			mp->setcmch(ant_y, ant_x, ROCK_SYMBOL);
			mp->setcmch(ant_y - y, ant_x - x, ' ');

		}
	}

	ant_x += x;
	ant_y += y;

	
	mp->setcmch(ant_y, ant_x, ANT_FACE);


}

//int index_rock(int y, int x) {
//	for (int i = 0; i < ROCK_NUMBER; i++) {
//		if ((rocks[i][0] == y) && (rocks[i][1] == x)) return i;
//	}
//	return -1;
//}

void modwin(WINDOW * local_win) {
	int y, x;

	while (!mp->modified->empty()) {
		y = mp->modified->front().first;
		x = mp->modified->front().second;
		modchwin(local_win, y, x, mp->getcmch(y, x));
		mp->modified->pop();
	}
	wrefresh(local_win);
}

void modchwin(WINDOW * local_win, int y, int x, char ch) {
	
	int wa = (ch == ANT_FACE) ? (COLOR_PAIR((grabbing) ? 2 : 1) | A_BOLD) : ((ch == SLATER_SYMBOL) ? (COLOR_PAIR(4) | A_BOLD) : ((ch == ROCK_SYMBOL) ? (COLOR_PAIR(3)) : ((ch == DESTINATION_SYMBOL) ? (COLOR_PAIR(1) | A_STANDOUT) : (COLOR_PAIR(5)))));
	wattron(local_win, wa);
	mvwaddch(local_win, y + 1, x + 1, ch);
	wattroff(local_win, wa);

}
