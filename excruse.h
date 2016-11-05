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
/*           Основной файл настроек программы.                       */
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
/* Дата создания: 19 июня 2010 года                                  */
/* ***************************************************************** */
/* ДОПОЛНИТЕЛЬНЫЙ КОММЕНТАРИЙ К ФАЙЛУ:                               */
/* данный файл является основным конфигурационном файлом программы.  */
/* Если Вы желаете настроить программу по вашему усмотрению, вносите */
/* изменения в этот файл, изменяя значения констант, начальные значе-*/
/* ния переменных, комментируя или расскоментируя определения препро-*/
/* цессора и т.п.                                                    */
/* Вносить изменения в файл excruse.c без навыков программирования   */
/* и знания языка C стандарта ANSI C99 не рекомендуется!             */
/*                                                                   */
/* Можно так же при компиляции установить define.                    */
/* ***************************************************************** */

// ПОДКЛЮЧАЕМЫЕ ЗАГОЛОВОЧНЫЕ ФАЙЛЫ ====================================

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glext.h>
#include <GL/glxext.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

// МАКРООПРЕДЕЛЕНИЯ ПРЕПРОЦЕССОРА =====================================

// Тригонометрические константы и макросы
#define PI 3.14159265                    // Число пи в радианах
#define PI2 6.28318531                   // Число 2 * пи в радианах
#define PI_DIV2 1.570796325              // Число пи / 2 в радианах

#define PI_RADIAN PI                     // Число пи в радианах
#define PI2_RADIAN PI2                   // Число 2 * пи в радианах
#define PI_DIV2_RADIAN PI_DIV2           // Число пи / 2 в радианах

#define PI_GRADUS 180                    // Число пи в градусах
#define PI2_GRADUS 360                   // Число 2 * пи в градусах

#define RADIANTOGRADUS(angle) ((angle) * PI_GRADUS / PI_RADIAN)

// Управление отображением и отладкой
// (закоментируйте или раскоментируйте необходимые определения)

// Вывод отладочных сообщение в стандартный поток вывода (stdout)
//#define SHOW_DEBUG_MESSAGES

// Частота производства необходимых математических вычислений (не рекомендуется устанавливать более 1000)
// (другими словами можно сказать что здесь задаётся скорость преобразования фигуры)
#define CALCULATE_PER_SECOND 60
#define CALCULATEPERSECONDTOEPSILON(cps) (1000 / cps)

// Частота кадров (не рекомендуется устанавливать более 100)
#define FPS 100
#define FPSTOMS(fps) ((unsigned int) (1000 / (fps)))

// Вывод FPS в стандартный поток вывода (stdout)
#define SHOW_FPS
#ifdef SHOW_FPS
#define SHOW_FPS_TIMER_MS 1000
#endif // SHOW_FPS

// Использование фигуры (использовать фигуру или нет). В случае отказа от использования основных фигур
// (тороидальная спираль) будут использоваться некоторые фигуры, применяемые для отладки программы
#define USE_TORUSES_TWIN

// Отображать экскрузивную форму (саму оболочку тороидальной спирали).
// Если не использовать оболочку, то всё равно можно воспользоваться другими способами визуализации
// функции - тороидальной спирали
#define SHOW_TORUS_SECTIONS

// Координатные оси и визуальные средства измерения размеров фигур
//#define SHOW_COORD
#ifdef SHOW_COORD
#define SHOW_COORD_QUADS
#define SHOW_COORD_CUBS
#define SHOW_COORD_LINES
#endif // SHOW_COORD

// Костяк фигуры - осевой линией тороидальной спирали
//#define SHOW_SKELETON
#ifdef SHOW_SKELETON
#define SHOW_SKELETON_LINE
#define SHOW_SKELETON_POINTS
#endif // SHOW_SKELETON

// Отображение перегородки между сегментами тороидальной спирали (можно использовать без отображения
// самих сегментов спирали)
#define SHOW_PARTITION

// Отображение сферы по центру сегмента (членика) тороидальной спирали
#define SHOW_SPHERE_SECTION

// Освещение сцены (управление светом)
#define USE_LIGHT
#ifdef USE_LIGHT
#define USE_VISUAL_LIGHT
#endif // USE_LIGHT

// Режим полного экрана и размеры окна при оконном режиме
#define USE_FULLSCREEN
#ifndef USE_FULLSCREEN
#define SCREEN_SIZE_WIDTH  900
#define SCREEN_SIZE_HEIGHT 900
#endif

// Туман (включение/выключение использования тумана)
#define USE_FOG

// Вращение сегментов вокруг осевой линии
#define USE_SPIN_SEGMENT

// Автоматическое изменение масштаба
//#define USE_AUTO_SCALE

// Постоянное масштабирование системы
//#define STATIC_SCALE

// Использование прозрачности
#define USE_BLENDING

// Вращение системы
#define USE_SPIN

// Постоянный поворот системы
//#define USE_STATIC_SPIN

// Использование твининга
#define USE_TWINING

// Включение режима сглаживания точек
#define ENABLE_POINT_SMOOTH

// Включение режима сглаживания линий
#define ENABLE_LINE_SMOOTH

// Включение режима сглаживания полигонов
//#define ENABLE_POLYGON_SMOOTH


// КОНСТАНТЫ ==========================================================

#ifdef SHOW_TORUS_SECTIONS
// Тороидальная спираль (секции)
static const GLfloat front_mat_diffuse1[] = {0.0, 0.3, 0.0, 1.0};
static const GLfloat front_mat_ambient1[] = {0.1, 0.0, 0.0, 1.0};
static const GLfloat front_mat_specular1[] = {1.0, 1.0, 1.0, 1.0};
static const GLfloat front_mat_emission1[] = {0.0, 0.3, 0.0, 1.0};
static const GLfloat front_mat_shininess1[] = {127};
static const GLfloat back_mat_diffuse1[] = {0.0, 0.1, 0.0, 1.0};
static const GLfloat back_mat_ambient1[] = {0.0, 0.0, 0.0, 1.0};
static const GLfloat back_mat_specular1[] = {0.0, 0.0, 0.0, 1.0};
static const GLfloat back_mat_emission1[] = {0.0, 0.1, 0.0, 1.0};
static const GLfloat back_mat_shininess1[] = {1};
#endif // SHOW_TORUS_SECTIONS

#ifdef USE_LIGHT
#ifdef USE_VISUAL_LIGHT
// Источники света
static const GLfloat front_mat_diffuse_light[] = {1.0, 1.0, 0.3, 1.0};
static const GLfloat front_mat_ambient_light[] = {0.1, 0.1, 0.1, 1.0};
static const GLfloat front_mat_specular_light[] = {1.0, 1.0, 1.0, 1.0};
static const GLfloat front_mat_emission_light[] = {0.5, 0.5, 0.3, 1.0};
static const GLfloat front_mat_shininess_light[] = {127};
static const GLfloat back_mat_diffuse_light[] = {0.5, 0.5, 0.5, 1.0};
static const GLfloat back_mat_ambient_light[] = {0.1, 0.1, 0.1, 1.0};
static const GLfloat back_mat_specular_light[] = {0.1, 0.1, 0.1, 1.0};
static const GLfloat back_mat_emission_light[] = {0.0, 0.0, 0.0, 1.0};
static const GLfloat back_mat_shininess_light[] = {100};
#endif // USE_VISUAL_LIGHT

// Источник света #0
static const GLfloat ambient_light0[] = {0.1, 0.1, 0.1, 1.0};
static const GLfloat diffuse_light0[] = {0.7, 0.7, 0.7, 1.0};
static const GLfloat specular_light0[] = {0.7, 0.7, 0.7, 1.0};
static const GLfloat position_light0[] = {-1.0, -1.0, 1.7, 1.0};

// Источник света #1
static const GLfloat ambient_light1[] = {0.1, 0.1, 0.1, 1.0};
static const GLfloat diffuse_light1[] = {0.7, 0.7, 0.7, 1.0};
static const GLfloat specular_light1[] = {0.7, 0.7, 0.7, 1.0};
static const GLfloat position_light1[] = {1.0, -1.0, 1.7, 1.0};

// Источник света #2
static const GLfloat ambient_light2[] = {0.1, 0.1, 0.1, 1.0};
static const GLfloat diffuse_light2[] = {0.7, 0.7, 0.7, 1.0};
static const GLfloat specular_light2[] = {0.7, 0.7, 0.7, 1.0};
static const GLfloat position_light2[] = {-1.0, 1.0, 1.7, 1.0};

// Источник света #3
static const GLfloat ambient_light3[] = {0.1, 0.1, 0.1, 1.0};
static const GLfloat diffuse_light3[] = {0.7, 0.7, 0.7, 1.0};
static const GLfloat specular_light3[] = {0.7, 0.7, 0.7, 1.0};
static const GLfloat position_light3[] = {1.0, 1.0, 1.7, 1.0};

// Источник света #4
static const GLfloat ambient_light4[] = {0.1, 0.1, 0.1, 1.0};
static const GLfloat diffuse_light4[] = {0.7, 0.7, 0.7, 1.0};
static const GLfloat specular_light4[] = {0.7, 0.7, 0.7, 1.0};
static const GLfloat position_light4[] = {-1.0, -1.0, -1.7, 1.0};

// Источник света #5
static const GLfloat ambient_light5[] = {0.1, 0.1, 0.1, 1.0};
static const GLfloat diffuse_light5[] = {0.7, 0.7, 0.7, 1.0};
static const GLfloat specular_light5[] = {0.7, 0.7, 0.7, 1.0};
static const GLfloat position_light5[] = {1.0, -1.0, -1.7, 1.0};

// Источник света #6
static const GLfloat ambient_light6[] = {0.1, 0.1, 0.1, 1.0};
static const GLfloat diffuse_light6[] = {0.7, 0.7, 0.7, 1.0};
static const GLfloat specular_light6[] = {0.7, 0.7, 0.7, 1.0};
static const GLfloat position_light6[] = {-1.0, 1.0, -1.7, 1.0};

// Источник света #7
static const GLfloat ambient_light7[] = {0.1, 0.1, 0.1, 1.0};
static const GLfloat diffuse_light7[] = {0.7, 0.7, 0.7, 1.0};
static const GLfloat specular_light7[] = {0.7, 0.7, 0.7, 1.0};
static const GLfloat position_light7[] = {1.0, 1.0, -1.7, 1.0};
#endif // USE_LIGHT

#ifdef SHOW_PARTITION
static const GLfloat front_mat_diffuse_partition[] = {0.0, 0.3, 0.0, 1.0};
static const GLfloat front_mat_ambient_partition[] = {0.1, 0.0, 0.0, 1.0};
static const GLfloat front_mat_specular_partition[] = {1.0, 1.0, 1.0, 1.0};
static const GLfloat front_mat_emission_partition[] = {0.0, 0.3, 0.0, 1.0};
static const GLfloat front_mat_shininess_partition[] = {127};
static const GLfloat back_mat_diffuse_partition[] = {0.0, 0.1, 0.0, 1.0};
static const GLfloat back_mat_ambient_partition[] = {0.0, 0.0, 0.0, 1.0};
static const GLfloat back_mat_specular_partition[] = {1.0, 1.0, 1.0, 1.0};
static const GLfloat back_mat_emission_partition[] = {0.0, 0.1, 0.0, 1.0};
static const GLfloat back_mat_shininess_partition[] = {127};
#endif // SHOW_PARTITION

#ifdef SHOW_SPHERE_SECTION
static const GLfloat front_mat_diffuse_sphere_section[] = {0.3, 0.3, 0.0, 1.0};
static const GLfloat front_mat_ambient_sphere_section[] = {0.05, 0.05, 0.0, 1.0};
static const GLfloat front_mat_specular_sphere_section[] = {1.0, 1.0, 1.0, 1.0};
static const GLfloat front_mat_emission_sphere_section[] = {0.3, 0.3, 0.0, 1.0};
static const GLfloat front_mat_shininess_sphere_section[] = {127};
static const GLfloat back_mat_diffuse_sphere_section[] = {0.0, 0.0, 0.0, 1.0};
static const GLfloat back_mat_ambient_sphere_section[] = {0.0, 0.0, 0.0, 1.0};
static const GLfloat back_mat_specular_sphere_section[] = {0.0, 0.0, 0.0, 1.0};
static const GLfloat back_mat_emission_sphere_section[] = {0.0, 0.0, 0.0, 1.0};
static const GLfloat back_mat_shininess_sphere_section[] = {1};
#endif // SHOW_SPHERE_SECTION

#ifdef SHOW_COORD
#ifdef SHOW_COORD_LINES
// Координатные оси (ось X)
static const GLfloat front_mat_diffuse_coord_x[] = {1.0, 0.0, 0.0, 1.0};
static const GLfloat front_mat_ambient_coord_x[] = {0.3, 0.0, 0.0, 1.0};
static const GLfloat front_mat_specular_coord_x[] = {1.0, 1.0, 1.0, 1.0};
static const GLfloat front_mat_emission_coord_x[] = {0.5, 0.0, 0.0, 1.0};
static const GLfloat front_mat_shininess_coord_x[] = {127};
static const GLfloat back_mat_diffuse_coord_x[] = {0.1, 0.0, 0.0, 1.0};
static const GLfloat back_mat_ambient_coord_x[] = {0.1, 0.0, 0.0, 1.0};
static const GLfloat back_mat_specular_coord_x[] = {0.1, 0.1, 0.1, 1.0};
static const GLfloat back_mat_emission_coord_x[] = {0.1, 0.0, 0.0, 1.0};
static const GLfloat back_mat_shininess_coord_x[] = {100};

// Координатные оси (ось Y)
static const GLfloat front_mat_diffuse_coord_y[] = {0.0, 1.0, 0.0, 1.0};
static const GLfloat front_mat_ambient_coord_y[] = {0.0, 0.3, 0.0, 1.0};
static const GLfloat front_mat_specular_coord_y[] = {1.0, 1.0, 1.0, 1.0};
static const GLfloat front_mat_emission_coord_y[] = {0.0, 0.5, 0.0, 1.0};
static const GLfloat front_mat_shininess_coord_y[] = {127};
static const GLfloat back_mat_diffuse_coord_y[] = {0.0, 0.1, 0.0, 1.0};
static const GLfloat back_mat_ambient_coord_y[] = {0.0, 0.1, 0.0, 1.0};
static const GLfloat back_mat_specular_coord_y[] = {0.1, 0.1, 0.1, 1.0};
static const GLfloat back_mat_emission_coord_y[] = {0.0, 0.1, 0.0, 1.0};
static const GLfloat back_mat_shininess_coord_y[] = {100};

// Координатные оси (ось Z)
static const GLfloat front_mat_diffuse_coord_z[] = {0.0, 0.0, 1.0, 1.0};
static const GLfloat front_mat_ambient_coord_z[] = {0.0, 0.0, 0.3, 1.0};
static const GLfloat front_mat_specular_coord_z[] = {1.0, 1.0, 1.0, 1.0};
static const GLfloat front_mat_emission_coord_z[] = {0.0, 0.0, 0.5, 1.0};
static const GLfloat front_mat_shininess_coord_z[] = {127};
static const GLfloat back_mat_diffuse_coord_z[] = {0.0, 0.0, 0.1, 1.0};
static const GLfloat back_mat_ambient_coord_z[] = {0.0, 0.0, 0.1, 1.0};
static const GLfloat back_mat_specular_coord_z[] = {0.1, 0.1, 0.1, 1.0};
static const GLfloat back_mat_emission_coord_z[] = {0.0, 0.0, 0.1, 1.0};
static const GLfloat back_mat_shininess_coord_z[] = {100};

// Координатные оси (центр - начало отсчёта)
static const GLfloat front_mat_diffuse_coord_center[] = {0.3, 0.3, 0.3, 1.0};
static const GLfloat front_mat_ambient_coord_center[] = {0.1, 0.1, 0.1, 1.0};
static const GLfloat front_mat_specular_coord_center[] = {1.0, 1.0, 1.0, 1.0};
static const GLfloat front_mat_emission_coord_center[] = {0.2, 0.2, 0.2, 1.0};
static const GLfloat front_mat_shininess_coord_center[] = {127};
static const GLfloat back_mat_diffuse_coord_center[] = {0.1, 0.1, 0.1, 1.0};
static const GLfloat back_mat_ambient_coord_center[] = {0.1, 0.1, 0.1, 1.0};
static const GLfloat back_mat_specular_coord_center[] = {0.1, 0.1, 0.1, 1.0};
static const GLfloat back_mat_emission_coord_center[] = {0.1, 0.1, 0.1, 1.0};
static const GLfloat back_mat_shininess_coord_center[] = {100};
#endif // SHOW_COORD_LINES

#ifdef SHOW_COORD_CUBS
// Координатные оси (грани кубов)
static const GLfloat front_mat_diffuse_coord_cube_wire[] = {0.5, 0.5, 0.5, 1.0};
static const GLfloat front_mat_ambient_coord_cube_wire[] = {0.1, 0.1, 0.1, 1.0};
static const GLfloat front_mat_specular_coord_cube_wire[] = {1.0, 1.0, 1.0, 1.0};
static const GLfloat front_mat_emission_coord_cube_wire[] = {0.1, 0.1, 0.1, 1.0};
static const GLfloat front_mat_shininess_coord_cube_wire[] = {127};
static const GLfloat back_mat_diffuse_coord_cube_wire[] = {0.1, 0.1, 0.1, 1.0};
static const GLfloat back_mat_ambient_coord_cube_wire[] = {0.1, 0.1, 0.1, 1.0};
static const GLfloat back_mat_specular_coord_cube_wire[] = {0.1, 0.1, 0.1, 1.0};
static const GLfloat back_mat_emission_coord_cube_wire[] = {0.1, 0.1, 0.1, 1.0};
static const GLfloat back_mat_shininess_coord_cube_wire[] = {100};

// Координатные оси (кубы)
static const GLfloat front_mat_diffuse_coord_cube_solid[] = {0.1, 0.1, 0.1, 0.25};
static const GLfloat front_mat_ambient_coord_cube_solid[] = {0.2, 0.2, 0.2, 0.25};
static const GLfloat front_mat_specular_coord_cube_solid[] = {1.0, 1.0, 1.0, 0.25};
static const GLfloat front_mat_emission_coord_cube_solid[] = {0.1, 0.1, 0.1, 0.25};
static const GLfloat front_mat_shininess_coord_cube_solid[] = {127};
static const GLfloat back_mat_diffuse_coord_cube_solid[] = {0.1, 0.1, 0.1, 0.25};
static const GLfloat back_mat_ambient_coord_cube_solid[] = {0.0, 0.0, 0.0, 0.25};
static const GLfloat back_mat_specular_coord_cube_solid[] = {0.1, 0.1, 0.1, 0.25};
static const GLfloat back_mat_emission_coord_cube_solid[] = {0.0, 0.0, 0.0, 0.25};
static const GLfloat back_mat_shininess_coord_cube_solid[] = {100};
#endif // SHOW_COORD_CUBS

#ifdef SHOW_COORD_QUADS
// Координатные оси (координатные плоскости)
static const GLfloat front_mat_diffuse_coord_quads[] = {0.1, 0.1, 0.0, 0.3};
static const GLfloat front_mat_ambient_coord_quads[] = {0.3, 0.2, 0.0, 0.3};
static const GLfloat front_mat_specular_coord_quads[] = {1.0, 1.0, 1.0, 0.3};
static const GLfloat front_mat_emission_coord_quads[] = {0.5, 0.4, 0.0, 0.3};
static const GLfloat front_mat_shininess_coord_quads[] = {127};
static const GLfloat back_mat_diffuse_coord_quads[] = {0.1, 0.1, 0.0, 0.3};
static const GLfloat back_mat_ambient_coord_quads[] = {0.3, 0.2, 0.0, 0.3};
static const GLfloat back_mat_specular_coord_quads[] = {0.1, 0.1, 0.1, 0.3};
static const GLfloat back_mat_emission_coord_quads[] = {0.5, 0.4, 0.0, 0.3};
static const GLfloat back_mat_shininess_coord_quads[] = {127};
#endif // SHOW_COORD_QUADS
#endif // SHOW_COORD

#ifdef SHOW_SKELETON 
#ifdef SHOW_SKELETON_LINE
static const GLfloat front_mat_diffuse_skeleton_line[] = {0.5, 0.0, 0.0, 1.0};
static const GLfloat front_mat_ambient_skeleton_line[] = {0.1, 0.0, 0.0, 1.0};
static const GLfloat front_mat_specular_skeleton_line[] = {1.0, 1.0, 1.0, 1.0};
static const GLfloat front_mat_emission_skeleton_line[] = {0.1, 0.0, 0.0, 1.0};
static const GLfloat front_mat_shininess_skeleton_line[] = {127};
static const GLfloat back_mat_diffuse_skeleton_line[] = {0.1, 0.0, 0.0, 1.0};
static const GLfloat back_mat_ambient_skeleton_line[] = {0.1, 0.0, 0.0, 1.0};
static const GLfloat back_mat_specular_skeleton_line[] = {0.0, 0.0, 0.0, 1.0};
static const GLfloat back_mat_emission_skeleton_line[] = {0.0, 0.0, 0.0, 1.0};
static const GLfloat back_mat_shininess_skeleton_line[] = {1};
#endif // SHOW_SKELETON_LINE

#ifdef SHOW_SKELETON_POINTS
static const GLfloat front_mat_diffuse_skeleton_points[] = {0.0, 0.0, 0.7, 1.0};
static const GLfloat front_mat_ambient_skeleton_points[] = {0.0, 0.0, 0.2, 1.0};
static const GLfloat front_mat_specular_skeleton_points[] = {1.0, 1.0, 1.0, 1.0};
static const GLfloat front_mat_emission_skeleton_points[] = {0.0, 0.0, 0.5, 1.0};
static const GLfloat front_mat_shininess_skeleton_points[] = {127};
static const GLfloat back_mat_diffuse_skeleton_points[] = {0.1, 0.1, 0.1, 1.0};
static const GLfloat back_mat_ambient_skeleton_points[] = {0.1, 0.1, 0.1, 1.0};
static const GLfloat back_mat_specular_skeleton_points[] = {0.0, 0.0, 0.0, 1.0};
static const GLfloat back_mat_emission_skeleton_points[] = {0.0, 0.0, 0.0, 1.0};
static const GLfloat back_mat_shininess_skeleton_points[] = {1};
#endif // SHOW_SKELETON_POINTS
#endif // SHOW_SKELETON

#ifdef USE_TORUSES_TWIN
// Параметры тороидальной спирали 1
static const GLdouble torus1_param_a = 1.0;
static const GLdouble torus1_param_b = 0.3;
static const GLdouble torus1_param_c = 0.6;
static const GLdouble torus1_param_p = 2.0;
static const GLdouble torus1_param_q = 5.0;

#ifdef USE_TWINING
// Параметры тороидальной спирали 2
static const GLdouble torus2_param_a = 1.0;
static const GLdouble torus2_param_b = 0.3;
static const GLdouble torus2_param_c = 0.7;
static const GLdouble torus2_param_p = 1.0;
static const GLdouble torus2_param_q = 7.0;
#endif // USE_TWINING

#endif // USE_TORUSES_TWIN

// Цвет фона
static const GLclampf clear_color[] = {0.01, 0.07, 0.01, 0.0};

#ifdef USE_FOG
// Цвет дымки (тумана)
static const GLfloat fog_color[] = {0.01, 0.07, 0.01, 1.0};

// Плотность дымки (тумана)
static const GLfloat fog_density = 3.7;
#endif // USE_FOG

#ifdef USE_TWINING
// Шаг твин-преобразования
static const GLdouble step_twin = 0.002;

// Нижний предел изменения твининга
static const GLdouble down_lim_twin = 0.0;

// Верхний предел изменения твининга
static const GLdouble up_lim_twin = 1.0;
#endif // USE_TWINING

#ifdef USE_SPIN
// Шаг изменения угла поворота (вращения) системы
static const GLdouble step_spin = 0.2;

// Вектор (ось) вращения системы
static const GLdouble vector_spin[] = {0.0, 0.0, 1.0};
#endif // USE_SPIN

#ifdef USE_STATIC_SPIN
// Угол поворота системы (не изменяемый) в градусах
static const GLdouble static_spin = 0.0;

// Вектор поворота системы (не изменяемый)
static const GLdouble vector_static_spin[] = {0.0, 1.0, 0.0};
#endif // USE_STATIC_SPIN

#ifdef USE_SPIN_SEGMENT
// Шаг изменения вращения чешуйки (сегмента) (в градусах)
static GLdouble step_spin_segment = 0.02;
#endif // USE_SPIN_SEGMENT

#ifdef USE_AUTO_SCALE
// Шаг изменения коэффициентов масштабирования по осям
static const GLdouble step_scale_k[] = {0.005, 0.005, 0.005};

// Минимальная (нижняя) граница значения коэффициентов масштабирования по осям
static const GLdouble min_lim_scale_k[] = {0.2, 0.2, 0.2};

// Максимальная (верхнаяя) граница значения коэффициентов масштабирования по осям
static const GLdouble max_lim_scale_k[] = {1.1, 1.1, 1.1};
#endif // USE_AUTO_SCALE

#ifdef STATIC_SCALE
static const GLdouble static_scale_k[] = {1.0, 1.0, 1.0};
#endif // STATIC_SCALE

// Шаг прорисовки торов (фигур)
static const GLdouble deltat = 0.05;

// Радиус оболочки фигур
static const GLdouble torus_radius = 0.12;

// Количество прямоугольников, составляющих один член оболочки фигуры
static const GLdouble torus_sections = 10;

// Флаг прорисовки полной оболочки фигур или в виде сетки
static const bool solid_flag = true;
//static const bool solid_flag = false;

#if defined(SHOW_PARTITION) || defined(SHOW_TORUS_SECTIONS)
// Толщина линий при прорисовке фигур в виде сетки
static const GLfloat line_width = 1.0;
#endif // SHOW_PARTITION || SHOW_TORUS_SECTIONS

#ifdef SHOW_SKELETON
#ifdef SHOW_SKELETON_LINE
// Размер внутренней осевой линии
static const GLfloat skeleton_line_width = 3.0;
#endif // SHOW_SKELETON_LINE

#ifdef SHOW_SKELETON_POINTS
// Размер опорных точек фигур
static const GLfloat skeleton_point_size = 10.0;
#endif // SHOW_SKELETON_POINTS
#endif // SHOW_SKELETON

#ifdef SHOW_COORD_LINES
// Сфера в точке отсчёта системы координат
static const GLdouble center_coord_sphere_radius = 0.05;
static const GLint center_coord_sphere_slices = 30;
static const GLint center_coord_sphere_stack = 30;
#endif // SHOW_COORD_LINES

#ifdef SHOW_SPHERE_SECTION
// Сфера в центре секции тороидальной спирали
static const bool sphere_section_solid = true;
static const GLdouble sphere_section_radius = 0.05;
static const GLint sphere_section_slices = 25;
static const GLint sphere_section_stack = 25;
#endif // SHOW_SPHERE_SECTION

// ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ ==============================================

#ifdef USE_TWINING
// Твин-преобразование
static GLdouble current_twin = 0.0;
#endif // USE_TWINING

#ifdef USE_SPIN
// Угол поворота системы координат
static GLdouble spin = 0.0;
#endif // USE_SPIN

#ifdef USE_AUTO_SCALE
// Текущие коэффициенты масштабирования по осям
static GLdouble scale_k[] = {1.0, 1.0, 1.0};
#endif // USE_AUTO_SCALE

#ifdef USE_SPIN_SEGMENT
// Угол поворота чешуйки (сегмента) (в градусах)
static GLdouble spin_segment = 0.0;
#endif // USE_SPIN_SEGMENT

// Основные массивы точек
static GLdouble *C1_0 = NULL;
static GLdouble *C2_0 = NULL;
static GLdouble *C1_1 = NULL;
static GLdouble *C2_1 = NULL;
static GLdouble *C1_2 = NULL;
static GLdouble *C2_2 = NULL;

#ifdef SHOW_FPS
static time_t fpstime = 0;
static unsigned int fpscount = 0;
static unsigned int cpscount = 0;
static unsigned int cpscountfake = 0;
static unsigned int cpscountfakeret = 0;
#endif // SHOW_FPS

/* ***************************************************************** */
/* End of file                                                       */
/* ***************************************************************** */
