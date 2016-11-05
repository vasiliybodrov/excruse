/* ***************************************************************** */
/*                       Copyright (C) 2010 Bodro                    */
/*                         All Rights Reserved                       */
/* ----------------------------------------------------------------- */
/* Язык программирования: ANSI C99                                   */
/* Аппаратная платформа: любая                                       */
/* Операционная система: любая                                       */
/* Дополнительно: OpenGL, GLUT                                       */
/* Язык комментариев: русский                                        */
/* ----------------------------------------------------------------- */
/* Описание: демо-сцена.                                             */
/*           Изучение свойств и внешнего вида тороидальных спиралей. */
/* Автор, программист: Бодров Василий Викторович, 1984 г.р.          */
/* Контактная информация: Tel. +7 (960) 576-46-86                    */
/*                        http://bodro.narod.ru                      */
/*                        mailto: bodro-mail@list.ru                 */
/*                        ICQ: 309999423                             */
/*                        XMPP: vbodrov@jabber.ru                    */
/*                        YID: vbodrov                               */
/*                        MSN: vbodrov@hotmail.com                   */
/*                        AIM: vbodrov                               */
/*                        Skype: vasilij.bodrov                      */
/* Дата создания: 17 июня 2010 года                                  */
/* ***************************************************************** */
/* ДОПОЛНИТЕЛЬНЫЙ КОММЕНТАРИЙ К ФАЙЛУ: НЕТ                           */
/* ***************************************************************** */

// ПОДКЛЮЧАЕМЫЕ ЗАГОЛОВОЧНЫЕ ФАЙЛЫ ====================================

#include "excruse.h"


// ОПИСАНИЕ ФУНКЦИЙ ===================================================
static void idle(void);
static void timer_redisplay(int);
#ifdef SHOW_FPS
static void timer_show_fps(int);
#endif // SHOW_FPS
static void display(void);
static void reshape(int, int);
static void keyboard(unsigned char, int, int);
static GLdouble* tour(GLdouble*, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble);
static void render_skin(GLdouble*, GLdouble*, GLint, GLdouble, bool);
#ifdef SHOW_COORD
static void draw_coord(void);
#endif // SHOW_COORD
#ifdef USE_TWINING
static inline GLdouble twin(GLdouble, GLdouble, GLdouble);
static GLdouble* twin_array(GLdouble*, GLdouble*, GLdouble*, GLdouble);
#endif // USE_TWINING
static void set_vsync(int);
static void init(void);
extern int main(int, char**);


// ОПРЕДЕЛЕНИЕ ФУНКЦИЙ ================================================

/* ----------------------------------------------------------------- */
/* Функция: idle                                                     */
/* Описание: вычисление параметров. Вызывается в GLUT                */
/* Аргументы: нет                                                    */
/* Возвращаемые значения: нет                                        */
/* ----------------------------------------------------------------- */
static void idle(void){	
	static clock_t epstime = CALCULATEPERSECONDTOEPSILON(CALCULATE_PER_SECOND);
	static clock_t oldtime = 0;
	static clock_t newtime = 0;
	static clock_t difftime = 0;

#ifdef SHOW_FPS
	cpscountfake++;
#endif // SHOW_FPS
	newtime = clock() * 1000 / CLOCKS_PER_SEC;
	difftime = newtime - oldtime;
	if(difftime < epstime){
#ifdef SHOW_FPS
		cpscountfakeret++;
#endif // SHOW_FPS
		return;
	}
	oldtime = newtime;

#ifdef USE_TWINING
	static bool twin_flag = true;
#endif // USE_TWINING
#ifdef USE_AUTO_SCALE
	static bool scale_flag[] = {true, true, true};
#endif // USE_AUTO_SCALE

#ifdef USE_TWINING
	// Твининг
	current_twin += ((twin_flag) ? +step_twin : -step_twin);
	twin_flag = (current_twin >= up_lim_twin || current_twin <= down_lim_twin) ? !twin_flag : twin_flag;
#endif // USE_TWINING

#ifdef USE_SPIN
	// Вращение
	spin = ((spin >= PI2_GRADUS) ? spin - PI2_GRADUS : spin) + step_spin;
#endif // USE_SPIN

#ifdef USE_AUTO_SCALE
	// Масштабирование
	for(register unsigned int i = 0; i < 3; i++){
		*(scale_k + i) += (*(scale_flag + i)) ? + *(step_scale_k + i) : - *(step_scale_k + i);
		*(scale_flag + i) = (*(scale_k + i) >= *(max_lim_scale_k + i) || *(scale_k + i) <= *(min_lim_scale_k + i)) ? ! *(scale_flag + i) : *(scale_flag + i);
	}
#endif // USE_AUTO_SCALE

#ifdef SHOW_FPS
	cpscount++;
#endif // SHOW_FPS
}

/* ----------------------------------------------------------------- */
/* Функция: timer_redisplay                                          */
/* Описание:                           */
/* Аргументы:                                                     */
/* Возвращаемые значения: нет                                        */
/* ----------------------------------------------------------------- */
static void timer_redisplay(int timer_id){
	(void) timer_id;
	glutPostRedisplay();
	glutTimerFunc(FPSTOMS(FPS), timer_redisplay, 0);
}

#ifdef SHOW_FPS
/* ----------------------------------------------------------------- */
/* Функция: timer_show_fps                                          */
/* Описание:                           */
/* Аргументы:                                                     */
/* Возвращаемые значения: нет                                        */
/* ----------------------------------------------------------------- */
static void timer_show_fps(int timer_id){
	time_t newfpstime = 0;
	double diff = 0.0;
	double fps = 0.0;
	double cps = 0.0;
	(void) timer_id;

	newfpstime = time(NULL);
	if(fpstime)
	{
		diff = difftime(newfpstime, fpstime);
		fps = (double) fpscount / diff;
		cps = (double) cpscount / diff;
		printf("Diff time = %f; frames = %u; FPS = %f; fake calculate = %u; fake return calculate = %u; calculate = %u; CPS = %f;\n", diff, fpscount, fps, cpscountfake, cpscountfakeret, cpscount, cps);
	}

	fpstime = newfpstime;
	fpscount = 0;
	cpscount = 0;
	cpscountfake = 0;
	cpscountfakeret = 0;

	glutTimerFunc(SHOW_FPS_TIMER_MS, timer_show_fps, 0);
}
#endif // SHOW_FPS

/* ----------------------------------------------------------------- */
/* Функция: display                                                  */
/* Описание: Отображение. Вызывается в GLUT.                         */
/* Аргументы: нет                                                    */
/* Возвращаемые значения: нет                                        */
/* ----------------------------------------------------------------- */
static void display(void){
	glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

#ifdef USE_STATIC_SPIN
	glRotated(static_spin, *(vector_static_spin + 0), *(vector_static_spin + 1), *(vector_static_spin + 2));
#endif // USE_STATIC_SPIN

#ifdef USE_SPIN
	glRotated(spin, *(vector_spin + 0), *(vector_spin + 1), *(vector_spin + 2));
#endif // USE_SPIN

#ifdef STATIC_SCALE
	glScaled(*(static_scale_k + 0), *(static_scale_k + 1), *(static_scale_k + 2));
#endif // STATIC_SCALE
	
#ifdef USE_AUTO_SCALE
	glScaled(*(scale_k + 0), *(scale_k + 1), *(scale_k + 2));
#endif // USE_AUTO_SCALE

#ifdef USE_LIGHT
#ifdef USE_VISUAL_LIGHT
 	glMaterialfv(GL_FRONT, GL_DIFFUSE, front_mat_diffuse_light);
	glMaterialfv(GL_FRONT, GL_AMBIENT, front_mat_ambient_light);
	glMaterialfv(GL_FRONT, GL_SPECULAR, front_mat_specular_light);
	glMaterialfv(GL_FRONT, GL_EMISSION, front_mat_emission_light);
	glMaterialfv(GL_FRONT, GL_SHININESS, front_mat_shininess_light);
	glMaterialfv(GL_BACK, GL_DIFFUSE, back_mat_diffuse_light);
	glMaterialfv(GL_BACK, GL_AMBIENT, back_mat_ambient_light);
	glMaterialfv(GL_BACK, GL_SPECULAR, back_mat_specular_light);
	glMaterialfv(GL_BACK, GL_EMISSION, back_mat_emission_light);
	glMaterialfv(GL_BACK, GL_SHININESS, back_mat_shininess_light);
#endif // USE_VISUAL_LIGHT

	glPushMatrix();
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular_light0);
	glLightfv(GL_LIGHT0, GL_POSITION, position_light0);
	glTranslated(position_light0[0], position_light0[1], position_light0[2]);
#ifdef USE_VISUAL_LIGHT
	glutSolidSphere(0.03, 20, 20);
#endif
	glPopMatrix();

	glPushMatrix();
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient_light1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse_light1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular_light1);
	glLightfv(GL_LIGHT1, GL_POSITION, position_light1);
	glTranslated(position_light1[0], position_light1[1], position_light1[2]);
#ifdef USE_VISUAL_LIGHT
	glutSolidSphere(0.03, 20, 20);
#endif
	glPopMatrix();

	glPushMatrix();
	glLightfv(GL_LIGHT2, GL_AMBIENT, ambient_light2);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse_light2);
	glLightfv(GL_LIGHT2, GL_SPECULAR, specular_light2);
	glLightfv(GL_LIGHT2, GL_POSITION, position_light2);
	glTranslated(position_light2[0], position_light2[1], position_light2[2]);
#ifdef USE_VISUAL_LIGHT
	glutSolidSphere(0.03, 20, 20);
#endif
	glPopMatrix();

	glPushMatrix();
	glLightfv(GL_LIGHT3, GL_AMBIENT, ambient_light3);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, diffuse_light3);
	glLightfv(GL_LIGHT3, GL_SPECULAR, specular_light3);
	glLightfv(GL_LIGHT3, GL_POSITION, position_light3);
	glTranslated(position_light3[0], position_light3[1], position_light3[2]);
#ifdef USE_VISUAL_LIGHT
	glutSolidSphere(0.03, 20, 20);
#endif
	glPopMatrix();

	glPushMatrix();
	glLightfv(GL_LIGHT4, GL_AMBIENT, ambient_light4);
	glLightfv(GL_LIGHT4, GL_DIFFUSE, diffuse_light4);
	glLightfv(GL_LIGHT4, GL_SPECULAR, specular_light4);
	glLightfv(GL_LIGHT4, GL_POSITION, position_light4);
	glTranslated(position_light4[0], position_light4[1], position_light4[2]);
#ifdef USE_VISUAL_LIGHT
	glutSolidSphere(0.03, 20, 20);
#endif
	glPopMatrix();

	glPushMatrix();
	glLightfv(GL_LIGHT5, GL_AMBIENT, ambient_light5);
	glLightfv(GL_LIGHT5, GL_DIFFUSE, diffuse_light5);
	glLightfv(GL_LIGHT5, GL_SPECULAR, specular_light5);
	glLightfv(GL_LIGHT5, GL_POSITION, position_light5);
	glTranslated(position_light5[0], position_light5[1], position_light5[2]);
#ifdef USE_VISUAL_LIGHT
	glutSolidSphere(0.03, 20, 20);
#endif
	glPopMatrix();

	glPushMatrix();
	glLightfv(GL_LIGHT6, GL_AMBIENT, ambient_light6);
	glLightfv(GL_LIGHT6, GL_DIFFUSE, diffuse_light6);
	glLightfv(GL_LIGHT6, GL_SPECULAR, specular_light6);
	glLightfv(GL_LIGHT6, GL_POSITION, position_light6);
	glTranslated(position_light6[0], position_light6[1], position_light6[2]);
#ifdef USE_VISUAL_LIGHT
	glutSolidSphere(0.03, 20, 20);
#endif
	glPopMatrix();

	glPushMatrix();
	glLightfv(GL_LIGHT7, GL_AMBIENT, ambient_light7);
	glLightfv(GL_LIGHT7, GL_DIFFUSE, diffuse_light7);
	glLightfv(GL_LIGHT7, GL_SPECULAR, specular_light7);
	glLightfv(GL_LIGHT7, GL_POSITION, position_light7);
	glTranslated(position_light7[0], position_light7[1], position_light7[2]);
#ifdef USE_VISUAL_LIGHT
	glutSolidSphere(0.03, 20, 20);
#endif
	glPopMatrix();
#endif // USE_LIGHT

	if(C1_0 != NULL && C2_0 != NULL && C1_1 != NULL && C2_1 != NULL && C1_2 != NULL && C2_2 != NULL){

#ifdef USE_TORUSES_TWIN
		for(GLdouble t = 0.0; t <= PI2; t += deltat){
#ifdef USE_SPIN_SEGMENT
			if(step_spin_segment > 0){
				spin_segment = (spin_segment >= PI2_GRADUS) ? spin_segment - PI2_GRADUS : spin_segment;
			}else{
				spin_segment = (spin_segment <= -PI2_GRADUS) ? spin_segment + PI2_GRADUS : spin_segment;
			}
			spin_segment += step_spin_segment;
#endif // USE_SPIN_SEGMENT

			C1_1 = tour(C1_1, t, torus1_param_a, torus1_param_b, torus1_param_c, torus1_param_p, torus1_param_q);
			C2_1 = tour(C2_1, t + deltat, torus1_param_a, torus1_param_b, torus1_param_c, torus1_param_p, torus1_param_q);

#ifdef USE_TWINING
			C1_2 = tour(C1_2, t, torus2_param_a, torus2_param_b, torus2_param_c, torus2_param_p, torus2_param_q);
			C2_2 = tour(C2_2, t + deltat, torus2_param_a, torus2_param_b, torus2_param_c, torus2_param_p, torus2_param_q);

			C1_0 = twin_array(C1_0, C1_1, C1_2, current_twin);
			C2_0 = twin_array(C2_0, C2_1, C2_2, current_twin);

			render_skin(C1_0, C2_0, torus_sections, torus_radius, solid_flag);
#else
			render_skin(C1_1, C2_1, torus_sections, torus_radius, solid_flag);
#endif // USE_TWINING
		}
#else
		*(C1_0 + 0) = -1.2;
		*(C1_0 + 1) = 0.9;
		*(C1_0 + 2) = 0;

		*(C2_0 + 0) = -0.7;
		*(C2_0 + 1) = 0.9;
		*(C2_0 + 2) = 0;

		render_skin(C1_0, C2_0, torus_sections, torus_radius, solid_flag);
		
		*(C1_0 + 0) = 0.0;
		*(C1_0 + 1) = -0.5;
		*(C1_0 + 2) = 0;
 
		*(C2_0 + 0) = 0.0;
		*(C2_0 + 1) = 0.5;
		*(C2_0 + 2) = 0.57735;

		render_skin(C1_0, C2_0, torus_sections, torus_radius, solid_flag);

		*(C1_0 + 0) = 1.0;
		*(C1_0 + 1) = 0.0;
		*(C1_0 + 2) = 0.0;
 
		*(C2_0 + 0) = 1.0;
		*(C2_0 + 1) = 0.5;
		*(C2_0 + 2) = 0.0;

		render_skin(C1_0, C2_0, torus_sections, torus_radius, solid_flag);

		*(C1_0 + 0) = -1.0;
		*(C1_0 + 1) = -1.5;
		*(C1_0 + 2) = -1.0;
 
		*(C2_0 + 0) = 1.0;
		*(C2_0 + 1) = 1.0;
		*(C2_0 + 2) = 1.0;

		render_skin(C1_0, C2_0, torus_sections, torus_radius, solid_flag);

		*(C1_0 + 0) = 1.0;
		*(C1_0 + 1) = -1.0;
		*(C1_0 + 2) = 1.0;
 
		*(C2_0 + 0) = -1.0;
		*(C2_0 + 1) = 1.0;
		*(C2_0 + 2) = -1.0;

		render_skin(C1_0, C2_0, torus_sections, torus_radius, solid_flag);

		*(C1_0 + 0) = 1.4;
		*(C1_0 + 1) = -1.0;
		*(C1_0 + 2) = 1.0;
 
		*(C2_0 + 0) = 1.4;
		*(C2_0 + 1) = 1.0;
		*(C2_0 + 2) = -1.0;

		render_skin(C1_0, C2_0, torus_sections, torus_radius, solid_flag);

		*(C1_0 + 0) = -0.4;
		*(C1_0 + 1) = 0.3;
		*(C1_0 + 2) = -1.0;
 
		*(C2_0 + 0) = 0.7;
		*(C2_0 + 1) = 0.3;
		*(C2_0 + 2) = -1.0;

		render_skin(C1_0, C2_0, torus_sections, torus_radius, solid_flag);

		*(C1_0 + 0) = 0.4;
		*(C1_0 + 1) = 1.4;
		*(C1_0 + 2) = 1.0;
 
		*(C2_0 + 0) = 0.4;
		*(C2_0 + 1) = 1.4;
		*(C2_0 + 2) = -1.0;

		render_skin(C1_0, C2_0, torus_sections, torus_radius, solid_flag);
#endif

#ifdef SHOW_DEBUG_MESSAGES		
	}else{
		printf("Main arrays is NULL!\n");
	}
#else
	}
#endif // SHOW_DEBUG_MESSAGES

#ifdef SHOW_COORD
	// Прорисовка системы координат
	draw_coord();
#endif

	// Выталкивание матрицы 
	glPopMatrix();

	// Необходимо для работы под некоторые ОС
	//glFlush();

	// Необходимо для работы под некоторые ОС
	glFinish();

	// Обмен буферов
	glutSwapBuffers();

#ifdef SHOW_FPS
	fpscount++;
#endif // SHOW_FPS
}

/* ----------------------------------------------------------------- */
/* Функция: reshape */
/* Описание: */
/* Аргументы: нет                                                    */
/* Возвращаемые значения: нет                                        */
/* ----------------------------------------------------------------- */
static void reshape(int w, int h){
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -1.0, 1.0, 3.0, 10.0);
	glTranslatef(0.0, 0.0, -5.0);
	glMatrixMode(GL_MODELVIEW);
}

/* ----------------------------------------------------------------- */
/* Функция: keyboard */
/* Описание: */
/* Аргументы:                                                     */
/* Возвращаемые значения: нет                                        */
/* ----------------------------------------------------------------- */
static void keyboard(unsigned char key, int x, int y){
	(void) x;
	(void) y;

	switch (key){
	case 27:
		exit(0);
		break;
	case '0':
		set_vsync(0);
		break;
	case '1':
		set_vsync(1);
		break;
	case 32:
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

/* ----------------------------------------------------------------- */
/* Функция: tour                                                     */
/* Описание: функция тороидальной спирали                            */
/* Аргументы: C - массив с координатами (просто адрес)               */
/*            t - аргумент функции - тороидальная спираль            */
/*            a, b, c, p, q - параметры функции - спирали            */
/* Возвращаемые значения: массив с координатами                      */
/* ----------------------------------------------------------------- */
static GLdouble* tour(GLdouble *C, GLdouble t, GLdouble a, GLdouble b, GLdouble c, GLdouble p, GLdouble q){
	if(C == NULL){
		return (GLdouble*) NULL;
	}

 	*(C + 0) = (a + b * cos(q * t)) * cos(p * t);
	*(C + 1) = (a + b * cos(q * t)) * sin(p * t);
	*(C + 2) = c * sin(q * t);

	return (GLdouble*) C;
}

/* ----------------------------------------------------------------- */
/* Функция: render_skin                                              */
/* Описание: вычисление и прорисовка тороидальной спирали и её обо-  */
/*           лочки. включая разные эффекты                           */
/* Аргументы: C1, C2 - координаты начала и конеца прямой             */
/*            count - количество граней (элементов) одной секции     */
/*            radius - радиус оболочки                               */
/*            solid - флаг заполненности оболочки                    */
/* Возвращаемые значения: нет                                        */
/* ----------------------------------------------------------------- */
static void render_skin(GLdouble *C1, GLdouble *C2, GLint count, GLdouble radius, bool solid){
#if defined(SHOW_PARTITION) || defined(SHOW_TORUS_SECTIONS)
	GLdouble A1[] = {0.0, 0.0, 0.0}; // Точка прямоугольника (чешуйки)
#endif // SHOW_PARTITION || SHOW_TORUS_SECTIONS
#ifdef SHOW_TORUS_SECTIONS
	GLdouble A2[] = {0.0, 0.0, 0.0}; // Точка прямоугольника (чешуйки)
	GLdouble A3[] = {0.0, 0.0, 0.0}; // Точка прямоугольника (чешуйки)
	GLdouble A4[] = {0.0, 0.0, 0.0}; // Точка прямоугольника (чешуйки)
#endif // SHOW_TORUS_SECTIONS
	GLdouble l = 0.0;                // Длина (модуль) вектора C
	GLdouble cx = 0.0;               // Проекция вектора C на ось X
	GLdouble cy = 0.0;               // Проекция вектора C на ось Y
	GLdouble cz = 0.0;               // Проекция вектора C на ось Z
	GLdouble cxn = 0.0;              // Проекция вектора C на ось X (нормализованная)
	GLdouble cyn = 0.0;              // Проекция вектора C на ось Y (нормализованная)
	GLdouble czn = 0.0;              // Проекция вектора C на ось Z (нормализованная)
	GLdouble step = 0.0;             // Шаг прорисовки чешуек (в радианах)
#ifdef SHOW_TORUS_SECTIONS
	GLdouble nx = 0.0;               // X составляющая вектора нормали к чешуйке
	GLdouble ny = 0.0;               // Y составляющая вектора нормали к чешуйке
	GLdouble nz = 0.0;               // Z составляющая вектора нормали к чешуйке
	GLdouble nxn = 0.0;              // Нормализованная x составляющая вектора нормали к чешуйке
	GLdouble nyn = 0.0;              // Нормализованная y составляющая вектора нормали к чешуйке
	GLdouble nzn = 0.0;              // Нормализованная z составляющая вектора нормали к чешуйке
	GLdouble nl = 0.0;               // Длина вектора нормали к чешуйке
#endif // SHOW_TORUS_SECTIONS
	GLdouble angle_z_xoz = 0.0;      // Угол между проекцией вектора C на плоскость XOZ с осью Z 
	GLdouble angle_y_yoz = 0.0;      // Угол между проекцией вектора C на плоскость YOZ с осью Y после поворота
	GLdouble czn_new = 0.0;          // Проекция вектора C на ось OZ после поворота

	// Проверка корректности входных параметров
	if(C1 == NULL || C2 == NULL || count <= 0 || radius <= 0){
		return;
	}

	// Вычисление проекций вектора С на оси
	cx = (C2[0] - C1[0]);
	cy = (C2[1] - C1[1]);
	cz = (C2[2] - C1[2]);

	// Вычисление длины вектора С (модуля вектора C)
	l = sqrt(cx * cx + cy * cy + cz * cz);
	
	// Нормализация вектора C
	cxn = cx / l;
	cyn = cy / l;
	czn = cz / l;

	// Вычисление углов поворота системы координат
	// Вычисление угла между осью Z и вектором C в плоскости XOZ. Поворот по оси Y.
	angle_z_xoz = (cxn == 0) ? 0 : ((czn == 0) ? PI_DIV2 : atan2(fabs(cxn), fabs(czn)));
	czn_new = (czn != 0) ? (czn / cos(angle_z_xoz)) : (cxn / sin(angle_z_xoz));

	angle_z_xoz = ((*(C2 + 0) > *(C1 + 0) && *(C2 + 2) > *(C1 + 2)) ? angle_z_xoz : ((*(C2 + 0) < *(C1 + 0) && *(C2 + 2) > *(C1 + 2)) ? -angle_z_xoz : ((*(C2 + 0) < *(C1 + 0) && *(C2 + 2) < *(C1 + 2)) ? angle_z_xoz: ((*(C2 + 0) > *(C1 + 0) && *(C2 + 2) < *(C1 + 2)) ? -angle_z_xoz : angle_z_xoz))));

	angle_z_xoz = RADIANTOGRADUS(angle_z_xoz);

	// Вычисление угла между осью Y и вектором C в плоскости YOZ. Поворот по оси X.
	// Хотя можно было бы считать угол между осью Z в той же плоскости.
	angle_y_yoz = (cyn == 0 && (*(C2 + 2) >= *(C1 + 2))) ? 0 : ((cyn == 0 && (*(C2 + 2) < *(C1 + 2))) ? -PI : ((czn == 0 && (*(C2 + 1) > *(C1 + 1))) ? -PI_DIV2 : ((czn == 0 && (*(C2 + 1) < *(C1 + 1))) ? PI_DIV2 : atan2(fabs(czn_new), fabs(cyn)))));

	angle_y_yoz = ((*(C2 + 2) > *(C1 + 2) && *(C2 + 1) > *(C1 + 1)) ? angle_y_yoz - PI_DIV2 : ((*(C2 + 2) < *(C1 + 2) && *(C2 + 1) > *(C1 + 1)) ? -angle_y_yoz - PI_DIV2 : ((*(C2 + 2) < *(C1 + 2) && *(C2 + 1) < *(C1 + 1)) ? angle_y_yoz + PI_DIV2 : ((*(C2 + 2) > *(C1 + 2) && *(C2 + 1) < *(C1 + 1)) ? -angle_y_yoz + PI_DIV2: angle_y_yoz))));

	angle_y_yoz = RADIANTOGRADUS(angle_y_yoz);
	
#ifdef SHOW_DEBUG_MESSAGES
	printf("length = %10.5f\n", l);
	printf("C1x  = %10.5f; C1y  = %10.5f; C1z  = %10.5f \t - begin vector\n", C1[0], C1[1], C1[2]);
	printf("C2x  = %10.5f; C2y  = %10.5f; C2z  = %10.5f \t - end vector\n", C2[0], C2[1], C2[2]);
	printf("cx   = %10.5f; cy   = %10.5f; cz   = %10.5f \t - vector (not normalize)\n", cx, cy, cz);
	printf("cxn  = %10.5f; cyn  = %10.5f; czn  = %10.5f \t - vector (normalize)\n", cxn, cyn, czn);
	printf("angle_z_xoz = %10.5f \t - angle\n", angle_z_xoz);
	printf("angle_y_yoz = %10.5f \t - angle\n", angle_y_yoz);
	printf("czn_new = %10.5f \t - new czn\n", czn_new);
	printf("\n");
#endif

#ifdef SHOW_SKELETON

#ifdef SHOW_SKELETON_LINE
	// Вывод внутренней осевой линии
	glMaterialfv(GL_FRONT, GL_DIFFUSE, front_mat_diffuse_skeleton_line);
	glMaterialfv(GL_FRONT, GL_AMBIENT, front_mat_ambient_skeleton_line);
	glMaterialfv(GL_FRONT, GL_SPECULAR, front_mat_specular_skeleton_line);
	glMaterialfv(GL_FRONT, GL_EMISSION, front_mat_emission_skeleton_line);
	glMaterialfv(GL_FRONT, GL_SHININESS, front_mat_shininess_skeleton_line);
	glMaterialfv(GL_BACK, GL_DIFFUSE, back_mat_diffuse_skeleton_line);
	glMaterialfv(GL_BACK, GL_AMBIENT, back_mat_ambient_skeleton_line);
	glMaterialfv(GL_BACK, GL_SPECULAR, back_mat_specular_skeleton_line);
	glMaterialfv(GL_BACK, GL_EMISSION, back_mat_emission_skeleton_line);
	glMaterialfv(GL_BACK, GL_SHININESS, back_mat_shininess_skeleton_line);

	glLineWidth(skeleton_line_width);
	glBegin(GL_LINE);
	glVertex3dv(C1);
	glVertex3dv(C2);
	glEnd();
#endif //SHOW_SKELETON_LINE

#ifdef SHOW_SKELETON_POINTS
	// Вывод опорных точек
	glMaterialfv(GL_FRONT, GL_DIFFUSE, front_mat_diffuse_skeleton_points);
	glMaterialfv(GL_FRONT, GL_AMBIENT, front_mat_ambient_skeleton_points);
	glMaterialfv(GL_FRONT, GL_SPECULAR, front_mat_specular_skeleton_points);
	glMaterialfv(GL_FRONT, GL_EMISSION, front_mat_emission_skeleton_points);
	glMaterialfv(GL_FRONT, GL_SHININESS, front_mat_shininess_skeleton_points);
	glMaterialfv(GL_BACK, GL_DIFFUSE, back_mat_diffuse_skeleton_points);
	glMaterialfv(GL_BACK, GL_AMBIENT, back_mat_ambient_skeleton_points);
	glMaterialfv(GL_BACK, GL_SPECULAR, back_mat_specular_skeleton_points);
	glMaterialfv(GL_BACK, GL_EMISSION, back_mat_emission_skeleton_points);
	glMaterialfv(GL_BACK, GL_SHININESS, back_mat_shininess_skeleton_points);

	glPointSize(skeleton_point_size);
	glBegin(GL_POINT);
	glVertex3dv(C1);
	glVertex3dv(C2);
	glEnd();
#endif // SHOW_SKELETON_POINTS

#endif // SHOW_SKELETON

	// Заталкивание матрицы
	glPushMatrix();

	// Перенос системы кординат в точку C1
	glTranslated(*(C1 + 0), *(C1 + 1), *(C1 + 2));

	// Поворот системы координат
	glRotated(angle_z_xoz, 0, 1, 0);
	glRotated(angle_y_yoz, 1, 0, 0);

#ifdef USE_SPIN_SEGMENT
	glRotated(spin_segment, 0, 0, 1);
#endif // USE_SPIN_SEGMENT

	// Вычисление шага
	step = PI2 / count;

#ifdef SHOW_TORUS_SECTIONS
	// Вычисление и вывод одного члена (вычисление и вывод чешуек)
	glMaterialfv(GL_FRONT, GL_DIFFUSE, front_mat_diffuse1);
	glMaterialfv(GL_FRONT, GL_AMBIENT, front_mat_ambient1);
	glMaterialfv(GL_FRONT, GL_SPECULAR, front_mat_specular1);
	glMaterialfv(GL_FRONT, GL_EMISSION, front_mat_emission1);
	glMaterialfv(GL_FRONT, GL_SHININESS, front_mat_shininess1);
	glMaterialfv(GL_BACK, GL_DIFFUSE, back_mat_diffuse1);
	glMaterialfv(GL_BACK, GL_AMBIENT, back_mat_ambient1);
	glMaterialfv(GL_BACK, GL_SPECULAR, back_mat_specular1);
	glMaterialfv(GL_BACK, GL_EMISSION, back_mat_emission1);
	glMaterialfv(GL_BACK, GL_SHININESS, back_mat_shininess1);

	glLineWidth(line_width);

	for(register GLdouble i = 0; i < PI2; i += step){
		// Вычисление прямоугольника (одной части чешуйки - одной части сегмента фигуры)
		A1[0] = radius * sin(i);
		A1[1] = radius * cos(i);
		A1[2] = 0;

		A2[0] = radius * sin(i);
		A2[1] = radius * cos(i);
		A2[2] = l;

		A3[0] = radius * sin(i + step);
		A3[1] = radius * cos(i + step);
		A3[2] = l;

		A4[0] = radius * sin(i + step);
		A4[1] = radius * cos(i + step);
		A4[2] = 0;
		
		// Вычисление нормали
		nx = ((A3[1] - A4[1]) * (A1[2] - A1[2])) - ((A3[2] - A4[2]) * (A1[1] - A4[1]));
		ny = ((A3[2] - A4[2]) * (A1[0] - A4[0])) - ((A3[0] - A4[0]) * (A1[2] - A4[2]));
		nz = ((A3[0] - A4[0]) * (A1[1] - A4[1])) - ((A3[1] - A4[1]) * (A1[0] - A4[0]));
		
		// Вычисление модуля вектора нормали
		nl = sqrt(nx * nx + ny * ny + nz * nz);

		// Нормализация вектора нормали
		nxn = - (nx / nl);
		nyn = - (ny / nl);
		nzn = - (nz / nl);
		
		// Вывод чешуйки (части сегмента фигуры)
		glBegin((solid) ? GL_QUADS : GL_LINE_LOOP);
		glNormal3f(nxn, nyn, nzn);
		glVertex3dv(A1);
		glVertex3dv(A2);
		glVertex3dv(A3);
		glVertex3dv(A4);
		glEnd();
	}
#endif // SHOW_TORUS_SECTIONS

#ifdef SHOW_PARTITION

	// Вычисление и вывод перегородок
	glMaterialfv(GL_FRONT, GL_DIFFUSE, front_mat_diffuse_partition);
	glMaterialfv(GL_FRONT, GL_AMBIENT, front_mat_ambient_partition);
	glMaterialfv(GL_FRONT, GL_SPECULAR, front_mat_specular_partition);
	glMaterialfv(GL_FRONT, GL_EMISSION, front_mat_emission_partition);
	glMaterialfv(GL_FRONT, GL_SHININESS, front_mat_shininess_partition);
	glMaterialfv(GL_BACK, GL_DIFFUSE, back_mat_diffuse_partition);
	glMaterialfv(GL_BACK, GL_AMBIENT, back_mat_ambient_partition);
	glMaterialfv(GL_BACK, GL_SPECULAR, back_mat_specular_partition);
	glMaterialfv(GL_BACK, GL_EMISSION, back_mat_emission_partition);
	glMaterialfv(GL_BACK, GL_SHININESS, back_mat_shininess_partition);

	glLineWidth(line_width);

	// Одна сторона
	glBegin((solid) ? GL_POLYGON : GL_LINE_STRIP);
	glNormal3f(0, 0, -1);
	for(register GLdouble i = 0; i <= PI2; i += step){
		A1[0] = radius * sin(i);
		A1[1] = radius * cos(i);
		A1[2] = 0;
		glVertex3dv(A1);
	}
	glEnd();

	// Другая сторона
	glPushMatrix();
	glRotated(180, 0, 1, 0);
	glBegin((solid) ? GL_POLYGON : GL_LINE_STRIP);
	glNormal3f(0, 0, -1);
	for(register GLdouble i = 0; i <= PI2; i += step){
		A1[0] = radius * sin(i);
		A1[1] = radius * cos(i);
		A1[2] = -l;
		glVertex3dv(A1);
	}
	glEnd();
	glPopMatrix();

#endif // SHOW_PARTITION

#ifdef SHOW_SPHERE_SECTION
	// Вывод сферы по центру секции
	glMaterialfv(GL_FRONT, GL_DIFFUSE, front_mat_diffuse_sphere_section);
	glMaterialfv(GL_FRONT, GL_AMBIENT, front_mat_ambient_sphere_section);
	glMaterialfv(GL_FRONT, GL_SPECULAR, front_mat_specular_sphere_section);
	glMaterialfv(GL_FRONT, GL_EMISSION, front_mat_emission_sphere_section);
	glMaterialfv(GL_FRONT, GL_SHININESS, front_mat_shininess_sphere_section);
	glMaterialfv(GL_BACK, GL_DIFFUSE, back_mat_diffuse_sphere_section);
	glMaterialfv(GL_BACK, GL_AMBIENT, back_mat_ambient_sphere_section);
	glMaterialfv(GL_BACK, GL_SPECULAR, back_mat_specular_sphere_section);
	glMaterialfv(GL_BACK, GL_EMISSION, back_mat_emission_sphere_section);
	glMaterialfv(GL_BACK, GL_SHININESS, back_mat_shininess_sphere_section);

	glTranslated(0, 0, l / 2);

	if(sphere_section_solid){
		glutSolidSphere(sphere_section_radius, sphere_section_slices, sphere_section_stack);
	}else{
		glutWireSphere(sphere_section_radius, sphere_section_slices, sphere_section_stack);
	}
#endif // SHOW_SPHERE_SECTION

	// Выталкивание матрицы
	glPopMatrix();
}

#ifdef SHOW_COORD
/* ----------------------------------------------------------------- */
/* Функция: draw_coord                                               */
/* Описание: функция прорисовки координатных осей и вспомогательных  */
/*           фигур                                                   */
/* Аргументы: нет                                                    */
/* Возвращаемые значения: нет                                        */
/* ----------------------------------------------------------------- */
static void draw_coord(void){
	glPushAttrib(GL_CURRENT_BIT);

#ifdef SHOW_COORD_LINES
	// Координатные оси
	// Начало отсчёта
	glMaterialfv(GL_FRONT, GL_DIFFUSE, front_mat_diffuse_coord_center);
	glMaterialfv(GL_FRONT, GL_AMBIENT, front_mat_ambient_coord_center);
	glMaterialfv(GL_FRONT, GL_SPECULAR, front_mat_specular_coord_center);
	glMaterialfv(GL_FRONT, GL_EMISSION, front_mat_emission_coord_center);
	glMaterialfv(GL_FRONT, GL_SHININESS, front_mat_shininess_coord_center);
	glMaterialfv(GL_BACK, GL_DIFFUSE, back_mat_diffuse_coord_center);
	glMaterialfv(GL_BACK, GL_AMBIENT, back_mat_ambient_coord_center);
	glMaterialfv(GL_BACK, GL_SPECULAR, back_mat_specular_coord_center);
	glMaterialfv(GL_BACK, GL_EMISSION, back_mat_emission_coord_center);
	glMaterialfv(GL_BACK, GL_SHININESS, back_mat_shininess_coord_center);

	glPushMatrix();
	glTranslated(0, 0, 0);
	glutSolidSphere(center_coord_sphere_radius, center_coord_sphere_slices, center_coord_sphere_stack);
	glPopMatrix();

	// Ось X
	glMaterialfv(GL_FRONT, GL_DIFFUSE, front_mat_diffuse_coord_x);
	glMaterialfv(GL_FRONT, GL_AMBIENT, front_mat_ambient_coord_x);
	glMaterialfv(GL_FRONT, GL_SPECULAR, front_mat_specular_coord_x);
	glMaterialfv(GL_FRONT, GL_EMISSION, front_mat_emission_coord_x);
	glMaterialfv(GL_FRONT, GL_SHININESS, front_mat_shininess_coord_x);
	glMaterialfv(GL_BACK, GL_DIFFUSE, back_mat_diffuse_coord_x);
	glMaterialfv(GL_BACK, GL_AMBIENT, back_mat_ambient_coord_x);
	glMaterialfv(GL_BACK, GL_SPECULAR, back_mat_specular_coord_x);
	glMaterialfv(GL_BACK, GL_EMISSION, back_mat_emission_coord_x);
	glMaterialfv(GL_BACK, GL_SHININESS, back_mat_shininess_coord_x);

	glLineWidth(5);

	glBegin(GL_LINE);
	glVertex3f(0,0,0);
	glVertex3f(2,0,0);
	glEnd();

	glPushMatrix();
	glTranslated(1.5, 0, 0);
	glRotated(90, 0, 1, 0);
	glutSolidCone(0.1, 0.6, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-1.5, 0, 0);
	glRotated(90, 0, 1, 0);
	glutSolidTorus(0.02, 0.1, 20, 20);
	glPopMatrix();

	glLineWidth(2);

	glBegin(GL_LINE);
	glVertex3f(0,0,0);
	glVertex3f(-2,0,0);
	glEnd();

	// Ось Y
	glMaterialfv(GL_FRONT, GL_DIFFUSE, front_mat_diffuse_coord_y);
	glMaterialfv(GL_FRONT, GL_AMBIENT, front_mat_ambient_coord_y);
	glMaterialfv(GL_FRONT, GL_SPECULAR, front_mat_specular_coord_y);
	glMaterialfv(GL_FRONT, GL_EMISSION, front_mat_emission_coord_y);
	glMaterialfv(GL_FRONT, GL_SHININESS, front_mat_shininess_coord_y);
	glMaterialfv(GL_BACK, GL_DIFFUSE, back_mat_diffuse_coord_y);
	glMaterialfv(GL_BACK, GL_AMBIENT, back_mat_ambient_coord_y);
	glMaterialfv(GL_BACK, GL_SPECULAR, back_mat_specular_coord_y);
	glMaterialfv(GL_BACK, GL_EMISSION, back_mat_emission_coord_y);
	glMaterialfv(GL_BACK, GL_SHININESS, back_mat_shininess_coord_y);

	glLineWidth(5);

	glBegin(GL_LINE);
	glVertex3f(0,0,0);
	glVertex3f(0,2,0);
	glEnd();

	glPushMatrix();
	glTranslated(0, 1.5, 0);
	glRotated(-90, 1, 0, 0);
	glutSolidCone(0.1, 0.6, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, -1.5, 0);
	glRotated(-90, 1, 0, 0);
	glutSolidTorus(0.02, 0.1, 20, 20);
	glPopMatrix();

	glLineWidth(2);

	glBegin(GL_LINE);
	glVertex3f(0,0,0);
	glVertex3f(0,-2,0);
	glEnd();

	// Ось Z
	glMaterialfv(GL_FRONT, GL_DIFFUSE, front_mat_diffuse_coord_z);
	glMaterialfv(GL_FRONT, GL_AMBIENT, front_mat_ambient_coord_z);
	glMaterialfv(GL_FRONT, GL_SPECULAR, front_mat_specular_coord_z);
	glMaterialfv(GL_FRONT, GL_EMISSION, front_mat_emission_coord_z);
	glMaterialfv(GL_FRONT, GL_SHININESS, front_mat_shininess_coord_z);
	glMaterialfv(GL_BACK, GL_DIFFUSE, back_mat_diffuse_coord_z);
	glMaterialfv(GL_BACK, GL_AMBIENT, back_mat_ambient_coord_z);
	glMaterialfv(GL_BACK, GL_SPECULAR, back_mat_specular_coord_z);
	glMaterialfv(GL_BACK, GL_EMISSION, back_mat_emission_coord_z);
	glMaterialfv(GL_BACK, GL_SHININESS, back_mat_shininess_coord_z);

	glLineWidth(5);

	glBegin(GL_LINE);
	glVertex3f(0,0,0);
	glVertex3f(0,0,2);
	glEnd();

	glPushMatrix();
	glTranslated(0, 0, 1.5);
	glutSolidCone(0.1, 0.6, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0, -1.5);
	glutSolidTorus(0.02, 0.1, 20, 20);
	glPopMatrix();

	glLineWidth(2);

	glBegin(GL_LINE);
	glVertex3f(0,0,0);
	glVertex3f(0,0,-2);
	glEnd();
#endif // SHOW_COORD_LINES

#ifdef SHOW_COORD_QUADS
	// Плоскости, проходящие через оси кординат (кординатные плоскости)
	glMaterialfv(GL_FRONT, GL_DIFFUSE, front_mat_diffuse_coord_quads);
	glMaterialfv(GL_FRONT, GL_AMBIENT, front_mat_ambient_coord_quads);
	glMaterialfv(GL_FRONT, GL_SPECULAR, front_mat_specular_coord_quads);
	glMaterialfv(GL_FRONT, GL_EMISSION, front_mat_emission_coord_quads);
	glMaterialfv(GL_FRONT, GL_SHININESS, front_mat_shininess_coord_quads);
	glMaterialfv(GL_BACK, GL_DIFFUSE, back_mat_diffuse_coord_quads);
	glMaterialfv(GL_BACK, GL_AMBIENT, back_mat_ambient_coord_quads);
	glMaterialfv(GL_BACK, GL_SPECULAR, back_mat_specular_coord_quads);
	glMaterialfv(GL_BACK, GL_EMISSION, back_mat_emission_coord_quads);
	glMaterialfv(GL_BACK, GL_SHININESS, back_mat_shininess_coord_quads);

	glBegin(GL_QUADS);
	glVertex3d(0, -1.25, -1.25);
	glVertex3d(0, -1.25, 1.25);
	glVertex3d(0, 1.25, 1.25);
	glVertex3d(0, 1.25, -1.25);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3d(-1.25, 0, -1.25);
	glVertex3d(-1.25, 0, 1.25);
	glVertex3d(1.25, 0, 1.25);
	glVertex3d(1.25, 0, -1.25);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3d(-1.25, -1.25, 0);
	glVertex3d(-1.25, 1.25, 0);
	glVertex3d(1.25, 1.25, 0);
	glVertex3d(1.25, -1.25, 0);
	glEnd();
#endif // SHOW_COORD_QUADS

#ifdef SHOW_COORD_CUBS
	// Сетка (в виде полупрозрачных кубов)
	glLineWidth(1);

	glMaterialfv(GL_FRONT, GL_DIFFUSE, front_mat_diffuse_coord_cube_wire);
	glMaterialfv(GL_FRONT, GL_AMBIENT, front_mat_ambient_coord_cube_wire);
	glMaterialfv(GL_FRONT, GL_SPECULAR, front_mat_specular_coord_cube_wire);
	glMaterialfv(GL_FRONT, GL_EMISSION, front_mat_emission_coord_cube_wire);
	glMaterialfv(GL_FRONT, GL_SHININESS, front_mat_shininess_coord_cube_wire);
	glMaterialfv(GL_BACK, GL_DIFFUSE, back_mat_diffuse_coord_cube_wire);
	glMaterialfv(GL_BACK, GL_AMBIENT, back_mat_ambient_coord_cube_wire);
	glMaterialfv(GL_BACK, GL_SPECULAR, back_mat_specular_coord_cube_wire);
	glMaterialfv(GL_BACK, GL_EMISSION, back_mat_emission_coord_cube_wire);
	glMaterialfv(GL_BACK, GL_SHININESS, back_mat_shininess_coord_cube_wire);

	glPushMatrix();
	glTranslated(0, 0, 0);
	glutWireCube(0.5);
	glutWireCube(1.0);
	glutWireCube(1.5);
	glutWireCube(2.0);
	glutWireCube(2.5);
	glPopMatrix();

	glMaterialfv(GL_FRONT, GL_DIFFUSE, front_mat_diffuse_coord_cube_solid);
	glMaterialfv(GL_FRONT, GL_AMBIENT, front_mat_ambient_coord_cube_solid);
	glMaterialfv(GL_FRONT, GL_SPECULAR, front_mat_specular_coord_cube_solid);
	glMaterialfv(GL_FRONT, GL_EMISSION, front_mat_emission_coord_cube_solid);
	glMaterialfv(GL_FRONT, GL_SHININESS, front_mat_shininess_coord_cube_solid);
	glMaterialfv(GL_BACK, GL_DIFFUSE, back_mat_diffuse_coord_cube_solid);
	glMaterialfv(GL_BACK, GL_AMBIENT, back_mat_ambient_coord_cube_solid);
	glMaterialfv(GL_BACK, GL_SPECULAR, back_mat_specular_coord_cube_solid);
	glMaterialfv(GL_BACK, GL_EMISSION, back_mat_emission_coord_cube_solid);
	glMaterialfv(GL_BACK, GL_SHININESS, back_mat_shininess_coord_cube_solid);

	glPushMatrix();
	glTranslated(0, 0, 0);
	glutSolidCube(0.5);
	glutSolidCube(1.0);
	glutSolidCube(1.5);
	glutSolidCube(2.0);
	glutSolidCube(2.5);
	glPopMatrix();
#endif // SHOW_COORD_CUBS

	glPopAttrib();
}
#endif

#ifdef USE_TWINING
/* ----------------------------------------------------------------- */
/* Функция: twin                                                     */
/* Описание: линейный твининг объектов (твининг двух объектов)       */
/*           объекты - числа в формате с ПТ                          */
/* Аргументы: a - первый объект (число)                              */
/*            b - второй объект (число)                              */
/*            t - степень перехода (от 0 до 1)                       */
/* Возвращяемое значение: новый объект (число)                       */
/* ----------------------------------------------------------------- */
static inline GLdouble twin(GLdouble a, GLdouble b, GLdouble t){
	return (GLdouble) ((1 - t) * a) + (t * b);
}

/* ----------------------------------------------------------------- */
/* Функция: twin_array                                               */
/* Описание: линейный твининг координат точки в трёхмерном простран- */
/*           стве.                                                   */
/* Аргументы: A - первый массив                                      */
/*            B - второй массив                                      */
/*            t - доля преобразования                                */
/* Возвращяемое значение: результирующий массив                      */
/* ----------------------------------------------------------------- */
static GLdouble* twin_array(GLdouble *C, GLdouble *A, GLdouble *B, GLdouble t){
	if(C != NULL){
		for(unsigned int i = 0; i < 3; i++){
			*(C + i) = (GLdouble) ((1 - t) * *(A + i)) + (t * *(B + i));
		}
	}

	return C;
}
#endif // USE_TWINING


/* ----------------------------------------------------------------- */
/* Функция:                                                */
/* Описание:*/
/* Аргументы:*/
/* Возвращяемое значение: */
/* ----------------------------------------------------------------- */
static void set_vsync(int interval){
	GLXDrawable drawable;
    Display *dpy = NULL;
	PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT = NULL;

    dpy = glXGetCurrentDisplay();
    drawable = glXGetCurrentDrawable();

	if(strstr((const char*) glXQueryExtensionsString(dpy, DefaultScreen(dpy)), "GLX_EXT_swap_control") != 0){
		glXSwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC) glXGetProcAddress((const GLubyte*) "glXSwapIntervalEXT");
		//glXSwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC) glXGetProcAddressARB((const GLubyte*) "glXSwapIntervalEXT");
		if(glXSwapIntervalEXT){
			glXSwapIntervalEXT(dpy, drawable, interval);
		}
	}
}

/* ----------------------------------------------------------------- */
/* Функция: init                                                     */
/* Описание: первоначальная инициализация программы, установка осно- */
/*           вных параметров, включение/выключение режимов, выделе-  */
/*           ние памяти под основные массивы и т.п.                  */
/* Аргументы: нет                                                    */
/* Возвращаемые значения: нет                                        */
/* ----------------------------------------------------------------- */
static void init(void){
	// Включение и выключение разных режимов
#ifdef USE_LIGHT
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);	
	glEnable(GL_LIGHT3);
	glEnable(GL_LIGHT4);
	glEnable(GL_LIGHT5);
	glEnable(GL_LIGHT6);
	glEnable(GL_LIGHT7);
#endif

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

#ifdef USE_FOG
	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE, GL_EXP);
	glFogfv(GL_FOG_COLOR, fog_color);
	glFogf(GL_FOG_DENSITY, fog_density);
	glHint(GL_FOG_HINT, GL_DONT_CARE);
#else
	glDisable(GL_FOG);
#endif // USE_FOG

#ifdef USE_BLENDING
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif // USE_BLENDING

	glFrontFace(GL_CCW);
	glDisable(GL_COLOR_MATERIAL);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);

#ifdef ENABLE_POINT_SMOOTH
	glEnable(GL_POINT_SMOOTH);
#else
	glDisable(GL_POINT_SMOOTH);
#endif // ENABLE_POINT_SMOOTH

#ifdef ENABLE_LINE_SMOOTH
	glEnable(GL_LINE_SMOOTH);
#else
	glDisable(GL_LINE_SMOOTH);
#endif // ENABLE_LINE_SMOOTH

#ifdef ENABLE_POLYGON_SMOOTH
	glEnable(GL_POLYGON_SMOOTH);
#else
	glDisable(GL_POLYGON_SMOOTH);
#endif // ENABLE_POLYGON_SMOOTH

	set_vsync(0);

	if(C1_0 == NULL){
		C1_0 = (GLdouble*) calloc(3, sizeof(GLdouble));
	}

	if(C2_0 == NULL){
		C2_0 = (GLdouble*) calloc(3, sizeof(GLdouble));
	}

	if(C1_1 == NULL){
		C1_1 = (GLdouble*) calloc(3, sizeof(GLdouble));
	}

	if(C2_1 == NULL){
		C2_1 = (GLdouble*) calloc(3, sizeof(GLdouble));
	}

	if(C1_2 == NULL){
		C1_2 = (GLdouble*) calloc(3, sizeof(GLdouble));
	}

	if(C2_2 == NULL){
		C2_2 = (GLdouble*) calloc(3, sizeof(GLdouble));
	}
}

/* ----------------------------------------------------------------- */
/* Функция: main                                                     */
/* Описание: точка входа в программу                                 */
/* Аргументы: argcp - количество аргументов командной строки         */
/*            argv - аргументы коммандной строки                     */
/* Возвращяемое значение: 0 - успешное завершение                    */
/* ----------------------------------------------------------------- */
extern int main(int argc, char **argv){
	int winid = 0;

	glutInit((int*) &argc, (char**) argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#ifndef USE_FULLSCREEN
	glutInitWindowSize(SCREEN_SIZE_WIDTH, SCREEN_SIZE_HEIGHT);
#endif // USE_FULLSCREEN
	glutInitWindowPosition(0, 0);
	winid = glutCreateWindow(argv[0]);
	if(!winid){
		return 1;
	}

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(FPSTOMS(FPS), timer_redisplay, 0);

#ifdef SHOW_FPS
	glutTimerFunc(SHOW_FPS_TIMER_MS, timer_show_fps, 0);
#endif // SHOW_FPS

	init();

#ifdef USE_FULLSCREEN
	glutFullScreen();
#endif // USE_FULLSCREEN

	glutMainLoop();

	return 0;
}
/* ***************************************************************** */
/* End of file                                                       */
/* ***************************************************************** */
