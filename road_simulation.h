#ifndef ROAD_SIMULATION_H
#define ROAD_SIMULATION_H

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

// Общие константы
#define MAX_LANES 10
#define MIN_SPEED 0.1f
#define MAX_SPEED 0.2f
#define MAX_CARS 40
#define CAR_LENGTH 2.0
#define ROAD_LENGTH 20.0
#define SAFE_DISTANCE 3.0
#define LANE_CHANGE_SPEED 0.008
#define BRAKING_FACTOR 0.92
#define ACCELERATION 0.002
#define TURN_SPEED 0.15f
#define MARKING_WIDTH 0.1
#define LINE_WIDTH 1.0f
#define WINDOW_BORDER 20

// Общие перечисления
typedef enum
{
    FORWARD,
    TURN_LEFT,
    TURN_RIGHT
} TurnDirection;

typedef enum
{
    RIGHT,
    LEFT
} CarDirection;

typedef enum
{
    HIGHWAY,
    CROSSROAD,
    MENU
} RoadType;

typedef enum
{
    MAIN_MENU,
    APP_INFO,
    ABOUT_US,
    LOAD_FROM_FILE
} MenuOption;

typedef enum
{
    CAR_STATE_NORMAL,
    CAR_STATE_ACCIDENT
} CarState;

// Универсальная структура машины
typedef struct
{
    // Общие поля для всех типов дорог
    float x, y;     // позиция (для перекрестка)
    float position; // позиция на полосе (для шоссе)
    float speed;
    float max_speed;

    // Полосы и направление
    int lane;
    int target_lane;
    CarDirection direction;        // для шоссе
    char direction_x, direction_y; // для перекрестка

    // Перестроение
    bool is_changing_lane;
    float lane_change_progress;

    // Внешний вид
    float color[3];

    // Поведение
    bool is_braking;
    float target_speed;

    // Для перекрестка
    bool in_intersection;
    bool has_priority;
    bool is_turning;
    TurnDirection turn_direction;
    float turn_progress;
    char road_id; // 'H' - горизонтальная, 'V' - вертикальная
    bool will_turn;
    TurnDirection planned_turn;
    bool just_have_turn;

    // Углы поворота
    float turn_start_x, turn_start_y;
    float current_display_angle;

    // Состояние
    CarState state;
    float fixed_position;

    // Тип симуляции
    RoadType road_type;
} UniversalCar;

// Универсальный узел списка
typedef struct ListNode
{
    UniversalCar car;
    struct ListNode *next;
} ListNode;

// Глобальные переменные
extern clock_t start_time;
extern int lines_count;
extern float intersection_size;
extern bool track;
extern MenuOption menu_option;
extern char button_hover;
extern bool in_simulation;
extern double last_time;
extern bool horizontal_green;
extern float last_light_switch;
extern RoadType type_simulation;
extern bool accident_flag;
extern int accident_lane;

// Для перекрестка - массив дорог
extern ListNode *crossroad_roads[];
#define CROSSROAD_ROAD_COUNT 4 // 4 направления: север, юг, запад, восток
extern int crossroad_car_count;

// Основные функции
void init();
void processNormalKeys(unsigned char key, int x, int y);
void menuWindow();
void drawButton(float x, float y, float width, float height, const char *text, int hover);
void drawInfoScreen();
void drawAboutScreen();
void processMouseClick(int button, int state, int x, int y);
void processMouseMove(int x, int y);
void drawButtonNumber(float x, float y, float width, float height, const char *label, bool active);
void drawText(float x, float y, const unsigned char *text);
void infoStatistic(bool flag);
void displayTime(float x, float y);

// Функции для перекрестка
void initCrossroadCar();
void timerCrossroad(int value);
void displayCrossroad();
void addCrossroadCar();
void updateCars();
void decideChangeCrossroad(UniversalCar *car);
void checkCollisionsCrossroad(UniversalCar *car);
bool isSafeToChangeCrossroad(UniversalCar *car, char new_lane, float *safe_speed);
float calculateSpeedCrossroad(UniversalCar *car, float distance);
void drawCrossroadCar(UniversalCar car);
void drawRoads();
void drawIntersection();
void updateTrafficLight();
void decideTurn(UniversalCar *car);
void executeTurn(UniversalCar *car);
bool canTurn(UniversalCar *car, TurnDirection turn);
char getRandomLane(char road_id, char direction);
bool isCollisionImminent(UniversalCar *car1, UniversalCar *car2);

// Функции для работы со списками перекрестка
void init_crossroad_roads();
void free_crossroad_roads();
int get_crossroad_road_index(char road_id, char direction);
ListNode *get_crossroad_road(char road_id, char direction);
void update_all_crossroad_cars();
void draw_all_crossroad_cars();
void add_random_crossroad_car();
void remove_crossroad_cars_out_of_bounds();
ListNode *create_car_node_cross(UniversalCar car);
void insert_car_cross(ListNode **head, UniversalCar car);
void remove_car_from_list(ListNode **head, ListNode *car_to_remove);
int count_cars_in_list(ListNode *head);
UniversalCar create_crossroad_car();

#endif