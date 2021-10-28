#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>					//�����\���p
#include "glut.h"

#pragma warning (disable:4996)//���ɂ���Ă͋H�ɐ�����sprintf�������̃G���[���o���Ȃ�

#define ENEMY_NUM 3				//�G�̐�
#define AREA_NUM 256			//�쐬�\�ȋ�`�̏����

double a[2], b[2], c[2], tmp[2];

//�G�̃|�W�V�����ƐF�A�ړ��ʂ�\���O���[�o���ϐ�
double posEnemy[ENEMY_NUM][2];
GLfloat colEnemy[ENEMY_NUM][3];
double velEnemy[ENEMY_NUM][2];

//��`�̍��W
double posBegin[2];
double posEnd[2];
double posMouse[2];

//�쐬������`�̍��W���Ǘ�����O���[�o���ϐ�
double posArea[AREA_NUM][4];

//�L�[�̏�Ԃ�\���O���[�o���ϐ��i0�F������Ă��Ȃ��C1:������Ă���j
int key_F1;

//�X�R�A��ʗp�̃O���[�o���ϐ�
double area;								//�ʐϗp�O���[�o���ϐ�
double gametime;							//�c�莞�ԗp�O���[�o���ϐ�
int rect_num;								//�쐬������`�̌���\���O���[�o���ϐ�

											//�Q�[���I���t���O�p�O���[�o���ϐ�			�i0�F�t���O�������Ă��Ȃ��C1�ȏ�F�t���O�������Ă���j
int GameClearFlag;
int GameOverFlag;

//�����蔻��̃t���O���Ǘ�����O���[�o���ϐ��i0�F�Ԃ����Ă��Ȃ��C1�ȏ�F�Ԃ����Ă���j
int isCrushingEnemy;						//�G�Ƃ̓����蔻��
int isCrushingArea;							//�쐬�����̈�Ƃ̓����蔻��

											//��`��\���\����rectangle
struct rectangle
{
	double pos[2];				//����̒��_�̍��W
	double size[2];				//�T�C�Y
	double color[3];			//�F
	struct rectangle *next;		//���̍\���̂ւ̃����N
};

//�}�E�X�̃{�^���𗣂����Ƃ��ɕ`�悷���`
struct rectangle *newRect;

//�ŏ��̍\���̂̃|�C���^root�̐錾�Ə�����
struct rectangle *root;

//������
void init(void)
{
	int enemyId;
	int i;

	//�O���[�o���ϐ���������
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

	glClearColor(0.9, 0.9, 0.9, 1.0);			//�w�i�F���w��

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

//��`��`��
void drawRect(struct rectangle *r)
{
	glColor3d(r->color[0], r->color[1], r->color[2]);
	glBegin(GL_POLYGON);
	glVertex2d(r->pos[0], r->pos[1]);
	glVertex2d(r->pos[0] + r->size[0], r->pos[1]);
	glVertex2d(r->pos[0] + r->size[0], r->pos[1] - r->size[1]);
	glVertex2d(r->pos[0], r->pos[1] - r->size[1]);
	glEnd();

	//���̋�`������Ε`��
	if (r->next != NULL)
	{
		drawRect(r->next);
	}
}

//��`���ɂ��邩�ǂ������`�F�b�N����֐�
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

//�h���b�O���̋�`�ɂ��邩�ǂ������`�F�b�N����֐�
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

//�V������`����`���X�g�ɒǉ�����֐�insert
void insert(struct rectangle *p)
{
	struct rectangle *curRect;//�\���̃|�C���^curRect��錾

	if (root == NULL)
	{
		root = p;//�֐��ɓn���ꂽ�A�h���X��root�ɃR�s�[
		return;
	}

	curRect = root;

	while (curRect->next != NULL)
	{
		curRect = curRect->next;
	}

	curRect->next = p;
}

//�G�`��֐�
void drawEnemy(double posGiven[2], GLfloat color[3])
{
	glColor3d(color[0], color[1], color[2]);
	glPointSize(10);
	glBegin(GL_POINTS);
	glVertex2d(posGiven[0], posGiven[1]);
	glEnd();
}

//��ʂ�`��
void display(void)
{
	int enemyId;
	int i;

	//�e�L�X�g�p�ϐ�
	char str[99];
	char str2[99];

	glClear(GL_COLOR_BUFFER_BIT);//�o�b�t�@���N���A

								 //��`���X�g��`��
	if (root != NULL)
	{
		drawRect(root);
	}

	//�h���b�O���̋�`��`��
	if (newRect != NULL)
	{
		drawRect(newRect);
	}

	//�����̓G��`��
	for (enemyId = 0; enemyId < ENEMY_NUM; enemyId++)
	{
		glColor3d(colEnemy[enemyId][0], colEnemy[enemyId][1], colEnemy[enemyId][2]);
		drawEnemy(posEnemy[enemyId], colEnemy[enemyId]);
	}

	//�X�R�A��\��
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
			GameClearFlag++;		//�Q�[���N���A�t���O�����Z
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
			GameOverFlag++;		//�Q�[���I�[�o�[�t���O�����Z
	}

	//�o�b�t�@�����������i��ʂ�`��j
	glFlush();

	//�Q�[���I�[�o�[�ł��Q�[���N���A�ł��Ȃ��Ƃ��A�`��X�V���s��
	if (GameOverFlag < 2 && GameClearFlag < 2)
	{
		//�_�u���o�b�t�@������
		glutSwapBuffers();
	}
}

//�N���b�N���̃}�E�X�̏������s���֐�
void mouse(int button, int state, int x, int y)
{
	int i, enemyId;
	double tate, yoko, menseki = 0;

	//�}�E�X�̍��{�^���ȊO���������Ƃ��͖�������
	if (button != GLUT_LEFT_BUTTON)
	{
		return;
	}

	//�}�E�X�̍��{�^�����������Ƃ�
	if (state == GLUT_DOWN)
	{
		//��ԍ��W�\���i�[
		a[0] = (double)x / glutGet(GLUT_WINDOW_WIDTH) * 2 - 1;
		a[1] = -(double)y / glutGet(GLUT_WINDOW_HEIGHT) * 2 + 1;

		if (checkArea(a) == 0)
		{
			posBegin[0] = a[0];
			posBegin[1] = a[1];
			//�X�N���[�����W�\��
			printf("button pressed (%d, %d) -> (%f, %f)\n", x, y, a[0], a[1]);
		}
		else
		{
			//�w�i�F�����F�ɂ���
			printf("�d�˂ċ�`��`�����Ƃ͂ł��܂���\n");
			glClearColor(1.0, 1.0, 0.6, 1.0);
		}
	}
	//�}�E�X�̍��{�^���𗣂����Ƃ�
	else if (state == GLUT_UP)
	{
		//�w�i�F������������
		glClearColor(0.9, 0.9, 0.9, 1.0);

		if (checkArea(a) == 0)
		{
			glClearColor(0.9, 0.9, 0.9, 1.0);

			//��ԍ��W�\���i�[
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

			//�X�N���[�����W�\��
			printf("button released (%d, %d) -> (%f, %f)\n\n", x, y, posEnd[0], posEnd[1]);
			//�h���b�O���̍��W��������
			posMouse[0] = -3;
			posMouse[1] = -3;
			//��ԍ��W��posArea�Ɋi�[(posArea[0] < posArea[1]�AposArea[2] < posArea[3]�ɂȂ�悤�ɓ����)
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

			//�V�����쐬�����̈���ɓG��������Q�[���I�[�o�[
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

			//�쐬�����ʐς̌����P���₷
			rect_num++;

			menseki += tate * yoko;

			//�}�`����ʂ��߂銄��
			area += menseki * 25;

			//��ԍ��W�ɐV������`�m�[�h���쐬
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
			//���̍\���̂ւ̃����N��������
			newRect->next = NULL;

			//�V�����\����newRect�̃A�h���X���֐�insert�ɓn��
			insert(newRect);
		}
	}
	glutPostRedisplay();			//glutDisplayFunc()�Ŏw�肳�ꂽ�֐����Ăяo��
}

//�h���b�O���̃}�E�X�̏������s���֐�
void motion(int x, int y)
{
	int i;
	int enemyId;

	if (checkArea(a) == 0)
	{
		c[0] = (double)x / glutGet(GLUT_WINDOW_WIDTH) * 2 - 1;
		c[1] = -(double)y / glutGet(GLUT_WINDOW_HEIGHT) * 2 + 1;

		//c���̈�O�ł���Ƃ��C���̋�`��`��
		if (isCrushingArea == 0)
		{
			posMouse[0] = c[0];
			posMouse[1] = c[1];

			printf("posMouse[0] = %f : posMouse[1] = %f\n", posMouse[0], posMouse[1]);

			//��`�`��Ɋւ������newRect�Ɋi�[
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
				newRect->color[i] = 0.3;						//���̍\���̂ւ̃����N��������
			}

			newRect->next = NULL;
		}
		//���̋�`���̈���ł������Ƃ�
		if (checkArea(c) == 1)
		{
			isCrushingArea++;

			printf("�d�˂ċ�`��`�����Ƃ͂ł��܂���\n");
			glClearColor(1.0, 1.0, 0.6, 1.0);

			//��`�`��Ɋւ������newRect�Ɋi�[
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
				newRect->color[i] = 0.3;						//���̍\���̂ւ̃����N��������
			}

			newRect->next = NULL;
		}
		//���̋�`����ʊO�ɂ���Ƃ�
		if (c[0] > 1.0 || c[0]<-1.0 || c[1] > 1.0 || c[1] < -1.0)
		{
			isCrushingArea++;

			printf("��ʊO�ɋ�`��`�����Ƃ͂ł��܂���\n");
			glClearColor(1.0, 1.0, 0.6, 1.0);

			//��`�`��Ɋւ������newRect�Ɋi�[
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
				newRect->color[i] = 0.3;						//���̍\���̂ւ̃����N��������
			}

			newRect->next = NULL;
		}

		//�h���b�O���̋�`�ƓG�Ƃ̓����蔻��
		for (enemyId = 0; enemyId < ENEMY_NUM; enemyId++)
		{
			if (checkDrag(posEnemy[enemyId]) == 2)
			{
				isCrushingEnemy++;
			}
		}
	}
}

//����L�[�������ꂽ�ۂ̏������s���֐����w��
void checkSpecialKeyPressed(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_F1:key_F1 = 1; break;
	}
}

//����L�[���߂��ꂽ�ۂ̏������s���֐����w��
void checkSpecialKeyReleased(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_F1:key_F1 = 0; break;
	}
}

//��莞�Ԃ��Ƃɍĕ`��
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

	//�G�̈ʒu���X�V
	for (enemyId = 0; enemyId < ENEMY_NUM; enemyId++)
	{
		for (i = 0; i < 2; i++)
		{
			posEnemy[enemyId][i] += velEnemy[enemyId][i];

			//��ʂ̒[�܂ňړ������ꍇ�͕�����]��
			if (posEnemy[enemyId][i] >= 1.0) velEnemy[enemyId][i] *= -1.0;
			if (posEnemy[enemyId][i] <= -1.0) velEnemy[enemyId][i] *= -1.0;
		}

		if (checkArea(posEnemy[enemyId]) != 0)
		{
			//�G�̍��W���������炷
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
				if (checkArea(posEnemydy1) == 0 || checkArea(posEnemydy2) == 0)			//�̈�̊p�ɓ����������̔���
				{
					velEnemy[enemyId][0] *= -1.0;
					velEnemy[enemyId][1] *= -1.0;
				}
				else
				{
					velEnemy[enemyId][0] *= -1.0;										//�̈�̍��E�ɓ����������̔���
				}
			}
			else
			{
				velEnemy[enemyId][1] *= -1.0;											//�̈�̏㉺�ɓ����������̔���
			}
		}
	}

	gametime -= 0.05;

	//��ʂ��ĕ`��
	glutPostRedisplay();
	//��莞�ԁi�~���b�j��ɂ��̊֐������s
	glutTimerFunc(50, updatePos, 0);
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);							//GLUT���C�u������������

	srand((unsigned)time(NULL));

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);	//�\�����[�h���_�u���o�b�t�@�����O�ɐݒ�
	glutCreateWindow(argv[0]);						//�E�B���h�E�̐���
	glutDisplayFunc(display);						//�E�B���h�E�̕`����s���֐����w��
	glutTimerFunc(1000, updatePos, 0);				//��莞�Ԍ�Ɏ��s����֐����w��

													//�}�E�X�������֐����w��
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	//����L�[�������ꂽ�i�����ꂽ�j�ۂ̏������s���֐����w��
	glutSpecialFunc(checkSpecialKeyPressed);
	glutSpecialUpFunc(checkSpecialKeyReleased);

	init();//������

	glutMainLoop();//���C�����[�v

	return EXIT_SUCCESS;
}
