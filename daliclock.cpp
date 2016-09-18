// DALICLOCK.CPP
// Implements the actual drawing of the melting numbers
// Earle F. Philhower, III <earle@ziplabel.com>
// Most of this code was adapted from xdaliclock by Jamie Zawinski <jwz@jwz.org>
// 
// His copyright message is included below:
/* xscreensaver, Copyright (c) 1992, 1997 Jamie Zawinski <jwz@jwz.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  No representations are made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 */

#include "stdafx.h"
#include "WDaliClock.h"
#include "daliclock.h"

#undef MAX
#undef MIN
#define MAX(a, b) ((a)>(b)?(a):(b))
#define MIN(a, b) ((a)<(b)?(a):(b))



typedef struct raw_numer {
	unsigned char *bits;
	int width, height;
} raw_number;
#include "numbers0\zero0.xbm"
#include "numbers0\one0.xbm"
#include "numbers0\two0.xbm"
#include "numbers0\three0.xbm"
#include "numbers0\four0.xbm"
#include "numbers0\five0.xbm"
#include "numbers0\six0.xbm"
#include "numbers0\seven0.xbm"
#include "numbers0\eight0.xbm"
#include "numbers0\nine0.xbm"
#include "numbers0\colon0.xbm"
#include "numbers0\slash0.xbm"
static raw_number numbers_0 [] = {
	{ zero0_bits, zero0_width, zero0_height },
	{ one0_bits, one0_width, one0_height },
	{ two0_bits, two0_width, two0_height },
	{ three0_bits, three0_width, three0_height },
	{ four0_bits, four0_width, four0_height },
	{ five0_bits, five0_width, five0_height },
	{ six0_bits, six0_width, six0_height },
	{ seven0_bits, seven0_width, seven0_height },
	{ eight0_bits, eight0_width, eight0_height },
	{ nine0_bits, nine0_width, nine0_height },
	{ colon0_bits, colon0_width, colon0_height },
	{ slash0_bits, slash0_width, slash0_height },
	{ 0, }
};
#include "numbers1\zero1.xbm"
#include "numbers1\one1.xbm"
#include "numbers1\two1.xbm"
#include "numbers1\three1.xbm"
#include "numbers1\four1.xbm"
#include "numbers1\five1.xbm"
#include "numbers1\six1.xbm"
#include "numbers1\seven1.xbm"
#include "numbers1\eight1.xbm"
#include "numbers1\nine1.xbm"
#include "numbers1\colon1.xbm"
#include "numbers1\slash1.xbm"

static raw_number numbers_1 [] = {
	{ zero1_bits, zero1_width, zero1_height },
	{ one1_bits, one1_width, one1_height },
	{ two1_bits, two1_width, two1_height },
	{ three1_bits, three1_width, three1_height },
	{ four1_bits, four1_width, four1_height },
	{ five1_bits, five1_width, five1_height },
	{ six1_bits, six1_width, six1_height },
	{ seven1_bits, seven1_width, seven1_height },
	{ eight1_bits, eight1_width, eight1_height },
	{ nine1_bits, nine1_width, nine1_height },
	{ colon1_bits, colon1_width, colon1_height },
	{ slash1_bits, slash1_width, slash1_height },
	{ 0, }
};

#include "numbers2\zero2.xbm"
#include "numbers2\one2.xbm"
#include "numbers2\two2.xbm"
#include "numbers2\three2.xbm"
#include "numbers2\four2.xbm"
#include "numbers2\five2.xbm"
#include "numbers2\six2.xbm"
#include "numbers2\seven2.xbm"
#include "numbers2\eight2.xbm"
#include "numbers2\nine2.xbm"
#include "numbers2\colon2.xbm"
#include "numbers2\slash2.xbm"

static raw_number numbers_2 [] = {
	{ zero2_bits, zero2_width, zero2_height },
	{ one2_bits, one2_width, one2_height },
	{ two2_bits, two2_width, two2_height },
	{ three2_bits, three2_width, three2_height },
	{ four2_bits, four2_width, four2_height },
	{ five2_bits, five2_width, five2_height },
	{ six2_bits, six2_width, six2_height },
	{ seven2_bits, seven2_width, seven2_height },
	{ eight2_bits, eight2_width, eight2_height },
	{ nine2_bits, nine2_width, nine2_height },
	{ colon2_bits, colon2_width, colon2_height },
	{ slash2_bits, slash2_width, slash2_height },
	{ 0, }
};

#include "numbers3\zero3.xbm"
#include "numbers3\one3.xbm"
#include "numbers3\two3.xbm"
#include "numbers3\three3.xbm"
#include "numbers3\four3.xbm"
#include "numbers3\five3.xbm"
#include "numbers3\six3.xbm"
#include "numbers3\seven3.xbm"
#include "numbers3\eight3.xbm"
#include "numbers3\nine3.xbm"
#include "numbers3\colon3.xbm"
#include "numbers3\slash3.xbm"

static raw_number numbers_3 [] = {
	{ zero3_bits, zero3_width, zero3_height },
	{ one3_bits, one3_width, one3_height },
	{ two3_bits, two3_width, two3_height },
	{ three3_bits, three3_width, three3_height },
	{ four3_bits, four3_width, four3_height },
	{ five3_bits, five3_width, five3_height },
	{ six3_bits, six3_width, six3_height },
	{ seven3_bits, seven3_width, seven3_height },
	{ eight3_bits, eight3_width, eight3_height },
	{ nine3_bits, nine3_width, nine3_height },
	{ colon3_bits, colon3_width, colon3_height },
	{ slash3_bits, slash3_width, slash3_height },
	{ 0, }
};


COLORREF hsv_to_rgb (int h, double s, double v)
{
	double H, S, V, R, G, B;
	double p1, p2, p3;
	double f;
	int i;
	int r, g, b;

	if (s < 0) s = 0;
	if (v < 0) v = 0;
	if (s > 1) s = 1;
	if (v > 1) v = 1;

	S = s; V = v;
	H = (h % 360) / 60.0;
	i = (int)H;
	f = H - i;
	p1 = V * (1 - S);
	p2 = V * (1 - (S * f));
	p3 = V * (1 - (S * (1 - f)));
	if	  (i == 0) { R = V;  G = p3; B = p1; }
	else if (i == 1) { R = p2; G = V;  B = p1; }
	else if (i == 2) { R = p1; G = V;  B = p3; }
	else if (i == 3) { R = p1; G = p2; B = V;  }
	else if (i == 4) { R = p3; G = p1; B = V;  }
	else		   { R = V;  G = p1; B = p2; }
	r = (int)(R * 255.0);
	g = (int)(G * 255.0);
	b = (int)(B * 255.0);
	return RGB(r,g,b);
}

typedef short unsigned int POS;
#define MAX_SEGS_PER_LINE 20
typedef struct scanline {
	POS left[MAX_SEGS_PER_LINE];
	POS right[MAX_SEGS_PER_LINE];
} scanline;

typedef struct frame {
	scanline scanlines[1]; /* scanlines are contiguous here */
} frame;

inline int getbit(raw_number *num, int x, int y)
{
	int ret;
	ret = (!! ((num->bits) [((y) * ((num->width+7) >> 3)) + ((x) >> 3)] & (1 << ((x) & 7))));
	return ret;
}

static frame *image_to_frame(raw_number *image)
{
	register int y, x;
	struct frame *frame;
	int width = image->width;
	int height = image->height;
	POS *left, *right;
	int maxsegments = 0;

	frame = (struct frame *)
		malloc (sizeof (struct frame) +
		(sizeof (struct scanline) * (height - 1)));

	for (y = 0; y < height; y++)
	{
		int seg, end;
		x = 0;

		left = frame->scanlines[y].left;
		right = frame->scanlines[y].right;

		for (seg = 0; seg < MAX_SEGS_PER_LINE; seg++)
			left [seg] = right [seg] = width / 2;

		for (seg = 0; seg < MAX_SEGS_PER_LINE; seg++)
		{
			for (; x < width; x++)
				if (getbit(image, x, y)) break;
			if (x == width) break;
			left [seg] = x;
			for (; x < width; x++)
				if (! getbit(image, x, y)) break;
			right [seg] = x;
		}

		for (; x < width; x++)
			if (getbit(image, x, y))
			{
				exit (-1);
			}

			/* If there were any segments on this line, then replicate the last
			one out to the end of the line.  If it's blank, leave it alone,
			meaning it will be a 0-pixel-wide line down the middle.
			*/
			end = seg;
			if (end > 0)
				for (; seg < MAX_SEGS_PER_LINE; seg++)
				{
					left [seg] = left [end-1];
					right [seg] = right [end-1];
				}
				if (end > maxsegments) maxsegments = end;
	}
	return frame;
}


static frame *base_frames [12];
static frame *clear_frame;
int character_width, character_height;
int colon_char_width;

static void load_builtin_font(raw_number *nums)
{
	int i;	

	character_width = character_height = 0;

	for (i = 0; i < 12; i++) {
		raw_number *number = &nums [i];

		character_width = MAX (character_width, number->width);
		character_height = MAX (character_height, number->height);
		if (base_frames[i]) free(base_frames[i]);
		base_frames [i] = image_to_frame (number);
	}
	colon_char_width = MAX (nums [10].width, nums [11].width);
}

void initialize_digits(int fontno)
{
	raw_number *numbers;

	switch(fontno) {
		case 1: numbers=numbers_1; break;
		case 2: numbers=numbers_2; break;
		case 3: numbers=numbers_3; break;
		default: numbers=numbers_0; break;
	}
	load_builtin_font (numbers );

	if (clear_frame) free(clear_frame);
	clear_frame = (struct frame *)
		malloc (sizeof (struct frame) +
		(sizeof (struct scanline) * (character_height - 1)));
}



static void set_current_scanlines (frame *into_frame, frame *from_frame)
{
	register int i;
	for (i = 0; i < character_height; i++)
		into_frame->scanlines [i] = from_frame->scanlines [i];
}

static void one_step (frame *current_frame, frame *target_frame, int tick)
{
	register scanline *line = &current_frame->scanlines [0];
	register scanline *target = &target_frame->scanlines [0];
	register int i = 0, x;
	for (i = 0; i < character_height; i++)
	{
#define STEP(field) (line->field += ((int) (target->field - line->field)) / tick)
		for (x = 0; x < MAX_SEGS_PER_LINE; x++)
		{
			STEP (left [x]);
			STEP (right [x]);
		}
		line++;
		target++;
	}
}

static void draw_frame(HDC hdc, frame *frame, int x_off, int y_off, int width)
{
	register int y, x;
	for (y = 0; y < character_height; y++)
	{
		SelectObject(hdc, back);
		MoveToEx(hdc, x_off, y_off+y, NULL);
		LineTo(hdc, x_off+width, y_off+y);
		SelectObject(hdc, fore);
		struct scanline *line = &frame->scanlines [y];
		for (x = 0; x < MAX_SEGS_PER_LINE; x++) {
			if (line->left[x] == line->right[x] ||
				(x > 0 &&
				line->left[x] == line->left[x-1] &&
				line->right[x] == line->right[x-1]))
				continue;
			MoveToEx(hdc, x_off+line->left[x], y_off+y, NULL);
			LineTo(hdc, x_off+line->right[x], y_off+y);
		}
	}
}

int DrawColon(HDC hdc, int x, int y)
{
	draw_frame(hdc, base_frames[10], x, y,colon_char_width );
	return colon_char_width;
}

void DrawSpace(HDC hdc, int x, int y)
{
	int i;
	SelectObject(hdc, back);
	for (i = 0; i < character_height; i++)
	{
		MoveToEx(hdc, x , y + i, NULL);
		LineTo(hdc, x+character_width, y + i);
	}
}

void DrawStage(HDC hdc, int x, int y, int start, int end, int count)
{
	set_current_scanlines(clear_frame, base_frames[start]);
	if (count<10)
		for (int j=10; j>=count; j--)
			one_step(clear_frame, base_frames[end], 10-count);
	draw_frame(hdc, clear_frame, x, y, character_width);
}

void cycle_colors ()
{
	static int hue_tick=0;
	if (!trans) bc = hsv_to_rgb (hue_tick, 1.0, 1.0);
	else bc=RGB(255,255,255);
	fc = hsv_to_rgb ((hue_tick + 180) % 360, 1.0, 1.0);
	hue_tick = (hue_tick+1) % 360;
	DeleteObject(fore);
	DeleteObject(back);
	back=CreatePen(0,0,bc);
	fore=CreatePen(0,0,fc);
}
