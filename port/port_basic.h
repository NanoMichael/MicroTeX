#ifndef PORT_BASIC_H_INCLUDED
#define PORT_BASIC_H_INCLUDED

#include <string>
#include <sstream>
#include "common.h"

using namespace std;
using namespace tex;

namespace tex {
namespace port {

typedef unsigned int color;

static const color trans   = 0x00000000;
static const color black   = 0xff000000;
static const color white   = 0xffffffff;
static const color red     = 0xffff0000;
static const color green   = 0xff00ff00;
static const color blue    = 0xff0000ff;
static const color yellow  = 0xffffff00;
static const color cyan    = 0xff00ffff;
static const color magenta = 0xffff00ff;

static const color TRANS   = trans;
static const color BLACK   = black;
static const color WHITE   = white;
static const color RED     = red;
static const color GREEN   = green;
static const color BLUE    = blue;
static const color YELLOW  = yellow;
static const color CYAN    = cyan;
static const color MAGENTA = magenta;

inline color rgb(int r, int g, int b) {
	return (0xff << 24) | (r << 16) | (g << 8) | b;
}

inline color rgb(float r, float g, float b) {
	return rgb((int)(r * 255), (int)(g * 255), (int)(b * 255));
}

inline bool istrans(color c) {
	return (c >> 24 == 0);
}

inline color decode(const string& s) {
	if (s[0] == '#') {
		const string x = s.substr(1);
		stringstream ss;
		ss << std::hex << x;
		color c;
		ss >> c;
		if (s.size() == 7) {
			// set alpha value
			c |= 0xff000000;
		} else if (s.size() != 9) {
			return black;
		}
		return c;
	}
	return black;
}

struct Point {

	float x, y;

	Point() :
		x(0), y(0) {
	}

	Point(float x1, float y1) :
		x(x1), y(y1) {
	}
};

struct Rect {

	float x, y, w, h;

	Rect() :
		x(0), y(0), w(0), h(0) {
	}

	Rect(float x1, float y1, float w1, float h1) :
		x(x1), y(y1), w(w1), h(h1) {
	}
};

struct Insets {

	int left, top, right, bottom;

	Insets() :
		left(0), top(0), right(0), bottom(0) {
	}

	Insets(int t, int l, int b, int r) :
		left(t), top(t), right(r), bottom(b) {
	}

	void set(int t, int l, int b, int r) {
		left = l;
		top = t;
		right = r;
		bottom = b;
	}
};

enum Cap { CAP_BUTT, CAP_ROUND, CAP_SQUARE };
enum Join { JOIN_BEVEL, JOIN_MITER, JOIN_ROUND };

struct Stroke {

	float lineWidth;
	float miterLimit;
	Cap cap;
	Join join;

	Stroke() :
		lineWidth(1.f), cap(CAP_ROUND), join(JOIN_ROUND), miterLimit(0) {
	}

	Stroke(float w, Cap c, Join j, float ml = 0) :
		lineWidth(w), cap(c), join(j), miterLimit(ml) {
	}

	void setStroke(float w, Cap c, Join j, float ml = 0) {
		lineWidth = w;
		cap = c;
		join = j;
		miterLimit = ml;
	}
};

}
}

#endif // PORT_BASIC_H_INCLUDED
