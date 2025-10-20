#define _USE_MATH_DEFINES

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

// макросы

#define MIN_SPEED 0.05f
#define MAX_SPEED 0.1f
#define MARKING_WIDTH 0.1
#define LINE_WIDTH 1.0f
#define SAFE_DISTANCE 4.0

#define MAX_CARS 15
#define ROAD_LENGTH 20.0
#define LANES_PER_DIRECTION 3 // по умолчанию
#define CAR_LENGTH 2.0
#define BRAKING_FACTOR 0.92
#define ACCELERATION 0.002
#define LANE_CHANGE_SPEED 0.008

#define TURN_SPEED 0.01
#define TURN_RADIUS 1.0
#define WINDOW_BORDER 20

// направление движения на перекрестке
typedef enum
{
    FORWARD,
    TURN_LEFT,
    TURN_RIGHT
} TurnDirection;

// тип дороги при моделировании
typedef enum
{
    HIGHWAY,
    CROSSROAD,
    MENU
} RoadType;

// опции меню
typedef enum
{
    MAIN_MENU,
    APP_INFO,
    ABOUT_US,
    LOAD_FROM_FILE
} MenuOption;

// флаги для поддержки дтп
typedef enum
{
    CAR_STATE_NORMAL,
    CAR_STATE_ACCIDENT
} CarState;

// структура для машин на автостраде
typedef struct
{
    float position; // положение на полосе
    float speed;
    float max_speed;
    char lane;
    char target_lane; // для перестроения
    float lane_change_progress;
    char direction;
    float color[3];
    bool is_changing_lane;
    bool is_braking;
    float target_speed; // скорость после торможения

    CarState state;
    float fixed_position;
} AdvancedCar;

// структура для машин на перекрестке
typedef struct
{
    float x, y;
    float speed; // текущая скорость
    float max_speed;
    char lane; // текущая полоса
    char target_lane;
    float lane_change_progress;
    char direction_x, direction_y; // направление движения
    float color[3];
    bool is_changing_lane;
    bool is_braking;
    float target_speed;

    bool in_intersection;
    bool has_priority; // приоритет пересечения перекрестка
    bool is_turning;
    TurnDirection turn_direction;
    float turn_progress;
    char road_id; // тип дороги (вертикальная или горизонтальная)
    bool will_turn;
    TurnDirection planned_turn;
    bool just_have_turn;
} AdvancedCar2;

// вот тут динамический список (пока тестовый вариант)

typedef struct 
{
    // общие поля
    float x; //position for highway
    float speed;
    float max_speed;
    char lane; // текущая полоса
    char target_lane;
    float lane_change_progress;
    char direction_x; // направление движения
    float color[3];
    bool is_changing_lane;
    bool is_braking;
    float target_speed;

    RoadType road_type;

    union 
    {
        struct {
            CarState state;
            float fixed_position;
        } highway;

        struct {
            float y;
            char direction_y;
            bool in_intersection;
            bool has_priority; // приоритет пересечения перекрестка
            bool is_turning;
            TurnDirection turn_direction;
            float turn_progress;
            char road_id; // тип дороги (вертикальная или горизонтальная)
            bool will_turn;
            TurnDirection planned_turn;
            bool just_have_turn;
        } crossroad;
    } more_info;
} ListCar;

//узел
typedef struct CarNode 
{
    ListCar car;
    // struct CarNode *prev; // хз надо ли, но пусть будет
    struct CarNode *next;
} CarNode;

// конец описания динамич списка



// Глобальные переменные

AdvancedCar advanced_cars[MAX_CARS]; // машины на автостраде
AdvancedCar2 cars[MAX_CARS];         // машины на перекрестке

clock_t start_time; // начальное время для таймера

int lines_count = LANES_PER_DIRECTION; // в одну сторону
float intersection_size = (float)LANES_PER_DIRECTION;

bool track = true;                  // флаг для режима паузы
MenuOption menu_option = MAIN_MENU; // опции главного меню

char button_hover = -1;     // флаг наведения мыши на кнопку
bool in_simulation = false; // флаг определения начала моделирования
double last_time = 0.0;     // последнее время (для режима паузы)

int car_count = 0; // текущее количество машин, которые инициализированы

bool horizontal_green = true; // Светофор для горизонтальной дороги
float last_light_switch = 0;

RoadType type_simulation = MENU; // текущий режим моделирования

// переменные для поддержки дтп

bool accident_flag = false;
int accident_lane = 0; // полоса на которой авария

// общие функции

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

// прямая дорога

void displayTime(float x, float y);
void drawHighway();
void initHighwayCar();

void drawHighwayCar(AdvancedCar car);
void displayHighway();
void updateHighway(int value);

float calculateSafeSpeed(AdvancedCar *car, float distance);
bool isSafeToChangeLane(AdvancedCar *car, char new_lane, float *safe_speed);
void checkCollisionAvoidance(AdvancedCar *car);
void decideLaneChange(AdvancedCar *car);
void updateAdvancedCars();
void addRandomCar();

// перекресток

void timerCrossroad(int value);
void initCrossroadCar();

void displayCrossroad();
void addCrossroadCar();
void updateCars();

void decideChangeCrossroad(AdvancedCar2 *car);
void checkCollisionsCrossroad(AdvancedCar2 *car);
bool isSafeToChangeCrossroad(AdvancedCar2 *car, char new_lane, float *safe_speed);
float calculateSpeedCrossroad(AdvancedCar2 *car, float distance);

void drawCrossroadCar(AdvancedCar2 car);
void drawRoads();
void drawIntersection();
void updateTrafficLight();

void decideTurn(AdvancedCar2 *car);
void executeTurn(AdvancedCar2 *car);
bool canTurn(AdvancedCar2 *car, TurnDirection turn);
char getRandomLane(char road_id, char direction);
bool isCollisionImminent(AdvancedCar2 *car1, AdvancedCar2 *car2);

// работа с файлами

void loadFromFile();
void loadSimulation(const char *filename);
void saveSimulation(const char *filename);

void displayAccident();