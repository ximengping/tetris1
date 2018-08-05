#include <stdio.h>
#include <tetris.h>
#include <def.h>
#include <keyboard.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

//绘制单个方块
void draw_element(int x, int y, int c)
{
	x *= 2;
	x++;
	y++;

	printf("\033[%d;%dH", y, x);
	printf("\033[3%dm\033[4%dm", c, c);
	printf("[]");
	fflush(stdout);
	printf("\033[?25l");
	printf("\033[0m");
}

//绘制方块
void draw_shape(int x, int y, struct shape p, int c)
{
	for (int i=0; i<5; i++) {
		for (int j=0; j<5; j++) {
			if ( p.s[i][j] != 0 ) {
				draw_element(x+j, y+i, c);
			}
		}
	}
}

//回显绘制方块
void draw_shape1(int x, int y, struct shape p, int c)
{
        for (int i=0; i<5; i++) {
                for (int j=0; j<5; j++) {
                        if ( p.s[i][j] != 0 ) {
                                draw_element(x+j+20, y+i, c);
                        }
                }
        }
}


//绘制背景
void draw_back( int array[H][W])
{
	for (int i=0; i<H; i++) {
		for (int j=0; j<W; j++){
			if ( array[i][j] != 0 )
				draw_element(j, i, array[i][j]);
			else
				draw_element(j, i, BC);
		}
	}
}

void draw_back1()
{
   for (int i=0; i<H; i++) {
                for (int j=20; j<W+20; j++){
                        if ( background2[i][j-20] != 0 )
                                draw_element(j, i, colour1);
                        else
                                draw_element(j, i, BC);
                }
        }

}


//判断能否移动
int can_move(int x, int y, struct shape shpe,int array[H][W])
{
	for (int i=0; i<5; i++) {
		for (int j=0; j<5; j++) {
			if ( shpe.s[i][j] == 0 )
				continue;
			if ( x+j >= W )//加上=，坐标原点为（1，1）
				return 0;
			if ( x+j < 0 )
				return 0;
			if ( y+i >= H )
				return 0;
			if ( array[y+i][x+j] != 0 )
				return 0;
		}
	}

	return 1;
}

//存储障碍物
void set_back(int x, int y, struct shape shp,int array[H][W])
{
	for (int i=0; i<5; i++){
		for (int j=0; j<5; j++) {
			if ( shp.s[i][j] != 0 )
				array[y+i][x+j] = colour;
		}
	}
}

//消行
void mclean( int array[H][W],int level,int *gra)
{
	for (int i=0; i<H; i++) {
		int total = 0;
		for (int j=0; j<W; j++) {
			if ( array[i][j] != 0 )
				total ++;
		}
		if ( total == W ) {
                       switch(level){
                         case 1:(*gra)+=10;
                         case 2:(*gra)+=20;
                         case 3:(*gra)+=30;
                         default:break;
                       }
			for (int k = i; k>0; k--)
				memcpy(array[k], array[k-1], sizeof array[k-1] );
			memset(array[0], 0x00, sizeof array[0]);
		}
	}
}

//游戏结束
int is_over(int array[H][W])
{
  for (int i=0;i<W;i++)
     if(array[1][i]!=0)
        return 1;
  return 0;
}

//定时下落
void timer_tetris(struct data *p,int array[H][W],int level,int *gra)
{
	if ( can_move(p->x,p->y+1, shape_arr[cur],array)  ) {
		draw_shape(p->x, p->y, shape_arr[cur], BC);
		p->y++;
		draw_shape(p->x, p->y, shape_arr[cur], colour);
           
                 if ( nfd != -1 ) {
                        memset(&g_info, 0x00, sizeof g_info);
                        g_info.type = htonl(1);
                        g_info.s = shape_arr[cur];
                        g_info.t = *p;
                        g_info.next = next;
                        g_info.grade3=*gra;
                        g_info.colour=colour;
                        if ( write(nfd, &g_info, sizeof g_info) == -1 ) {
                                recover_keyboard(),perror("write"),exit(1);
                        }
                }

	} else {
		set_back(p->x, p->y, shape_arr[cur],array);
		mclean(array,level,gra);
		draw_back(array);
                if ( nfd != -1 ) {
                        memset(&g_info, 0x00, sizeof g_info);
                        g_info.type = htonl(2);
                        g_info.next = next;
                        g_info.colour=colour;
                        g_info.grade3=*gra;
                        memcpy(g_info.background, (void*)array, sizeof(g_info.background));
                        if ( write(nfd, &g_info, sizeof g_info) == -1 ) {
                                recover_keyboard(),perror("write"),exit(1);
                        }
                }

               
                if(is_over(array)==1){
                 printf("\033[?25h");
                 recover_keyboard();
                 exit(1);
            }


		p->x = 0;
		p->y = 0;
		cur = next;
		next = rand() % 10;
                colour=rand()%6+1;
		show_next(next,gra);
	}
}

//预显下一个
void show_next(int nxt,int *gra)
{
	for (int i=0; i<5; i++) {
		for (int j=0; j<5; j++) {
			if ( shape_arr[nxt].s[i][j] == 0 )
				draw_element(W+2+j, 2+i, BC);
			else
				draw_element(W+2+j, 2+i, colour);
		}
	}
        printf("\033[%d;%dH\033[?25l", 8,25);
        printf("\033[3%dm\033[4%dm", colour, BC);
        printf("成绩是：%d\n",*gra);
}


void show_next1(int nxt,int *gra)
{
        for (int i=0; i<5; i++) {
                for (int j=0; j<5; j++) {
                        if ( shape_arr[nxt].s[i][j] == 0 )
                                draw_element(W+22+j, 2+i, BC);
                        else
                                draw_element(W+22+j, 2+i, colour);
                }
        }
         printf("\033[%d;%dH\033[?25l", 8,65);
        printf("\033[3%dm\033[4%dm", colour, BC);
        printf("成绩是：%d\n",*gra);

}


//向右旋转90度
static struct shape turn_90(struct shape p)
{
	struct shape t;

	for (int i=0; i<5; i++) {
		for (int j=0; j<5; j++) {
			t.s[i][j] = p.s[4-j][i];
		}
	}
	
	return t;
}

//上下左右控制
void tetris(struct data *p,int array[H][W],int *gra)
{
	int c = get_key();

	if ( is_left(c) ) {
		draw_shape(p->x, p->y, shape_arr[cur], BC);
		if ( can_move(p->x-1,p->y, shape_arr[cur],array)  )
			p->x--;
		draw_shape(p->x, p->y, shape_arr[cur], colour);
                  if ( nfd != -1 ) {
                        memset(&g_info, 0x00, sizeof g_info);
                        g_info.type = htonl(1);
                        g_info.s = shape_arr[cur];
                        g_info.t = *p;
                        g_info.next = next;
                        g_info.grade3=*gra;
                        g_info.colour=colour;
                        if ( write(nfd, &g_info, sizeof g_info) == -1 ) {
                                perror("write");
                        }
        }
	}
	if ( is_right(c) ) {
		draw_shape(p->x, p->y, shape_arr[cur], BC);
		if ( can_move(p->x+1,p->y, shape_arr[cur],array)  )
			p->x++;
		draw_shape(p->x, p->y, shape_arr[cur], colour);
                if ( nfd != -1 ) {
                        memset(&g_info, 0x00, sizeof g_info);
                        g_info.type = htonl(1);
                        g_info.s = shape_arr[cur];
                        g_info.t = *p;
                        g_info.next = next;
                        g_info.grade3=*gra;
                        g_info.colour=colour;
                        if ( write(nfd, &g_info, sizeof g_info) == -1 ) {
                                perror("write");
                        }
        }

	}
	if ( is_up(c) ) {
		draw_shape(p->x, p->y, shape_arr[cur], BC);
		shape_arr[cur] = turn_90(shape_arr[cur]);
		if ( 0 == can_move(p->x,p->y, shape_arr[cur],array) ) {
			shape_arr[cur] = turn_90(shape_arr[cur]);
			shape_arr[cur] = turn_90(shape_arr[cur]);
			shape_arr[cur] = turn_90(shape_arr[cur]);
		}
		draw_shape(p->x, p->y, shape_arr[cur], colour);
                if ( nfd != -1 ) {
                        memset(&g_info, 0x00, sizeof g_info);
                        g_info.type = htonl(1);
                        g_info.s = shape_arr[cur];
                        g_info.t = *p;
                        g_info.next = next;
                        g_info.grade3=*gra;
                        g_info.colour=colour;
                        if ( write(nfd, &g_info, sizeof g_info) == -1 ) {
                                perror("write");
                        }
        }

	}
	if ( is_down(c) ) {
		draw_shape(p->x, p->y, shape_arr[cur], BC);
		if ( can_move(p->x,p->y+1, shape_arr[cur],array)  )
			p->y++;
		draw_shape(p->x, p->y, shape_arr[cur], colour);
                if ( nfd != -1 ) {
                        memset(&g_info, 0x00, sizeof g_info);
                        g_info.type = htonl(1);
                        g_info.s = shape_arr[cur];
                        g_info.t = *p;
                        g_info.next = next;
                        g_info.grade3=*gra;
                        g_info.colour=colour;
                        if ( write(nfd, &g_info, sizeof g_info) == -1 ) {
                                perror("write");
                        }
        }

	}
	if ( is_esc(c) ) {
		printf("\033[?25h");
		recover_keyboard();
		exit(0);
	}
}

