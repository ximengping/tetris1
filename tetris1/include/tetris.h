#ifndef __TETRIS_H__
#define __TETRIS_H__

#include <def.h>

void draw_element(int x, int y, int c);
void draw_shape(int x, int y, struct shape p, int c);
void draw_shape1(int x, int y, struct shape p, int c);
void timer_tetris(struct data *p,int array[H][W],int level,int *gra);
void tetris(struct data *p,int array[H][W],int *gra);
void draw_back( int array[H][W]);
void draw_back1( );
void show_next(int nxt,int *gra );
void show_next1(int nxt,int *gra );
void set_back(int x, int y, struct shape shp,int array[H][W]);
int is_over(int array[H][W]);
void mclean( int array[H][W],int level,int *gra);
int can_move(int x, int y, struct shape shpe,int array[H][W]);

#endif // __TETRIS_H__

