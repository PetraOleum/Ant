/*
 * =====================================================================================
 *
 *       Filename:  cmap.cpp
 *
 *    Description:  cmap class
 *
 *        Version:  1.0
 *        Created:  26/03/16 10:58:27
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Petra (P.L.), oleumpetra@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "cmap.h"

using namespace std;

char cmap::getcmch(int y, int x) {
	if ((y < 0) || (y >= height) || (x < 0) || (x >= width)) return '\0';
	return imap[y][x];
}

void cmap::setcmch(int y, int x, char c) {
	if ((y < 0) || (y >= height) || (x < 0) || (x >= width)) return;
	imap[y][x] = c;
	modified->push(std::make_pair(y, x));
}

cmap::cmap(int h, int w) {
	modified = new queue< pair<int, int> >;
	imap = new char*[h];
	for (int i = 0; i < h; i++) {
		imap[i] = new char[w];
		for (int q = 0; q < w; q++) imap[i][q] = ' ';
	}
	width = w;
	height = h;

}

cmap::~cmap() {
	while(!modified->empty()) modified->pop();
	delete modified;
	for (int i = 0; i < height; i++) delete imap[i];
	delete imap;

}

void cmap::clearmq() {
	while (!modified->empty()) modified->pop();
}

int cmap::LOS(int y1, int x1, int y2, int x2) {
	//Something wrong here
	if ((y1 == y2) && (x1 == x2)) return 1; //Yes, you can see your own square
	int yd = y2 - y1; //The distance between y1 and y2, signed
	int xd = x2 - x1; //Ditto for x1 and x2
	int yc = y1; //Stores "current" value for y
	int xc = x1; //Current x value

	int yb = (yd != 0) ? (yd / ABS(yd)) : 0; /* -1, 0, or +1 */
	int xb = (xd != 0) ? (xd / ABS(xd)) : 0;

	if (x1 == x2) {
		while (ABS(yc - y2)) {
			if (getcmch(yc, xc) == '#') return 0;
			yc += yb;
		}
		return 1;
	} else if (y1 == y2) {
		while (ABS(xc - x2)) {
			if (getcmch(yc, xc) == '#') return 0;
			xc += xb;
		}
		return 1;
	}
	
	int ya = 0;
	int xa = 0;

	if (ABS(yd) > ABS(xd)) 
		ya = yb;
	else 
		xa = xb;

	double tm = (double)(y2 - y1) / (double)(x2 - x1);

	while (ABS(yc - y2) || ABS(xc - x2)) {
		if (getcmch(yc, xc) == '#') return 0;
		if (xc == x2) yc += yb;
		else if (yc == y2) xc += xb;
		else {
			double ma = double(y2 - (yc + ya)) / double(x2 - (xc + xa));
			double mb = double(y2 - (yc + yb)) / double(x2 - (xc + xb));
			if (ABS(mb - tm) < ABS(ma - tm)) {
				yc += yb;
				xc += xb;

			} else {
				yc += ya;
				xc += xa;
			}
		}
	}
	return 1;
}
