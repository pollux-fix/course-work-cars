/*
файл с последней рабочей версией всяких списков и этой ерунды
*/

#include "head_test.h"
#include "head_list.h"

int car_count = 0; // текущее количество машин, которые инициализированы

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

/*МЕНЮ И КНОПКИ*/

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
                if (glX >= -0.7f && glX <= -0.2f)
                {
                    // окно с информацией
                    if (glY >= 0.25f && glY <= 0.37f)
                    {
                        menu_option = APP_INFO;
                    }
                    // моделирование автострады
                    else if (glY >= 0.0f && glY <= 0.12f)
                    {
                        start_time = clock();
                        init();
                        initHighwayCar();
                        in_simulation = true;
                        type_simulation = HIGHWAY;
                        glutDisplayFunc(displayHighway);
                        glutTimerFunc(0, updateHighway, 0);
                        return;
                    }
                    // моделирование перекрестка
                    else if (glY >= -0.15f && glY <= -0.03)
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
                    else if (glY >= -0.4f && glY <= -0.28f)
                    {
                        menu_option = ABOUT_US;
                    }
                }
                // кнопки выбора количества полос
                else if (glY >= -0.06f && glY <= 0.06f)
                {
                    if (glX >= 0.1 + (1-1) * 0.15 && glX <= 0.2 + (1-1) * 0.15f)
                    {
                        lines_count = 1;
                        intersection_size = 1.0f;
                    }
                    else if (glX >= 0.1 + (2-1) * 0.15 && glX <= 0.2 + (2-1) * 0.15f)
                    {
                        lines_count = 2;
                        intersection_size = 2.0f;
                    }
                    else if (glX >= 0.1 + (3-1) * 0.15 && glX <= 0.2 + (3-1) * 0.15f)
                    {
                        lines_count = 3;
                        intersection_size = 3.0f;
                    }
                    else if (glX >= 0.1 + (4-1) * 0.15 && glX <= 0.2 + (4-1) * 0.15f)
                    {
                        lines_count = 4;
                        intersection_size = 4.0f;
                    }
                    else if (glX >= 0.1 + (5-1) * 0.15 && glX <= 0.2 + (5-1) * 0.15f)
                    {
                        lines_count = 5;
                        intersection_size = 5.0f;
                    }
                }
                // кнопка загрузки из файла
                else if (glX >= 0.2f && glX <= 0.7 && glY >= -0.4f && glY <= -0.28f)
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
            if (glX >= -0.7f && glX <= -0.2f)
            {
                if (glY >= 0.25f && glY <= 0.37f)
                    button_hover = 1;
                else if (glY >= 0.0f && glY <= 0.12f)
                    button_hover = 2;
                else if (glY >= -0.15f && glY <= -0.03)
                    button_hover = 3;
                else if (glY >= -0.4f && glY <= -0.28f)
                    button_hover = 4;
            }
            // кнопка загрузки моделирования
            if (glX >= 0.2f && glX <= 0.7 && glY >= -0.4f && glY <= -0.28f)
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

// отрисовка окна меню
void menuWindow()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Фон меню
    glColor3f(244.0f / 255.0f, 235.0f / 255.0f, 255.0f / 255.0f);
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
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[i]);

    glColor3f(0.0f, 0.0f, 0.0f);

    // Выбор количества полос
    drawText(0.1f, 0.2f, "Choose lanes count:");

    char *str3 = "(per direction. default - 3)";
    glRasterPos2f(0.1f, 0.15f);
    for (int i = 0; i < strlen(str3); i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, str3[i]);

    // Кнопки выбора количества полос
    for (int i = 1; i <= 5; i++)
    {
        char label[10];
        sprintf(label, "%d", i);
        bool active = (lines_count == i);
        drawButtonNumber(0.1f + (i - 1) * 0.15f, -0.06f, 0.1f, 0.12f, label, active);
    }

    // кнопки в главном меню
    if (menu_option == MAIN_MENU)
    {
        drawButton(-0.7f, 0.25f, 0.5f, 0.12f, "app info", button_hover == 1);
        drawButton(-0.7f, 0.0f, 0.5f, 0.12f, "highway", button_hover == 2);
        drawButton(-0.7f, -0.15f, 0.5f, 0.12f, "crossroad", button_hover == 3);
        drawButton(-0.7f, -0.40f, 0.5f, 0.12f, "about us", button_hover == 4);
        drawButton(0.2f, -0.40f, 0.5f, 0.12f, "load from file", button_hover == 8);
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
        loadFromFile();
    }

    glutSwapBuffers();
}

// отрисовка кнопки с информацей
void drawButton(float x, float y, float width, float height, const char *text, int hover)
{
    // определяем наведение мыши на кнопку
    if (hover) glColor3f(186.0f / 255.0f, 123.0f / 255.0f, 186.0f / 255.0f);
    else glColor3f(227.0f / 255.0f, 177.0f / 255.0f, 227.0f / 255.0f);

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
    // Центрирование текста
    float text_width = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)text) / 450.0f;
    float text_x = x + (width - text_width) / 2.0f;
    float text_y = y + height / 2.0f - 0.02f;
    glRasterPos2f(text_x, text_y);
    //glRasterPos2f(x + width / 2 - 0.05f, y + height / 2 - 0.03f);
    for (int i = 0; i < strlen(text); i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
}

// отрисовка кнопки с номером
void drawButtonNumber(float x, float y, float width, float height, const char *label, bool active)
{
    // определяем нажатие на кнопку
    if (active) glColor3f(0.55f, 0.615f, 0.886f);
    else glColor3f(0.74f, 0.8f, 1.0f);

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
    float textX = x + width / 2.0f - glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char *)label) / 900.0f;
    float textY = y - 0.05f;
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
            saveSimulation(file);
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
            loadSimulation(file);
        }
    }
    // обеспечение дтп на полосе
    else if ((key == 'A' || key == 'a') && in_simulation && type_simulation == HIGHWAY)
    {
        accident_flag = !accident_flag;
        displayAccident();
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
    char type[2][25] = {" Highway", " Crossroad"};
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

// инициализация машин на перекрестке
void initCrossroadCar()
{
    for (int i = 0; i < MAX_CARS; i++)
        addCrossroadCar();
}

// инициализация машин на автостраде
void initHighwayCar()
{
    // Инициализируем полосы
    init_lanes(lines_count);

    for (int i = 0; i < 4; i++)
        addRandomCar();
}

/* ПРЯМАЯ ДОРОГА */

// Реализация массива полос
ListCar *lanes[MAX_LANES] = {NULL};
int lane_count = 6; // По умолчанию 3 полосы в каждую сторону

// Инициализация полос
void init_lanes(int lanes_per_direction)
{
    // Освобождаем старые полосы, если они есть
    free_all_lanes();

    lane_count = lanes_per_direction * 2;

    // Инициализируем все указатели в NULL
    for (int i = 0; i < MAX_LANES; i++)
        lanes[i] = NULL;
}

// Освобождение памяти всех полос
void free_all_lanes(void)
{
    for (int i = 0; i < MAX_LANES; i++)
    {
        ListCar *current = lanes[i];
        while (current != NULL)
        {
            ListCar *to_free = current;
            current = current->next;
            free(to_free);
        }
        lanes[i] = NULL;
    }
}

// Получение индекса полосы в массиве
int get_lane_index(CarDirection direction, char lane_number)
{
    if (direction == RIGHT)
        return lane_number - 1; // Правые полосы: 0, 1, 2...
    else
        return (lane_count / 2) + (lane_number - 1); // Левые полосы
}

// Обновление всех машин
void update_all_cars(void)
{
    for (int i = 0; i < lane_count; i++)
    {
        if (lanes[i] != NULL)
            updateAdvancedCars(&lanes[i]);
    }
}

// Отрисовка всех машин
void draw_all_cars(void)
{
    for (int i = 0; i < lane_count; i++)
    {
        ListCar *current = lanes[i];
        while (current != NULL)
        {
            drawHighwayCar(current->car);
            current = current->next;
        }
    }
}

int count_cars(ListCar *head)
{
    int count = 0;
    ListCar *current = head;
    while (current != NULL)
    {
        count++;
        current = current->next;
    }
    return count;
}

// создает и инициализирует новую машину для автострады
CarNode create_highway_car(CarDirection direction, char lane)
{
    CarNode car = {0};

    car.direction = direction;
    car.lane = lane;

    car.is_braking = false;

    // с таким вариантом перестроения чаще происходят :)
    car.max_speed = MIN_SPEED + (rand() % (int)((MAX_SPEED - MIN_SPEED) * 100)) * 0.01;
    car.speed = car.max_speed * 0.5;
    // car.max_speed = MAX_SPEED;
    // car.speed = MIN_SPEED + (rand() % (int)((MAX_SPEED - MIN_SPEED) * 100)) * 0.01;

    car.color[0] = (50 + rand() % 50) / 100.0f; // R: 0.5-1.0 (исключает 0.0)
    car.color[1] = (rand() % 100) / 100.0f;     // G: 0.0-1.0
    car.color[2] = (rand() % 100) / 100.0f;     // B: 0.0-1.0

    if (car.direction == RIGHT)
        car.position = -WINDOW_BORDER - 1 - (rand() % 10);
    else
        car.position = WINDOW_BORDER + 1 + (rand() % 10);

    return car;
}

ListCar *createCarNode(CarNode car)
{
    ListCar *new = (ListCar *)malloc(sizeof(ListCar));
    if (!new)
    {
        printf("Failed to allocate memory");
        exit(0);
    }

    new->car = car;
    new->next = NULL;
    return new;
}

void insert_car(ListCar **head, CarNode car)
{
    ListCar *new = createCarNode(car);
    if (!new)
        return;

    if (*head == NULL)
        *head = new;
    else
    {
        ListCar *current = *head;
        while (current->next != NULL)
            current = current->next;

        current->next = new;
    }
}

// отрисовка прямой дороги
void drawHighway()
{
    // сама дорога
    glColor3f(0.47, 0.47, 0.47);
    glBegin(GL_QUADS);
    glVertex2f(-WINDOW_BORDER, -(lines_count + 0.5) * LINE_WIDTH);
    glVertex2f(WINDOW_BORDER, -(lines_count + 0.5) * LINE_WIDTH);
    glVertex2f(WINDOW_BORDER, (lines_count + 0.5) * LINE_WIDTH);
    glVertex2f(-WINDOW_BORDER, (lines_count + 0.5) * LINE_WIDTH);
    glEnd();

    // пунктирная разметка
    for (int i = -lines_count - 0.5; i <= (lines_count - 0.5); i++)
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

    // границы по краям
    glColor3f(0.0, 0.0, 0.0);
    glLineWidth(2.0);
    glBegin(GL_LINES);
    glVertex2f(-WINDOW_BORDER, -(lines_count + 0.5) * LINE_WIDTH);
    glVertex2f(WINDOW_BORDER, -(lines_count + 0.5) * LINE_WIDTH);
    glVertex2f(-WINDOW_BORDER, (lines_count + 0.5) * LINE_WIDTH);
    glVertex2f(WINDOW_BORDER, (lines_count + 0.5) * LINE_WIDTH);
    glEnd();
    glLineWidth(1.0);

    // островок безопасности между полосами
    glColor3f(0.2, 0.2, 0.2);
    glBegin(GL_QUADS);
    glVertex2f(-WINDOW_BORDER, -0.5 * LINE_WIDTH);
    glVertex2f(WINDOW_BORDER, -0.5 * LINE_WIDTH);
    glVertex2f(WINDOW_BORDER, 0.5 * LINE_WIDTH);
    glVertex2f(-WINDOW_BORDER, 0.5 * LINE_WIDTH);
    glEnd();

    glColor3f(0.0, 0.0, 0.0);
}

void drawHighwayCar(CarNode car)
{
    // изменения только в рамках одной машины
    glPushMatrix();

    // позиция машины на полосе
    float lane_pos;
    if (car.direction == RIGHT) lane_pos = -car.lane * LINE_WIDTH;
    else lane_pos = car.lane * LINE_WIDTH;

    // установка положения машины
    glTranslatef(car.position, lane_pos, 0);

    // поворот рисовки, если движение влево
    if (car.direction == RIGHT)
        glRotatef(180, 0, 1, 0);

    // свет при торможении
    if (car.is_braking)
    {
        glColor3f(1.0, 0.0, 0.0);
        glBegin(GL_LINES);
        glVertex2f(0.7, 0.15);
        glVertex2f(0.7, 0.3);
        glVertex2f(0.72, 0.15);
        glVertex2f(0.72, 0.3);
        glEnd();
        glBegin(GL_LINES);
        glVertex2f(0.7, -0.3);
        glVertex2f(0.7, -0.15);
        glVertex2f(0.72, -0.3);
        glVertex2f(0.72, -0.15);
        glEnd();
    }

    // кузов машины
    glColor3fv(car.color);
    glBegin(GL_QUADS);
    glVertex2f(-0.7, -0.3);
    glVertex2f(0.7, -0.3);
    glVertex2f(0.7, 0.3);
    glVertex2f(-0.7, 0.3);
    glEnd();

    // окно на машине
    glColor3f(car.color[0] * 0.7, car.color[1] * 0.7, car.color[2] * 0.7);
    glBegin(GL_QUADS);
    glVertex2f(-0.3, -0.3);
    glVertex2f(0.5, -0.3);
    glVertex2f(0.5, 0.3);
    glVertex2f(-0.3, 0.3);
    glEnd();

    // колеса машины
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

// визуализация автострады
void displayHighway()
{
    // очистка буфера
    glClear(GL_COLOR_BUFFER_BIT);
    // отрисовка дороги
    drawHighway();

    // отрисовка машин
    glColor3f(0.25f, 0.25f, 1.0f);

    // Используем новую универсальную функцию
    draw_all_cars();

    infoStatistic(false);

    glutSwapBuffers();
}

// обновление прямой дороги
void updateHighway(int value)
{
    // только если не на паузе
    if (track)
    {
        addRandomCar();
        update_all_cars();

        remove_cars_out_of_bounds();
    }

    glutPostRedisplay();
    glutTimerFunc(16, updateHighway, 0);
}

// подсчет оптимальной скорости в торможении
float calculateSafeSpeed(CarNode car, float distance)
{
    // на основе дистанции мд машинами
    float safe_speed = car.max_speed * (distance / (SAFE_DISTANCE * 2));
    return fmax(MIN_SPEED, fmin(car.max_speed, safe_speed));
}

void checkCollisionAvoidance(ListCar *current)
{
    if (current == NULL)
        return;

    CarNode *car = &current->car;
    float min_distance = SAFE_DISTANCE * 3;
    car->is_braking = false;

    // Проверяем ВСЕ машины на той же полосе
    int current_lane_index = get_lane_index(car->direction, car->lane);
    ListCar *other = lanes[current_lane_index];

    while (other != NULL)
    {
        if (other == current)
        {
            other = other->next;
            continue;
        }

        float path_distance = fabs(other->car.position - car->position);

        // Определяем относительное положение
        bool is_in_front = (car->direction == RIGHT && other->car.position > car->position) ||
                           (car->direction == LEFT && other->car.position < car->position);

        if (is_in_front && path_distance < min_distance)
        {
            min_distance = path_distance;

            if (path_distance < SAFE_DISTANCE * 1.5)
            {
                car->is_braking = true;
                float safe_speed = calculateSafeSpeed(*car, path_distance);

                if (car->speed > safe_speed)
                {
                    car->speed = fmax(safe_speed, car->speed * BRAKING_FACTOR);
                }

                if (path_distance < SAFE_DISTANCE * 0.7)
                {
                    car->speed *= 0.8;
                }
            }
        }

        other = other->next;
    }

    // Плавное ускорение, если нет препятствий
    if (!car->is_braking && car->speed < car->max_speed)
    {
        car->speed += ACCELERATION;
        car->speed = fmin(car->speed, car->max_speed);
    }
}

/*
// проверка для предотвращения дтп
void checkCollisionAvoidance(ListCar *current)
{
    // минимальная допустимая дистанция
    float min_distance = SAFE_DISTANCE * 3;
    current->car.is_braking = false;

    ListCar *next_car = current->next;
    if (next_car == NULL)
        return;

    float path_distance = fabs(next_car->car.position - current->car.position);
    // Определяем относительное положение
    bool is_in_front = (current->car.direction == RIGHT && next_car->car.position > current->car.position) ||
                       (current->car.direction == LEFT && next_car->car.position < current->car.position);

    if (is_in_front && path_distance < min_distance)
    {
        min_distance = path_distance;

        // Если машина впереди ближе безопасной дистанции
        if (path_distance < SAFE_DISTANCE * 1.5)
        {
            current->car.is_braking = true;
            float safe_speed = calculateSafeSpeed(current->car, path_distance);

            // Плавное торможение до безопасной скорости
            if (current->car.speed > safe_speed)
                current->car.speed = fmax(safe_speed, current->car.speed * BRAKING_FACTOR);

            // Если очень близко - экстренное торможение
            if (path_distance < SAFE_DISTANCE * 0.7)
                current->car.speed *= 0.8;
        }
    }

    // Плавное ускорение, если нет препятствий
    if (!current->car.is_braking && current->car.speed < current->car.max_speed)
    {
        current->car.speed += ACCELERATION;
        current->car.speed = fmin(current->car.speed, current->car.max_speed);
    }
}


// обновляем рисовку машинок на автостраде
// Теперь принимает адрес указателя на голову списка (ListCar **)

void updateAdvancedCars(ListCar **head)
{
    if (!track)
        return;

    ListCar *current = *head;
    ListCar *prev = NULL;

    while (current != NULL)
    {
        // ТУТ ОБРАБОТКА ТОРМОЖЕНИЙ И ПЕРЕСТРОЕНИЙ - ФУНКЦИИ СООТВ
        checkCollisionAvoidance(current);

        // Движение
        current->car.position += current->car.speed * (current->car.direction == RIGHT ? 1.0f : -1.0f);

        // Проверка на выезд за пределы (УДАЛЕНИЕ вместо телепортации)
        bool should_delete = false;
        // Проверяем, вышла ли машина за пределы окна (с небольшим запасом)
        if ((current->car.direction == RIGHT && current->car.position > WINDOW_BORDER + 2) ||
            (current->car.direction == LEFT && current->car.position < -WINDOW_BORDER - 2))
        {
            should_delete = true;
        }

        if (should_delete)
        {
            ListCar *next = current->next;
            if (prev == NULL)
            {
                // Удаление головы списка
                *head = next;
            }
            else
            {
                // Удаление элемента из середины/конца
                prev->next = next;
            }
            free(current);
            current = next; // Переходим к следующему элементу
        }
        else
        {
            // Если не удаляем, двигаем указатели
            prev = current;
            current = current->next;
        }
    }
}
*/

void updateAdvancedCars(ListCar **head)
{
    if (!track)
        return;

    ListCar *current = *head;
    ListCar *prev = NULL;

    while (current != NULL)
    {
        CarNode *car = &current->car;

        // Пропускаем машины в аварии
        if (car->state == CAR_STATE_ACCIDENT)
        {
            prev = current;
            current = current->next;
            continue;
        }

        // Проверяем столкновения
        checkCollisionAvoidance(current);

        // Решаем о перестроении
        if (!car->is_changing_lane && rand() % 100 < 80)
        {
            decideLaneChange(current);
        }

        // Обработка перестроения
        if (car->is_changing_lane)
        {
            // Плавное торможение перед перестроением
            if (car->speed > car->target_speed)
            {
                car->speed = fmax(car->target_speed, car->speed * 0.95);
                car->is_braking = true;
            }

            // Начинаем перестроение когда достигли безопасной скорости
            if (car->speed <= car->target_speed * 1.05)
            {
                car->lane_change_progress += LANE_CHANGE_SPEED;

                // если завершили перестроение
                if (car->lane_change_progress >= 1.0)
                {
                    // Перемещаем машину в новый список полосы
                    int old_lane_index = get_lane_index(car->direction, car->lane);
                    int new_lane_index = get_lane_index(car->direction, car->target_lane);

                    // Сохраняем указатель на следующий элемент перед перемещением
                    ListCar *next_car = current->next;

                    // Удаляем из текущего списка
                    if (prev == NULL)
                    {
                        *head = next_car;
                    }
                    else
                    {
                        prev->next = next_car;
                    }

                    // Создаем копию данных машины с обновленной полосой
                    CarNode car_data = current->car;
                    car_data.lane = car_data.target_lane;
                    car_data.is_changing_lane = false;
                    car_data.target_lane = -1;
                    car_data.lane_change_progress = 0.0;

                    // Освобождаем текущий узел
                    free(current);

                    // Добавляем в новую полосу
                    insert_car(&lanes[new_lane_index], car_data);

                    // Переходим к следующей машине
                    current = next_car;
                    continue;
                }
            }
        }

        // Движение
        car->position += car->speed * (car->direction == RIGHT ? 1.0f : -1.0f);

        // Проверка на выезд за пределы (УДАЛЕНИЕ вместо телепортации)
        bool should_delete = false;
        if ((car->direction == RIGHT && car->position > WINDOW_BORDER + 2) ||
            (car->direction == LEFT && car->position < -WINDOW_BORDER - 2))
        {
            should_delete = true;
        }

        if (should_delete)
        {
            ListCar *next = current->next;
            if (prev == NULL)
            {
                *head = next;
            }
            else
            {
                prev->next = next;
            }
            free(current);
            current = next;
        }
        else
        {
            prev = current;
            current = current->next;
        }
    }
}

// добавление новой машинки на автостраду
void addRandomCar()
{
    static float last_add_time = 0;
    float current_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;

    // Проверяем временной интервал - почаще пусть создаются
    if (current_time - last_add_time < 1.5)
        return;

// Проверяем общее количество машин
    int total_cars = 0;
    for (int i = 0; i < lane_count; i++)
    {
        total_cars += count_cars(lanes[i]);
    }

    if (total_cars > MAX_CARS)
    {
        return;
    }

    last_add_time = current_time;

    // Добавляем машины на все доступные полосы
    for (int lane_num = 1; lane_num <= lane_count / 2; lane_num++)
    {
        // Правые полосы
        int right_index = get_lane_index(RIGHT, lane_num);
        if (count_cars(lanes[right_index]) < MAX_LANE_CAR)
        {
            CarNode new_car = create_highway_car(RIGHT, lane_num);
            insert_car(&lanes[right_index], new_car);
        }

        // Левые полосы
        int left_index = get_lane_index(LEFT, lane_num);
        if (count_cars(lanes[left_index]) < MAX_LANE_CAR)
        {
            CarNode new_car = create_highway_car(LEFT, lane_num);
            insert_car(&lanes[left_index], new_car);
        }
    }
}

// проверка для смены полосы
bool isSafeToChangeLane(ListCar *current_car, char new_lane, float *safe_speed)
{
    if (current_car == NULL)
        return false;

    CarNode *car = &current_car->car;
    *safe_speed = car->max_speed;
    bool is_safe = true;

    int new_lane_index = get_lane_index(car->direction, new_lane);
    if (new_lane_index < 0 || new_lane_index >= lane_count)
        return false;

    // Проверяем машины на новой полосе
    ListCar *other = lanes[new_lane_index];
    while (other != NULL)
    {
        if (other == current_car)
        {
            other = other->next;
            continue;
        }

        CarNode *other_car = &other->car;

        // Учитываем перестроение других машин
        char other_current_lane = other_car->lane;
        if (other_car->is_changing_lane)
        {
            other_current_lane = other_car->target_lane;
        }

        if (other_current_lane == new_lane)
        {
            float dist = fabs(other_car->position - car->position);

            bool is_in_front = (car->direction == RIGHT && other_car->position > car->position) ||
                               (car->direction == LEFT && other_car->position < car->position);
            bool is_too_close = (car->direction == RIGHT && other_car->position < car->position) ||
                                (car->direction == LEFT && other_car->position > car->position);

            if (is_in_front && dist < SAFE_DISTANCE * 2.5)
            {
                float this_safe_speed = calculateSafeSpeed(*car, dist);
                if (this_safe_speed < *safe_speed)
                {
                    *safe_speed = this_safe_speed;
                }

                if (dist < SAFE_DISTANCE * 1.2)
                {
                    is_safe = false;
                }
            }
            else if (is_too_close && dist < SAFE_DISTANCE * 1.5)
            {
                is_safe = false;
            }
        }
        other = other->next;
    }

    return is_safe;
}

// решение о смене полосы
void decideLaneChange(ListCar *current_car)
{
    if (current_car == NULL)
        return;

    CarNode *car = &current_car->car;

    if (car->is_changing_lane)
        return;

    bool slow_car_ahead = false;
    float distance_to_slow_car = SAFE_DISTANCE * 10;

    int current_lane_index = get_lane_index(car->direction, car->lane);
    ListCar *other = lanes[current_lane_index];

    while (other != NULL)
    {
        if (other == current_car)
        {
            other = other->next;
            continue;
        }

        CarNode *other_car = &other->car;
        float dist = fabs(other_car->position - car->position);

        bool is_in_front = (car->direction == RIGHT && other_car->position > car->position) ||
                           (car->direction == LEFT && other_car->position < car->position);

        if (is_in_front && dist < SAFE_DISTANCE * 3.0)
        {
            if (other_car->speed < car->max_speed * 0.7)
            {
                slow_car_ahead = true;
                distance_to_slow_car = fmin(distance_to_slow_car, dist);
            }
        }
        other = other->next;
    }

    if (!slow_car_ahead)
    {
        return;
    }

    int lane_change_directions[2] = {-1, 1};
    bool found_safe_lane = false;

    for (int i = 0; i < 2; i++)
    {
        int dl = lane_change_directions[i];
        int new_lane = car->lane + dl;

        if (new_lane <= 0 || new_lane > lane_count / 2)
        {
            continue;
        }

        float safe_speed;
        if (isSafeToChangeLane(current_car, new_lane, &safe_speed))
        {
            car->target_lane = new_lane;
            car->is_changing_lane = true;
            car->lane_change_progress = 0.0;
            car->target_speed = safe_speed;
            found_safe_lane = true;
            break;
        }
    }

    if (!found_safe_lane)
    {
        car->is_braking = true;
        float safe_speed = calculateSafeSpeed(*car, distance_to_slow_car);
        if (car->speed > safe_speed)
        {
            car->speed = fmax(safe_speed, car->speed * 0.9);
        }
    }
}

// Удаление машин, которые выехали за пределы дороги - НОВАЯ ФУНКЦИЯ
void remove_cars_out_of_bounds()
{
    for (int i = 0; i < lane_count; i++)
    {
        ListCar **current = &lanes[i];
        while (*current != NULL)
        {
            CarNode *car = &(*current)->car;

            bool should_remove = false;
            if (car->direction == RIGHT && car->position > WINDOW_BORDER + 5.0f)
            {
                should_remove = true;
            }
            else if (car->direction == LEFT && car->position < -WINDOW_BORDER - 5.0f)
            {
                should_remove = true;
            }

            if (should_remove)
            {
                ListCar *to_remove = *current;
                *current = to_remove->next;
                free(to_remove);
            }
            else
            {
                current = &(*current)->next;
            }
        }
    }
}

// Удаление машины из полосы - ИСПРАВЛЕННАЯ ВЕРСИЯ
void remove_car_from_lane(int lane_index, ListCar *car_to_remove)
{
    if (lanes[lane_index] == NULL || car_to_remove == NULL)
        return;

    // Если удаляемая машина - голова списка
    if (lanes[lane_index] == car_to_remove)
    {
        lanes[lane_index] = car_to_remove->next;
        free(car_to_remove);
        return;
    }

    // Ищем предыдущий элемент
    ListCar *current = lanes[lane_index];
    while (current->next != NULL && current->next != car_to_remove)
    {
        current = current->next;
    }

    // Если нашли, удаляем
    if (current->next == car_to_remove)
    {
        ListCar *to_free = current->next;
        current->next = to_free->next;
        free(to_free);
    }
}

/* ПЕРЕКРЕСТОК */

// проверка на столкновение мд машинами
bool isCollisionImminent(AdvancedCar2 *car1, AdvancedCar2 *car2)
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
    char lane = 0;
    //while (lane != 1 && lane != -1)
        lane = (rand() % lines_count) + 1;

    // Горизонтальная дорога
    if (road_id == 0)
    {
        return (direction == 1) ? -lane : lane; // Вправо - левая сторона, влево - правая
    }
    // Вертикальная дорога
    else
    {
        return (direction == 1) ? lane : -lane; // Вверх - правая сторона, вниз - левая
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
void drawCrossroadCar(AdvancedCar2 car)
{
    glPushMatrix();

    // Позиция машины
    glTranslatef(car.x, car.y, 0);


    float display_angle;
    if (car.is_turning) {
        display_angle = car.current_display_angle; // Используем угол, рассчитанный в executeTurn
    } else {
        // Стандартные углы для прямолинейного движения
        if (car.direction_x == -1) display_angle = 0;
        else if (car.direction_y == 1) display_angle = 270;
        else if (car.direction_y == -1) display_angle = 90;
        else display_angle = 180;
    }
    glRotatef(display_angle, 0, 0, 1);

    // Индикатор перестроения
    if (car.is_changing_lane)
    {
        glColor3f(1.0, 0.5, 0.0);
        glBegin(GL_LINES);
        glVertex2f(0, 0);
        glVertex2f(0, (car.target_lane - car.lane) * LINE_WIDTH);
        glEnd();
    }

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
void decideTurn(AdvancedCar2 *car)
{
    // машина уже повернула или поворачивает
    if (car->will_turn || car->is_turning)
        return;

    // Если машина едет по крайней левой полосе — поворачивает налево
    if ((car->lane == 1 && (car->direction_x < 0 || car->direction_y > 0)) ||
        (car->lane == -1 && (car->direction_x > 0 || car->direction_y < 0)))
    {
        car->will_turn = true;
        car->planned_turn = TURN_LEFT;
    }
    // Если машина едет по крайней левой полосе — поворачивает налево
    // Машина поворачивает с 50% шансом
    else if ((rand() % 2) &&
            ((car->lane == lines_count && (car->direction_x < 0 || car->direction_y > 0)) ||
            (car->lane == -lines_count && (car->direction_x > 0 || car->direction_y < 0))))
    {
            car->will_turn = true;
            car->planned_turn = TURN_RIGHT;
    
    }
}

// проверка возможности поворота
bool canTurn(AdvancedCar2 *car, TurnDirection turn)
{
    // Проверяем, находится ли машина на правильной полосе для поворота
    // НАЛЕВО машина может повернуть только с крайней ЛЕВОЙ полосы
    if (turn == TURN_LEFT && (abs(car->lane) != 1)) return false;
    // НАПРАВО машина может повернуть только с крайней ПРАВОЙ полосы
    if (turn == TURN_RIGHT && (abs(car->lane) != lines_count)) return false;

    // Предсказываем новую полосу после поворота
    int new_lane = 0;
    if (turn == TURN_LEFT) new_lane = car->lane;
    else  new_lane = -(car->lane);

    // Проверяем машины на целевой дороге и полосе
    for (int i = 0; i < car_count; i++)
    {
        if (&cars[i] == car)
            continue;

        // Машины на целевой дороге и полосе
        if (cars[i].road_id != ((car->road_id + 1) % 2))
            continue;
        if (cars[i].lane != new_lane)
            continue;

        // Проверяем расстояние до перекрестка
        float dist;
        if (car->road_id == 0)
        {
            dist = fabs(cars[i].y) - intersection_size;
        }
        else
        {
            dist = fabs(cars[i].x) - intersection_size;
        }

        // Если машина приближается к перекрестку на целевой полосе - поворот запрещен
        if (dist < SAFE_DISTANCE * 2 && dist > -intersection_size)
        {
            return false;
        }
    }

    return true;
}

// совершение поворота
void executeTurn(AdvancedCar2 *car)
{
    // Если машина еще не поворачивает, проверяем условия для начала поворота
    if (!car->is_turning)
    {
        // Условие для начала поворота: центр машины достигает границы перекрестка.
        // Это упрощение. Более точный старт был бы, когда передняя часть машины достигает точки поворота. (ИСПРАВИТЬ!)
        bool at_turn_start_point = false;
        float turn_threshold;
        if (car->planned_turn == TURN_LEFT) turn_threshold = intersection_size;
        else turn_threshold = intersection_size + 1.4 - 0.25;

        if (car->road_id == 0) { // Горизонтальная дорога
            if (car->direction_x == 1 && car->x >= -turn_threshold) at_turn_start_point = true;
            if (car->direction_x == -1 && car->x <= turn_threshold) at_turn_start_point = true;
        } else { // Вертикальная дорога
            if (car->direction_y == 1 && car->y >= -turn_threshold) at_turn_start_point = true;
            if (car->direction_y == -1 && car->y <= turn_threshold) at_turn_start_point = true;
        }

        // Если машина планирует повернуть, находится в точке старта и поворот возможен
        if (car->will_turn && at_turn_start_point && canTurn(car, car->planned_turn))
        {
            car->is_turning = true;
            car->turn_progress = 0.0f;
            //car->speed *= 0.7f; // Замедление для поворота
            car->speed = TURN_SPEED;
            car->turn_direction = car->planned_turn; // Устанавливаем направление поворота
            // Сохраняем начальные координаты для точного расчета дуги
            car->turn_start_x = car->x;
            car->turn_start_y = car->y;
        }
        else if (car->will_turn && at_turn_start_point && !canTurn(car, car->planned_turn))
        {
            // Если запланированный поворот невозможен, отменяем его
            car->will_turn = false;
        }
        return;
    }


    // Выполнение поворота
    car->turn_progress += 0.01;
    float progress = fmin(1.0f, car->turn_progress); // Ограничиваем прогресс от 0 до 1

    float Cx, Cy, R_arc, start_angle, end_angle;
    float current_angle;

    float I = intersection_size; // Половина ширины/высоты перекрестка
    float LW = LINE_WIDTH;       // Ширина полосы (предполагается 1.0f)

    // Определяем параметры поворота в зависимости от направления движения и типа поворота
    if (car->road_id == 0) { // Горизонтальная дорога (въезд слева или справа)
        float entry_y = car->turn_start_y; // Начальная Y-координата (центр полосы)
        if (car->direction_x == 1) { // Движение вправо
            if (car->turn_direction == TURN_LEFT) { // Поворот налево (вверх)
                Cx = -I;
                Cy = I;
                R_arc = I - entry_y;
                start_angle = 3 * M_PI / 2;
                end_angle = 2 * M_PI ;
            } else { // Поворот направо (вниз)
                Cx = -I-1;
                Cy = -I-1;
                R_arc = I + entry_y + 1;
                start_angle = M_PI / 2;
                end_angle = 0;
            }
        } else { // Движение влево 
            if (car->turn_direction == TURN_LEFT) { // Поворот налево (вниз)
                Cx = I;
                Cy = -I;
                R_arc = I + entry_y;
                start_angle = M_PI / 2;
                end_angle = M_PI;
            } else { // Поворот направо (вверх)
                Cx = I + 1;
                Cy = I + 1;
                R_arc = I - entry_y -1;
                start_angle = M_PI / 2;
                end_angle = 0;
            }
        }
    } else { // Вертикальная дорога (въезд снизу или сверху)
        float entry_x = car->turn_start_x; // Начальная X-координата (центр полосы)
        if (car->direction_y == 1) { // Движение вверх
            if (car->turn_direction == TURN_LEFT) { // Поворот налево (влево)
                Cx = -I;
                Cy = -I;
                R_arc = I + entry_x;
                start_angle = 0; 
                end_angle = M_PI / 2;
            } else { // Поворот направо (вправо)
                Cx = I + 1;
                Cy = -I - 1;
                R_arc = I - entry_x - 1;
                start_angle = 0;
                end_angle = -M_PI / 2;
            }
        } else { // Движение вниз
            if (car->turn_direction == TURN_LEFT) { // Поворот налево (вправо)
                Cx = I;
                Cy = I;
                R_arc = I - entry_x; 
                start_angle = M_PI;
                end_angle = 3 * M_PI / 2;
            } else { // Поворот направо (влево)
                Cx = -I - 1;
                Cy = I + 1;
                R_arc = I + entry_x + 1;
                start_angle = 2 * M_PI;
                end_angle = 3 * M_PI / 2;
            }
        }
    }

    current_angle = start_angle + (end_angle - start_angle) * progress;
    car->x = Cx + R_arc * cos(current_angle);
    car->y = Cy + R_arc * sin(current_angle);

    //Обновляем угол для отрисовки машины (касательная к дуге)
    if (end_angle > start_angle) { // Против часовой стрелки
        car->current_display_angle = current_angle + M_PI / 2;
    } else { // По часовой стрелке
        car->current_display_angle = current_angle - M_PI / 2;
    }
    // Перевод в градусы (для разных х, чтобы машины не поворачивали задом)
    if (car->x < 0)
        car->current_display_angle = car->current_display_angle * 180.0f / M_PI + 180.0f;
    else
        car->current_display_angle = current_angle * 180.0f / M_PI - 90.0f;


    // Завершение поворота
    if (car->turn_progress >= 1.0)
    {
        car->is_turning = false;
        car->will_turn = false;

        // Меняем направление согласно правостороннему движению
        if (car->road_id == 0) // Горизонтальная -> Вертикальная
        {
            car->road_id = 1;

            if (car->turn_direction == TURN_LEFT)
            {
                // Поворот налево: сохраняем относительное направление
                car->direction_y = car->direction_x;
                // После левого поворота занимаем крайнюю левую полосу на новой дороге
                car->lane = (car->direction_y == 1) ? 1 : -1;
            }
            else // TURN_RIGHT
            {
                // Поворот направо: меняем направление на противоположное
                car->direction_y = -car->direction_x;
                // После правого поворота занимаем крайнюю правую полосу на новой дороге
                car->lane = (car->direction_y == 1) ? lines_count : -lines_count;
            }
            
            car->direction_x = 0;            
            // Позиционируем на правильной полосе
            car->x = car->lane * LINE_WIDTH;
        }
        else // Вертикальная -> Горизонтальная
        {
            car->road_id = 0;

            if (car->turn_direction == TURN_LEFT)
            {
                // Поворот налево: сохраняем относительное направление
                car->direction_x = -car->direction_y;
                // После левого поворота занимаем крайнюю левую полосу на новой дороге
                car->lane = (car->direction_x == 1) ? -1 : 1;
            }
            else // TURN_RIGHT
            {
                // Поворот направо: меняем направление на противоположное
                car->direction_x = car->direction_y;
                // После правого поворота занимаем крайнюю правую полосу на новой дороге
                car->lane = (car->direction_x == 1) ? -lines_count : lines_count;
            }
            
            car->direction_y = 0;            
            // Позиционируем на правильной полосе
            car->y = car->lane * LINE_WIDTH;
        }
    }
}


// безопасная скорость
float calculateSpeedCrossroad(AdvancedCar2 *car, float distance)
{
    float safe_speed = car->max_speed * (distance / (SAFE_DISTANCE * 2));
    return fmax(MIN_SPEED, fmin(car->max_speed, safe_speed));
}

// проверка для смены полосы
bool isSafeToChangeCrossroad(AdvancedCar2 *car, char new_lane, float *safe_speed)
{
    *safe_speed = car->max_speed;
    bool is_safe = true;

    for (int i = 0; i < car_count; i++)
    {
        // не проверяем текущую машину
        if (&cars[i] == car)
            continue;
        // не проверяем машины на встречных полосах
        if (cars[i].road_id != car->road_id)
            continue;

        int other_lane = cars[i].is_changing_lane ? (int)(cars[i].lane *
                                                              (1 - cars[i].lane_change_progress) +
                                                          cars[i].target_lane * cars[i].lane_change_progress)
                                                  : cars[i].lane;

        if (abs(other_lane - new_lane) < 1.5)
        {
            float dist;
            // Горизонтальная дорога
            if (car->road_id == 0)
            {
                dist = fabs(cars[i].x - car->x);
            }
            // Вертикальная дорога
            else
            {
                dist = fabs(cars[i].y - car->y);
            }

            bool is_in_front;
            if (car->road_id == 0)
            {
                is_in_front = (car->direction_x == 1 && cars[i].x > car->x) ||
                              (car->direction_x == -1 && cars[i].x < car->x);
            }
            else
            {
                is_in_front = (car->direction_y == 1 && cars[i].y > car->y) ||
                              (car->direction_y == -1 && cars[i].y < car->y);
            }

            if (is_in_front && dist < SAFE_DISTANCE * 3)
            {
                float this_safe_speed = calculateSpeedCrossroad(car, dist);
                if (this_safe_speed < *safe_speed)
                {
                    *safe_speed = this_safe_speed;
                }

                if (dist < SAFE_DISTANCE * 1.5)
                {
                    is_safe = false;
                }
            }
        }
    }

    return is_safe;
}

// избегаение столкновений на перекрестке
void checkCollisionsCrossroad(AdvancedCar2 *car)
{
    car->is_braking = false;
    car->has_priority = true;
    car->in_intersection = (fabs(car->x) < intersection_size && fabs(car->y) < intersection_size);

    // Определяем параметры в зависимости от дороги
    float *main_coord = (car->road_id == 0) ? &car->x : &car->y;
    char *main_dir = (car->road_id == 0) ? &car->direction_x : &car->direction_y;

    // Проверка красного сигнала для этой машины
    bool has_red_light = (car->road_id == 0 && !horizontal_green) ||
                         (car->road_id == 1 && horizontal_green);

    // Если машина уже на перекрестке, даем ей приоритет на завершение проезда
    if (car->in_intersection)
    {
        car->has_priority = true;
        car->just_have_turn = false;

        // Продолжаем движение через перекресток без остановки
        if (car->speed < car->max_speed / 2)
        {
            car->speed += ACCELERATION * 0.5;
        }
        return;
    }

    //Проверка приближения к перекрестку при красном свете
    if (has_red_light)
    {
        float dist_to_stop = fabs(*main_coord) - intersection_size;
        bool approaching = (*main_dir == 1)
                               ? (*main_coord < intersection_size && *main_coord > intersection_size - SAFE_DISTANCE * 3)
                               : (*main_coord > -intersection_size && *main_coord < -intersection_size + SAFE_DISTANCE * 3);

        if (approaching)
        {
            // Начинаем плавное торможение
            float brake_intensity = fmin(1.0, (SAFE_DISTANCE * 3 - dist_to_stop) / SAFE_DISTANCE);
            car->speed = fmax(0, car->speed - brake_intensity * 0.05);
            car->is_braking = true;
            car->has_priority = false;

            // Полная остановка перед перекрестком
            if (dist_to_stop < 0.5)
            {
                car->speed = 0;
                // Корректируем позицию, чтобы не заехать на перекресток
                if (*main_dir == 1 && *main_coord > intersection_size - 0.5)
                {
                    *main_coord = intersection_size - 0.5;
                }
                else if (*main_dir == -1 && *main_coord < -intersection_size + 0.5)
                {
                    *main_coord = -intersection_size + 0.5;
                }
            }
            return;
        }
    }

    // Проверка столкновений с другими машинами
    for (int i = 0; i < car_count; i++)
    {
        if (&cars[i] == car)
            continue;

        // Проверяем только машины на той же дороге или на перекрестке
        bool same_road = (cars[i].road_id == car->road_id);
        bool both_in_intersection = (car->in_intersection && cars[i].in_intersection);

        if (!same_road && !both_in_intersection)
            continue;

        // Проверяем, находятся ли машины на одной полосе или пересекающихся траекториях
        float car_lane = car->lane;
        if (car->is_changing_lane)
        {
            car_lane = car->lane * (1 - car->lane_change_progress) + car->target_lane * car->lane_change_progress;
        }

        float other_lane = cars[i].lane;
        if (cars[i].is_changing_lane)
        {
            other_lane = cars[i].lane * (1 - cars[i].lane_change_progress) + cars[i].target_lane * cars[i].lane_change_progress;
        }

        float lane_distance = fabs(car_lane - other_lane) * LINE_WIDTH;
        if (lane_distance < LINE_WIDTH * 0.8) // Учитываем ширину машины
        {
            float path_distance;
            if (same_road)
            {
                path_distance = (car->road_id == 0) ? fabs(cars[i].x - car->x) : fabs(cars[i].y - car->y);
            }
            else // На перекрестке
            {
                path_distance = sqrt(pow(cars[i].x - car->x, 2) + pow(cars[i].y - car->y, 2));
            }

            bool is_in_front;
            if (same_road)
            {
                if (car->road_id == 0)
                {
                    is_in_front = (car->direction_x == 1 && cars[i].x > car->x) ||
                                  (car->direction_x == -1 && cars[i].x < car->x);
                }
                else
                {
                    is_in_front = (car->direction_y == 1 && cars[i].y > car->y) ||
                                  (car->direction_y == -1 && cars[i].y < car->y);
                }
            }
            else // На перекрестке учитываем все направления
            {
                is_in_front = path_distance < SAFE_DISTANCE * 1.5; // Более агрессивное торможение на перекрестке
            }

            if (is_in_front && path_distance < SAFE_DISTANCE * 2)
            {
                car->is_braking = true;
                float safe_speed = calculateSpeedCrossroad(car, path_distance);

                // Более резкое торможение на перекрестке
                float braking_factor = both_in_intersection ? 0.6 : 0.8;

                if (car->speed > safe_speed)
                {
                    car->speed = fmax(safe_speed, car->speed * braking_factor);
                }

                // Полная остановка, если слишком близко
                if (path_distance < SAFE_DISTANCE * 0.5)
                {
                    car->speed = 0;
                }
            }
        }
    }

    // Ускорение, если нет препятствий
    if (!car->is_braking && car->speed < car->max_speed && !car->in_intersection)
    {
        car->speed += ACCELERATION;
        car->speed = fmin(car->speed, car->max_speed);
    }

    car->just_have_turn = false;
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

// обновление машин для перекрестка
void updateCars()
{
    updateTrafficLight();

    for (int i = 0; i < car_count; i++)
    {
        checkCollisionsCrossroad(&cars[i]);

        // Если машина будет поворачивать она должна снижать скорость
        if (cars[i].will_turn && !cars[i].is_turning) // Машина планирует поворот, но еще не начала его
        {
            if ((cars[i].x > -intersection_size * 3 && cars[i].x < -intersection_size) ||
                (cars[i].x < intersection_size * 3 && cars[i].x > intersection_size) ||
                (cars[i].y > -intersection_size * 3 && cars[i].y < -intersection_size) ||
                (cars[i].y < intersection_size * 3 && cars[i].y > intersection_size))
                
                if (cars[i].speed > TURN_SPEED)
                    cars[i].speed *= 0.95;
                else
                    cars[i].speed = TURN_SPEED;
        }


        // Выполнение поворота
        if (cars[i].will_turn || cars[i].is_turning)
        {
            executeTurn(&cars[i]);
        }

        // Решение о перестроении
        // if (!cars[i].is_changing_lane && rand() % 100 < 20)
        // {
        //     decideChangeCrossroad(&cars[i]);
        // }

        // Обычное движение
        if (!cars[i].is_turning)
        {
            // Горизонтальная дорога
            if (cars[i].road_id == 0)
            {
                cars[i].x += cars[i].speed * cars[i].direction_x;

                // УДАЛЕНИЕ машины вместо телепортации
                if ((cars[i].direction_x == 1 && cars[i].x > WINDOW_BORDER + 2) ||
                    (cars[i].direction_x == -1 && cars[i].x < -WINDOW_BORDER - 2))
                {
                    // Удаляем машину из массива
                    for (int j = i; j < car_count - 1; j++)
                    {
                        cars[j] = cars[j + 1];
                    }
                    car_count--;
                    continue; // Пропускаем оставшуюся обработку для удаленной машины
                }
            }
            // Вертикальная дорога
            else
            {
                cars[i].y += cars[i].speed * cars[i].direction_y;

                // УДАЛЕНИЕ машины вместо телепортации
                if ((cars[i].direction_y == 1 && cars[i].y > WINDOW_BORDER + 2) ||
                    (cars[i].direction_y == -1 && cars[i].y < -WINDOW_BORDER - 2))
                {
                    // Удаляем машину из массива
                    for (int j = i; j < car_count - 1; j++)
                    {
                        cars[j] = cars[j + 1];
                    }
                    car_count--;
                    continue; // Пропускаем оставшуюся обработку для удаленной машины
                }
            }
        }
    }
}

// добавление машины на перекресток
void addCrossroadCar()
{
    if (car_count >= MAX_CARS)
        return;

    static float last_add_time = 0;
    float current_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;

    if (current_time - last_add_time < 1.5) // генерация машин раз в N секунд
        return;

    last_add_time = current_time;

    char road_id = rand() % 2; // 0 - горизонтальная, 1 - вертикальная
    char direction = (rand() % 2) ? 1 : -1;
    char lane = getRandomLane(road_id, direction);
    float start_x, start_y;

    if (road_id == 0)
    {
        start_x = (direction == 1) ? -WINDOW_BORDER - 1 : WINDOW_BORDER + 1;
        start_y = lane * LINE_WIDTH;
    }
    else
    {
        start_x = lane * LINE_WIDTH;
        start_y = (direction == 1) ? -WINDOW_BORDER - 1 : WINDOW_BORDER + 1;
    }

    // Проверка возможности добавления машины
    bool can_add = true;
    for (int i = 0; i < car_count; i++)
    {
        if (cars[i].road_id != road_id)
            continue;

        float dist;
        if (road_id == 0)
        {
            dist = fabs(cars[i].x - start_x);
        }
        else
        {
            dist = fabs(cars[i].y - start_y);
        }

        if (dist < SAFE_DISTANCE * 3)
        {
            can_add = false;
            break;
        }
    }

    if (!can_add)
        return;

    AdvancedCar2 new_car;
    new_car.road_id = road_id;

    if (road_id == 0)
    {
        new_car.direction_x = direction;
        new_car.direction_y = 0;
        new_car.x = start_x;
        new_car.y = start_y;
    }
    else
    {
        new_car.direction_x = 0;
        new_car.direction_y = direction;
        new_car.x = start_x;
        new_car.y = start_y;
    }

    new_car.lane = lane;

    new_car.is_braking = false;
    new_car.in_intersection = false;
    new_car.has_priority = true;
    new_car.max_speed = MIN_SPEED + (rand() % (int)((MAX_SPEED - MIN_SPEED) * 100)) * 0.01;
    new_car.speed = new_car.max_speed * 0.6;
    do
    {
        new_car.color[0] = (50 + rand() % 50) / 100.0f; // R: 0.5-1.0 (исключает 0.0)
        new_car.color[1] = (rand() % 100) / 100.0f;     // G: 0.0-1.0
        new_car.color[2] = (rand() % 100) / 100.0f;     // B: 0.0-1.0

        // Проверяем, что цвет не черный (0,0,0) и не красный (1,0,0)
    } while ((new_car.color[0] == 1.0f && new_car.color[1] == 0.0f && new_car.color[2] == 0.0f) ||
             (new_car.color[0] == 0.0f && new_car.color[1] == 0.0f && new_car.color[2] == 0.0f));

    new_car.is_turning = false;
    new_car.turn_progress = 0.0;
    new_car.will_turn = false;
    new_car.road_id = road_id;
    new_car.just_have_turn = false;

    if (road_id == 0) // Горизонтальная дорога
        new_car.current_display_angle = (direction == 1) ? 0 : 180;
    else // Вертикальная дорога
        new_car.current_display_angle = (direction == 1) ? 90 : 270;
    
    // Решение о повороте сразу после добавления
    decideTurn(&new_car);

    cars[car_count++] = new_car;
}

// отображение перекрестка
void displayCrossroad()
{
    glClear(GL_COLOR_BUFFER_BIT);

    drawRoads();
    drawIntersection();

    for (int i = 0; i < car_count; i++)
    {
        drawCrossroadCar(cars[i]);
    }

    infoStatistic(true);

    glutSwapBuffers();
}

// обновление отображения
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

/*  РАБОТА С ФАЙЛАМИ */


// // сохранение моделирования
// void saveSimulation(const char *filename)
// {
//     FILE *file = fopen(filename, "w");
//     if (!file)
//     {
//         printf("ERROR: Could not open file %s for writing\n", filename);
//         return;
//     }

//     // Сохраняем тип дороги и количество полос
//     fprintf(file, "RoadType: %d\n", type_simulation);
//     fprintf(file, "Lanes: %d\n", lines_count);
//     fprintf(file, "CarsCount: %d\n", car_count);
//     fprintf(file, "Cars:\n");

//     for (int i = 0; i < car_count; i++)
//     {
//         fprintf(file, "Car %d:\n", i);

//         if (type_simulation == HIGHWAY)
//         {
//             printf("come later \n");

//             // Сохраняем параметры AdvancedCar для автострады

//             /*
//             fprintf(file, "  Type: AdvancedCar\n");
//             fprintf(file, "  Position: %.2f\n", advanced_cars[i].position);
//             fprintf(file, "  Speed: %.2f\n", advanced_cars[i].speed);
//             fprintf(file, "  MaxSpeed: %.2f\n", advanced_cars[i].max_speed);
//             fprintf(file, "  Lane: %d\n", advanced_cars[i].lane);
//             fprintf(file, "  TargetLane: %d\n", advanced_cars[i].target_lane);
//             fprintf(file, "  LaneChangeProgress: %.2f\n", advanced_cars[i].lane_change_progress);
//             fprintf(file, "  Direction: %d\n", advanced_cars[i].direction);
//             fprintf(file, "  Color: %.2f %.2f %.2f\n",
//                     advanced_cars[i].color[0],
//                     advanced_cars[i].color[1],
//                     advanced_cars[i].color[2]);
//             fprintf(file, "  IsChangingLane: %d\n", advanced_cars[i].is_changing_lane);
//             fprintf(file, "  IsBraking: %d\n", advanced_cars[i].is_braking);
//             fprintf(file, "  TargetSpeed: %.2f\n", advanced_cars[i].target_speed);
//             */
//         }
//         else
//         {
//             // Сохраняем параметры AdvancedCar2 для перекрестка
//             fprintf(file, "  Type: AdvancedCar2\n");
//             fprintf(file, "  Position: %.2f %.2f\n", cars[i].x, cars[i].y);
//             fprintf(file, "  Speed: %.2f\n", cars[i].speed);
//             fprintf(file, "  MaxSpeed: %.2f\n", cars[i].max_speed);
//             fprintf(file, "  Lane: %d\n", cars[i].lane);
//             fprintf(file, "  TargetLane: %d\n", cars[i].target_lane);
//             fprintf(file, "  LaneChangeProgress: %.2f\n", cars[i].lane_change_progress);
//             fprintf(file, "  Direction: %d %d\n", cars[i].direction_x, cars[i].direction_y);
//             fprintf(file, "  Color: %.2f %.2f %.2f\n",
//                     cars[i].color[0], cars[i].color[1], cars[i].color[2]);
//             fprintf(file, "  IsChangingLane: %d\n", cars[i].is_changing_lane);
//             fprintf(file, "  IsBraking: %d\n", cars[i].is_braking);
//             fprintf(file, "  TargetSpeed: %.2f\n", cars[i].target_speed);
//             fprintf(file, "  InIntersection: %d\n", cars[i].in_intersection);
//             fprintf(file, "  HasPriority: %d\n", cars[i].has_priority);
//             fprintf(file, "  IsTurning: %d\n", cars[i].is_turning);
//             fprintf(file, "  TurnDirection: %d\n", cars[i].turn_direction);
//             fprintf(file, "  TurnProgress: %.2f\n", cars[i].turn_progress);
//             fprintf(file, "  RoadID: %d\n", cars[i].road_id);
//             fprintf(file, "  WillTurn: %d\n", cars[i].will_turn);
//             fprintf(file, "  PlannedTurn: %d\n", cars[i].planned_turn);
//             fprintf(file, "  JustHaveTurn: %d\n", cars[i].just_have_turn);
//         }
//         fprintf(file, "\n");
//     }

//     fclose(file);
//     printf("Simulation saved to %s\n", filename);
//     printf("For continue, press TAB in simulation window\n");
// }

// // загрузка моделирования
// void loadSimulation(const char *filename)
// {
//     FILE *file = fopen(filename, "r");
//     if (!file)
//     {
//         printf("ERROR: Could not open file %s\n", filename);
//         return;
//     }

//     char line[256];
//     int current_car = -1;
//     bool car_type = false; // 0 - AdvancedCar, 1 - AdvancedCar2

//     // Сброс текущего состояния
//     car_count = 0;
//     type_simulation = MENU;

//     while (fgets(line, sizeof(line), file))
//     {
//         // Удаляем символ новой строки
//         line[strcspn(line, "\n")] = 0;

//         // Пропускаем пустые строки
//         if (strlen(line) == 0)
//             continue;

//         // основные параметры
//         if (strstr(line, "RoadType:") != NULL)
//         {
//             sscanf(line, "RoadType: %d", &type_simulation);
//         }
//         else if (strstr(line, "Lanes:") != NULL)
//         {
//             sscanf(line, "Lanes: %d", &lines_count);
//             if (type_simulation == CROSSROAD)
//             {
//                 if (lines_count == 1)
//                     intersection_size = 1.0f;
//                 else if (lines_count == 2)
//                     intersection_size = 2.0f;
//                 else if (lines_count == 3)
//                     intersection_size = 3.0f;
//                 else if (lines_count == 4)
//                     intersection_size = 4.0f;
//                 else if (lines_count == 5)
//                     intersection_size = 5.0f;
//             }
//         }
//         else if (strstr(line, "CarsCount:") != NULL)
//         {
//             sscanf(line, "CarsCount: %d", &car_count);
//         }
//         // Обработка данных машин
//         else if (strstr(line, "Car ") != NULL)
//         {
//             sscanf(line, "Car %d:", &current_car);
//         }
//         else if (strstr(line, "Type:") != NULL)
//         {
//             char type[20];
//             sscanf(line, "  Type: %s", type);
//             car_type = (strcmp(type, "AdvancedCar") == 0) ? false : true;
//         }
//         // Загрузка AdvancedCar
//         else if (!car_type && current_car >= 0)
//         {
//             printf("come later \n");
//             /*
//             if (strstr(line, "Position:") != NULL)
//             {
//                 sscanf(line, "  Position: %f", &advanced_cars[current_car].position);
//             }
//             else if (strstr(line, "Speed:") != NULL)
//             {
//                 sscanf(line, "  Speed: %f", &advanced_cars[current_car].speed);
//             }
//             else if (strstr(line, "MaxSpeed:") != NULL)
//             {
//                 sscanf(line, "  MaxSpeed: %f", &advanced_cars[current_car].max_speed);
//             }
//             else if (strstr(line, "Lane:") != NULL)
//             {
//                 sscanf(line, "  Lane: %d", &advanced_cars[current_car].lane);
//             }
//             else if (strstr(line, "TargetLane:") != NULL)
//             {
//                 sscanf(line, "  TargetLane: %d", &advanced_cars[current_car].target_lane);
//             }
//             else if (strstr(line, "LaneChangeProgress:") != NULL)
//             {
//                 sscanf(line, "  LaneChangeProgress: %f", &advanced_cars[current_car].lane_change_progress);
//             }
//             else if (strstr(line, "Direction:") != NULL)
//             {
//                 sscanf(line, "  Direction: %d", &advanced_cars[current_car].direction);
//             }
//             else if (strstr(line, "Color:") != NULL)
//             {
//                 sscanf(line, "  Color: %f %f %f",
//                        &advanced_cars[current_car].color[0],
//                        &advanced_cars[current_car].color[1],
//                        &advanced_cars[current_car].color[2]);
//             }
//             else if (strstr(line, "IsChangingLane:") != NULL)
//             {
//                 int val;
//                 sscanf(line, "  IsChangingLane: %d", &val);
//                 advanced_cars[current_car].is_changing_lane = val;
//             }
//             else if (strstr(line, "IsBraking:") != NULL)
//             {
//                 int val;
//                 sscanf(line, "  IsBraking: %d", &val);
//                 advanced_cars[current_car].is_braking = val;
//             }
//             else if (strstr(line, "TargetSpeed:") != NULL)
//             {
//                 sscanf(line, "  TargetSpeed: %f", &advanced_cars[current_car].target_speed);
//             }
//                 */
//         }
//         // Загрузка AdvancedCar2
//         else if (car_type && current_car >= 0)
//         {
//             if (strstr(line, "Position:") != NULL)
//             {
//                 sscanf(line, "  Position: %f %f", &cars[current_car].x, &cars[current_car].y);
//             }
//             else if (strstr(line, "Speed:") != NULL)
//             {
//                 sscanf(line, "  Speed: %f", &cars[current_car].speed);
//             }
//             else if (strstr(line, "MaxSpeed:") != NULL)
//             {
//                 sscanf(line, "  MaxSpeed: %f", &cars[current_car].max_speed);
//             }
//             else if (strstr(line, "Lane:") != NULL)
//             {
//                 sscanf(line, "  Lane: %d", &cars[current_car].lane);
//             }
//             else if (strstr(line, "TargetLane:") != NULL)
//             {
//                 sscanf(line, "  TargetLane: %d", &cars[current_car].target_lane);
//             }
//             else if (strstr(line, "LaneChangeProgress:") != NULL)
//             {
//                 sscanf(line, "  LaneChangeProgress: %f", &cars[current_car].lane_change_progress);
//             }
//             else if (strstr(line, "Direction:") != NULL)
//             {
//                 sscanf(line, "  Direction: %d %d",
//                        &cars[current_car].direction_x,
//                        &cars[current_car].direction_y);
//             }
//             else if (strstr(line, "Color:") != NULL)
//             {
//                 sscanf(line, "  Color: %f %f %f",
//                        &cars[current_car].color[0],
//                        &cars[current_car].color[1],
//                        &cars[current_car].color[2]);
//             }
//             else if (strstr(line, "IsChangingLane:") != NULL)
//             {
//                 int val;
//                 sscanf(line, "  IsChangingLane: %d", &val);
//                 cars[current_car].is_changing_lane = val;
//             }
//             else if (strstr(line, "IsBraking:") != NULL)
//             {
//                 int val;
//                 sscanf(line, "  IsBraking: %d", &val);
//                 cars[current_car].is_braking = val;
//             }
//             else if (strstr(line, "TargetSpeed:") != NULL)
//             {
//                 sscanf(line, "  TargetSpeed: %f", &cars[current_car].target_speed);
//             }
//             else if (strstr(line, "InIntersection:") != NULL)
//             {
//                 int val;
//                 sscanf(line, "  InIntersection: %d", &val);
//                 cars[current_car].in_intersection = val;
//             }
//             else if (strstr(line, "HasPriority:") != NULL)
//             {
//                 int val;
//                 sscanf(line, "  HasPriority: %d", &val);
//                 cars[current_car].has_priority = val;
//             }
//             else if (strstr(line, "IsTurning:") != NULL)
//             {
//                 int val;
//                 sscanf(line, "  IsTurning: %d", &val);
//                 cars[current_car].is_turning = val;
//             }
//             else if (strstr(line, "TurnDirection:") != NULL)
//             {
//                 int val;
//                 sscanf(line, "  TurnDirection: %d", &val);
//                 cars[current_car].turn_direction = val;
//             }
//             else if (strstr(line, "TurnProgress:") != NULL)
//             {
//                 sscanf(line, "  TurnProgress: %f", &cars[current_car].turn_progress);
//             }
//             else if (strstr(line, "RoadID:") != NULL)
//             {
//                 sscanf(line, "  RoadID: %d", &cars[current_car].road_id);
//             }
//             else if (strstr(line, "WillTurn:") != NULL)
//             {
//                 int val;
//                 sscanf(line, "  WillTurn: %d", &val);
//                 cars[current_car].will_turn = val;
//             }
//             else if (strstr(line, "PlannedTurn:") != NULL)
//             {
//                 int val;
//                 sscanf(line, "  PlannedTurn: %d", &val);
//                 cars[current_car].planned_turn = val;
//             }
//             else if (strstr(line, "JustHaveTurn:") != NULL)
//             {
//                 int val;
//                 sscanf(line, "  JustHaveTurn: %d", &val);
//                 cars[current_car].just_have_turn = val;
//             }
//         }
//     }

//     fclose(file);
//     printf("Simulation loaded successfully from %s\n", filename);
//     printf("Go back to simulation window\n");

//     // Запускаем соответствующую симуляцию
//     if (type_simulation == HIGHWAY)
//     {
//         /*
//         start_time = clock();
//         init();
//         in_simulation = true;
//         glutDisplayFunc(displayHighway);
//         glutTimerFunc(0, updateHighway, 0);
//         */
//     }
//     else if (type_simulation == CROSSROAD)
//     {
//         start_time = clock();
//         init();
//         in_simulation = true;
//         glutDisplayFunc(displayCrossroad);
//         glutTimerFunc(0, timerCrossroad, 0);
//     }
// }


// сохранение моделирования
void saveSimulation(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        printf("ERROR: Could not open file %s for writing\n", filename);
        return;
    }

    // Подсчет общего количества машин
    int total_cars = 0;
    for (int i = 0; i < lane_count; i++)
    {
        total_cars += count_cars(lanes[i]);
    }

    // Сохраняем тип дороги и количество полос
    fprintf(file, "RoadType: %d\n", type_simulation);
    fprintf(file, "Lanes: %d\n", lines_count);
    fprintf(file, "LaneCount: %d\n", lane_count);
    fprintf(file, "CarsCount: %d\n", total_cars);
    fprintf(file, "Cars:\n");

    int car_index = 0;

    // Сохраняем машины по полосам
    for (int lane_idx = 0; lane_idx < lane_count; lane_idx++)
    {
        ListCar *current = lanes[lane_idx];
        while (current != NULL)
        {
            CarNode *car = &current->car;
            fprintf(file, "Car %d:\n", car_index);
            fprintf(file, "  Type: HighwayCar\n");
            fprintf(file, "  Position: %.2f\n", car->position);
            fprintf(file, "  Speed: %.2f\n", car->speed);
            fprintf(file, "  MaxSpeed: %.2f\n", car->max_speed);
            fprintf(file, "  Lane: %d\n", car->lane);
            fprintf(file, "  Direction: %d\n", car->direction);
            fprintf(file, "  Color: %.2f %.2f %.2f\n",
                    car->color[0], car->color[1], car->color[2]);
            fprintf(file, "  IsBraking: %d\n", car->is_braking);

            // Параметры перестроения
            fprintf(file, "  IsChangingLane: %d\n", car->is_changing_lane);
            fprintf(file, "  TargetLane: %d\n", car->target_lane);
            fprintf(file, "  LaneChangeProgress: %.2f\n", car->lane_change_progress);
            fprintf(file, "  TargetSpeed: %.2f\n", car->target_speed);

            // Состояние машины
            fprintf(file, "  State: %d\n", car->state);
            fprintf(file, "  FixedPosition: %.2f\n", car->fixed_position);

            fprintf(file, "  LaneIndex: %d\n", lane_idx);
            fprintf(file, "\n");

            car_index++;
            current = current->next;
        }
    }

    fclose(file);
    printf("Simulation saved to %s\n", filename);
    printf("Total cars saved: %d\n", total_cars);
    printf("For continue, press TAB in simulation window\n");
}

// загрузка моделирования
void loadSimulation(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("ERROR: Could not open file %s\n", filename);
        return;
    }

    char line[256];
    int current_car = -1;
    bool is_highway_car = false;
    CarNode temp_car = {0};
    int lane_index = -1;

    // Сброс текущего состояния
    free_all_lanes();
    type_simulation = MENU;
    lines_count = 3; // значение по умолчанию

    while (fgets(line, sizeof(line), file))
    {
        // Удаляем символ новой строки
        line[strcspn(line, "\n")] = 0;

        // Пропускаем пустые строки
        if (strlen(line) == 0)
            continue;

        // основные параметры
        if (strstr(line, "RoadType:") != NULL)
        {
            sscanf(line, "RoadType: %d", &type_simulation);
        }
        else if (strstr(line, "Lanes:") != NULL)
        {
            sscanf(line, "Lanes: %d", &lines_count);
        }
        else if (strstr(line, "LaneCount:") != NULL)
        {
            int saved_lane_count;
            sscanf(line, "LaneCount: %d", &saved_lane_count);
            // Восстанавливаем полосы
            init_lanes(saved_lane_count / 2);
        }
        else if (strstr(line, "CarsCount:") != NULL)
        {
            int total_cars;
            sscanf(line, "CarsCount: %d", &total_cars);
            printf("Loading %d cars...\n", total_cars);
        }
        // Обработка данных машин
        else if (strstr(line, "Car ") != NULL)
        {
            // Если есть предыдущая загруженная машина, добавляем её
            if (current_car >= 0 && lane_index >= 0)
            {
                insert_car(&lanes[lane_index], temp_car);
            }

            sscanf(line, "Car %d:", &current_car);
            temp_car = (CarNode){0}; // Сбрасываем временную машину
            lane_index = -1;
        }
        else if (strstr(line, "Type:") != NULL)
        {
            char type[20];
            sscanf(line, "  Type: %s", type);
            is_highway_car = (strcmp(type, "HighwayCar") == 0);
        }
        // Загрузка HighwayCar
        else if (is_highway_car && current_car >= 0)
        {
            if (strstr(line, "Position:") != NULL)
            {
                sscanf(line, "  Position: %f", &temp_car.position);
            }
            else if (strstr(line, "Speed:") != NULL)
            {
                sscanf(line, "  Speed: %f", &temp_car.speed);
            }
            else if (strstr(line, "MaxSpeed:") != NULL)
            {
                sscanf(line, "  MaxSpeed: %f", &temp_car.max_speed);
            }
            else if (strstr(line, "Lane:") != NULL)
            {
                sscanf(line, "  Lane: %d", &temp_car.lane);
            }
            else if (strstr(line, "Direction:") != NULL)
            {
                int dir;
                sscanf(line, "  Direction: %d", &dir);
                temp_car.direction = (CarDirection)dir;
            }
            else if (strstr(line, "Color:") != NULL)
            {
                sscanf(line, "  Color: %f %f %f",
                       &temp_car.color[0],
                       &temp_car.color[1],
                       &temp_car.color[2]);
            }
            else if (strstr(line, "IsBraking:") != NULL)
            {
                int val;
                sscanf(line, "  IsBraking: %d", &val);
                temp_car.is_braking = val;
            }
            else if (strstr(line, "IsChangingLane:") != NULL)
            {
                int val;
                sscanf(line, "  IsChangingLane: %d", &val);
                temp_car.is_changing_lane = val;
            }
            else if (strstr(line, "TargetLane:") != NULL)
            {
                sscanf(line, "  TargetLane: %d", &temp_car.target_lane);
            }
            else if (strstr(line, "LaneChangeProgress:") != NULL)
            {
                sscanf(line, "  LaneChangeProgress: %f", &temp_car.lane_change_progress);
            }
            else if (strstr(line, "TargetSpeed:") != NULL)
            {
                sscanf(line, "  TargetSpeed: %f", &temp_car.target_speed);
            }
            else if (strstr(line, "State:") != NULL)
            {
                sscanf(line, "  State: %d", &temp_car.state);
            }
            else if (strstr(line, "FixedPosition:") != NULL)
            {
                sscanf(line, "  FixedPosition: %f", &temp_car.fixed_position);
            }
            else if (strstr(line, "LaneIndex:") != NULL)
            {
                sscanf(line, "  LaneIndex: %d", &lane_index);
            }
        }
    }

    // Добавляем последнюю загруженную машину
    if (current_car >= 0 && lane_index >= 0)
    {
        insert_car(&lanes[lane_index], temp_car);
    }

    fclose(file);
    printf("Simulation loaded successfully from %s\n", filename);

    // Подсчет загруженных машин
    int loaded_cars = 0;
    for (int i = 0; i < lane_count; i++)
    {
        loaded_cars += count_cars(lanes[i]);
    }
    printf("Loaded cars: %d\n", loaded_cars);
    printf("Go back to simulation window\n");

    // Запускаем соответствующую симуляцию
    if (type_simulation == HIGHWAY)
    {
        start_time = clock();
        init();
        in_simulation = true;
        glutDisplayFunc(displayHighway);
        glutTimerFunc(0, updateHighway, 0);
    }
    else if (type_simulation == CROSSROAD)
    {
        // Для перекрестка нужно использовать другую логику
        printf("Crossroad simulation loading not fully implemented yet\n");
        /*
        start_time = clock();
        init();
        in_simulation = true;
        glutDisplayFunc(displayCrossroad);
        glutTimerFunc(0, timerCrossroad, 0);
        */
    }
}


void displayAccident()
{
    printf("Write number of lane (1-%d for right, -1 to -%d for left):\n", lines_count, lines_count);
    int num;
    if (scanf("%d", &num) != 1)
    {
        printf("ERROR: Invalid input\n");
        accident_flag = !accident_flag;
        // Очищаем буфер ввода
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
            ;
    }
    else
    {
        if (num == 0 || num > lines_count || num < -lines_count)
        {
            printf("ERROR: Lane number must be between 1 and %d or -1 and -%d\n", lines_count, lines_count);
            accident_flag = !accident_flag;
        }
        else
        {
            accident_lane = num;
            printf("Accident set on lane %d\n", accident_lane);

            // Создаем аварию на указанной полосе
            CarDirection dir = (num > 0) ? RIGHT : LEFT;
            char lane_num = (char)abs(num);
            int lane_index = get_lane_index(dir, lane_num);

            // Находим две первые машины на этой полосе и создаем аварию
            ListCar *current = lanes[lane_index];
            int cars_found = 0;

            while (current != NULL && cars_found < 2)
            {
                current->car.state = CAR_STATE_ACCIDENT;
                current->car.speed = 0.0;
                current->car.is_braking = true;
                current->car.color[0] = 1.0;
                current->car.color[1] = 0.0;
                current->car.color[2] = 0.0;
                cars_found++;
                current = current->next;
            }

            if (cars_found == 0)
            {
                printf("No cars on lane %d to create accident\n", num);
            }
            else
            {
                printf("Accident created on lane %d affecting %d cars\n", num, cars_found);
            }
        }
    }
}

// - ---------


// окно с информацией о загрузке моделирования
void loadFromFile()
{
    glColor3f(0.69f, 0.87f, 0.90f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, 1.0f);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(1.0f, 1.0f);
    glEnd();

    const char str[10][100] = {"LOAD FROM FILE", " ", "Here you can load previous generate",
                               "(option available, if you saved some generation)", "press \'L\' button to write full filename in console",
                               "write filename in this format: <filename>.txt", "you need to be sure, that file exist in programm folder",
                               " ", " :-)"};

    glColor3f(0.0f, 0.0f, 0.0f);
    float y_pos = 0.6f;
    for (int i = 0; i < 10; i++)
    {
        glRasterPos2f(-0.7f, y_pos);
        for (int j = 0; j < strlen(str[i]); j++)
        {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[i][j]);
        }
        y_pos -= 0.1f;
    }

    drawButton(-0.3f, -0.5f, 0.4f, 0.1f, "Back", button_hover == 5);
}

// void displayAccident()
// {
//     printf("write number of lane:\n");
//     int num;
//     if (scanf("%d", &num) != 1)
//     {
//         printf("ERROR\n");
//         accident_flag = !accident_flag;
//     }
//     else
//     {
//         if (num > lines_count || num < -lines_count || num == 0)
//         {
//             printf("ERROR\n");
//             accident_flag = !accident_flag;
//         }
//         else
//         {
//             accident_lane = num;
//             printf("set to %d\n", accident_lane);
//         }
//     }
// }