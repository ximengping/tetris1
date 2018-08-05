#ifndef __DEF_H__
#define __DEF_H__

#define BC 7

#define H 20
#define W 10

struct shape {
	int s[5][5];
};

struct data {
	int x;
	int y;
};

// 在网络中传递数据的结构体
struct info {
	int type; // 1. 
	int background[H][W];
	struct data t;
	struct shape s;
	int next;
        int grade3;
        int colour;
};

extern struct shape shape_arr[10];
extern struct data t;
extern int background[H][W];
extern int background1[H][W];
extern int background2[H][W];
extern int cur;
extern int next;
extern struct info g_info;
extern int nfd;
extern int colour;
extern int colour1;
extern int level;
extern int level1;
extern int grade;
extern int grade1;

#endif //__DEF_H__

