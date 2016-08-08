#ifndef FUN_H
#define FUN_H

#define HASHSIZE (1<<21) // must be 2^nn
#ifndef MAPSIZE
#define MAPSIZE 15
#endif
#define ABS(x) \
	((x)<(0)?(-x):(x))

typedef long long LL;
typedef enum {HASHALPHA, HASHBETA, HASHEXACT} Type;

typedef struct{
	int player;
	int y, x;
}Coor;

typedef struct{
	LL check;
	Type type;
	int val;
	int depth;
}HashElem;

typedef struct{
	int g5;
	int l4, l3, rc4, rl3, l2, rl2, d4, d3, d2;
	int c4, m3;
}Points;

typedef struct{
	int y, x;
	int chose;
	int point[2]; /* 该点黑棋白棋得分 */
	int kill[2]; /* 该点情形，3 为成五，2 为绝杀，1 为双活三 */
}Subpoints;

extern int computerGo(int m[][MAPSIZE+5], int player, int &return_x, int &return_y, int &num, double &t, double &p);

#endif
