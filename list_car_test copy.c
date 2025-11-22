// реализация с массивом полос

#include "head_test.h"
#include "head_list.h"

//-----------------------------------------------------------------
// Реализация массива полос
ListCar *lanes[MAX_LANES] = {NULL};
int lane_count = 6;  // По умолчанию 3 полосы в каждую сторону

void decideLaneChange(ListCar *current_car);
bool isSafeToChangeLane(ListCar *current_car, char new_lane, float *safe_speed);

void remove_cars_out_of_bounds();
void move_car_to_new_lane(int old_lane_index, int new_lane_index, ListCar *car_node);
void remove_car_from_lane(int lane_index, ListCar *car_to_remove);


// Инициализация полос
void init_lanes(int lanes_per_direction)
{
    // Освобождаем старые полосы, если они есть
    free_all_lanes();

    lane_count = lanes_per_direction * 2;

    // Инициализируем все указатели в NULL
    for (int i = 0; i < MAX_LANES; i++)
    {
        lanes[i] = NULL;
    }
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
    {
        return lane_number - 1; // Правые полосы: 0, 1, 2...
    }
    else
    {
        return (lane_count / 2) + (lane_number - 1); // Левые полосы
    }
}

// Получение указателя на список полосы
ListCar *get_lane(CarDirection direction, char lane_number)
{
    int index = get_lane_index(direction, lane_number);
    if (index >= 0 && index < lane_count)
    {
        return lanes[index];
    }
    return NULL;
}

// Применение функции ко всем полосам
void for_each_lane(void (*func)(ListCar *))
{
    for (int i = 0; i < lane_count; i++)
    {
        if (lanes[i] != NULL)
        {
            func(lanes[i]);
        }
    }
}

// Обновление всех машин
void update_all_cars(void)
{
    for (int i = 0; i < lane_count; i++)
    {
        if (lanes[i] != NULL)
        {
            updateAdvancedCars(&lanes[i]);
        }
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
//-------------------------------------------------------

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

// БЫЛО
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
                    else if (glY >= 0.1f && glY <= 0.2)
                    {
                        // start_time = clock();
                        // init();
                        // initCrossroadCar();
                        // in_simulation = true;
                        // type_simulation = CROSSROAD;
                        // glutDisplayFunc(displayCrossroad);
                        // glutTimerFunc(0, timerCrossroad, 0);
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
                        init_lanes(1);  // Инициализируем полосы
                    }
                    else if (glX >= 0.3f && glX <= 0.4f)
                    {
                        lines_count = 2;
                        intersection_size = 2.0f;
                        init_lanes(2);  
                    }
                    else if (glX >= 0.45f && glX <= 0.55f)
                    {
                        lines_count = 3;
                        intersection_size = 3.0f;
                        init_lanes(3);  
                    }
                    else if (glX >= 0.6f && glX <= 0.7f)
                    {
                        lines_count = 4;
                        intersection_size = 4.0f;
                        init_lanes(4);  
                    }
                    else if (glX >= 0.75f && glX <= 0.85f)
                    {
                        lines_count = 5;
                        intersection_size = 5.0f;
                        init_lanes(5);  
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
        drawButton(-0.5f, 0.1f, 0.5f, 0.1f, "crossroad - no", button_hover == 3);
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
        printf("dont work now");

        // printf("saving mode\n");
        // track = !track;
        // printf("write <filename>.txt to save:\n");
        // char file[100];
        // if (!fgets(file, 100, stdin))
        // {
        //     printf("ERROR\n");
        // }
        // else
        // {
        //     file[strcspn(file, "\n")] = '\0';
        //     saveSimulation(file);
        // }
    }
    // загрузка прошлого моделирования
    else if ((key == 'L' || key == 'l') && !in_simulation && menu_option == LOAD_FROM_FILE)
    {
        printf("dont work now");

        // printf("write <filename>.txt to load it:\n");
        // char file[100];
        // if (!fgets(file, 100, stdin))
        // {
        //     printf("ERROR\n");
        // }
        // else
        // {
        //     file[strcspn(file, "\n")] = '\0';
        //     loadSimulation(file);
        // }
    }
    // обеспечение дтп на полосе
    else if ((key == 'A' || key == 'a') && in_simulation && type_simulation == HIGHWAY)
    {
        printf("dont work now");

        // accident_flag = !accident_flag;
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


// ВОТ ТУТ УЖЕ ИЗМЕНЕНИЯ
/* ПРЯМАЯ ДОРОГА */

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

void initHighwayCar()
{
    // Инициализируем полосы
    init_lanes(lines_count);

    for (int i = 0; i < 4; i++)
    {
        addRandomCar();
    }
}

void drawHighwayCar(CarNode car)
{
    // изменения только в рамках одной машины
    glPushMatrix();

    // позиция машины на полосе
    // float lane_pos = car.lane * LINE_WIDTH;

    float lane_pos;
    if (car.direction == RIGHT)
    {
        // Для движения направо - полосы сверху вниз: 1, 2, 3...
        lane_pos = (lines_count - car.lane + 1.0) * LINE_WIDTH;
    }
    else
    {
        // Для движения налево - полосы снизу вверх: -1, -2, -3...
        lane_pos = (-lines_count + car.lane - 1.0) * LINE_WIDTH;
    }

    // установка положения машины
    glTranslatef(car.position, lane_pos, 0);

    // поворот рисовки, если движение влево
    if (car.direction == LEFT)
    {
        glRotatef(180, 0, 1, 0);
    }

    // при перестроении линия
    if (car.is_changing_lane)
    {
        glColor3f(1.0, 0.5, 0.0);
        glBegin(GL_LINES);
        glVertex2f(0, 0);
        glVertex2f(0, (car.target_lane - car.lane) * LINE_WIDTH);
        glEnd();
    }

    // свет при торможении
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

    glutSwapBuffers();
}

void updateHighway(int value)
{
    // только если не на паузе
    if (track)
    {
        addRandomCar();
        update_all_cars();

        remove_cars_out_of_bounds(); 
        // ДОБАВЛЕНО: удаляем машины, которые уехали
    }

    glutPostRedisplay();
    glutTimerFunc(16, updateHighway, 0);
}


// // Новая функция с правильной сигнатурой
// void updateAdvancedCarsSingleLane(ListCar *head)
// {
//     if (!track)
//         return;

//     ListCar *current = head;
//     while (current != NULL)
//     {
//         CarNode *car = &current->car;

//         // Пропускаем машины в аварии
//         if (car->state == CAR_STATE_ACCIDENT)
//         {
//             current = current->next;
//             continue;
//         }

//         // Сначала проверяем столкновения
//         checkCollisionAvoidance(current);

//         // Затем решаем о перестроении (увеличена вероятность)
//         if (!car->is_changing_lane && rand() % 100 < 15)
//         { // УВЕЛИЧЕНА ВЕРОЯТНОСТЬ
//             decideLaneChange(current);
//         }

//         // Обработка перестроения
//         if (car->is_changing_lane)
//         {
//             // Плавное торможение перед перестроением
//             if (car->speed > car->target_speed)
//             {
//                 car->speed = fmax(car->target_speed, car->speed * 0.95);
//                 car->is_braking = true;
//             }

//             // Начинаем перестроение когда достигли безопасной скорости
//             if (car->speed <= car->target_speed * 1.05)
//             {
//                 car->lane_change_progress += LANE_CHANGE_SPEED;

//                 // Визуальный эффект перестроения - меняем позицию по Y
//                 float current_lane_pos, target_lane_pos;
//                 if (car->direction == RIGHT)
//                 {
//                     current_lane_pos = (lines_count - car->lane + 1.0) * LINE_WIDTH;
//                     target_lane_pos = (lines_count - car->target_lane + 1.0) * LINE_WIDTH;
//                 }
//                 else
//                 {
//                     current_lane_pos = (-lines_count + car->lane - 1.0) * LINE_WIDTH;
//                     target_lane_pos = (-lines_count + car->target_lane - 1.0) * LINE_WIDTH;
//                 }

//                 // Плавное перемещение между полосами (это только для визуализации)
//                 float interpolated_pos = current_lane_pos * (1 - car->lane_change_progress) +
//                                          target_lane_pos * car->lane_change_progress;

//                 // если завершили перестроение
//                 if (car->lane_change_progress >= 1.0)
//                 {
//                     // Перемещаем машину в новый список полосы
//                     int old_lane_index = get_lane_index(car->direction, car->lane);
//                     int new_lane_index = get_lane_index(car->direction, car->target_lane);

//                     // Сохраняем указатель на следующий элемент перед перемещением
//                     ListCar *next_car = current->next;

//                     // Удаляем из старой полосы и добавляем в новую
//                     move_car_to_new_lane(old_lane_index, new_lane_index, current);

//                     // Переходим к следующей машине в исходном списке
//                     current = next_car;
//                     continue;
//                 }
//             }
//         }

//         // Обновление позиции
//         car->position += car->speed * (car->direction == RIGHT ? 1.0f : -1.0f);

//         current = current->next;
//     }
// }

// // Обертка для существующего вызова
// void updateAdvancedCars(ListCar **head)
// {
//     updateAdvancedCarsSingleLane(*head);
// }

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
        if (!car->is_changing_lane && rand() % 100 < 15)
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

void addRandomCar()
{
    static float last_add_time = 0;
    float current_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;

    if (current_time - last_add_time < 3.0)
        return;

    // Проверяем общее количество машин
    int total_cars = 0;
    for (int i = 0; i < lane_count; i++)
    {
        total_cars += count_cars(lanes[i]);
    }

    if (total_cars > 50)
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

CarNode create_highway_car(CarDirection direction, char lane)
{
    CarNode car = {0}; // memset 0

    car.direction = direction;
    car.lane = lane;

    car.is_braking = false;
    // car.max_speed = MIN_SPEED + (rand() % (int)((MAX_SPEED - MIN_SPEED) * 100)) * 0.01;
    // car.speed = car.max_speed * 0.7;

    // Более разнообразные скорости для стимулирования перестроения
    car.max_speed = MIN_SPEED + (rand() % (int)((MAX_SPEED - MIN_SPEED) * 100)) * 0.01;
    car.speed = car.max_speed * (0.6 + (rand() % 40) * 0.01); // Разные начальные скорости

    car.color[0] = (50 + rand() % 50) / 100.0f; // R: 0.5-1.0 (исключает 0.0)
    car.color[1] = (rand() % 100) / 100.0f;     // G: 0.0-1.0
    car.color[2] = (rand() % 100) / 100.0f;     // B: 0.0-1.0

    // Инициализация новых полей
    car.is_changing_lane = false;
    car.target_lane = -1;
    car.lane_change_progress = 0.0;
    car.target_speed = car.max_speed;
    car.state = CAR_STATE_NORMAL;
    car.fixed_position = 0.0;


    if (car.direction == RIGHT)
    {
        car.position = -WINDOW_BORDER - 1 - (rand() % 10);
    }
    else
    {
        car.position = WINDOW_BORDER + 1 + (rand() % 10);
    }

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
    {
        *head = new;
    }
    else
    {
        ListCar *current = *head;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = new;
    }
}

// подсчет оптимальной скорости в торможении
float calculateSafeSpeed(CarNode car, float distance)
{
    // на основе дистанции мд машинами
    float safe_speed = car.max_speed * (distance / (SAFE_DISTANCE * 2));
    return fmax(MIN_SPEED, fmin(car.max_speed, safe_speed));
}



// проверка для предотвращения дтп
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