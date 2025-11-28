#include "road_simulation.h"

// Глобальные переменные
clock_t start_time;
int lines_count = 3;
float intersection_size = 3.0f;
bool track = true;
MenuOption menu_option = MAIN_MENU;
char button_hover = -1;
bool in_simulation = false;
double last_time = 0.0;
bool horizontal_green = true;
float last_light_switch = 0;
RoadType type_simulation = MENU;
bool accident_flag = false;
int accident_lane = 0;

// Для перекрестка - массив дорог
ListNode *crossroad_roads[CROSSROAD_ROAD_COUNT] = {NULL};
int crossroad_car_count = 0;

// Создание нового узла
ListNode *create_car_node_cross(UniversalCar car)
{
    ListNode *new_node = (ListNode *)malloc(sizeof(ListNode));
    if (!new_node)
    {
        printf("Memory allocation failed for car node\n");
        return NULL;
    }
    new_node->car = car;
    new_node->next = NULL;
    return new_node;
}

// Вставка машины в список
void insert_car_cross(ListNode **head, UniversalCar car)
{
    ListNode *new_node = create_car_node_cross(car);
    if (!new_node)
        return;

    if (*head == NULL)
    {
        *head = new_node;
    }
    else
    {
        ListNode *current = *head;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = new_node;
    }
}

// Удаление машины из списка
void remove_car_from_list(ListNode **head, ListNode *car_to_remove)
{
    if (*head == NULL || car_to_remove == NULL)
        return;

    if (*head == car_to_remove)
    {
        *head = car_to_remove->next;
        free(car_to_remove);
        return;
    }

    ListNode *current = *head;
    while (current->next != NULL && current->next != car_to_remove)
    {
        current = current->next;
    }

    if (current->next == car_to_remove)
    {
        ListNode *to_free = current->next;
        current->next = to_free->next;
        free(to_free);
    }
}

// Подсчет машин в списке
int count_cars_in_list(ListNode *head)
{
    int count = 0;
    ListNode *current = head;
    while (current != NULL)
    {
        count++;
        current = current->next;
    }
    return count;
}

void init_crossroad_roads()
{
    for (int i = 0; i < CROSSROAD_ROAD_COUNT; i++)
    {
        crossroad_roads[i] = NULL;
    }
}

void free_crossroad_roads()
{
    for (int i = 0; i < CROSSROAD_ROAD_COUNT; i++)
    {
        ListNode *current = crossroad_roads[i];
        while (current != NULL)
        {
            ListNode *to_free = current;
            current = current->next;
            free(to_free);
        }
        crossroad_roads[i] = NULL;
    }
}

int get_crossroad_road_index(char road_id, char direction)
{
    // Индексы: 0-север, 1-юг, 2-запад, 3-восток
    if (road_id == 'H')
    { // Горизонтальная дорога
        if (direction == 1)
            return 3; // восток
        else
            return 2; // запад
    }
    else
    { // Вертикальная дорога
        if (direction == 1)
            return 0; // север
        else
            return 1; // юг
    }
}

ListNode *get_crossroad_road(char road_id, char direction)
{
    int index = get_crossroad_road_index(road_id, direction);
    return crossroad_roads[index];
}

void update_all_crossroad_cars()
{
    for (int i = 0; i < CROSSROAD_ROAD_COUNT; i++)
    {
        ListNode *current = crossroad_roads[i];
        ListNode *prev = NULL;

        while (current != NULL)
        {
            UniversalCar *car = &current->car;
            ListNode *next = current->next;

            if (track)
            {
                // Проверяем столкновения
                checkCollisionsCrossroad(car);

                // Если машина планирует поворот, но еще не начала, проверяем условия
                if (car->will_turn && !car->is_turning)
                {
                    // Замедляемся при приближении к точке поворота с увеличенной дистанцией
                    float dist_to_turn;
                    if (car->road_id == 'H')
                        dist_to_turn = fabs(car->x) - intersection_size;
                    else
                        dist_to_turn = fabs(car->y) - intersection_size;

                    // Увеличиваем дистанцию замедления для поворота
                    if (dist_to_turn < SAFE_DISTANCE * 4 && dist_to_turn > 0)
                    {
                        if (car->speed > TURN_SPEED)
                            car->speed = fmax(TURN_SPEED, car->speed * 0.98f);
                    }
                }

                // Выполнение поворота
                if (car->is_turning)
                {
                    executeTurn(car);
                }
                else
                {
                    // Обычное движение
                    car->x += car->speed * car->direction_x;
                    car->y += car->speed * car->direction_y;
                }

                // Удаление машин, выехавших за пределы
                if (fabs(car->x) > WINDOW_BORDER + 10 || fabs(car->y) > WINDOW_BORDER + 10)
                {
                    remove_car_from_list(&crossroad_roads[i], current);
                    crossroad_car_count--;
                    current = next;
                    continue;
                }
            }

            prev = current;
            current = next;
        }
    }
}

void draw_all_crossroad_cars()
{
    for (int i = 0; i < CROSSROAD_ROAD_COUNT; i++)
    {
        ListNode *current = crossroad_roads[i];
        while (current != NULL)
        {
            drawCrossroadCar(current->car);
            current = current->next;
        }
    }
}

void add_random_crossroad_car()
{
    static float last_add_time = 0;
    float current_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;

    if (current_time - last_add_time < 2.0)
        return;

    // Проверяем общее количество машин
    int total_cars = 0;
    for (int i = 0; i < CROSSROAD_ROAD_COUNT; i++)
    {
        total_cars += count_cars_in_list(crossroad_roads[i]);
    }

    if (total_cars >= MAX_CARS)
        return;

    last_add_time = current_time;

    // Создаем новую машину для перекрестка
    UniversalCar new_car = create_crossroad_car();

    // Добавляем в соответствующую дорогу
    int road_index = get_crossroad_road_index(new_car.road_id, new_car.direction_x);
    insert_car_cross(&crossroad_roads[road_index], new_car);
    crossroad_car_count++;
}

UniversalCar create_crossroad_car()
{
    UniversalCar car = {0};

    car.road_type = CROSSROAD;
    car.speed = 0.05f + (rand() % 10) * 0.01f;
    car.max_speed = car.speed;

    // Более яркие цвета
    car.color[0] = 0.2f + (rand() % 80) / 100.0f;
    car.color[1] = 0.2f + (rand() % 80) / 100.0f;
    car.color[2] = 0.2f + (rand() % 80) / 100.0f;

    // Выбор случайной дороги и направления
    if (rand() % 2 == 0)
    {
        car.road_id = 'H'; // Горизонтальная дорога
        car.direction_x = (rand() % 2) ? 1 : -1;
        car.direction_y = 0;
        car.lane = getRandomLane('H', car.direction_x);
    }
    else
    {
        car.road_id = 'V'; // Вертикальная дорога
        car.direction_x = 0;
        car.direction_y = (rand() % 2) ? 1 : -1;
        car.lane = getRandomLane('V', car.direction_y);
    }

    // Установка начальной позиции (дальше от перекрестка)
    if (car.road_id == 'H')
    {
        car.x = (car.direction_x == 1) ? -WINDOW_BORDER - 8 : WINDOW_BORDER + 8;
        car.y = car.lane * LINE_WIDTH;
    }
    else
    {
        car.x = car.lane * LINE_WIDTH;
        car.y = (car.direction_y == 1) ? -WINDOW_BORDER - 8 : WINDOW_BORDER + 8;
    }

    car.is_braking = false;
    car.in_intersection = false;
    car.has_priority = true;
    car.is_turning = false;
    car.will_turn = false;
    car.just_have_turn = false;

    // Установка начального угла
    if (car.road_id == 'H')
        car.current_display_angle = (car.direction_x == 1) ? 0 : 180;
    else
        car.current_display_angle = (car.direction_y == 1) ? 90 : 270;

    // Решение о повороте
    decideTurn(&car);

    return car;
}

// Основная функция
int main(int argc, char **argv)
{
    srand(time(NULL));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowPosition(400, 50);
    glutInitWindowSize(900, 700);
    glutCreateWindow("Road Simulation");

    glutDisplayFunc(menuWindow);
    glutMouseFunc(processMouseClick);
    glutPassiveMotionFunc(processMouseMove);
    glutKeyboardFunc(processNormalKeys);
    glutMainLoop();

    return EXIT_SUCCESS;
}

// Инициализация машин на перекрестке
void initCrossroadCar()
{
    init_crossroad_roads();
    for (int i = 0; i < 8; i++)
    {
        addCrossroadCar();
    }
}

// Добавление машины на перекресток (старая функция для совместимости)
void addCrossroadCar()
{
    add_random_crossroad_car();
}

// Обновление отображения
void timerCrossroad(int value)
{
    addCrossroadCar();
    if (track)
    {
        updateCars();
    }
    glutPostRedisplay();
    glutTimerFunc(16, timerCrossroad, 0);
}

// Обновление машин для перекрестка
void updateCars()
{
    updateTrafficLight();
    update_all_crossroad_cars();
}

// Отображение перекрестка
void displayCrossroad()
{
    glClear(GL_COLOR_BUFFER_BIT);
    drawRoads();
    drawIntersection();
    draw_all_crossroad_cars();
    infoStatistic(true);
    glutSwapBuffers();
}

// обработка нажатия мыши
void processMouseClick(int button, int state, int x, int y)
{
    // нажата левая кнопка мыши
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        if (!in_simulation)
        {
            // преобразуем положение мыши на экране в глут координаты
            float glX = (x / (float)glutGet(GLUT_WINDOW_WIDTH)) * 2.0f - 1.0f;
            float glY = 1.0f - (y / (float)glutGet(GLUT_WINDOW_HEIGHT)) * 2.0f;

            // кнопки на главном меню
            if (menu_option == MAIN_MENU)
            {
                if (glX >= -0.5f && glX <= 0.0f)
                {
                    // окно с информацией
                    if (glY >= 0.4f && glY <= 0.5f)
                    {
                        menu_option = APP_INFO;
                    }
                    // моделирование автострады
                    else if (glY >= 0.25f && glY <= 0.35f)
                    {
                        // start_time = clock();
                        // init();
                        // initHighwayCar();
                        // in_simulation = true;
                        // type_simulation = HIGHWAY;
                        // glutDisplayFunc(displayHighway);
                        // glutTimerFunc(0, updateHighway, 0);
                        return;
                    }
                    // моделирование перекрестка
                    else if (glY >= 0.1f && glY <= 0.2)
                    {
                        start_time = clock();
                        init();
                        initCrossroadCar();
                        in_simulation = true;
                        type_simulation = CROSSROAD;
                        glutDisplayFunc(displayCrossroad);
                        glutTimerFunc(0, timerCrossroad, 0);
                        return;
                    }
                    // окно с информацией
                    else if (glY >= -0.2f && glY <= -0.1f)
                    {
                        menu_option = ABOUT_US;
                    }
                }
                // кнопки выбора количества полос
                else if (glY >= 0.1f && glY <= 0.2f)
                {
                    if (glX >= 0.15f && glX <= 0.25f)
                    {
                        lines_count = 1;
                        intersection_size = 1.0f;
                    }
                    else if (glX >= 0.3f && glX <= 0.4f)
                    {
                        lines_count = 2;
                        intersection_size = 2.0f;
                    }
                    else if (glX >= 0.45f && glX <= 0.55f)
                    {
                        lines_count = 3;
                        intersection_size = 3.0f;
                    }
                    else if (glX >= 0.6f && glX <= 0.7f)
                    {
                        lines_count = 4;
                        intersection_size = 4.0f;
                    }
                    else if (glX >= 0.75f && glX <= 0.85f)
                    {
                        lines_count = 5;
                        intersection_size = 5.0f;
                    }
                }
                // кнопка загрузки из файла
                else if (glX >= 0.15f && glX <= 0.65 && glY >= -0.2f && glY <= -0.1f)
                {
                    menu_option = LOAD_FROM_FILE;
                }
            }
            // возврат в главное меню из информационных окон
            else if (menu_option == ABOUT_US || menu_option == APP_INFO || menu_option == LOAD_FROM_FILE)
            {
                if (glX >= -0.3f && glX <= 0.1f && glY >= -0.5f && glY <= -0.4f)
                {
                    menu_option = MAIN_MENU;
                }
            }
        }

        glutPostRedisplay();
    }
}


// проверка на столкновение мд машинами
bool isCollisionImminent(UniversalCar *car1, UniversalCar *car2)
{
    // Проверяем столкновение по прямоугольникам
    float car1_left, car1_right, car1_top, car1_bottom;
    float car2_left, car2_right, car2_top, car2_bottom;

    // Определяем границы первой машины
    if (car1->direction_x != 0)
    {
        // Горизонтальное движение
        car1_left = car1->x - 1;
        car1_right = car1->x + 1;
        car1_top = car1->y + 0.5;
        car1_bottom = car1->y - 0.5;
    }
    else
    {
        // Вертикальное движение
        car1_left = car1->x - 0.5;
        car1_right = car1->x + 0.5;
        car1_top = car1->y + 1;
        car1_bottom = car1->y - 1;
    }

    // Определяем границы второй машины
    if (car2->direction_x != 0)
    {
        // Горизонтальное движение
        car2_left = car2->x - 1;
        car2_right = car2->x + 1;
        car2_top = car2->y + 0.5;
        car2_bottom = car2->y - 0.5;
    }
    else
    {
        // Вертикальное движение
        car2_left = car2->x - 0.5;
        car2_right = car2->x + 0.5;
        car2_top = car2->y + 1;
        car2_bottom = car2->y - 1;
    }

    // Проверяем пересечение прямоугольников
    return !(car1_right < car2_left ||
             car1_left > car2_right ||
             car1_top < car2_bottom ||
             car1_bottom > car2_top);
}

// выбираем случайную полосу
char getRandomLane(char road_id, char direction)
{
    char lane = (rand() % lines_count) + 1;

    // Горизонтальная дорога: положительные полосы сверху, отрицательные снизу
    if (road_id == 'H')
    {
        return (direction == 1) ? lane : -lane;
    }
    // Вертикальная дорога: положительные полосы справа, отрицательные слева
    else
    {
        return (direction == 1) ? lane : -lane;
    }
}

// отрисовка дорог для перекрестка
void drawRoads()
{
    // горизонтальная
    glColor3f(0.47, 0.47, 0.47);
    glBegin(GL_QUADS);
    glVertex2f(-WINDOW_BORDER, -(lines_count + 0.5) * LINE_WIDTH);
    glVertex2f(WINDOW_BORDER, -(lines_count + 0.5) * LINE_WIDTH);
    glVertex2f(WINDOW_BORDER, (lines_count + 0.5) * LINE_WIDTH);
    glVertex2f(-WINDOW_BORDER, (lines_count + 0.5) * LINE_WIDTH);
    glEnd();

    // Вертикальная дорога
    if (type_simulation == CROSSROAD)
    {
        glBegin(GL_QUADS);
        glVertex2f(-(lines_count + 0.5) * LINE_WIDTH, -WINDOW_BORDER);
        glVertex2f((lines_count + 0.5) * LINE_WIDTH, -WINDOW_BORDER);
        glVertex2f((lines_count + 0.5) * LINE_WIDTH, WINDOW_BORDER);
        glVertex2f(-(lines_count + 0.5) * LINE_WIDTH, WINDOW_BORDER);
        glEnd();
    }

    // Разметка горизонтальной дороги
    glColor3f(1.0, 1.0, 1.0);
    for (int i = -lines_count; i <= lines_count; i++)
    {
        glColor3f(0.81, 0.81, 0.81);
        glLineStipple(1.0f, 0x00FF);
        glEnable(GL_LINE_STIPPLE);
        glBegin(GL_LINES);
        glVertex2f(-WINDOW_BORDER, (i + 0.5) * LINE_WIDTH);
        glVertex2f(WINDOW_BORDER, (i + 0.5) * LINE_WIDTH);
        glEnd();
        glDisable(GL_LINE_STIPPLE);
    }

    // Разметка вертикальной дороги
    if (type_simulation == CROSSROAD)
    {
        for (int i = -lines_count; i <= lines_count; i++)
        {
            glColor3f(0.81, 0.81, 0.81);
            glLineStipple(1.0f, 0x00FF);
            glEnable(GL_LINE_STIPPLE);
            glBegin(GL_LINES);
            glVertex2f((i + 0.5) * LINE_WIDTH, -WINDOW_BORDER);
            glVertex2f((i + 0.5) * LINE_WIDTH, WINDOW_BORDER);
            glEnd();
            glDisable(GL_LINE_STIPPLE);
        }
    }

    // Края дорог
    glColor3f(0.0, 0.0, 0.0);
    glLineWidth(2.0);

    // Горизонтальная дорога
    glBegin(GL_LINES);
    glVertex2f(-WINDOW_BORDER, -(lines_count + 0.5) * LINE_WIDTH);
    glVertex2f(WINDOW_BORDER, -(lines_count + 0.5) * LINE_WIDTH);
    glVertex2f(-WINDOW_BORDER, (lines_count + 0.5) * LINE_WIDTH);
    glVertex2f(WINDOW_BORDER, (lines_count + 0.5) * LINE_WIDTH);
    glEnd();

    // Вертикальная дорога
    if (type_simulation == CROSSROAD)
    {
        glBegin(GL_LINES);
        glVertex2f(-(lines_count + 0.5) * LINE_WIDTH, -WINDOW_BORDER);
        glVertex2f(-(lines_count + 0.5) * LINE_WIDTH, WINDOW_BORDER);
        glVertex2f((lines_count + 0.5) * LINE_WIDTH, -WINDOW_BORDER);
        glVertex2f((lines_count + 0.5) * LINE_WIDTH, WINDOW_BORDER);
        glEnd();
    }

    glLineWidth(1.0);

    // островки безопасности
    glColor3f(0.2, 0.2, 0.2);
    glBegin(GL_QUADS);
    glVertex2f(-WINDOW_BORDER, -0.5 * LINE_WIDTH);
    glVertex2f(WINDOW_BORDER, -0.5 * LINE_WIDTH);
    glVertex2f(WINDOW_BORDER, 0.5 * LINE_WIDTH);
    glVertex2f(-WINDOW_BORDER, 0.5 * LINE_WIDTH);
    glEnd();

    if (type_simulation == CROSSROAD)
    {
        glBegin(GL_QUADS);
        glVertex2f(-0.5 * LINE_WIDTH - 0.1, -WINDOW_BORDER);
        glVertex2f(-0.5 * LINE_WIDTH - 0.1, WINDOW_BORDER);
        glVertex2f(0.5 * LINE_WIDTH + 0.1, WINDOW_BORDER);
        glVertex2f(0.5 * LINE_WIDTH + 0.1, -WINDOW_BORDER);
        glEnd();
    }
}

// отрисовка перекрестка
void drawIntersection()
{
    // сам перекресток
    glColor3f(0.47, 0.47, 0.47);
    if (type_simulation == CROSSROAD)
    {
        glBegin(GL_QUADS);
        glVertex2f(-intersection_size - 0.5, -intersection_size - 0.5);
        glVertex2f(intersection_size + 0.5, -intersection_size - 0.5);
        glVertex2f(intersection_size + 0.5, intersection_size + 0.5);
        glVertex2f(-intersection_size - 0.5, intersection_size + 0.5);
        glEnd();
    }

    // Отрисовка светофоров
    float light_size = 0.5;

    // Горизонтальный светофор (справа)
    glColor3f(0.0, 0.0, 0.0);

    if (type_simulation == CROSSROAD)
    {
        glBegin(GL_QUADS);
        glVertex2f(intersection_size + 0.5, -0.5);
        glVertex2f(intersection_size + 1.0, -0.5);
        glVertex2f(intersection_size + 1.0, 0.5);
        glVertex2f(intersection_size + 0.5, 0.5);
        glEnd();
    }

    // Красный/зеленый для горизонтальной дороги
    if (horizontal_green)
    {
        glColor3f(0.0, 1.0, 0.0);
    }
    else
    {
        glColor3f(1.0, 0.0, 0.0);
    }
    if (type_simulation == CROSSROAD)
    {
        glBegin(GL_QUADS);
        glVertex2f(intersection_size + 0.6, -0.4);
        glVertex2f(intersection_size + 0.9, -0.4);
        glVertex2f(intersection_size + 0.9, 0.4);
        glVertex2f(intersection_size + 0.6, 0.4);
        glEnd();
    }

    // Вертикальный светофор (сверху)
    glColor3f(0.0, 0.0, 0.0);
    if (type_simulation == CROSSROAD)
    {
        glBegin(GL_QUADS);
        glVertex2f(-0.5, intersection_size + 0.5);
        glVertex2f(0.5, intersection_size + 0.5);
        glVertex2f(0.5, intersection_size + 1.2);
        glVertex2f(-0.5, intersection_size + 1.2);
        glEnd();
    }

    // Красный/зеленый для вертикальной дороги
    if (!horizontal_green)
    {
        glColor3f(0.0, 1.0, 0.0); // зеленый
    }
    else
    {
        glColor3f(1.0, 0.0, 0.0); // красный
    }
    if (type_simulation == CROSSROAD)
    {
        glBegin(GL_QUADS);
        glVertex2f(-0.4, intersection_size + 0.6);
        glVertex2f(0.4, intersection_size + 0.6);
        glVertex2f(0.4, intersection_size + 1.0);
        glVertex2f(-0.4, intersection_size + 1.0);
        glEnd();
    }
}

// отрисовка машины на перекрестке
void drawCrossroadCar(UniversalCar car)
{
    glPushMatrix();

    // Позиция машины
    glTranslatef(car.x, car.y, 0);

    float display_angle;
    if (car.is_turning)
    {
        display_angle = car.current_display_angle; // Используем угол, рассчитанный в executeTurn
    }
    else
    {
        // Стандартные углы для прямолинейного движения
        if (car.direction_x == -1)
            display_angle = 180;
        else if (car.direction_y == 1)
            display_angle = 90;
        else if (car.direction_y == -1)
            display_angle = 270;
        else
            display_angle = 0; // По умолчанию для direction_x = 1
    }
    glRotatef(display_angle, 0, 0, 1);

    // Индикаторы
    if (car.is_braking)
    {
        glColor3f(1.0, 0.0, 0.0);
        glBegin(GL_LINES);
        glVertex2f(-0.8, -0.4);
        glVertex2f(-0.8, -0.5);
        glVertex2f(-0.7, -0.4);
        glVertex2f(-0.7, -0.5);
        glEnd();
    }

    if (car.will_turn)
    {
        glColor3f(0.0, 0.0, 1.0);
        glBegin(GL_TRIANGLES);
        if (car.planned_turn == TURN_LEFT)
        {
            glVertex2f(0.5, 0.5);
            glVertex2f(0.8, 0.0);
            glVertex2f(0.5, -0.5);
        }
        else
        {
            glVertex2f(-0.5, 0.5);
            glVertex2f(-0.8, 0.0);
            glVertex2f(-0.5, -0.5);
        }
        glEnd();
    }

    // Кузов машины
    glColor3fv(car.color);
    glBegin(GL_QUADS);
    glVertex2f(-0.7, -0.3);
    glVertex2f(0.7, -0.3);
    glVertex2f(0.7, 0.3);
    glVertex2f(-0.7, 0.3);
    glEnd();

    // Окна машины
    glColor3f(car.color[0] * 0.7, car.color[1] * 0.7, car.color[2] * 0.7);
    glBegin(GL_QUADS);
    glVertex2f(-0.3, -0.3);
    glVertex2f(0.5, -0.3);
    glVertex2f(0.5, 0.3);
    glVertex2f(-0.3, 0.3);
    glEnd();

    // Колеса
    glColor3f(0.1, 0.1, 0.1);
    glBegin(GL_QUADS);
    glVertex2f(-0.6, -0.4);
    glVertex2f(-0.4, -0.4);
    glVertex2f(-0.4, -0.3);
    glVertex2f(-0.6, -0.3);

    glVertex2f(0.4, -0.4);
    glVertex2f(0.6, -0.4);
    glVertex2f(0.6, -0.3);
    glVertex2f(0.4, -0.3);

    glVertex2f(-0.6, 0.3);
    glVertex2f(-0.4, 0.3);
    glVertex2f(-0.4, 0.4);
    glVertex2f(-0.6, 0.4);

    glVertex2f(0.4, 0.3);
    glVertex2f(0.6, 0.3);
    glVertex2f(0.6, 0.4);
    glVertex2f(0.4, 0.4);
    glEnd();

    glPopMatrix();
}

// решение о повороте
void decideTurn(UniversalCar *car)
{
    if (car->will_turn || car->is_turning)
        return;

    // Машины на крайних полосах почти всегда поворачивают
    if (abs(car->lane) == 1) // Крайняя левая полоса - поворачивает налево
    {
        car->will_turn = true;
        car->planned_turn = TURN_LEFT;
    }
    else if (abs(car->lane) == lines_count) // Крайняя правая полоса - поворачивает направо
    {
        car->will_turn = true;
        car->planned_turn = TURN_RIGHT;
    }
    // Для средних полос - 30% шанс повернуть (чтобы не все поворачивали)
    else if (rand() % 100 < 80)
    {
        car->will_turn = true;
        // Случайно выбираем направление поворота
        car->planned_turn = (rand() % 2 == 0) ? TURN_LEFT : TURN_RIGHT;

        // Проверяем, возможен ли выбранный поворот с текущей полосы
        if ((car->planned_turn == TURN_LEFT && abs(car->lane) != 1) ||
            (car->planned_turn == TURN_RIGHT && abs(car->lane) != lines_count))
        {
            // Если поворот невозможен с этой полосы, отменяем
            car->will_turn = false;
        }
    }
}

// проверка возможности поворота
bool canTurn(UniversalCar *car, TurnDirection turn)
{
    // Проверяем, находится ли машина на правильной полосе для поворота
    if (turn == TURN_LEFT && abs(car->lane) != 1)
        return false;
    if (turn == TURN_RIGHT && abs(car->lane) != lines_count)
        return false;

    // Предсказываем новую полосу после поворота
    int new_lane;
    if (turn == TURN_LEFT)
    {
        new_lane = (car->road_id == 'H') ? 1 : -1;
    }
    else // TURN_RIGHT
    {
        new_lane = (car->road_id == 'H') ? -lines_count : lines_count;
    }

    // Проверяем машины на целевой дороге
    char target_road_id = (car->road_id == 'H') ? 'V' : 'H';

    for (int i = 0; i < CROSSROAD_ROAD_COUNT; i++)
    {
        ListNode *current = crossroad_roads[i];
        while (current != NULL)
        {
            UniversalCar *other_car = &current->car;

            if (other_car == car || other_car->road_id != target_road_id)
            {
                current = current->next;
                continue;
            }

            // Проверяем машины на той же полосе
            if (other_car->lane == new_lane)
            {
                // Проверяем расстояние до перекрестка
                float dist_to_intersection;
                if (target_road_id == 'H')
                {
                    dist_to_intersection = fabs(other_car->x) - intersection_size;
                }
                else
                {
                    dist_to_intersection = fabs(other_car->y) - intersection_size;
                }

                // Если машина близко к перекрестку - поворот запрещен
                if (dist_to_intersection < SAFE_DISTANCE * 2)
                {
                    return false;
                }
            }
            current = current->next;
        }
    }

    return true;
}

// совершение поворота
void executeTurn(UniversalCar *car)
{
    // Если машина еще не поворачивает, проверяем условия для начала поворота
    if (!car->is_turning)
    {
        // Упрощенное условие: машина достигла точки начала поворота
        bool at_turn_point = false;

        if (car->road_id == 'H') // Горизонтальная дорога
        {
            if (car->direction_x == 1) // Движение вправо
                at_turn_point = (car->x >= -intersection_size);
            else // Движение влево
                at_turn_point = (car->x <= intersection_size);
        }
        else // Вертикальная дорога
        {
            if (car->direction_y == 1) // Движение вверх
                at_turn_point = (car->y >= -intersection_size);
            else // Движение вниз
                at_turn_point = (car->y <= intersection_size);
        }

        if (car->will_turn && at_turn_point && canTurn(car, car->planned_turn))
        {
            car->is_turning = true;
            car->turn_progress = 0.0f;
            car->speed = TURN_SPEED;
            car->turn_direction = car->planned_turn;
            car->turn_start_x = car->x;
            car->turn_start_y = car->y;
        }
        else if (car->will_turn && at_turn_point)
        {
            // Если поворот невозможен, отменяем
            car->will_turn = false;
        }
        return;
    }

    // Выполнение поворота - упрощенная версия
    car->turn_progress += 0.02f; // Более быстрый поворот
    float progress = fmin(1.0f, car->turn_progress);

    // Простая интерполяция для поворота
    if (car->road_id == 'H') // Горизонтальная -> Вертикальная
    {
        if (car->turn_direction == TURN_LEFT)
        {
            // Поворот налево
            if (car->direction_x == 1) // Вправо -> Вверх
            {
                car->x = car->turn_start_x + progress * (0 - car->turn_start_x);
                car->y = car->turn_start_y + progress * (intersection_size - car->turn_start_y);
                car->current_display_angle = 90.0f * progress;
            }
            else // Влево -> Вниз
            {
                car->x = car->turn_start_x + progress * (0 - car->turn_start_x);
                car->y = car->turn_start_y + progress * (-intersection_size - car->turn_start_y);
                car->current_display_angle = 180.0f + 90.0f * progress;
            }
        }
        else // TURN_RIGHT
        {
            // Поворот направо
            if (car->direction_x == 1) // Вправо -> Вниз
            {
                car->x = car->turn_start_x + progress * (0 - car->turn_start_x);
                car->y = car->turn_start_y + progress * (-intersection_size - car->turn_start_y);
                car->current_display_angle = -90.0f * progress;
            }
            else // Влево -> Вверх
            {
                car->x = car->turn_start_x + progress * (0 - car->turn_start_x);
                car->y = car->turn_start_y + progress * (intersection_size - car->turn_start_y);
                car->current_display_angle = 180.0f - 90.0f * progress;
            }
        }
    }
    else // Вертикальная -> Горизонтальная
    {
        if (car->turn_direction == TURN_LEFT)
        {
            // Поворот налево
            if (car->direction_y == 1) // Вверх -> Влево
            {
                car->x = car->turn_start_x + progress * (-intersection_size - car->turn_start_x);
                car->y = car->turn_start_y + progress * (0 - car->turn_start_y);
                car->current_display_angle = 90.0f + 90.0f * progress;
            }
            else // Вниз -> Вправо
            {
                car->x = car->turn_start_x + progress * (intersection_size - car->turn_start_x);
                car->y = car->turn_start_y + progress * (0 - car->turn_start_y);
                car->current_display_angle = 270.0f + 90.0f * progress;
            }
        }
        else // TURN_RIGHT
        {
            // Поворот направо
            if (car->direction_y == 1) // Вверх -> Вправо
            {
                car->x = car->turn_start_x + progress * (intersection_size - car->turn_start_x);
                car->y = car->turn_start_y + progress * (0 - car->turn_start_y);
                car->current_display_angle = 90.0f - 90.0f * progress;
            }
            else // Вниз -> Влево
            {
                car->x = car->turn_start_x + progress * (-intersection_size - car->turn_start_x);
                car->y = car->turn_start_y + progress * (0 - car->turn_start_y);
                car->current_display_angle = 270.0f - 90.0f * progress;
            }
        }
    }

    // Завершение поворота
    if (car->turn_progress >= 1.0f)
    {
        car->is_turning = false;
        car->will_turn = false;

        // Обновляем состояние машины после поворота
        if (car->road_id == 'H') // Горизонтальная -> Вертикальная
        {
            car->road_id = 'V';

            if (car->turn_direction == TURN_LEFT)
            {
                car->direction_y = (car->direction_x == 1) ? 1 : -1;
                car->lane = (car->direction_y == 1) ? 1 : -1;
            }
            else // TURN_RIGHT
            {
                car->direction_y = (car->direction_x == 1) ? -1 : 1;
                car->lane = (car->direction_y == 1) ? lines_count : -lines_count;
            }

            car->direction_x = 0;
            // Корректируем позицию
            car->x = car->lane * LINE_WIDTH;
        }
        else // Вертикальная -> Горизонтальная
        {
            car->road_id = 'H';

            if (car->turn_direction == TURN_LEFT)
            {
                car->direction_x = (car->direction_y == 1) ? -1 : 1;
                car->lane = (car->direction_x == 1) ? -1 : 1;
            }
            else // TURN_RIGHT
            {
                car->direction_x = (car->direction_y == 1) ? 1 : -1;
                car->lane = (car->direction_x == 1) ? -lines_count : lines_count;
            }

            car->direction_y = 0;
            // Корректируем позицию
            car->y = car->lane * LINE_WIDTH;
        }

        // Восстанавливаем нормальную скорость
        car->speed = car->max_speed;
    }
}

// безопасная скорость
float calculateSpeedCrossroad(UniversalCar *car, float distance)
{
    // Увеличиваем безопасную дистанцию в 2 раза
    float safe_speed = car->max_speed * (distance / (SAFE_DISTANCE * 4));
    return fmax(MIN_SPEED, fmin(car->max_speed, safe_speed));
}

// избегаение столкновений на перекрестке
void checkCollisionsCrossroad(UniversalCar *car)
{
    car->is_braking = false;
    car->in_intersection = (fabs(car->x) < intersection_size && fabs(car->y) < intersection_size);

    // Не проверяем столкновения во время поворота
    if (car->is_turning)
        return;

    // Проверка светофора - только если машина еще не проехала перекресток
    bool has_red_light = (car->road_id == 'H' && !horizontal_green) ||
                         (car->road_id == 'V' && horizontal_green);

    // Определяем, проехала ли машина уже перекресток
    bool has_passed_intersection = false;
    if (car->road_id == 'H')
    {
        has_passed_intersection = (car->direction_x == 1 && car->x > intersection_size) ||
                                  (car->direction_x == -1 && car->x < -intersection_size);
    }
    else
    {
        has_passed_intersection = (car->direction_y == 1 && car->y > intersection_size) ||
                                  (car->direction_y == -1 && car->y < -intersection_size);
    }

    if (has_red_light && !car->in_intersection && !has_passed_intersection)
    {
        // Проверяем расстояние до перекрестка
        float dist_to_intersection;
        if (car->road_id == 'H')
            dist_to_intersection = fabs(car->x) - intersection_size;
        else
            dist_to_intersection = fabs(car->y) - intersection_size;

        // Увеличиваем дистанцию торможения в 3 раза
        if (dist_to_intersection < SAFE_DISTANCE * 3 && dist_to_intersection > 0)
        {
            car->is_braking = true;
            car->speed = fmax(0, car->speed - 0.01f);

            // Полная остановка перед стоп-линией
            if (dist_to_intersection < 1.0f)
            {
                car->speed = 0;
            }
        }
    }

    // Проверка столкновений с другими машинами
    for (int i = 0; i < CROSSROAD_ROAD_COUNT; i++)
    {
        ListNode *current = crossroad_roads[i];
        while (current != NULL)
        {
            UniversalCar *other_car = &current->car;

            if (other_car == car || other_car->is_turning)
            {
                current = current->next;
                continue;
            }

            // Проверяем только машины на той же дороге
            if (other_car->road_id != car->road_id)
            {
                current = current->next;
                continue;
            }

            // Проверяем, находятся ли машины на одной полосе
            if (other_car->lane == car->lane)
            {
                float distance;
                if (car->road_id == 'H')
                    distance = fabs(other_car->x - car->x);
                else
                    distance = fabs(other_car->y - car->y);

                // Определяем, находится ли другая машина впереди
                bool is_in_front = false;
                if (car->road_id == 'H')
                {
                    is_in_front = (car->direction_x == 1 && other_car->x > car->x) ||
                                  (car->direction_x == -1 && other_car->x < car->x);
                }
                else
                {
                    is_in_front = (car->direction_y == 1 && other_car->y > car->y) ||
                                  (car->direction_y == -1 && other_car->y < car->y);
                }

                // Увеличиваем дистанцию безопасности в 2.5 раза
                if (is_in_front && distance < SAFE_DISTANCE * 2.5)
                {
                    car->is_braking = true;
                    float safe_speed = calculateSpeedCrossroad(car, distance);

                    if (car->speed > safe_speed)
                    {
                        car->speed = fmax(safe_speed, car->speed * 0.95f);
                    }

                    // Экстренное торможение с увеличенной дистанцией
                    if (distance < SAFE_DISTANCE * 0.8f)
                    {
                        car->speed *= 0.8f;
                    }
                }
            }
            current = current->next;
        }
    }

    // Плавное ускорение, если нет препятствий
    if (!car->is_braking && car->speed < car->max_speed)
    {
        car->speed = fmin(car->max_speed, car->speed + ACCELERATION);
    }
}

// обновление светофора
void updateTrafficLight()
{
    float current_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    // Меняем свет каждые 10 секунд
    if (current_time - last_light_switch > 10.0)
    {
        horizontal_green = !horizontal_green;
        last_light_switch = current_time;
    }
}

// ------------------------------------------------------------------------

// просто для работы функции - ничего не менялось

// обработка движений мыши
void processMouseMove(int x, int y)
{
    // определение координат мыши в окне
    float glX = (x / (float)glutGet(GLUT_WINDOW_WIDTH)) * 2.0f - 1.0f;
    float glY = 1.0f - (y / (float)glutGet(GLUT_WINDOW_HEIGHT)) * 2.0f;

    // флаги определения наведения на кнопку
    char old_hover = button_hover;
    button_hover = -1;

    if (!in_simulation)
    {
        // в главном меню
        if (menu_option == MAIN_MENU)
        {
            // кнопки изменеия окна
            if (glX >= -0.5f && glX <= 0.0f)
            {
                if (glY >= 0.4f && glY <= 0.5f)
                    button_hover = 1;
                else if (glY >= 0.25f && glY <= 0.35f)
                    button_hover = 2;
                else if (glY >= 0.1f && glY <= 0.2)
                    button_hover = 3;
                else if (glY >= -0.2f && glY <= -0.1f)
                    button_hover = 4;
            }
            // кнопка загрузки моделирования
            if (glX >= 0.15f && glX <= 0.65 && glY >= -0.2f && glY <= -0.1f)
                button_hover = 8;
        }
        // кнопка возврата на главное меню
        else if (menu_option == APP_INFO || menu_option == ABOUT_US || menu_option == LOAD_FROM_FILE)
        {
            if (glX >= -0.3f && glX <= 0.1f && glY >= -0.5f && glY <= -0.4f)
            {
                button_hover = 5;
            }
        }
    }

    // изменение цвета, если произошло наведение мыши
    if (button_hover != old_hover)
    {
        glutPostRedisplay();
    }
}

// отрисовка кнопки с информацей
void drawButton(float x, float y, float width, float height, const char *text, int hover)
{
    // определяем наведение мыши на кнопку
    if (hover)
    {
        glColor3f(0.80f, 0.54f, 0.80f);
    }
    else
    {
        glColor3f(0.92f, 0.76f, 0.92f);
    }

    // сама кнопка
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();

    // линия обводки
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();

    // текст внутри кнопки
    glColor3f(0.0f, 0.0f, 0.0f);
    glRasterPos2f(x + width / 2 - 0.05f, y + height / 2 - 0.03f);
    for (int i = 0; i < strlen(text); i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
}

// отрисовка кнопки с номером
void drawButtonNumber(float x, float y, float width, float height, const char *label, bool active)
{
    // определяем нажатие на кнопку
    if (active)
    {
        glColor3f(0.42f, 0.58f, 0.85f);
    }
    else
    {
        glColor3f(0.64f, 0.76f, 0.94f);
    }

    // сама кнопка
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();

    // обводка
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();

    // число полос
    glColor3f(0.25f, 0.25f, 0.25f);
    float textX = x + 0.1 + (width - glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char *)label)) / 200.0f;
    float textY = y + height / 2.0f - 0.1f;
    drawText(textX, textY, label);
}

// отрисовка окна с информацией о программе
void drawInfoScreen()
{
    // цвет фона
    glColor3f(0.93f, 0.75f, 0.81f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, 1.0f);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(1.0f, 1.0f);
    glEnd();

    const char str[12][100] = {"INFO", " ", "This is car generate app",
                               "In the main menu, you can select the type of road by clicking on",
                               "you can choose count lines per direction (buttons with nums)",
                               "click ESC for exit", "click TAB for generate pause", " ",
                               "L - loading the past simulation", "S - saving simulation",
                               "A - handling accident in highway",
                               "  :)"};

    glColor3f(0.0f, 0.0f, 0.0f);
    float y_pos = 0.8f;
    for (int i = 0; i < 12; i++)
    {
        glRasterPos2f(-0.7f, y_pos);
        for (int j = 0; j < strlen(str[i]); j++)
        {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[i][j]);
        }
        y_pos -= 0.1f;
    }

    // кнопка возврата в главное меню
    drawButton(-0.3f, -0.5f, 0.4f, 0.1f, "Back", button_hover == 5);
}

// отрисовка окна с информацией о создателе
void drawAboutScreen()
{
    // заливка фона
    glColor3f(0.93f, 0.75f, 0.81f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, 1.0f);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(1.0f, 1.0f);
    glEnd();

    char str[8][1000] = {"ABOUT US", " ", "This program for modeling automobile traffic was created as a coursework",
                         "Made by: Kalinichenko Vera", "group: 5131001/40001",
                         "SPBSTU, Information security", " 2025 year", "Supervisor: Pankov I.D."};

    glColor3f(0.0f, 0.0f, 0.0f);
    float y_pos = 0.6f;
    for (int i = 0; i < 8; i++)
    {
        glRasterPos2f(-0.7f, y_pos);
        for (int j = 0; j < strlen(str[i]); j++)
        {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[i][j]);
        }
        y_pos -= 0.1f;
    }

    // кнопка возврата в главное меню
    drawButton(-0.3f, -0.5f, 0.4f, 0.1f, "Back", button_hover == 5);
}

// отрисовка таймера в генерации
void displayTime(float x, float y)
{
    // приостановка таймера на паузе
    if (track)
    {
        clock_t currentTime = clock();
        double elapsedTime = (double)(currentTime - start_time) / CLOCKS_PER_SEC;
        last_time = elapsedTime;
    }

    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2f(x, y);

    char timeString[50];
    snprintf(timeString, sizeof(timeString), "%.2f", last_time);
    for (char *c = timeString; *c != '\0'; c++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
}

// обработка нажатия клавиш
void processNormalKeys(unsigned char key, int x, int y)
{
    // пауза при моделировании
    if (key == 9 && in_simulation)
    {
        if (track)
            printf("pause mode\n");
        track = !track;
    }
    // выход из программы
    else if (key == 27)
        exit(0);
    // сохранение при моделировании
    else if ((key == 'S' || key == 's') && in_simulation)
    {
        printf("saving mode\n");
        track = !track;
        printf("write <filename>.txt to save:\n");
        char file[100];
        if (!fgets(file, 100, stdin))
        {
            printf("ERROR\n");
        }
        else
        {
            file[strcspn(file, "\n")] = '\0';
            // saveSimulation(file);
        }
    }
    // загрузка прошлого моделирования
    else if ((key == 'L' || key == 'l') && !in_simulation && menu_option == LOAD_FROM_FILE)
    {
        printf("write <filename>.txt to load it:\n");
        char file[100];
        if (!fgets(file, 100, stdin))
        {
            printf("ERROR\n");
        }
        else
        {
            file[strcspn(file, "\n")] = '\0';
            // loadSimulation(file);
        }
    }
    // обеспечение дтп на полосе
    else if ((key == 'A' || key == 'a') && in_simulation && type_simulation == HIGHWAY)
    {
        accident_flag = !accident_flag;
        // displayAccident();
    }
}

// отрисовка текста
void drawText(float x, float y, const unsigned char *text)
{
    glRasterPos2f(x, y);
    for (const unsigned char *c = text; *c != '\0'; c++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

// вывод информации о моделировании
void infoStatistic(bool flag)
{
    displayTime(-WINDOW_BORDER + 1, WINDOW_BORDER - 1);

    // вывод только для перекрестка
    if (flag)
    {
        // информация о светофоре
        glColor3f(1.0, 1.0, 1.0);
        glRasterPos2f(-WINDOW_BORDER + 1, WINDOW_BORDER - 2);
        char info[100];
        sprintf(info, "horizontal: %s | Time to switch: %.1f",
                horizontal_green ? "GREEN" : "RED",
                10.0 - (glutGet(GLUT_ELAPSED_TIME) / 1000.0 - last_light_switch));
        for (char *c = info; *c != '\0'; c++)
        {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
        }
    }

    char str3[100] = "ESC - exit, TAB - pause, S - save";

    glRasterPos2f(-WINDOW_BORDER + 1, WINDOW_BORDER - 3);
    for (int j = 0; j < strlen(str3); j++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str3[j]);
    }

    if (type_simulation == HIGHWAY)
    {
        char str3[50] = "A - accident";
        glRasterPos2f(-WINDOW_BORDER + 1, WINDOW_BORDER - 2);
        for (int j = 0; j < strlen(str3); j++)
        {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str3[j]);
        }
    }

    glRasterPos2f(-WINDOW_BORDER + 1, WINDOW_BORDER - 4);
    char type[3][25] = {" Highway", " Crossroad", " 2 Crossroads"};
    switch (type_simulation)
    {
    case (HIGHWAY):
    {
        for (int j = 0; j < strlen(type[0]); j++)
        {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, type[0][j]);
        }
        break;
    }
    case (CROSSROAD):
    {
        for (int j = 0; j < strlen(type[1]); j++)
        {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, type[1][j]);
        }
        break;
    }
    default:
        break;
    }

    glRasterPos2f(-WINDOW_BORDER + 1, WINDOW_BORDER - 5);
    char str2[50];
    snprintf(str2, sizeof(str2), "lines per direction: %d", lines_count);
    for (char *c = str2; *c != '\0'; c++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }

    // количество машин на дороге
    // количество дтп с начала моделирования
}

/*  ИНИЦИАЛИЗАЦИЯ  */

// инициализация глут окна
void init()
{
    glClearColor(0.13f, 0.46f, 0.25f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-WINDOW_BORDER, WINDOW_BORDER, -WINDOW_BORDER, WINDOW_BORDER);
    srand(time(NULL));
}

// отрисовка окна меню
void menuWindow()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Фон меню
    glColor3f(0.80f, 0.80f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, 1.0f);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(1.0f, 1.0f);
    glEnd();

    // Заголовок
    char *str = "MAIN MENU";
    glColor3f(0.0f, 0.0f, 0.0f);
    glRasterPos2f(-0.15f, 0.65f);
    for (int i = 0; i < strlen(str); i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);

    glColor3f(0.0f, 0.0f, 0.0f);

    // Выбор количества полос
    // drawText(0.2f, 0.3f, "Select number of lanes:");
    drawText(0.2f, 0.3f, "Choose lanes count:");

    char *str3 = "(per direction. default - 3)";
    glRasterPos2f(0.2f, 0.25f);
    for (int i = 0; i < strlen(str3); i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, str3[i]);

    drawText(-0.3f, -0.3f, "(test) for crossroad - dont touch no more");

    // Кнопки выбора количества полос
    for (int i = 1; i <= 5; i++)
    {
        char label[10];
        sprintf(label, "%d", i);
        bool active = (lines_count == i);
        drawButtonNumber(0.15f + (i - 1) * 0.15f, 0.1f, 0.1f, 0.1f, label, active);
    }

    // кнопки в главном меню
    if (menu_option == MAIN_MENU)
    {
        drawButton(-0.5f, 0.4f, 0.5f, 0.1f, "app info", button_hover == 1);
        drawButton(-0.5f, 0.25f, 0.5f, 0.1f, "highway", button_hover == 2);
        drawButton(-0.5f, 0.1f, 0.5f, 0.1f, "crossroad", button_hover == 3);
        drawButton(-0.5f, -0.2f, 0.5f, 0.1f, "about us", button_hover == 4);
        drawButton(0.15f, -0.2f, 0.5f, 0.1f, "load from file", button_hover == 8);
    }
    else if (menu_option == APP_INFO)
    {
        drawInfoScreen();
    }
    else if (menu_option == ABOUT_US)
    {
        drawAboutScreen();
    }
    else if (menu_option == LOAD_FROM_FILE)
    {
        // loadFromFile();
    }

    glutSwapBuffers();
}
