// файл для работы со список
// тут всякие функции, типо добавления и удаления из списка
// для динамического списка с машинками

 #include "head_test.h"

 /* 
 - инициализация списка
 - добавление в список
 - удаление из списка (?) мб просто перемещение из головы в хвост
 - поиск машины по позиции (по х или у)
 - создание машины для прямой дороги и перекрестка
 */

CarNode *highway_car = NULL;

// CarNode *crossroad_car = NULL;  //пока не используется
//тестируем чисто на прямой дороге пока что


// создаение узла машинки (выделение памяти)
CarNode* createCarNode(ListCar car)
{
    CarNode *new = (CarNode *)malloc(sizeof(CarNode));
    if (!new)
    {
        printf("Failed to allocate memory");
        return NULL;
    }

    new->car = car;
    new->next = NULL;
    return new;
}

// вставка узла машинки в список
void insertInList(CarNode** head, ListCar car)
{
    CarNode *new = createCarNode(car);
    if (!new)
        return;

    if (*head == NULL)
    {
        *head = new;
    }
    else 
    {
        CarNode *current = *head;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = new;
    }
}

// инициализация инфо про машину
ListCar createHighwayCar()
{
    if (car_count >= MAX_CARS)
        return;

    ListCar car = {0}; // memset 0
    char direction = (rand() % 2) ? 1 : -1;
    char lane = (rand() % lines_count) + 1;
    if (direction == -1)
        lane = -lane;

    // проверка на возможность добавления машины - пока пофиг

    car.lane = lane;
    car.direction_x = direction;
    car.target_lane = -1;
    car.lane_change_progress = 0.0;
    car.is_braking = false;
    car.is_changing_lane = false;
    car.max_speed = MIN_SPEED + (rand() % (int)((MAX_SPEED - MIN_SPEED) * 100)) * 0.01;
    car.speed = car.max_speed * 0.5;

    new_car.color[0] = (50 + rand() % 50) / 100.0f; // R: 0.5-1.0 (исключает 0.0)
    new_car.color[1] = (rand() % 100) / 100.0f;     // G: 0.0-1.0
    new_car.color[2] = (rand() % 100) / 100.0f;     // B: 0.0-1.0

    if (direction == 1)
    {
        car.x = -WINDOW_BORDER - 1;
    }
    else
    {
        car.x = WINDOW_BORDER + 1;
    }

    car_count++;
    return car;
}

void updateListCar()
{
    // это переделка updateHighway... ну в идеале
}



// аналогичная инициализация для перекрестка, но мне пока лень