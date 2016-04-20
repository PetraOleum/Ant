/*
 * =====================================================================================
 *
 *       Filename:  cmap.h
 *
 *    Description:  Header file for the map
 *
 *        Version:  1.0
 *        Created:  26/03/16 10:38:49
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Petra (P.L.), oleumpetra@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CMAP_H
#define CMAP_H

#include <utility>
#include <queue>

#define ABS(A) ((A < 0) ? (-(A)) : (A))

class cmap {
	private:
		char ** imap;
	public:
		int width, height;
		char getcmch(int y, int x);
		void setcmch(int y, int x, char c);

		std::queue< std::pair<int, int> > * modified;

		cmap(int h, int w);
		~cmap();


		void clearmq();

		int LOS(int y1, int x1, int y2, int x2);

};




#endif
