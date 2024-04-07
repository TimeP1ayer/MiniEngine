///////////////////////////////////////////////////////////////////////////////
// ����: Mini 3D Engine
// �汾: 1.2.2021-05-27.2100
// 
// ����: ���
// ���ʣ�lizheng AT gdut.edu.cn
// ��λ�������ѧԺ���㶫��ҵ��ѧ
//
// ���������GNU GPLv3��Ȩ������ http://www.gnu.org/copyleft/gpl.html��
// �㶫��ҵ��ѧ��Ȩ���У�2018-2021�ꡣ��������Ȩ����
// 
// �÷����������д���������������
//        > MiniEngine [filename].obj
// 
// �����ѡ�������Ŀ���Եġ����ԡ������������������Ϊ��$(TargetDir)/models/bunny.obj����
//       ������ Visual V++ �У�ʹ��Releas/Debug������ʱ��ֱ�ӵ���bunny.obj��
//
///////////////////////////////////////////////////////////////////////////////
//
// Project: Mini 3D Engine
// Version: 1.1.2020-05-10.0030
// 
// Author: LI Zheng
// E-Mail: lizheng AT gdut.edu.cn
// Affiliation: School of Computers, Guangdong University of Technology
//
// Licensed under the GNU GPLv3. See http://www.gnu.org/copyleft/gpl.html.
// Copyright: 2020 Guangdong University of Technology. All rights reserved.
// 
// Usage: In command line window, enter the following command
//        > MiniEngine [filename].obj
// 
///////////////////////////////////////////////////////////////////////////////
// 
//			Patience is a virtue.
//			Possess it if you can.
//			Seldom found in woman.
//			Never found in man.
// 
///////////////////////////////////////////////////////////////////////////////

#define _CRT_SECURE_NO_WARNINGS

// ͷ�ļ� /////////////////////////////////////////////////////////////////////

#include <windows.h>			// ���ɴ��ڡ�VS���Ѱ�װ Windows SDK������VC Express������
#include <stdio.h>				// ��׼�������
#include <iostream>
#include <vector>				// std����
#include <time.h>				// ʱ��

#include <gl/freeglut.h>		// FreeGLUT��ʵ��ʹ��freeGLUT��
#include "resource.h"
//#include <gl/gl.h>			// OpenGL
//#include <gl/glu.h>			// OpenGL utility
//#include <gl/glaux.h>			// OpenGL auxilary

using namespace std;			// ʹ��std�����ռ䣬�򻯱�������


// ȫ�ֲ��� ///////////////////////////////////////////////////////////////////

float _camx = 0.0f;				// �����λ��x
float _camy = 0.0f;				// �����λ��y
float _camz = 3.0f;				// �����λ��z
float _tgtx = 0.0f;				// �����Ŀ��x
float _tgty = 0.0f;				// �����Ŀ��y
float _tgtz = 0.0f;				// �����Ŀ��z

bool _fullscreen = false;		// ȫ����ʾ
bool _wireframe = true;			// �߿���Ⱦ
bool _cull_back_faces = true;	// ȥ�������棬�����Ⱦ�ٶ�
bool _lighting = true;			// ����
bool _smooth_shading = true;	// �⻬��Ⱦ

vector<float> _verts;			// �����������飺x0, y0, z0, x1, y1, z1, ...
vector<unsigned int> _faces;	// �����ζ���������飺v0x, v0y, v0z, v1x, v1y, v1z, ...
vector<float> _vert_normals;	// ���㷨�����顣ÿ�������Ӧһ�����ߡ�nx0, ny0, nz0, nx1, ny1, nz1, ...
vector<float> _face_normals;	// �淨�����顣ÿ�����Ӧһ�����ߡ�n0x, n0y, n0z, n1x, n1y, n1z, ...

float _scale = 1.0f;			// ��������
float _rot_x;					// ��x�����ת�Ƕ�
float _rot_y;					// ��y�����ת�Ƕ�

int _mouse_state = GLUT_UP;		// ��갴ť״̬
int _mouse_button;				// ��갴ť
int _mouse_x, _mouse_y;			// �ϴ�������Ļ����

clock_t _now, _justnow;			// ��ǰʱ�䡢�ղ�ʱ��
clock_t _msec;					// ����ʱ�䣨���룩�������ڼ��㶯���ٶ���ر���

clock_t _fps_time;				// ֡�ٲ�����ʼʱ��
int _fps_interval = 1;			// ֡�ٲ���������룩
int _fps_cnt = 0;				// ֡/��

int switch_pin = 1;			    //���뿪��

char _title[128];				// ���ڱ���

const char _help[] = "Mini 3D Engine 1.1\n"
"LI Zheng, lizheng AT gdut.edu.cn\n"
"School of Computers, Guangdong University of Technology\n"
"Licensed under the GNU GPLv3.\n"
"Copyright: 2018 Guangdong University of Technology. All rights reserved.\n\n"
"Usage: MiniEngine [filename].obj\n"
"Note that only a limited version of Wavefront Object File format is supported.\n"
"arrow keys / left mouse button: rotate the model.\n"
"w, s, a, d, q, e: pan the camera\n"
"z, x: scale the model\n"
"r: reset the camera\n"
"h: print this help info\n"
"Esc: exit the program\n"
"F1: full screen / windowed mode\n"
"F2: wireframe / surface mode\n"
"F3: smooth / flat shading\n"
"F4: cull back faces on / off\n"
"F5: lighting on / off\n\n"
"p:  stylus switch on / off\n\n";

// ���� ///////////////////////////////////////////////////////////////////////

// ��ȡWavefront Object File��
// ֻ֧�����޵ĸ�ʽ��
// ��֧�������涥�����������ܴ����ߡ���ͼ����������
// 
// path:     �ļ�·��
// verts:    �������顣
//           ���� x0, y0, z0, x1, y1, z1, x2, y2, z2, ... ÿ��������ֵΪ˳���¼��
//           n������3*n����¼��
// faces:    ������Ķ���������顣
//           ���� f0_0, f0_1, f0_2, f1_0, f1_1, f1_2, f2_0, f2_1, f2_2, ... ÿ�����������Ϊ˳���¼��
//           n����������3*n����¼��
bool load_obj_file(const char *path, vector<float> &verts, vector<unsigned int> &faces, 
	bool centerize = true, bool scaleToUnit = true){
	FILE *file = fopen(path, "r");
	if (file == NULL)			// �Ҳ����ļ�
		return false;

	while (true) {
		char lineHeader[128];
		// ��ȡ��ǰ�е�һ������
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)		// ���ļ���β���˳�ѭ��
			break;

		// �����ĵ�����
		if (strcmp(lineHeader, "v") == 0) {				// ��������
			float vx, vy, vz;
			fscanf(file, "%f %f %f\n", &vx, &vy, &vz);
			verts.push_back(vx);
			verts.push_back(vy);
			verts.push_back(vz);
		}
		else if (strcmp(lineHeader, "f") == 0) {		// ������ <<��֧�������涥�����������ܴ����ߡ���ͼ����������>>
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3];
			int matches = fscanf(file, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);
			if (matches != 3){
				printf("File format error. Please check the file content.\n");
				return false;
			}
			// ע�⣺obj�ļ��Ķ�����Ŵ�1��ʼ����������Ŵ�0��ʼ��
			faces.push_back(vertexIndex[0] - 1);
			faces.push_back(vertexIndex[1] - 1);
			faces.push_back(vertexIndex[2] - 1);
		}
		else {											// ����������
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}

	// ��ģ���ƶ������뼰��������ģ��
	if (verts.size() >= 3) {
		
		// �����Χ��
		float vxmin, vxmax, vymin, vymax, vzmin, vzmax;

		// ��ʼ����Χ��
		vxmin = vxmax = verts[0];
		vymin = vymax = verts[1];
		vzmin = vzmax = verts[2];

		// �������ж��㣬���°�Χ������
		float x, y, z;
		for (int i = 1; i < verts.size() / 3; ++i) {
			x = verts[i * 3];
			y = verts[i * 3 + 1];
			z = verts[i * 3 + 2];

			vxmin = min(vxmin, x);
			vymin = min(vymin, y);
			vzmin = min(vzmin, z);

			vxmax = max(vxmax, x);
			vymax = max(vymax, y);
			vzmax = max(vzmax, z);
		}

		// �����Χ����������
		float cx = (vxmax + vxmin) / 2;
		float cy = (vymax + vymin) / 2;
		float cz = (vzmax + vzmin) / 2;

		// �������ű���
		float s = 1.0f / max(0.01f, max(vxmax - vxmin, max(vymax - vymin, vzmax - vzmin)));
		
		// �������ж���
		for (int i = 0; i < verts.size() / 3; ++i) {

			// ��ģ���ƶ�������
			if (centerize) {
				verts[i * 3    ] -= cx;
				verts[i * 3 + 1] -= cy;
				verts[i * 3 + 2] -= cz;
			}

			// ��ģ����������λ��С
			if (scaleToUnit) {
				verts[i * 3    ] *= s;
				verts[i * 3 + 1] *= s;
				verts[i * 3 + 2] *= s;
			}
		}
	}

	return true;
}

// GLUT�����Ϣ������ ///////////////////////////////////////////////////////

// ��ʼ��OpenGL�����ɴ��ں���á�
void init()
{
	if (_smooth_shading)
		glShadeModel(GL_SMOOTH);						// ʹ�ù⻬��Ⱦ
	else
		glShadeModel(GL_FLAT);							// ʹ��ƽ̹��Ⱦ
	glClearColor(225, 225, 225, 225);				// ��ɫ��������ֵ��RGBA������Ϊ��ɫ����
	//glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// ��ɫ��������ֵ��RGBA��
	glClearDepth(1.0f);									// ��Ȼ�������ֵ
	glEnable(GL_DEPTH_TEST);							// ʹ����Ȳ���
	glDepthFunc(GL_LEQUAL);								// ������Ȳ��Է���
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// �������͸��ͶӰ��������
	_wireframe
		? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) 	// ��Ⱦģʽ���߿�����
		: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// ��Ⱦģʽ���������
	glCullFace(GL_BACK);								// �趨ȥ��ģʽ��ȥ��������
	_cull_back_faces
		? glEnable(GL_CULL_FACE)						// ��ȥ��������
		: glDisable(GL_CULL_FACE);						// �ر�ȥ��������

	// ���ù��պͲ��� /////////////////////////////////////
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);	// ������ɫ���ʲ���
	glEnable(GL_COLOR_MATERIAL);								// ʹ����ɫ����
	glColor3f(1.0, 1.0, 1.0);									// ����Ĭ�ϵ�������ɫ
	glEnable(GL_LIGHTING);										// ʹ�ù���

	// ���0�ŵ�
	GLfloat LightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };		// ��������ɫ
	GLfloat LightDiffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };		// ɢ�����ɫ
	GLfloat LightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };		// �߹���ɫ
	GLfloat LightPosition[] = { 1.0f, 1.0f, 1.0f, 0.0f };		// ƽ�й⣨ע����������0����ʾ������
	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);				// ���û�����
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);				// ����ɢ���
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecular);			// ���ø߹�
	glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);			// ����ƽ�й�
	glEnable(GL_LIGHT0);										// ��0�ŵ�

	// ���� ///////////////////////////////////////////////
	_justnow = _fps_time = clock();								// ��ʼ��ʱ��

	
}

// ��ʼ����������OBJ�ļ�����ģ��
void init_scene_with_obj_file(const char *path) {
	// ���������Ϣ ///////////////////////////////////////
	_verts.clear();
	_faces.clear();
	_vert_normals.clear();
	_face_normals.clear();

	// ��ģ���ļ� ///////////////////////////////////////
	if (!load_obj_file(path, _verts, _faces)) {
		printf("Error occured when initilizing the scene.");
		return;
	}

	// ���㷨�� ///////////////////////////////////////////

	unsigned int i0, i1, i2;
	float x0, y0, z0, x1, y1, z1, x2, y2, z2;
	float ux, uy, uz, vx, vy, vz;
	float nx, ny, nz;
	float length;

	// ��ʼ�����㷨��
	_vert_normals.assign(_verts.size(), 0.0f);

	// �����淨��
	for (unsigned int i = 0; i < _faces.size() / 3; ++i) {		// �������������Σ�ÿ3������Ϊһ��
		// ȡ�ø������ε�3����������
		i0 = _faces[i * 3] * 3;
		i1 = _faces[i * 3 + 1] * 3;
		i2 = _faces[i * 3 + 2] * 3;

		// ȡ�ø������ε�3�����������
		x0 = _verts[i0];   y0 = _verts[i0 + 1];   z0 = _verts[i0 + 2];
		x1 = _verts[i1];   y1 = _verts[i1 + 1];   z1 = _verts[i1 + 2];
		x2 = _verts[i2];   y2 = _verts[i2 + 1];   z2 = _verts[i2 + 2];

		// ��������������
		ux = x1 - x0;	uy = y1 - y0;	uz = z1 - z0;
		vx = x2 - x0;	vy = y2 - y0;	vz = z2 - z0;

		// ������
		nx = uy * vz - uz * vy;		ny = uz * vx - ux * vz;		nz = ux * vy - uy * vx;

		// �������滯�����淨������Ϊ��λ����
		length = sqrt(nx*nx + ny*ny + nz*nz);						// ���㳤��
		if (length < 0.00001f)
			length = 0.00001f;										// Ҫ���������Է�Ϊ��
		length = (float)(1.0 / length);								// Ԥ�ȼ��㵹��
		nx = nx * length;	ny = ny * length;	nz = nz * length;	// ��������Ϊ1

		// �����淨��
		_face_normals.push_back(nx);	_face_normals.push_back(ny);	_face_normals.push_back(nz);

		// ���淨���ۼӵ����㷨�ߣ�Ȩ��Ϊ1
		_vert_normals[i0] += nx;   _vert_normals[i0 + 1] += ny;   _vert_normals[i0 + 2] += nz;	// �ۼӵ���1�����㷨��
		_vert_normals[i1] += nx;   _vert_normals[i1 + 1] += ny;   _vert_normals[i1 + 2] += nz;	// �ۼӵ���2�����㷨��
		_vert_normals[i2] += nx;   _vert_normals[i2 + 1] += ny;   _vert_normals[i2 + 2] += nz;	// �ۼӵ���3�����㷨��
	}

	// ���㷨�߳������滯�������㷨������Ϊ��λ����
	for (unsigned int i = 0; i < _vert_normals.size() / 3; ++i) {
		// ȡ���ۼӽ��
		nx = _vert_normals[i * 3];
		ny = _vert_normals[i * 3 + 1];
		nz = _vert_normals[i * 3 + 2];

		// �������滯
		length = sqrt(nx*nx + ny*ny + nz*nz);						// ���㳤��
		if (length < 0.00001f)
			length = 0.00001f;										// Ҫ���������Է�Ϊ��
		length = (float)(1.0 / length);								// Ԥ�ȼ��㵹��
		_vert_normals[i * 3] = nx * length;							// ��������Ϊ1
		_vert_normals[i * 3 + 1] = ny * length;
		_vert_normals[i * 3 + 2] = nz * length;
	}
}

// ÿ���������ŵ��ã�init()�����һ�Ρ�
// w�������ؼ���Ĵ��ڿ��
// h�������ؼ���Ĵ��ڸ߶�
void reshape(int w, int h)
{
	// ��hҪ����������ͶӰ�ݺ��������Ԥ����Ϊ��
	if (h == 0)
		h = 1;

	// ����viewport
	glViewport(0, 0, w, h);			// ������viewport��ռ����������

	// ����Projection����
	glMatrixMode(GL_PROJECTION);	// ѡ���趨ͶӰ��Projection������
	glLoadIdentity();				// ��֮����Ϊ��λ����
	gluPerspective(45.0f,			// �����ӽ�
		1.0f * w / h,				// �����ݺ��
		0.01f,						// ����ƽ��
		100.0f);					// Զ��ƽ��

	// ����Model-View����M
	glMatrixMode(GL_MODELVIEW);	// ѡ���趨Model-View����
	glLoadIdentity();				// ��֮����Ϊ��λ����M=I
}



//��Ƭ��
void Turntable() {
	
	static float fElect1 = 0.0f;
	static float fElect2 = -25;
	static float way = 1;
	static float color = 0;
	static float colorway = 1;
	
	//���Ǳ�ǩ
	glPushMatrix();
	glColor3f(1, 0, 0);

	glRotatef(fElect1, 0, 0, 1);
	glTranslatef(0.4, 0.3, 0.06);
	glutSolidTorus(0.01, 0.05, 2, 3);
	glPopMatrix();

	//��Ƭ
	//��Բ��
	glPushMatrix();
	glColor3f(0.1, 0.1, 0.0);
	glRotatef(0, 1, 0,0);
	glTranslatef(0, 0, 0.02);
	glutSolidTorus(0.25, 0.5, 2, 360);  

	//СԲ��
	//��ɫ״̬�ı�
	if (colorway == 1) {
		color++;
		if (color == 180) colorway = -1;
	}


	if (colorway == -1) {
		color--;
		if (color == 0)colorway = 1;
	}


	glColor3f(0.01+color*0.003, color*0.001, color*0.002);
	glTranslatef(0, 0, 0.020);
	glutSolidTorus(0.15, 0.20, 2, 360);  
	
	glPopMatrix();
	//-------------------------------

	glPushMatrix();
	int n = 360;  //Բ���ƴ���
	float PI = 3.1415926f;
	float R = 0.2f;  //�뾶
	
	glPolygonMode(GL_FRONT, GL_LINE);//�޸�Ϊ�߿�ģʽ
	glBegin(GL_POLYGON);
	//�뾶R���ϱ仯���Ӷ�����Բ��
	for (R = 0.10f; R <= 0.20f; R += 0.001f) {

		for (int i = 0; i <= n; i++)  //��һ��Բ�߿�
		{
	
			glVertex3f(R * cos(2 * PI * i / n), R * sin(2 * PI * i / n),0.04);  //���嶥��
			
			//��ɫ����
			glColor3f(0.05 + color * 0.001, 0.02+color * 0.001, 0.01+color * 0.001);
		}
	}
	glEnd();
	glFlush();
	glPolygonMode(GL_FRONT, GL_FILL);//�޸�Ϊ���ģʽ
	glPopMatrix();


	//��Ƭ�м��С����
	glPushMatrix();
	glColor3f(0.5, 0.3, 0.7);
	glRotatef(0, 1, 0, 0);
	glTranslatef(0, 0, 0.01);
	glutSolidCylinder(0.015, 0.07, 360, 1);
	glPopMatrix();
	//-------------------------------

	//��Ƭ����
	//����������
	glPushMatrix();
	glColor3f(0.5, 0.3, 0.3);	
	glRotatef(0, 0.1, 0, 0);

	glTranslatef(0.1, 0.2, -0.51);
	glutSolidCylinder(1.4, 0.5, 4, 1);
	glPopMatrix();
	//-------------------------------
	
	//�ײ���Χͻ��
	glPushMatrix();
	glColor3f(0.5, 0.3, 0.0);
	glRotatef(0, 0.1, 0, 0);
	glTranslatef(0.1, 0.2, -0.51);
	glutSolidCylinder(1.5, 0.05, 4, 1);
	glPopMatrix();
	//�ײ���Χͻ��
	glPushMatrix();
	glColor3f(0.5, 0.3, 0.0);
	glRotatef(0, 0.1, 0, 0);
	glTranslatef(0.1, 0.2, -0.45);
	glutSolidCylinder(1.45, 0.05, 4, 1);
	glPopMatrix();
	//-------------------------------

	//������Χͻ��
	glPushMatrix();
	glColor3f(0.5, 0.3, 0.0);
	glRotatef(0, 0.1, 0, 0);
	glTranslatef(0.1, 0.2, -0.04);
	glutSolidCylinder(1.5, 0.05, 4, 1);
	glPopMatrix();
	//������Χͻ��
	glPushMatrix();
	glColor3f(0.5, 0.3, 0.0);
	glRotatef(0, 0.1, 0, 0);
	glTranslatef(0.1, 0.2, -0.09);
	glutSolidCylinder(1.45, 0.05, 4, 1);
	glPopMatrix();
	//-------------------------------

	
	//����
	
	//�ײ����
	glPushMatrix();
	glColor3f(0.5, 0.5, 0.5);

	glRotatef(0, 1, 0, 0);
	glTranslatef(1.03, 0.4, -0.04);
	glutSolidCylinder(0.13, 0.1, 360, 1);
	glPopMatrix();
	//-------------------------------
	
	//����Ϸ����ӵ�	
	glPushMatrix();
	glColor3f(0.8, 0.3, 0.0);

	glTranslatef(1.03, 0.4, 0.05);
	glRotatef(-fElect2, 0, 0, 2);

	glutSolidCylinder(0.2, 0.2, 4, 1.2);
	glPopMatrix();
	//-------------------------------
	
	
	//̽��
	glPushMatrix();
	glColor3f(1, 1, 1.0);

	glTranslatef(1.03, 0.4, 0.15);
	glRotatef(fElect2+180, 0, 0, -1);  //��z����ת

	
	glRotatef(90, 0, 0, 1);  //��z����ת90
	glRotatef(90, 1, 0, 0);  //��x����ת90
	glRotatef(45, 0, 1, 0);  //��y����ת45
	
	
	glutSolidCylinder(0.05, 0.85, 360, 1.2);
	glPopMatrix();
	//-------------------------------
	//�뼰���ӵ�
	//���ӵ�
	glPushMatrix();
	glColor3f(0.8, 0.3, 0.0);

	glTranslatef(1.03, 0.4, 0.10);
	glRotatef(fElect2 + 180, 0, 0, -1);  //��z����ת

	glTranslatef(0.62, 0.62, 0);
	glutSolidCylinder(0.1, 0.12, 4, 1.2);
	glPopMatrix();
	//-------------------------------
	//��
	glPushMatrix();
	glColor3f(1, 1, 1);

	glTranslatef(1.03, 0.4, 0.1);
	glRotatef(1*(-fElect2+90), 0, 0, 2);  //��z����ת
	glRotatef(180, 1, 0, 0);  //��x����ת180
	glTranslatef(-0.62, -0.62, 0);

	glutWireCone(0.02, 0.08, 360, 1.2);
	glPopMatrix();
	//-------------------------------

	//���뿪��
	if (switch_pin == 1) {  //����״̬
		if(fElect2>-25)
		fElect2 -= 0.1f * switch_pin;
		
	}
	else {  //����״̬
		if (fElect2 < -5)
		fElect2 -= 0.1f * switch_pin;
	}
	
	
	//���Ǳ���ת
	if(switch_pin == -1){
		fElect1 += 0.5f;
		if (fElect1 > 360.0f)
			fElect1 = 0.0f;
	}
	
}



void draw_obj_model() {
	glBegin(GL_TRIANGLES);				// ����������
	unsigned i0, i1, i2;
	float x0, y0, z0, x1, y1, z1, x2, y2, z2;
	for (unsigned int i = 0; i < _faces.size() / 3; ++i) {		// �������������Σ�ÿ3������Ϊһ��
		// ȡ��3����������
		i0 = _faces[i * 3] * 3;		i1 = _faces[i * 3 + 1] * 3;	i2 = _faces[i * 3 + 2] * 3;

		// ȡ�ö��������
		x0 = _verts[i0];   y0 = _verts[i0 + 1];   z0 = _verts[i0 + 2];	// ��1�����������
		x1 = _verts[i1];   y1 = _verts[i1 + 1];   z1 = _verts[i1 + 2];	// ��2�����������
		x2 = _verts[i2];   y2 = _verts[i2 + 1];   z2 = _verts[i2 + 2];	// ��3�����������

		// ע�⣺Ϊ����ȷ������գ����߳���ӦΪ��λ���ȡ���˷��߶�����_scale��
		if (_smooth_shading) {		// ʹ�ù⻬��Ⱦ
			glNormal3f(_vert_normals[i0] * _scale,
				_vert_normals[i0 + 1] * _scale,
				_vert_normals[i0 + 2] * _scale);	// ����0����
			glVertex3f(x0, y0, z0);					// ����0λ��
			glNormal3f(_vert_normals[i1] * _scale,
				_vert_normals[i1 + 1] * _scale,
				_vert_normals[i1 + 2] * _scale);	// ����1����
			glVertex3f(x1, y1, z1);					// ����1λ��
			glNormal3f(_vert_normals[i2] * _scale,
				_vert_normals[i2 + 1] * _scale,
				_vert_normals[i2 + 2] * _scale);	// ����2����
			glVertex3f(x2, y2, z2);					// ����2λ��
		}
		else {					// ʹ��ƽ̹��Ⱦ
			glNormal3f(_face_normals[i * 3] * _scale,
				_face_normals[i * 3 + 1] * _scale,
				_face_normals[i * 3 + 2] * _scale);	// �淨��
			
			glVertex3f(x0, y0, z0);
			glVertex3f(x1, y1, z1);
			glVertex3f(x2, y2, z2);					// ����λ��
		}
	}
	glEnd();						// ���ƽ���
}


void draw_xmas_tree() {
	float scale = 1.0f;

	glLineWidth(1.0f);
	
	glColor3f(1.0, 1.0, 0.0);
	
	
	glPushMatrix();				// M = A
	glRotatef(-90, 1, 0, 0);		// M = .....
	glutSolidCone(0.5, 0.7, 10, 3);	
	glPopMatrix();				// M = A
	
	glColor3f(0.3, 0.8, 0.0);
	glPushMatrix();				// M = A
	glRotatef(-90, 1, 0, 0);
	glTranslatef(0, 0, -0.5);
	glutSolidCone(0.7, 0.8, 10, 3);
	glPopMatrix();
	
	glColor3f(0.1, 0.3, 0.2);
	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	glTranslatef(0, 0, -1.0);
	glutSolidCone(0.1, 0.7, 4, 3);
	glPopMatrix();
	
}


void draw_helicopter() {
	float scale = 1.0f;

	glLineWidth(6.0f);

	// helicopter
	glPushMatrix();
	glScaled( 0.75f * scale, 0.75f * scale, 0.75f * scale );

	glPushMatrix();
	glRotated( 90.0f, 0.0f, 1.0f, 0.0f );
	glScaled( 1.0f, 1.0f, 1.3f );
	glutSolidSphere( 0.25f, 5, 5 );			// ����
	glPopMatrix();
	
	glPushMatrix();
	glTranslated( 0.0f, -0.24f, 0.0f );
	glRotated( 90.0f, 0.0f, 1.0f, 0.0f );
	glScaled( 1.0f, 1.0f, 5.0f );
	glTranslated( 0.2f, 0.0f, 0.0f );
	glutSolidSphere( 0.07f, 3, 3 );			// �ż�
	glTranslated( -0.4f, 0.0f, 0.0f );
	glutSolidSphere( 0.07f, 3, 3 );
	glPopMatrix();
	
	glPushMatrix();
	glTranslated( 0.0f, 0.2f, 0.0f );
	glRotated( -90.0f, 1.0f, 0.0f, 0.0f );
	glutSolidCone( 0.04, 0.16, 4, 1 );		// ������
	glPopMatrix();

	glPushMatrix();
	glTranslated( 0.0f, 0.4f, 0.0f );
	glRotated( 90.0f, 1.0f, 0.0f, 0.0f );
	glutSolidCone( 0.4, 0.05, 16, 1 );		// ����
	glPopMatrix();

	glPushMatrix();
	glRotated( -90.0f, 0.0f, 1.0f, 0.0f );
	glTranslated( 0.0f, 0.05f, 0.29f );
	glRotated( -15.0f, 1.0f, 0.0f, 0.0f );
	glutSolidCone( 0.05, 0.3, 4, 1 );		// ��β

	glTranslated( 0.03f, 0.0f, 0.3f );
	glRotated( 90.0f, 0.0f, 1.0f, 0.0f );
	glutSolidCone( 0.15, 0.0, 8, 1 );		// β����
	glPopMatrix();
	
	glPopMatrix();
}

//���ƺ���
void draw_scene() {

	glPushMatrix();
	Turntable();
	glPopMatrix();
}

// ÿ����Ҫ�ػ�ʱ���ã�init()��reshape()����á�
void display(void)
{
	// ��ʱ ///////////////////////////////////////////////
	_now = clock();					// ��ǰʱ��
	_msec = _now - _justnow;		// ����ʱ�䣨���룩
	_justnow = _now;				// ���浱ǰʱ��

	// ����֡�� ///////////////////////////////////////////
	_fps_cnt += 1;											// ֡��+1
	// ÿ _fps_interval �����һ��֡��
	if (_now - _fps_time > CLK_TCK * _fps_interval) {		// CLK_TCK=1000
		_fps_time = _now;
		sprintf(_title, "Mini 3D Engine - %dfps", _fps_cnt / _fps_interval);
		glutSetWindowTitle(_title);
		_fps_cnt = 0;
	}

	// ��������ɫ���桢��Ȼ��棩 /////////////////////////
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ����Model-View����M ////////////////////////////////
	glLoadIdentity();				// ��֮����Ϊ��λ����M=I
	gluLookAt(						// �����������ʹM=C
		_camx, _camy, _camz,		// �������λ��
		_tgtx, _tgty, _tgtz,		// �۲�Ŀ���λ��
		0.0f, 1.0f, 0.0f);			// ������ġ��ϡ���

	// ���Ƴ��� ///////////////////////////////////////////

	if (_rot_x > 90.0f)					// ������תֵ�Ի�����õĹ۲�Ч��
		_rot_x = 90.0f;
	else if (_rot_x < -90.0f)
		_rot_x = -90.0f;

	glPushMatrix();						// ��ǰModel-View����M=C����ջ

	glTranslatef(0.0f, 0.0f, 0.0f);		// �޸�Model-View����ʹM=C*T
	glRotatef(_rot_x, 1.0, 0.0, 0.0);	// �޸�Model-View����ʹM=C*T*Rx
	glRotatef(_rot_y, 0.0, 1.0, 0.0);	// �޸�Model-View����ʹM=C*T*Rx*Ry
	glScalef(_scale, _scale, _scale);	// �޸�Model-View����ʹM=C*T*Rx*Ry*S

	/*�޸�����ϵ*/

	draw_scene();
	
	glPopMatrix();					// Model-View����M��ջ��ʹM=C

	// �л������� /////////////////////////////////////////
	glutSwapBuffers();
}

// ������ͨ�ַ���Ϣ������
void keyboard(unsigned char key, int x, int y)
{
	key = tolower(key);
	switch (key) {
	case 27:						// ESC: �˳�����
		exit(0);
		break;

	case 'z':						// z: ����Ŵ�
		_scale += 0.05f;
		break;

	case 'x':						// x: ������С
		_scale -= 0.05f;
		break;

	case 'w':						// w: �����ƽ��
		_camy -= 0.05f;
		_tgty -= 0.05f;
		break;

	case 's':						// s: �����ƽ��
		_camy += 0.05f;
		_tgty += 0.05f;
		break;

	case 'a':						// a: �����ƽ��
		_camx += 0.05f;
		_tgtx += 0.05f;
		break;

	case 'd':						// d: �����ƽ��
		_camx -= 0.05f;
		_tgtx -= 0.05f;
		break;

	case 'q':						// q: �����ƽ��
		_camz -= 0.05f;
		_tgtz -= 0.05f;
		break;

	case 'e':						// e: �����ƽ��
		_camz += 0.05f;
		_tgtz += 0.05f;
		break;

	case 'r':						// r: �������λ
		_camx = _camy = 0.0f;
		_camz = 3.0f;
		_tgtx = _tgty = _tgtz = 0.0f;
		break;

	case 'h':						// h: ������Ϣ
		printf(_help);
		break;

	case 'p':				  		// h: ������Ϣ
		switch_pin *= -1;
		
		if(switch_pin == -1){  //��������
			//PlaySound(TEXT("./music_2.WAV"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
			//��������޷����ſ�����������ĺ���
			PlaySound(LPWSTR(IDR_WAVE1), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC | SND_LOOP);
		}
		

		if (switch_pin == 1) {  //��ͣ����
			PlaySound(NULL, NULL, SND_PURGE);
		}
		

		break;

	default:
		break;
	}
}



// ���������ַ���Ϣ������
void arrow_keys(int a_keys, int x, int y)
{
	switch (a_keys) {
	case GLUT_KEY_UP:				// ARROW UP
		_rot_x += 1.0f;
		break;

	case GLUT_KEY_DOWN:				// ARROW DOWN
		_rot_x -= 1.0f;
		break;

	case GLUT_KEY_RIGHT:			// ARROW RIGHT
		_rot_y += 1.0f;
		break;

	case GLUT_KEY_LEFT:				// ARROW LEFT
		_rot_y -= 1.0f;
		break;

	case GLUT_KEY_F1:				// F1: ȫ����ʾ
		_fullscreen = !_fullscreen;
		if (_fullscreen)
			glutFullScreen();				// ȫ����ʾ
		else {
			glutReshapeWindow(800, 600);	// �������سߴ�
			glutPositionWindow(50, 50);	// ����λ��
		}
		break;

	case GLUT_KEY_F2:				// F2: �߿���Ⱦ/����Ⱦ
		_wireframe = !_wireframe;
		_wireframe ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;

	case GLUT_KEY_F3:				// F3: �⻬��Ⱦ/ƽ̹��Ⱦ
		_smooth_shading = !_smooth_shading;
		_smooth_shading ? glShadeModel(GL_SMOOTH) : glShadeModel(GL_FLAT);
		break;

	case GLUT_KEY_F4:				// F4: �޳�������
		_cull_back_faces = !_cull_back_faces;
		_cull_back_faces ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
		break;

	case GLUT_KEY_F5:				// F5: ��/�رչ���
		_lighting = !_lighting;
		_lighting ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING);
		break;

	default:
		break;
	}


}

// ��갴����Ϣ������
void mouse_buttons(int button, int state, int x, int y) {
	switch (button) {
	case  GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) {		// LEFT BUTTON DOWN
			_mouse_state = GLUT_DOWN;
			_mouse_button = button;
			_mouse_x = x;
			_mouse_y = y;
		}
		else {						// LEFT BUTTON UP
			_mouse_state = GLUT_UP;
		}
		break;

	case  GLUT_MIDDLE_BUTTON:
		break;

	case  GLUT_RIGHT_BUTTON:
		break;

	default:
		break;
	}
}

// ����ƶ���Ϣ������
void mouse_moving(int x, int y) {
	if (_mouse_state == GLUT_DOWN) {
		int dx, dy;
		switch (_mouse_button) {
		case  GLUT_LEFT_BUTTON:
			// ��������ƶ���
			dx = x - _mouse_x;
			dy = y - _mouse_y;

			// �޸�������򳡾�
			_rot_x += dy;
			_rot_y += dx;

			// ������굱ǰλ�ã���������
			_mouse_x = x;
			_mouse_y = y;
			break;

		case  GLUT_MIDDLE_BUTTON:
			break;

		case  GLUT_RIGHT_BUTTON:
			break;

		default:
			break;
		}
	}
}

// ������
// argc��������Ŀ
// argv�������ַ���
int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("Usage: MiniEngine [filename].obj\n");
		return 1;
	}

	// ��ʼ��GLUT
	glutInit(&argc, argv);					// ��ʼ��GLUT
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);	// ��ʾģʽ����Ȳ��ԡ�˫���塢RGBA��ɫģ��
	glutInitWindowPosition(250, 250);			// ����λ��
	glutInitWindowSize(800, 600);			// �������سߴ�
	glutCreateWindow("Mini 3D Engine");		// ���ڱ���
	init();									// ��ʼ��
	if (argc >= 2)
		init_scene_with_obj_file(argv[1]);	// ��ʼ�����������ļ�����ģ��
	if (_fullscreen)
		glutFullScreen();					// ȫ����ʾ

	// ���ø�����Ϣ������
	glutDisplayFunc(display);				// ���ڻ��ƴ�����
	glutReshapeFunc(reshape);				// �������Ŵ�����
	glutKeyboardFunc(keyboard);				// ������ͨ�ַ���Ϣ������
	glutSpecialFunc(arrow_keys);			// ���������ַ���Ϣ������
	glutMouseFunc(mouse_buttons);			// ��갴����Ϣ������
	glutMotionFunc(mouse_moving);			// ����ƶ���Ϣ������
	glutIdleFunc(display);					// ���д�����

	// ��ӡ������Ϣ
	printf(_help);

	// ������Ϣѭ����ֱ���رմ���
	glutMainLoop();

	return 0;
}



