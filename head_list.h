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

//------------------------------------------------------------
// Массив указателей на списки полос
extern ListCar *lanes[];
extern int lane_count;

// Функции для работы с полосами
void init_lanes(int lanes_per_direction);
void free_all_lanes(void);
int get_lane_index(CarDirection direction, char lane_number);
ListCar* get_lane(CarDirection direction, char lane_number);
//------------------------------------------------------

void drawHighwayCar(CarNode car);
void updateAdvancedCars(ListCar **head);
// void updateAdvancedCars(ListCar *head);
float calculateSafeSpeed(CarNode car, float distance);
void checkCollisionAvoidance(ListCar *current);

CarNode create_highway_car(CarDirection direction, char lane);
void insert_car(ListCar **head, CarNode car);
ListCar *createCarNode(CarNode car);

int count_cars(ListCar *head);

//------------------------------------------------------
// Новые универсальные функции
void for_each_lane(void (*func)(ListCar*));
void update_all_cars(void);
void draw_all_cars(void);
//------------------------------------------------------

void decideLaneChange(ListCar *current_car);
bool isSafeToChangeLane(ListCar *current_car, char new_lane, float *safe_speed);

void remove_cars_out_of_bounds();
void remove_car_from_lane(int lane_index, ListCar *car_to_remove);
