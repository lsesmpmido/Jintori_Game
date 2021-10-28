#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>					//文字表示用
#include "glut.h"

#pragma warning (disable:4996)//環境によっては稀に生じるsprintfが原因のエラーを出さない

#define ENEMY_NUM 3				//敵の数
#define AREA_NUM 256			//作成可能な矩形の上限個数

double a[2], b[2], c[2], tmp[2];

//敵のポジションと色、移動量を表すグローバル変数
double posEnemy[ENEMY_NUM][2];
GLfloat colEnemy[ENEMY_NUM][3];
double velEnemy[ENEMY_NUM][2];

//矩形の座標
double posBegin[2];
double posEnd[2];
double posMouse[2];

//作成した矩形の座標を管理するグローバル変数
double posArea[AREA_NUM][4];

//キーの状態を表すグローバル変数（0：押されていない，1:押されている）
int key_F1;

//スコア画面用のグローバル変数
double area;								//面積用グローバル変数
double gametime;							//残り時間用グローバル変数
int rect_num;								//作成した矩形の個数を表すグローバル変数

											//ゲーム終了フラグ用グローバル変数			（0：フラグが立っていない，1以上：フラグが立っている）
int GameClearFlag;
int GameOverFlag;

//当たり判定のフラグを管理するグローバル変数（0：ぶつかっていない，1以上：ぶつかっている）
int isCrushingEnemy;						//敵との当たり判定
int isCrushingArea;							//作成した領域との当たり判定

											//矩形を表す構造体rectangle
struct rectangle
{
	double pos[2];				//左上の頂点の座標
	double size[2];				//サイズ
	double color[3];			//色
	struct rectangle *next;		//次の構造体へのリンク
};

//マウスのボタンを離したときに描画する矩形
struct rectangle *newRect;

//最初の構造体のポインタrootの宣言と初期化
struct rectangle *root;

//初期化
void init(void)
{
	int enemyId;
	int i;

	//グローバル変数を初期化
	gametime = 100;
	area = 0;
	rect_num = 0;
	GameClearFlag = 0;
	GameOverFlag = 0;
	isCrushingEnemy = 0;
	isCrushingArea = 0;
	newRect = NULL;
	root = NULL;
	key_F1 = 0;

	glClearColor(0.9, 0.9, 0.9, 1.0);			//背景色を指定

	for (enemyId = 0; enemyId < ENEMY_NUM; enemyId++)
	{
		for (i = 0; i < 2; i++)
		{
			posEnemy[enemyId][i] = 2.0*rand() / RAND_MAX - 1.0;
			velEnemy[enemyId][i] = 0.05*pow(-1, rand() / 2);
		}
		for (i = 0; i < ENEMY_NUM; i++)
		{
			colEnemy[enemyId][i] = (double)rand() / RAND_MAX - 0.2;
		}
	}
}

//矩形を描画
void drawRect(struct rectangle *r)
{
	glColor3d(r->color[0], r->color[1], r->color[2]);
	glBegin(GL_POLYGON);
	glVertex2d(r->pos[0], r->pos[1]);
	glVertex2d(r->pos[0] + r->size[0], r->pos[1]);
	glVertex2d(r->pos[0] + r->size[0], r->pos[1] - r->size[1]);
	glVertex2d(r->pos[0], r->pos[1] - r->size[1]);
	glEnd();

	//次の矩形があれば描画
	if (r->next != NULL)
	{
		drawRect(r->next);
	}
}

//矩形内にいるかどうかをチェックする関数
int checkArea(double pos[])
{
	int i;

	for (i = 0; i < rect_num; i++)
	{
		if (posArea[i][0] <= pos[0] && posArea[i][1] >= pos[0])
		{
			if (posArea[i][2] <= pos[1] && posArea[i][3] >= pos[1])
			{
				return 1;
			}
		}
	}

	return 0;
}

//ドラッグ中の矩形にいるかどうかをチェックする関数
int checkDrag(double pos[])
{
	int c = 0;

	if (posBegin[0] < posMouse[0])
	{
		if ((posBegin[0] < pos[0]) && (pos[0] < posMouse[0]))
		{
			c++;
		}
	}
	else
	{
		if ((posMouse[0] < pos[0]) && (pos[0] < posBegin[0]))
		{
			c++;
		}
	}

	if (posBegin[1] < posMouse[1])
	{
		if ((posBegin[1] < pos[1]) && (pos[1] < posMouse[1]))
		{
			c++;
		}
	}
	else
	{
		if ((posMouse[1] < pos[1]) && (pos[1] < posBegin[1]))
		{
			c++;
		}
	}

	return c;
}

//新しい矩形を矩形リストに追加する関数insert
void insert(struct rectangle *p)
{
	struct rectangle *curRect;//構造体ポインタcurRectを宣言

	if (root == NULL)
	{
		root = p;//関数に渡されたアドレスをrootにコピー
		return;
	}

	curRect = root;

	while (curRect->next != NULL)
	{
		curRect = curRect->next;
	}

	curRect->next = p;
}

//敵描画関数
void drawEnemy(double posGiven[2], GLfloat color[3])
{
	glColor3d(color[0], color[1], color[2]);
	glPointSize(10);
	glBegin(GL_POINTS);
	glVertex2d(posGiven[0], posGiven[1]);
	glEnd();
}

//画面を描画
void display(void)
{
	int enemyId;
	int i;

	//テキスト用変数
	char str[99];
	char str2[99];

	glClear(GL_COLOR_BUFFER_BIT);//バッファをクリア

								 //矩形リストを描画
	if (root != NULL)
	{
		drawRect(root);
	}

	//ドラッグ中の矩形を描画
	if (newRect != NULL)
	{
		drawRect(newRect);
	}

	//複数の敵を描画
	for (enemyId = 0; enemyId < ENEMY_NUM; enemyId++)
	{
		glColor3d(colEnemy[enemyId][0], colEnemy[enemyId][1], colEnemy[enemyId][2]);
		drawEnemy(posEnemy[enemyId], colEnemy[enemyId]);
	}

	//スコアを表示
	sprintf(str, "Time: %3.0f area: %.2f%% rect_num: %03d", gametime, area, rect_num);
	glColor3f(0.1, 0.1, 0.1);
	glRasterPos2i(-1.0, -1.0);
	for (i = 0; i < strlen(str); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, str[i]);
	}

	if (area >= 65)
	{
		sprintf(str, "Game Clear!");
		glColor3f(0.5, 1.0, 0.5);
		glRasterPos2f(-0.35, 0.1);
		for (i = 0; i < strlen(str); i++)
		{
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
		}

		sprintf(str, "\nYou Get Territoty : %.2f%%", area);
		glColor3f(0.5, 1.0, 0.5);
		glRasterPos2f(-0.5, -0.1);
		for (i = 0; i < strlen(str); i++)
		{
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[i]);
		}

		sprintf(str, "CLICK \" F1 \" TO RETRY");
		glColor3f(0.5, 1.0, 0.5);
		glRasterPos2f(-0.7, -0.5);
		for (i = 0; i < strlen(str); i++)
		{
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
		}

		if (GameClearFlag <= 2)
			GameClearFlag++;		//ゲームクリアフラグを加算
	}
	else if (gametime <= 0 || isCrushingEnemy > 0)
	{
		sprintf(str, "Game Over!");
		glColor3f(0.9, 0.1, 0.1);
		glRasterPos2f(-0.35, 0.1);
		for (i = 0; i < strlen(str); i++)
		{
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
		}

		sprintf(str, "\nYou Get Territoty : %.2f%%", area);
		glColor3f(0.9, 0.1, 0.1);
		glRasterPos2f(-0.5, -0.1);
		for (i = 0; i < strlen(str); i++)
		{
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[i]);
		}

		sprintf(str, "Necessary territory for GAME CLEAR : %.2f%%", 65.0 - area);
		glColor3f(0.9, 0.1, 0.1);
		glRasterPos2f(-0.9, -0.2);
		for (i = 0; i < strlen(str); i++)
		{
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[i]);
		}

		sprintf(str, "CLICK \" F1 \" TO RETRY");
		glColor3f(0.9, 0.1, 0.1);
		glRasterPos2f(-0.7, -0.5);
		for (i = 0; i < strlen(str); i++)
		{
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
		}

		if (GameOverFlag <= 2)
			GameOverFlag++;		//ゲームオーバーフラグを加算
	}

	//バッファを書きだす（画面を描画）
	glFlush();

	//ゲームオーバーでもゲームクリアでもないとき、描画更新を行う
	if (GameOverFlag < 2 && GameClearFlag < 2)
	{
		//ダブルバッファを交換
		glutSwapBuffers();
	}
}

//クリック時のマウスの処理を行う関数
void mouse(int button, int state, int x, int y)
{
	int i, enemyId;
	double tate, yoko, menseki = 0;

	//マウスの左ボタン以外を押したときは無視する
	if (button != GLUT_LEFT_BUTTON)
	{
		return;
	}

	//マウスの左ボタンを押したとき
	if (state == GLUT_DOWN)
	{
		//空間座標表示格納
		a[0] = (double)x / glutGet(GLUT_WINDOW_WIDTH) * 2 - 1;
		a[1] = -(double)y / glutGet(GLUT_WINDOW_HEIGHT) * 2 + 1;

		if (checkArea(a) == 0)
		{
			posBegin[0] = a[0];
			posBegin[1] = a[1];
			//スクリーン座標表示
			printf("button pressed (%d, %d) -> (%f, %f)\n", x, y, a[0], a[1]);
		}
		else
		{
			//背景色を黄色にする
			printf("重ねて矩形を描くことはできません\n");
			glClearColor(1.0, 1.0, 0.6, 1.0);
		}
	}
	//マウスの左ボタンを離したとき
	else if (state == GLUT_UP)
	{
		//背景色を初期化する
		glClearColor(0.9, 0.9, 0.9, 1.0);

		if (checkArea(a) == 0)
		{
			glClearColor(0.9, 0.9, 0.9, 1.0);

			//空間座標表示格納
			b[0] = (double)x / glutGet(GLUT_WINDOW_WIDTH) * 2 - 1;
			b[1] = -(double)y / glutGet(GLUT_WINDOW_HEIGHT) * 2 + 1;

			if (isCrushingArea == 0)
			{
				posEnd[0] = b[0];
				posEnd[1] = b[1];
			}
			else
			{
				isCrushingArea = 0;
				posEnd[0] = posMouse[0];
				posEnd[1] = posMouse[1];
			}

			//スクリーン座標表示
			printf("button released (%d, %d) -> (%f, %f)\n\n", x, y, posEnd[0], posEnd[1]);
			//ドラッグ中の座標を初期化
			posMouse[0] = -3;
			posMouse[1] = -3;
			//空間座標をposAreaに格納(posArea[0] < posArea[1]、posArea[2] < posArea[3]になるように入れる)
			if (posBegin[0] < posEnd[0])
			{
				posArea[rect_num][0] = posBegin[0];
				posArea[rect_num][1] = posEnd[0];
				yoko = posEnd[0] - posBegin[0];

			}
			else
			{
				posArea[rect_num][0] = posEnd[0];
				posArea[rect_num][1] = posBegin[0];
				yoko = posBegin[0] - posEnd[0];
			}

			if (posBegin[1] < posEnd[1])
			{
				posArea[rect_num][2] = posBegin[1];
				posArea[rect_num][3] = posEnd[1];

				tate = posEnd[1] - posBegin[1];
			}
			else
			{
				posArea[rect_num][2] = posEnd[1];
				posArea[rect_num][3] = posBegin[1];

				tate = posBegin[1] - posEnd[1];
			}

			//新しく作成した領域内に敵がいたらゲームオーバー
			for (enemyId = 0; enemyId < ENEMY_NUM; enemyId++)
			{
				if (posArea[rect_num][0]<posEnemy[enemyId][0] && posArea[rect_num][1] > posEnemy[enemyId][0])
				{
					if (posArea[rect_num][2]<posEnemy[enemyId][1] && posArea[rect_num][3] > posEnemy[enemyId][1])
					{
						isCrushingEnemy++;
					}
				}
			}

			//作成した面積の個数を１つ増やす
			rect_num++;

			menseki += tate * yoko;

			//図形が画面を占める割合
			area += menseki * 25;

			//空間座標に新しい矩形ノードを作成
			newRect = (struct rectangle *)malloc(sizeof(struct rectangle));
			for (i = 0; i < 2; i++)
			{
				newRect->pos[i] = posBegin[i];
				newRect->size[i] = posEnd[i] - posBegin[i];
				if (i == 1)
				{
					newRect->size[i] *= -1;
				}
			}
			for (i = 0; i < 3; i++)
			{
				newRect->color[i] = (double)rand() / RAND_MAX - 0.2;
			}
			//次の構造体へのリンクを初期化
			newRect->next = NULL;

			//新しい構造体newRectのアドレスを関数insertに渡す
			insert(newRect);
		}
	}
	glutPostRedisplay();			//glutDisplayFunc()で指定された関数を呼び出し
}

//ドラッグ中のマウスの処理を行う関数
void motion(int x, int y)
{
	int i;
	int enemyId;

	if (checkArea(a) == 0)
	{
		c[0] = (double)x / glutGet(GLUT_WINDOW_WIDTH) * 2 - 1;
		c[1] = -(double)y / glutGet(GLUT_WINDOW_HEIGHT) * 2 + 1;

		//cが領域外であるとき，その矩形を描画
		if (isCrushingArea == 0)
		{
			posMouse[0] = c[0];
			posMouse[1] = c[1];

			printf("posMouse[0] = %f : posMouse[1] = %f\n", posMouse[0], posMouse[1]);

			//矩形描画に関する情報をnewRectに格納
			newRect = (struct rectangle *)malloc(sizeof(struct rectangle));
			for (i = 0; i < 2; i++)
			{
				newRect->pos[i] = posBegin[i];
				newRect->size[i] = posMouse[i] - posBegin[i];
				if (i == 1)
				{
					newRect->size[i] *= -1;
				}
			}
			for (i = 0; i < 3; i++)
			{
				newRect->color[i] = 0.3;						//次の構造体へのリンクを初期化
			}

			newRect->next = NULL;
		}
		//その矩形が領域内であったとき
		if (checkArea(c) == 1)
		{
			isCrushingArea++;

			printf("重ねて矩形を描くことはできません\n");
			glClearColor(1.0, 1.0, 0.6, 1.0);

			//矩形描画に関する情報をnewRectに格納
			newRect = (struct rectangle *)malloc(sizeof(struct rectangle));
			for (i = 0; i < 2; i++)
			{
				newRect->pos[i] = posBegin[i];
				newRect->size[i] = posMouse[i] - posBegin[i];
				if (i == 1)
				{
					newRect->size[i] *= -1;
				}
			}
			for (i = 0; i < 3; i++)
			{
				newRect->color[i] = 0.3;						//次の構造体へのリンクを初期化
			}

			newRect->next = NULL;
		}
		//その矩形が画面外にいるとき
		if (c[0] > 1.0 || c[0]<-1.0 || c[1] > 1.0 || c[1] < -1.0)
		{
			isCrushingArea++;

			printf("画面外に矩形を描くことはできません\n");
			glClearColor(1.0, 1.0, 0.6, 1.0);

			//矩形描画に関する情報をnewRectに格納
			newRect = (struct rectangle *)malloc(sizeof(struct rectangle));
			for (i = 0; i < 2; i++)
			{
				newRect->pos[i] = posBegin[i];
				newRect->size[i] = posMouse[i] - posBegin[i];
				if (i == 1)
				{
					newRect->size[i] *= -1;
				}
			}
			for (i = 0; i < 3; i++)
			{
				newRect->color[i] = 0.3;						//次の構造体へのリンクを初期化
			}

			newRect->next = NULL;
		}

		//ドラッグ中の矩形と敵との当たり判定
		for (enemyId = 0; enemyId < ENEMY_NUM; enemyId++)
		{
			if (checkDrag(posEnemy[enemyId]) == 2)
			{
				isCrushingEnemy++;
			}
		}
	}
}

//特殊キーが押された際の処理を行う関数を指定
void checkSpecialKeyPressed(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_F1:key_F1 = 1; break;
	}
}

//特殊キーが戻された際の処理を行う関数を指定
void checkSpecialKeyReleased(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_F1:key_F1 = 0; break;
	}
}

//一定時間ごとに再描画
void updatePos(int value)
{
	int enemyId;
	int i;

	double posEnemydx1[2];
	double posEnemydx2[2];
	double posEnemydy1[2];
	double posEnemydy2[2];

	if (key_F1 == 1)
	{
		init();
	}

	//敵の位置を更新
	for (enemyId = 0; enemyId < ENEMY_NUM; enemyId++)
	{
		for (i = 0; i < 2; i++)
		{
			posEnemy[enemyId][i] += velEnemy[enemyId][i];

			//画面の端まで移動した場合は方向を転換
			if (posEnemy[enemyId][i] >= 1.0) velEnemy[enemyId][i] *= -1.0;
			if (posEnemy[enemyId][i] <= -1.0) velEnemy[enemyId][i] *= -1.0;
		}

		if (checkArea(posEnemy[enemyId]) != 0)
		{
			//敵の座標を少しずらす
			posEnemydx1[0] = posEnemy[enemyId][0] + 0.1;
			posEnemydx1[1] = posEnemy[enemyId][1];
			posEnemydx2[0] = posEnemy[enemyId][0] - 0.1;
			posEnemydx2[1] = posEnemy[enemyId][1];
			posEnemydy1[0] = posEnemy[enemyId][0];
			posEnemydy1[1] = posEnemy[enemyId][1] + 0.1;
			posEnemydy2[0] = posEnemy[enemyId][0];
			posEnemydy2[1] = posEnemy[enemyId][1] - 0.1;

			if (checkArea(posEnemydx1) == 0 || checkArea(posEnemydx2) == 0)
			{
				if (checkArea(posEnemydy1) == 0 || checkArea(posEnemydy2) == 0)			//領域の角に当たった時の反射
				{
					velEnemy[enemyId][0] *= -1.0;
					velEnemy[enemyId][1] *= -1.0;
				}
				else
				{
					velEnemy[enemyId][0] *= -1.0;										//領域の左右に当たった時の反射
				}
			}
			else
			{
				velEnemy[enemyId][1] *= -1.0;											//領域の上下に当たった時の反射
			}
		}
	}

	gametime -= 0.05;

	//画面を再描画
	glutPostRedisplay();
	//一定時間（ミリ秒）後にこの関数を実行
	glutTimerFunc(50, updatePos, 0);
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);							//GLUTライブラリを初期化

	srand((unsigned)time(NULL));

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);	//表示モードをダブルバッファリングに設定
	glutCreateWindow(argv[0]);						//ウィンドウの生成
	glutDisplayFunc(display);						//ウィンドウの描画を行う関数を指定
	glutTimerFunc(1000, updatePos, 0);				//一定時間後に実行する関数を指定

													//マウスを扱う関数を指定
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	//特殊キーが押された（離された）際の処理を行う関数を指定
	glutSpecialFunc(checkSpecialKeyPressed);
	glutSpecialUpFunc(checkSpecialKeyReleased);

	init();//初期化

	glutMainLoop();//メインループ

	return EXIT_SUCCESS;
}
