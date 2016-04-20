/*
 * =====================================================================================
 *
 *       Filename:  slater.cpp
 *
 *    Description:  Code file for slater class
 *
 *        Version:  1.0
 *        Created:  26/03/16 13:34:51
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Petra (P.L.), oleumpetra@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "slater.h"
#include <map>
#include <queue>
#include <iostream>
#include <stdlib.h>
//#include "FronteirPriorityQueue.h"
#include <ncurses.h>

using namespace std;

int checkorder[][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {1, 1}, {1, -1}, {-1, 1}};

Slater::Slater(cmap * mp, int yp, int xp) {
	y = yp;
	x = xp;
	wmp = mp;
	mp->setcmch(y, x, SLATER_SYMBOL);
	path = new list < pair<int, int> >;
	state = 0;
	breaking = 0;
	alsx = alsy = 0;
}

Slater::~Slater() {
	clearpath();
	delete path;
}

void Slater::clearpath() {
	while (!path->empty()) path->pop_front();
}

// Defunct method
void Slater::astar(pair<int, int> goal) {
//	def_prog_mode();
//	endwin();
	cout << "Starting astar. Goal:" << goal.first << ":" << goal.second << endl;
	clearpath();
	map< pair<int, int>, pair<int, int> > * came_from = new map<pair<int, int>, pair<int, int> >;
	map< pair<int, int>, int> * cost_so_far = new map<pair<int, int>, int>;
	priority_queue<pair<int, pair<int, int> >, std::vector<pair<int, pair<int, int> > >, Compare > fronteir;
	pair<int, int> start = make_pair(y, x);
//	cout<< "Start: " << start.first << ":" << start.second << endl;
	fronteir.push(make_pair(0, start));
	(*came_from)[start] = make_pair(-1, -1);
	(*cost_so_far)[start] = 0;
	int suc = 0;
//	cout << fronteir.size() << " " << (*came_from)[start].first << "/" << (*came_from)[start].second << " " << (*cost_so_far)[start] << endl;
	while (!fronteir.empty()) {
		pair<int, pair<int, int> > current = fronteir.top();
		if (current.second == goal) {
			suc = 1;
			break;
		}

  		for (int i = 0; i < 8; i++) {
			pair<int, int> pn = make_pair(current.second.first + checkorder[i][0], current.second.second + checkorder[i][1]);
			int new_cost = 0;
			if ((wmp->getcmch(pn.first, pn.second) != '#') && (wmp->getcmch(pn.first, pn.second) != '\0') ) {
				new_cost = (*cost_so_far )[current.second] + 1 + i % 4;
			} else if (wmp->getcmch(pn.first, pn.second) == '#') {
				new_cost = (*cost_so_far )[current.second] + 4;
			}
//			if ((new_cost) && ((!(cost_so_far->count(pn))) || ((*cost_so_far)[pn] > new_cost))) {
			if ((new_cost) && (cost_so_far->find(pn) == cost_so_far->end())) {
//				cout << new_cost << " " << endl;
				(*cost_so_far)[pn] = new_cost;
				(*came_from)[pn] = current.second;
				fronteir.push(make_pair(new_cost, pn));
			}


		}
		fronteir.pop();
//		cout << fronteir.size() << endl;
	}

//  	if (came_from->find(goal) == came_from->end()) return;
	if (suc)  {

		pair<int, int> c2 = goal;
		cout << "Path: <" << c2.first << ", " << c2.second << ">";
		while (c2 != start) {
			path->push_front(c2);
			c2 = (*came_from )[c2];
			cout << ", <" << c2.first << ", " << c2.second << ">";
		}
	}
	cout << endl;
//	cout << "done \t" << goal.first << ":" << goal.second << " " << path->empty() << " \t" << start.first << ":" << start.second << "                  " << endl;

	delete cost_so_far;
	delete came_from;
//	reset_prog_mode();
//	getch();
//	refresh();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  Slater
 *      Method:  Slater :: act
 * Description:  Given position of target, generate path and move along it
 *--------------------------------------------------------------------------------------
 */
void Slater::act(int asy, int asx) {
//	cout << "made it 1" << endl;
	
	if (((asy != alsy) || (asx != alsx)) && (asy != -1) && (asx != -1)) {
		alsy = asy;
		alsx = asx;
		astar3(make_pair(alsy, alsx));
		state = 1;
	}

//	if (path->empty()) astar3(make_pair(asy, asx));
//	cout << "made it 2" << endl;
	int wesc = 15;
	while (path->empty() && wesc) {
		alsy = rand() % wmp->height;
		alsx = rand() % wmp->width;
		astar3(make_pair(alsy, alsx));
		state = 0;
		wesc--;
	}
	if (path->empty()) return;
//	cout << "made it 3" << endl;
	pair<int, int> nxt = path->front();
//	cout << "made it 3.0" << endl;
/* 	if (wmp->getcmch(nxt.first, nxt.second) != ' '){
 * 		clearpath();
 * 	}
 * 	else {
 * 		wmp->setcmch(y,x, ' ');
 * 		y = nxt.first;
 * 		x = nxt.second;
 * 		wmp->setcmch(y, x, SLATER_SYMBOL);
 * 		path->pop_front();
 * 	}
 */
//	cout << "made it 3.1" << endl;
	if (breaking) {
		if (wmp->getcmch(nxt.first, nxt.second) == '#') {
			wmp->setcmch(nxt.first, nxt.second, ' ');
		} else {
			breaking = 0;
		}
	} else if (wmp->getcmch(nxt.first, nxt.second) == ' ') {
//		cout << "made it 3.2a" << endl;
		wmp->setcmch(y, x, ' ');
		y = nxt.first;
		x = nxt.second;
		wmp->setcmch(y, x, SLATER_SYMBOL);
		path->pop_front();
		breaking = 0;
	} else if (wmp->getcmch(nxt.first, nxt.second) == '#') {
		breaking = 1;
	}
	else {
//		cout << "made it 3.2b" << endl;
		clearpath();
	}

//	cout << "made it 4" << endl;
}

void Slater::astar0(pair<int, int> goal) {
	pair<int, int> start = make_pair(y, x);
	clearpath();
	queue<pair<int, int> > fronteir;
	fronteir.push(start);
	map<pair<int, int>, pair<int, int> > came_from;
	pair<int, int> nowhere = make_pair(-1, -1);
	came_from[start] = nowhere;
	int success = 0;
	while (!fronteir.empty()) {
		pair<int, int> current = fronteir.front();
		fronteir.pop();
//		cout << current.first << ":" << current.second << "|" << fronteir.size() << "_____" << endl;
		if (current == goal) {
			success = 1;
//			cout << "Success" << endl;
			break;
		}
		for (int i = 0; i < 8; i++) {
			pair<int, int> next = make_pair(current.first + checkorder[i][0], current.second + checkorder[i][1]);
			char nc = wmp->getcmch(next.first, next.second);
			if((came_from.find(next) == came_from.end()) && ((nc != '\0') && (nc != '#'))) {
				fronteir.push(next);
				came_from[next] = current;
			}
		}

	}
	if (!success) return;
	

	pair<int, int> c2 = goal;
	while (c2 != start) {
		path->push_front(c2);
		c2 = came_from[c2];
	}
//	cout << path->size() << endl;

}

void Slater::astar3(pair<int, int> goal) {
	pair<int, int> start = make_pair(y, x);
	clearpath();
	priority_queue<pair<int, pair<int, int> >, std::vector<pair<int, pair<int, int> > >, Compare > fronteir;
	fronteir.push(make_pair(0, start));
	map<pair<int, int>, pair<int, int> > came_from;
	pair<int, int> nowhere = make_pair(-1, -1);
	came_from[start] = nowhere;
	map<pair<int, int>, int> cost_so_far;
	cost_so_far[start] = 0;
	int success = 0;
	while (!fronteir.empty()) {
		pair<int, pair<int, int> > current_state = fronteir.top();
		fronteir.pop();
		pair<int, int> current = current_state.second;
		int current_cost = current_state.first;
		if (current == goal) {
			success = 1;
			break;
		}
		for (int i = 0; i < 8; i++) {
			pair<int, int> next = make_pair(current.first + checkorder[i][0], current.second + checkorder[i][1]);
			char nc = wmp->getcmch(next.first, next.second);
			int nccost = (nc == ' ' || nc == '@') ? (1) : ((nc == '#') ? (4) : (0));
			if (!nccost) continue;
			int new_cost = nccost + current_cost + (i / 4);
			if (cost_so_far.find(next) == cost_so_far.end()) {
				fronteir.push(make_pair(new_cost, next));
				came_from[next] = current;
				cost_so_far[next] = new_cost;
			} 
			else if (cost_so_far[next] > (nccost + current_cost)) {
				fronteir.push(make_pair(nccost + current_cost, next));
				came_from[next] = current;
				cost_so_far[next] = nccost + current_cost;
			}


		}
	}
	if (!success) return;
	

	pair<int, int> c2 = goal;
	while (c2 != start) {
		path->push_front(c2);
		c2 = came_from[c2];
	}

	

}

void Slater::visualastar(pair<int, int> goal, WINDOW * win) {
	pair<int, int> start = make_pair(y, x);
	mvwprintw(win, 0, 0, "Goal: %d:%d; Start: %d:%d", goal.first, 
			goal.second, start.first, start.second);
	wrefresh(win);
	clearpath();
	queue<pair<int, int> > fronteir;
	fronteir.push(start);
	map<pair<int, int>, pair<int, int> > came_from;
	pair<int, int> nowhere = make_pair(-1, -1);
	came_from[start] = nowhere;
	int success = 0;
	while (!fronteir.empty()) {
		pair<int, int> current = fronteir.front();
		char current_char = wmp->getcmch(current.first, current.second);
		mvwaddch(win, current.first + 1, current.second + 1, current_char | A_REVERSE);
		wrefresh(win);
		fronteir.pop();
//		cout << current.first << ":" << current.second << "|" << fronteir.size() << "_____" << endl;
		if (current == goal) {
			success = 1;
			mvwaddch(win, current.first + 1, current.second + 1, current_char);
//			cout << "Success" << endl;
			break;
		}
		for (int i = 0; i < 4; i++) {
			pair<int, int> next = make_pair(current.first + checkorder[i][0], current.second + checkorder[i][1]);
			char nc = wmp->getcmch(next.first, next.second);
			mvwaddch(win, next.first + 1, next.second + 1, nc | A_REVERSE);
			wrefresh(win);
			if((came_from.find(next) == came_from.end()) && ((nc != '\0') && (nc != '#'))) {
				fronteir.push(next);
				came_from[next] = current;
			}
			mvwaddch(win, next.first + 1, next.second + 1, nc);
		}
		mvwaddch(win, current.first + 1, current.second + 1, current_char);

	}
	wrefresh(win);

	if (!success) return;
	

	pair<int, int> c2 = goal;
	wattron(win, COLOR_PAIR(2));
	while (c2 != start) {
		mvwaddch(win, c2.first + 1, c2.second + 1, '*');
		wrefresh(win);
		path->push_front(c2);
		c2 = came_from[c2];
	}
	wattroff(win, COLOR_PAIR(2));
//	cout << path->size() << endl;

}

void Slater::visualastar3(pair<int, int> goal, WINDOW * win) {
	pair<int, int> start = make_pair(y, x);
	clearpath();
	priority_queue<pair<int, pair<int, int> >, std::vector<pair<int, pair<int, int> > >, std::greater<pair<int, pair<int, int> > > > fronteir;
	fronteir.push(make_pair(0, start));
	map<pair<int, int>, pair<int, int> > came_from;
	came_from[start] = start;
	map<pair<int, int>, int> cost_so_far;
	cost_so_far[start] = 0;
	int success = 0;
	while (!fronteir.empty()) {
		pair<int, pair<int, int> > current_state = fronteir.top();
		pair<int, int> current = current_state.second;
		char current_char = wmp->getcmch(current.first, current.second);
		mvwaddch(win, current.first + 1, current.second + 1, current_char | A_REVERSE);
		wrefresh(win);
		int current_cost = current_state.first;
		if (current == goal) {
			success = 1;
			mvwaddch(win, current.first + 1, current.second + 1, current_char);
			break;
		}
		fronteir.pop();
		for (int i = 0; i < 8; i++) {
			pair<int, int> next = make_pair(current.first + checkorder[i][0], current.second + checkorder[i][1]);
			if (((next.first < 0) || (next.second <0)) || ((next.first >= wmp->height) || (next.second >= wmp->width))) continue;
			char nc = wmp->getcmch(next.first, next.second);
			mvwaddch(win, next.first + 1, next.second + 1, nc | A_REVERSE);
			wrefresh(win);
			int nccost = (nc == ' ' || nc == '@') ? (1) : ((nc == '#') ? (4) : (0));
//			int nccost = (nc == ' ') ? (1) : (0);
			if (!nccost) continue;
			int new_cost = nccost + current_cost + (i / 4);
			if (cost_so_far.find(next) == cost_so_far.end()) {
				fronteir.push(make_pair(new_cost, next));
				came_from[next] = current;
				cost_so_far[next] = new_cost;
			} 
			else if (cost_so_far[next] > (nccost + current_cost)) {
//				cout << "replacing" << endl;
				fronteir.push(make_pair(nccost + current_cost, next));
				came_from[next] = current;
				cost_so_far[next] = nccost + current_cost;
			}
			mvwaddch(win, next.first + 1, next.second + 1, nc);


		}
		mvwaddch(win, current.first + 1, current.second + 1, current_char);
	}
	mvwprintw(win, 0, 0, "Goal: %d:%d; Start: %d:%d; GCF: %d:%d", goal.first, goal.second, start.first, start.second, came_from[goal].first, came_from[goal].second);
	wrefresh(win);
	if (!success) return;
	

	pair<int, int> c2 = goal;
	wattron(win, COLOR_PAIR(2));
	while (c2 != start) {
		mvwaddch(win, c2.first + 1, c2.second + 1, '*');
		wrefresh(win);
		path->push_front(c2);
		c2 = came_from[c2];
	}
	wattroff(win, COLOR_PAIR(2));

	

}
