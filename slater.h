/*
 * =====================================================================================
 *
 *       Filename:  slater.h
 *
 *    Description:  Header file for slater class
 *
 *        Version:  1.0
 *        Created:  26/03/16 13:35:07
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Petra (P.L.), oleumpetra@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef SLATER_H
#define SLATER_H

#include <utility>
#include <list>
#include "cmap.h"
#include <ncurses.h>

#define SLATER_SYMBOL '0'

class Slater {
	private:
		std::list< std::pair<int, int> > * path;
		cmap * wmp;

		void astar(std::pair<int, int> goal);
		void astar0(std::pair<int, int> goal);
		void astar3(std::pair<int, int> goal);
		void visualastar(std::pair<int, int>, WINDOW * win);
		void visualastar3(std::pair<int, int>, WINDOW * win);
		void clearpath();

		int alsx;
		int alsy;

	public:
		int x, y;
		int state;
		int breaking;

		void act(int asy, int asx);

		Slater(cmap * mp, int yp, int xp);
		~Slater();



};

class Compare {
	public:
		bool operator () (std::pair<int, std::pair<int, int> > a, std::pair<int, std::pair<int, int> > b) {
			return (a.first > b.first);
		}
};


#endif
