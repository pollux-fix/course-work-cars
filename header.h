#define _USE_MATH_DEFINES

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

// МАКРОСЫ (их еще проверить, мб что-то не используется)

#define MAX_LANES 10 // Максимум 5 полос в каждую сторону

#define MIN_SPEED 0.1f
#define MAX_SPEED 0.2f
#define TURN_SPEED 0.05f
#define MARKING_WIDTH 0.1
#define LINE_WIDTH 1.0f
#define SAFE_DISTANCE 3.0

#define MAX_CARS 40
#define MAX_LANE_CAR 4
#define ROAD_LENGTH 20.0
#define LANES_PER_DIRECTION 3
#define CAR_LENGTH 2.0
#define BRAKING_FACTOR 0.92
#define ACCELERATION 0.002
#define LANE_CHANGE_SPEED 0.008

#define TURN_RADIUS 1.0
#define WINDOW_BORDER 20

// СТРУКТУРЫ

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
    CAR_STATE_ACCIDENT,
    ANGRY_BIRD
} CarState;

// куда едет машина - направо/налево
typedef enum CarDirection
{
    RIGHT,
    LEFT
} CarDirection;

enum LightState {
    RED,
    YELLOW,
    GREEN
};


typedef struct
{
    float speed;
    float max_speed;
    CarDirection direction;
    char lane;

    float color[3];
    float position;

    bool is_braking;

    // Новые поля для перестроения
    bool is_changing_lane;
    char target_lane;
    float lane_change_progress;
    float target_speed;

    // Для аварий
    int state;
    float fixed_position;

    // bool avaria;

} CarNode;

typedef struct ListCar
{
    CarNode car;
    struct ListCar *next;
} ListCar;

typedef struct 
{
    ListCar *head;
    ListCar *tail;

    int count;
} HighwayLanes;


// структура для машин на перекрестке
typedef struct
{
    float x, y;
    float speed; // текущая скорость
    float max_speed;
    char lane; // текущая полоса
    /* need to delete (3)*/
    char target_lane;
    float lane_change_progress;
    bool is_changing_lane;

    char direction_x, direction_y, direction; // направление движения
    float color[3];
    bool is_braking;
    float target_speed;

    bool has_priority; // приоритет пересечения перекрестка
    bool is_turning;
    TurnDirection turn_direction;
    float turn_progress;
    char road_id; // тип дороги (вертикальная или горизонтальная)
    bool will_turn;
    TurnDirection planned_turn;

    float turn_start_x;          // Начальная X-координата при начале поворота
    float turn_start_y;          // Начальная Y-координата при начале поворота
    float current_display_angle; // Текущий угол поворота для отрисовки машины

    // float start_display_angle;; // Начальный угол поворота для отрисовки машины

    int original_direction; // Для запоминания исходного направления при повороте
    int original_lane;      // Для запоминания исходной полосы при повороте

    //bool GhostCar; // Флаг для фантомной машины (голова списка)

    bool in_intersection;      // Находится ли машина на перекрестке
    bool entered_on_green;     // Въехала ли машина на зеленый
    bool just_have_turn;       // Только что повернула

} AdvancedCar2;

typedef struct ListCarCross
{
    AdvancedCar2 car;
    struct ListCarCross *next;
} ListCarCross;

// Структура для хранения головы и хвоста списка
typedef struct
{
    ListCarCross *head;
    ListCarCross *tail;
} LaneList;



// ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ (тоже проверить, как макросы)

clock_t start_time; // начальное время для таймера

int lines_count = LANES_PER_DIRECTION; // в одну сторону
float intersection_size = (float)LANES_PER_DIRECTION;

bool track = true;                  // флаг для режима паузы
MenuOption menu_option = MAIN_MENU; // опции главного меню

char button_hover = -1;     // флаг наведения мыши на кнопку
bool in_simulation = false; // флаг определения начала моделирования
double last_time = 0.0;     // последнее время (для режима паузы)

bool horizontal_green = true; // Светофор для горизонтальной дороги
float last_light_switch = 0;

RoadType type_simulation = MENU; // текущий режим моделирования

// переменные для поддержки дтп

bool accident_flag = false;
int accident_lane = 0; // полоса на которой авария


// ФУНКЦИИ (пока что все подряд, потом рассортирую)

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

void displayHighway();
void updateHighway(int value);

void addRandomCar();

// перекресток

void timerCrossroad(int value);
void initCrossroadCar();

void displayCrossroad();
void addCrossroadCar();
void updateCars();

void drawRoads();
void drawIntersection();
void updateTrafficLight();

char getRandomLane(char road_id, char direction);

enum LightState checkTrafficLightForCar(AdvancedCar2 *car);

// работа с файлами

void loadFromFile();
void loadSimulation(const char *filename);
void saveSimulation(const char *filename);

void displayAccident();

// Функции для работы с полосами
void init_lanes(int lanes_per_direction);
void free_all_lanes(void);
int get_lane_index(CarDirection direction, char lane_number);
ListCar *get_lane(CarDirection direction, char lane_number);

void drawHighwayCar(CarNode car);
void updateAdvancedCars(ListCar **head);
// void updateAdvancedCars(ListCar *head);
float calculateSafeSpeed(CarNode car, float distance);
void checkCollisionAvoidance(ListCar *current);

CarNode create_highway_car(CarDirection direction, char lane);
// void insert_car(ListCar **head, CarNode car);
void insert_car(HighwayLanes *list, CarNode car);
ListCar *createCarNode(CarNode car);

// int count_cars(ListCar *head);

// Новые универсальные функции
void for_each_lane(void (*func)(ListCar *));
void update_all_cars(void);
void draw_all_cars(void);

void decideLaneChange(ListCar *current_car);
bool isSafeToChangeLane(ListCar *current_car, char new_lane, float *safe_speed);

void remove_cars_out_of_bounds();
void remove_car_from_lane(int lane_index, ListCar *car_to_remove);

void initCrossroadLanes(int lanes_per_direction);