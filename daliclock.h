// DALICLOCK.H
// Implements the actual drawing of the melting numbers
// Earle F. Philhower, III <earle@ziplabel.com>
// Most of this code was adapted from xdaliclock by Jamie Zawinski <jwz@jwz.org>
// 
extern int character_width, character_height;
extern int colon_char_width;
COLORREF hsv_to_rgb (int h, double s, double v);
void initialize_digits(int fontno);
int DrawColon(HDC hdc, int x, int y);
void DrawSpace(HDC hdc, int x, int y);
void DrawStage(HDC hdc, int x, int y, int start, int end, int count);
void cycle_colors ();


// Global variables
extern HPEN back, fore;
extern COLORREF bc;
extern COLORREF fc;
extern int trans;
extern int top;
extern int cycle;
extern int fontid;
