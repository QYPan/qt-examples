#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "fun.h"
#define JUDGE_EDGE(y, x) \
	((y < 0 || y > 14 || x < 0 || x > 14) ? 0 : 1)

/* cpoint[i][j][2] 表示电脑得分 */
/* cpoint[i][j][3] 表示玩家得分 */
/* cpoint[i][j][0] 表示电脑绝杀棋情况 */
/* cpoint[i][j][1] 表示玩家绝杀棋情况 */

int cpoint[MAPSIZE+5][MAPSIZE+5][4];
const int G5 = 20000;
const int LIVE4 = 2000;
const int C4 = 300; /* 冲四 */
const int RC4 = 250; /* 跳冲四 */
const int LIVE3 = 450;
const int RLIVE3 = 300;
const int LIVE2 = 100;
const int RLIVE2 = 70;
const int M3 = 50;
const int DEAD4 = -10;
const int DEAD3 = -10;
const int DEAD2 = -10;
const int inf = 9000000;
const int unknow = 9900000;
int DEPTH;
int counter;
int find;

HashElem hashtable[HASHSIZE];
LL zobrist[3][20][20];
int state[MAPSIZE+5][MAPSIZE+5];
int comy, comx;

LL rand14()
{
	double val = rand()/(RAND_MAX+1.0);
	return (LL)(val * 10e14);
}

void init_zobrist()
{
	int i, j, k;
	for(i = 0; i < 3; i++){
		for(j = 0; j < 20; j++){
			for(k = 0; k < 20; k++){
				zobrist[i][j][k] = rand14();
			}
		}
	}
}

void init_time()
{
	static int flag = 0;
	if(!flag){
		srand((unsigned int)time(NULL));
		flag = 1;
	}
}

int compare(const void* _a, const void* _b)
{
	Subpoints *a = (Subpoints *)_a;
	Subpoints *b = (Subpoints *)_b;
	return ((b->point[0] + b->point[1]) - (a->point[0] + a->point[1]));
}

/* 计算某点在一个方向上的棋形  */

void cal_chess(Points *po, Coor *co, int d1, int d2)
{
	int player = co->player;
	int y = co->y;
	int x = co->x;
	int i, j, mid = 0;
	int lchess[2], rchess[2];
	int lempty[2], rempty[2];
	lchess[0] = lchess[1] = lempty[0] = lempty[1] = 0;
	rchess[0] = rchess[1] = rempty[0] = rempty[1] = 0;
	i = y - d1;
	j = x - d2;
	while(JUDGE_EDGE(i, j) && state[i][j] == player) { lchess[0]++; i -= d1; j -= d2; }
	while(JUDGE_EDGE(i, j) && state[i][j] == -1) { lempty[0]++; i -= d1; j -= d2; }
	while(JUDGE_EDGE(i, j) && state[i][j] == player) { lchess[1]++; i -= d1; j -= d2; }
	while(JUDGE_EDGE(i, j) && state[i][j] == -1) { lempty[1]++; i -= d1; j -= d2; }
	i = y + d1;
	j = x + d2;
	while(JUDGE_EDGE(i, j) && state[i][j] == player) { rchess[0]++; i += d1; j += d2; }
	while(JUDGE_EDGE(i, j) && state[i][j] == -1) { rempty[0]++; i += d1; j += d2; }
	while(JUDGE_EDGE(i, j) && state[i][j] == player) { rchess[1]++; i += d1; j += d2; }
	while(JUDGE_EDGE(i, j) && state[i][j] == -1) { rempty[1]++; i += d1; j += d2; }
	mid = lchess[0] + rchess[0] + 1;
	if(mid >= 5) po->g5++; /* ooooo */
	else if(mid == 4){
		if(lempty[0] >= 1 && rempty[0] >= 1) po->l4++; /* xoooox */
		else if(lempty[0] + rempty[0]) po->c4++;
		else po->d4++;
	}
	else if(mid == 3){
		int ok = 0; /* 同一个方向上如果可形成活三和冲四，舍弃活三  */
		if((lempty[0] == 1 && lchess[1] >= 1) || (rempty[0] == 1 && rchess[1] >= 1)){
			po->rc4++;
			ok = 1;
		}
		if(!ok && lempty[0] + rempty[0] >= 3 && lempty[0] >= 1 && rempty[0] >= 1){
			po->l3++;
			ok = 1;
		}
		else if(lempty[0] + rempty[0])
			po->m3++;
		else
			po->d3++;
	}
	else if(mid == 2){
		int ok = 0;
		if((lempty[0] == 1 && lchess[1] >= 2) || (rempty[0] == 1 && rchess[1] >= 2)){
			po->rc4++;
			ok = 1;
		}
		if(!ok && ((lempty[0] == 1 && lchess[1] == 1 && rempty[0] >= 1 && lempty[1] >= 1)
				|| (rempty[0] == 1 && rchess[1] == 1 && lempty[0] >= 1 && rempty[1] >= 1))){
			po->rl3++;
		}
		else if((lempty[0] == 1 && lchess[1] == 1 && rempty[0] + lempty[1] >= 1)
				|| (rempty[0] == 1 && rchess[1] == 1 && lempty[0] + rempty[1] >= 1))
			po->m3++;
		if(lempty[0] + rempty[0] >= 4 && lempty[0] >= 1 && rempty[0] >= 1)
			po->l2++;
		else if(lempty[0] + rempty[0] == 0)
			po->d2++;
	}
	else if(mid == 1){
		int ok = 0;
		if((lempty[0] == 1 && lchess[1] >= 3) || (rempty[0] == 1 && rchess[1] >= 3)){
			po->rc4++;
			ok = 1;
		}
		if(!ok && ((lempty[0] == 1 && lchess[1] == 2 && rempty[0] >= 1 && lempty[1] >= 1)
				|| (rempty[0] == 1 && rchess[1] == 2 && lempty[0] >= 1 && rempty[1] >= 1))){
			po->rl3++;
		}
		else if((lempty[0] == 1 && lchess[1] == 2 && rempty[0] + lempty[1] >= 1)
				|| (rempty[0] == 1 && rchess[1] == 2 && lempty[0] + rempty[1] >= 1))
			po->m3++;
		if((lempty[0] == 1 && lchess[1] == 1 && rempty[0] + lempty[1] >= 3 && rempty[0] >= 1 && lempty[1] >= 1)
				|| (rempty[0] == 1 && rchess[1] == 1 && rempty[1] + lempty[0] >= 3 && lempty[0] >= 1 && rempty[1] >= 1)){
			po->rl2++;
		}
		if((lempty[0] == 2 && lchess[1] == 1 && rempty[0] >= 1 && lempty[1] >= 1)
				|| (rempty[0] == 2 && rchess[1] == 1 && lempty[0] >= 1 && rempty[1] >= 1)){
			po->rl2++;
		}
	}
}

/* 计算某玩家在某个点的分数  */
int get_points(Coor *co, int *kill)
{
	Points po;
	Coor tco;
	int player = co->player;
	int y = co->y;
	int x = co->x;
	int ans = 0;
	int nc4, nl3;
	*kill = 0;
	po.g5 = po.rc4 = po.l4 = po.l3 = po.rl3 = po.l2 = po.rl2 = po.c4 = po.m3 = po.d4 = po.d3 = po.d2 = 0;
	tco.player = player;
	tco.y = y;
	tco.x = x;

	cal_chess(&po, &tco, 1, 0);
	cal_chess(&po, &tco, 0, 1);
	cal_chess(&po, &tco, 1, 1);
	cal_chess(&po, &tco, -1, 1);

	nc4 = po.c4 + po.rc4;
	nl3 = po.l3 + po.rl3;
	if(po.g5 >= 1) { /* 成五 */
		*kill = 3;
		ans = G5;
	}
	else if(po.l4 >= 1 || nc4 >= 2 || (nc4 && nl3)){ /* 绝杀 */
		*kill = 2;
		ans = LIVE4;
	}
	else{
		if(nl3 >= 2) { /* 双活三 */
			*kill = 1;
		}
		ans = po.l3*LIVE3 + po.rl3*RLIVE3 + po.l2*LIVE2 + po.rl2*RLIVE2 + po.c4*C4 + po.rc4*RC4 + po.m3*M3 + po.d4*DEAD4 + po.d3*DEAD3 + po.d2*DEAD2;
	}
	return ans;
}

/* 计算某个点的分数  */
void cal_point(int y, int x)
{
	Coor tco;
	tco.y = y;
	tco.x = x;
	if(state[y][x] == -1){
		tco.player = 0;
		cpoint[y][x][2] = get_points(&tco, &cpoint[y][x][0]); /* 攻击力 */
		tco.player = 1;
		cpoint[y][x][3] = get_points(&tco, &cpoint[y][x][1]); /* 防御力 */
	}
}

/* 计算某玩家整个局面的分数  */
int cal_all_points(Coor *co, int *kill)
{
	int i, j, flag;
	int ans = 0;
	*kill = 0;
	if(co->player)
		flag = 3;
	else 
		flag = 2;
	for(i = 0; i < MAPSIZE; i++){
		for(j = 0; j < MAPSIZE; j++){
			if(state[i][j] == -1){
				ans += cpoint[i][j][flag];
				if(cpoint[i][j][flag-2] > (*kill)){
					*kill = cpoint[i][j][flag-2];
					co->y = i;
					co->x = j;
				}
			}
		}
	}
	return ans;
}

/* 改变点 (y, x) 4 个方向下可落子点的分数  */
void change_cpoint(int y, int x)
{
	int i, j;
	for(i = 0; i < MAPSIZE; i++){
		if(state[y][i] == -1){
			cal_point(y, i);
		}
		if(state[i][x] == -1){
			cal_point(i, x);
		}
	}
	for(i = 0; i < MAPSIZE; i++){
		j = i - (y - x);
		if(0 < j && j < MAPSIZE && state[i][j] == -1){
			cal_point(i, j);
		}
		j = (y + x) - i;
		if(0 < j && j < MAPSIZE && state[i][j] == -1){
			cal_point(i, j);
		}
	}
}

int set_order(Subpoints *od)
{
	int i, j;
	int n = 0;
	for(i = 0; i < MAPSIZE; i++){
		for(j = 0; j < MAPSIZE; j++){
			if(state[i][j] == -1){
				od[n].y = i;
				od[n].x = j;
				od[n].point[0] = cpoint[i][j][2];
				od[n].point[1] = cpoint[i][j][3];
				od[n].kill[0] = cpoint[i][j][0];
				od[n].kill[1] = cpoint[i][j][1];
				n++;
			}
		}
	}
#if 1
	qsort(od, n, sizeof(Subpoints), compare);
#endif
	return n;
}

LL cal_zobrist()
{
	LL z = 0;
	int i, j;
	for(i = 0; i < MAPSIZE; i++){
		for(j = 0; j < MAPSIZE; j++){
			if(state[i][j] == -1)
				z ^= zobrist[2][i][j];
			else if(state[i][j] == 0)
				z ^= zobrist[0][i][j];
			else if(state[i][j] == 1)
				z ^= zobrist[1][i][j];
		}
	}
	return z;
}

void init_hashtable()
{
	int i;
	find = 0;
	for(i = 0; i < HASHSIZE; i++)
		hashtable[i].val = unknow;
}

int find_in_hash(int depth, int alpha, int beta, LL st)
{
	int p = (st&(HASHSIZE-1));
	int val = hashtable[p].val;
	if(val == unknow)
		return val;
	if(hashtable[p].check == st){
		find++;
		if(hashtable[p].depth >= depth){
			if(hashtable[p].type == HASHEXACT){
				return val;
			}
			if(hashtable[p].type == HASHALPHA && val <= alpha){
				return alpha;
			}
			if(hashtable[p].type == HASHBETA && val >= beta){
				return beta;
			}
		}
	}
	return unknow;
}

void record_hash(int depth, int val, LL st, Type type)
{
	int p = (st&(HASHSIZE-1));
	hashtable[p].check = st;
	hashtable[p].val = val;
	hashtable[p].depth = depth;
	hashtable[p].type = type;
}

int alpha_beta(int player, int depth, int alpha, int beta, LL st)
{
	counter++; /* 记录搜索节点数 */
	Coor co[2];
	int i;
	int kill[2];
	co[0].player = 0;
	co[1].player = 1;
	int s1 = cal_all_points(&co[0], &kill[0]);
	int s2 = cal_all_points(&co[1], &kill[1]);
	int val;
	Type hashf = HASHALPHA;

#if 1
	if((val = find_in_hash(depth, alpha, beta, st)) != unknow)
		return val;
#endif

	if(depth == 0){ /* 达到搜索深度限制，返回估分 */
		int s = s1 - s2;
		record_hash(depth, s, st, HASHEXACT);
		if(((!player) && s >= 0) || (player && s < 0)) /* 如果 player 占优则返回正值，否则返回负值 */
			return ABS(s);
		if(((!player) && s < 0) || (player && s >= 0))
			return -ABS(s);
	}
	
	Subpoints sp[250];
	LL tst = st;
	int n = set_order(sp); /* 对候选点按高分到低分排序 */
	int y, x;

#if 1
	/* 己方可一步成五 || (对方不能一步成五 && 己方可一步成绝杀棋) ||  己方可一步成双活三而对方不能一步成绝杀棋*/
	int self = player;
	int opp = player^1;
	if(kill[self] == 3 || (kill[opp] < 3 && kill[self] == 2) || (kill[self] == 1 && kill[opp] < 2)){
		if(depth == DEPTH){
			comy = co[self].y;
			comx = co[self].x;
		}
		alpha = G5;
		return alpha;
	}
#endif

	for(i = 0; i < 25 && i < n; i++){ /* 最多选择 n 个候选点 */
		tst = st;
		y = sp[i].y;
		x = sp[i].x;
		state[y][x] = player; /* 在 (y, x) 落子 */
		st ^= zobrist[player][y][x];
		change_cpoint(y, x); /* (y, x) 四个方向上的得分受到影响，需要改变 */
		val = -alpha_beta(player^1, depth-1, -beta, -alpha, st);
		state[y][x] = -1;
		st ^= zobrist[player][y][x];
		change_cpoint(y, x);

		if(val > alpha){
			if(depth == DEPTH){
				comy = y;
				comx = x;
			}
			tst = st;
			hashf = HASHEXACT;
			alpha = val;
		}

		if(alpha >= beta){ /* 千万不能把等号去掉！！！ */
			record_hash(depth, beta, tst, HASHBETA);
			return beta;
		}
	}
	record_hash(depth, alpha, tst, hashf);
	return alpha;
}

int copy_and_cal_points(int m[][MAPSIZE+5])
{
	int i, j;
	int yes = 0;
	memset(cpoint, 0, sizeof(cpoint));
	for(i = 0; i < MAPSIZE; i++){
		for(j = 0; j < MAPSIZE; j++){
			state[i][j] = m[i][j];
		}
	}
	for(i = 0; i < MAPSIZE; i++){
		for(j = 0; j < MAPSIZE; j++){
			if(state[i][j] == -1){
				cal_point(i, j); /* 计算每个可落子点的分数 */
			}
			else yes++;
		}
	}
	return yes;
}

#if 0
int test(int player)
{
	Subpoints sp[300];
	Coor co, kb, kw;
	int n = 0;
	int i, j;
	int yes;
	int killb, killw;
	yes = copy_and_cal_points();
	if(!yes)
		return 0;
	for(i = 0; i < MAPSIZE; i++){
		for(j = 0; j < MAPSIZE; j++){
			int t = state[i][j];
			if(t == -1){
				sp[n].y = i;
				sp[n].x = j;
				co.y = i;
				sp[n].point[0] = cpoint[i][j][2];
				sp[n].point[1] = cpoint[i][j][3];
				sp[n].kill[0] = cpoint[i][j][0];
				sp[n].kill[1] = cpoint[i][j][1];
				n++;
			}
		}
	}
	qsort(sp, n, sizeof(Subpoints), compare);
	killb = killw = 0;
	for(i = 0 ; i < n; i++){
		if(sp[i].kill[0] > killb){
			killb = sp[i].kill[0];
			kb.y = sp[i].y;
			kb.x = sp[i].x;
		}
		if(sp[i].kill[1] > killw){
			killw = sp[i].kill[1];
			kw.y = sp[i].y;
			kw.x = sp[i].x;
		}
	}

	if(killb == 3) {comy = kb.y; comx = kb.x;}
	else if(killw == 3) {comy = kw.y; comx = kw.x;}
	else if(killb == 2) {comy = kb.y; comx = kb.x;}
	else if(killw == 2) {comy = kw.y; comx = kw.x;}
	else if(killb == 1) {comy = kb.y; comx = kb.x;}
	else if(killw == 1) {comy = kw.y; comx = kw.x;}
	else {comy = sp[0].y; comx = sp[0].x;}

	if(map[comy][comx] == EMPTY_POINT){
		draw_out_coor(j_cr, j_cc*2);
		j_cr = comy;
		j_cc = comx;
		set_chose(player);
		draw_coor(j_cr, j_cc*2);
	}
	return 1;
}
#endif

int computerGo(int m[][MAPSIZE+5], int player, int &return_x, int &return_y, int &num, double &t, double &p)
{
	LL st;
	int alpha = -inf;
	int beta = inf;
	int yes;

	yes = copy_and_cal_points(m);
	if(yes >= 225)  return 0;
	DEPTH = 6;

	comy = comx = 0;
	counter = 0;

	init_time();
	init_zobrist();
	init_hashtable();
	st = cal_zobrist();

	clock_t beg_t;
	clock_t end_t;
	double use_t;
	beg_t = clock();
	alpha_beta(player, DEPTH, alpha, beta, st); /* 搜索 */
	end_t = clock();

	use_t = (double)(end_t - beg_t) / CLOCKS_PER_SEC;

	return_x = comx;
	return_y = comy;
	num = counter;
	t = use_t;
	p = counter / t;

	return 1;
}
