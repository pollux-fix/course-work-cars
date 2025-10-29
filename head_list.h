/*
всякие штуки для динамических списков
потому что иначе первый заголовочный в помойку превратится
*/

// куда едет машина - направо/налево
typedef enum CarDirection
{
    RIGHT,
    LEFT
} CarDirection;

// на какой полосе из всех машина находится
// макс сколько полос может быть
// typedef enum CarLane
// {
//     ONE,
//     TWO,
//     THREE,
//     FOUR,
//     FIVE
// } CarLane;

typedef struct
{
    float speed;
    float max_speed;
    CarDirection direction;
    // CarLane lane;
    char lane;

    float color[3];
    float position;

    bool is_braking;
    // float target_speed; // скорость после торможения
    // char target_lane; // для перестроения
    // float lane_change_progress;
    // bool is_changing_lane;

} CarNode;

typedef struct ListCar
{
    CarNode car;
    // struct ListCar *prev;
    struct ListCar *next;
} ListCar;

// to right
ListCar *lane_1 = NULL;
ListCar *lane_2 = NULL;
ListCar *lane_3 = NULL;
// to left
ListCar *lane_m1 = NULL;
ListCar *lane_m2 = NULL;
ListCar *lane_m3 = NULL;

void drawHighwayCar(CarNode car);
void updateAdvancedCars(ListCar *head);
float calculateSafeSpeed(CarNode car, float distance);
void checkCollisionAvoidance(ListCar *current);

CarNode create_highway_car(CarDirection direction, char lane);
void insert_car(ListCar **head, CarNode car);
ListCar *createCarNode(CarNode car);

int count_cars(ListCar *head);