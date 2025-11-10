// реализация с списками для прямой дороги 1-5 полос в одну сторону

#include "head_test.h"
#include "head_list.h"


ListCar *lane_1 = NULL;
ListCar *lane_2 = NULL;
ListCar *lane_3 = NULL;
ListCar *lane_4 = NULL;
ListCar *lane_5 = NULL;

ListCar *lane_m1 = NULL;
ListCar *lane_m2 = NULL;
ListCar *lane_m3 = NULL;
ListCar *lane_m4 = NULL;
ListCar *lane_m5 = NULL;

/*
здесь чисто прямая дорога + нет аварий
чисто для проверки динамического списка

пока нет перестроений, потому что мне влом делать
*/

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


// ТУТ ПРОСТО ПРОВЕРИТЬ ВЫЗОВ ФУНКЦИЙ ПРЯМОЙ ДОРОГИ - КОГДА ВСТРАИВАТЬ БУДУ
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

    switch (lines_count)
    {
    case 1:
    {
        struct ListCar *current1 = lane_1;
        while (current1 != NULL)
        {
            drawHighwayCar(current1->car);
            current1 = current1->next;
        }

        struct ListCar *current4 = lane_m1;
        while (current4 != NULL)
        {
            drawHighwayCar(current4->car);
            current4 = current4->next;
        }
    }
    break;
    case 2:
    {
        struct ListCar *current1 = lane_1;
        while (current1 != NULL)
        {
            drawHighwayCar(current1->car);
            current1 = current1->next;
        }

        struct ListCar *current2 = lane_2;
        while (current2 != NULL)
        {
            drawHighwayCar(current2->car);
            current2 = current2->next;
        }

        struct ListCar *current4 = lane_m1;
        while (current4 != NULL)
        {
            drawHighwayCar(current4->car);
            current4 = current4->next;
        }

        struct ListCar *current5 = lane_m2;
        while (current5 != NULL)
        {
            drawHighwayCar(current5->car);
            current5 = current5->next;
        }
    }
    break;
    case 3:
    {
        struct ListCar *current1 = lane_1;
        while (current1 != NULL)
        {
            drawHighwayCar(current1->car);
            current1 = current1->next;
        }

        struct ListCar *current2 = lane_2;
        while (current2 != NULL)
        {
            drawHighwayCar(current2->car);
            current2 = current2->next;
        }

        struct ListCar *current3 = lane_3;
        while (current3 != NULL)
        {
            drawHighwayCar(current3->car);
            current3 = current3->next;
        }

        struct ListCar *current4 = lane_m1;
        while (current4 != NULL)
        {
            drawHighwayCar(current4->car);
            current4 = current4->next;
        }

        struct ListCar *current5 = lane_m2;
        while (current5 != NULL)
        {
            drawHighwayCar(current5->car);
            current5 = current5->next;
        }

        struct ListCar *current6 = lane_m3;
        while (current6 != NULL)
        {
            drawHighwayCar(current6->car);
            current6 = current6->next;
        }
    }
    break;
    case 4:
    {
        struct ListCar *current1 = lane_1;
        while (current1 != NULL)
        {
            drawHighwayCar(current1->car);
            current1 = current1->next;
        }

        struct ListCar *current2 = lane_2;
        while (current2 != NULL)
        {
            drawHighwayCar(current2->car);
            current2 = current2->next;
        }

        struct ListCar *current3 = lane_3;
        while (current3 != NULL)
        {
            drawHighwayCar(current3->car);
            current3 = current3->next;
        }

        struct ListCar *current3_1 = lane_4;
        while (current3_1 != NULL)
        {
            drawHighwayCar(current3_1->car);
            current3_1 = current3_1->next;
        }

        struct ListCar *current4 = lane_m1;
        while (current4 != NULL)
        {
            drawHighwayCar(current4->car);
            current4 = current4->next;
        }

        struct ListCar *current5 = lane_m2;
        while (current5 != NULL)
        {
            drawHighwayCar(current5->car);
            current5 = current5->next;
        }

        struct ListCar *current6 = lane_m3;
        while (current6 != NULL)
        {
            drawHighwayCar(current6->car);
            current6 = current6->next;
        }

        struct ListCar *current6_1 = lane_m4;
        while (current6_1 != NULL)
        {
            drawHighwayCar(current6_1->car);
            current6_1 = current6_1->next;
        }
    }
    break;
    case 5:
    {
        struct ListCar *current1 = lane_1;
        while (current1 != NULL)
        {
            drawHighwayCar(current1->car);
            current1 = current1->next;
        }

        struct ListCar *current2 = lane_2;
        while (current2 != NULL)
        {
            drawHighwayCar(current2->car);
            current2 = current2->next;
        }

        struct ListCar *current3 = lane_3;
        while (current3 != NULL)
        {
            drawHighwayCar(current3->car);
            current3 = current3->next;
        }

        struct ListCar *current3_1 = lane_4;
        while (current3_1 != NULL)
        {
            drawHighwayCar(current3_1->car);
            current3_1 = current3_1->next;
        }

        struct ListCar *current3_2 = lane_5;
        while (current3_2 != NULL)
        {
            drawHighwayCar(current3_2->car);
            current3_2 = current3_2->next;
        }

        struct ListCar *current4 = lane_m1;
        while (current4 != NULL)
        {
            drawHighwayCar(current4->car);
            current4 = current4->next;
        }

        struct ListCar *current5 = lane_m2;
        while (current5 != NULL)
        {
            drawHighwayCar(current5->car);
            current5 = current5->next;
        }

        struct ListCar *current6 = lane_m3;
        while (current6 != NULL)
        {
            drawHighwayCar(current6->car);
            current6 = current6->next;
        }

        struct ListCar *current6_1 = lane_m4;
        while (current6_1 != NULL)
        {
            drawHighwayCar(current6_1->car);
            current6_1 = current6_1->next;
        }

        struct ListCar *current6_2 = lane_m5;
        while (current6_2 != NULL)
        {
            drawHighwayCar(current6_2->car);
            current6_2 = current6_2->next;
        }
    }
    break;

    default:
        break;
    }

    glutSwapBuffers();
}

void updateHighway(int value)
{
    // только если не на паузе
    if (track)
    {
        addRandomCar();

        switch (lines_count)
        {
        case 1:
        {
            updateAdvancedCars(lane_1);
            updateAdvancedCars(lane_m1);
        }
        break;
        case 2:
        {
            updateAdvancedCars(lane_1);
            updateAdvancedCars(lane_2);

            updateAdvancedCars(lane_m1);
            updateAdvancedCars(lane_m2);
        }
        break;
        case 3:
        {
            updateAdvancedCars(lane_1);
            updateAdvancedCars(lane_2);
            updateAdvancedCars(lane_3);

            updateAdvancedCars(lane_m1);
            updateAdvancedCars(lane_m2);
            updateAdvancedCars(lane_m3);
        }
        break;
        case 4:
        {
            updateAdvancedCars(lane_1);
            updateAdvancedCars(lane_2);
            updateAdvancedCars(lane_3);
            updateAdvancedCars(lane_4);

            updateAdvancedCars(lane_m1);
            updateAdvancedCars(lane_m2);
            updateAdvancedCars(lane_m3);
            updateAdvancedCars(lane_m4);
        }
        break;
        case 5:
        {
            updateAdvancedCars(lane_1);
            updateAdvancedCars(lane_2);
            updateAdvancedCars(lane_3);
            updateAdvancedCars(lane_4);
            updateAdvancedCars(lane_5);

            updateAdvancedCars(lane_m1);
            updateAdvancedCars(lane_m2);
            updateAdvancedCars(lane_m3);
            updateAdvancedCars(lane_m4);
            updateAdvancedCars(lane_m5);
        }
        break;
        default: break;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, updateHighway, 0);
}

void updateAdvancedCars(ListCar* head)
{
    if (!track)
        return;

    struct ListCar *current = head;
    while (current != NULL)
    {
        checkCollisionAvoidance(current);

        current->car.position += current->car.speed * (current->car.direction == RIGHT ? 1.0f : -1.0f);
        // Если машина выехала за пределы дороги
        if ((current->car.direction == RIGHT && current->car.position > WINDOW_BORDER) ||
            (current->car.direction == LEFT && current->car.position < -WINDOW_BORDER))
        {
            if (current->car.direction == RIGHT)
            {
                current->car.position = -WINDOW_BORDER - 1;
            }
            else
            {
                current->car.position = WINDOW_BORDER + 1;
            }

            current->car.speed = current->car.max_speed * 0.7;
        }

        current = current->next;
    }
}

void addRandomCar()
{
    static float last_add_time = 0;
    float current_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;

    // Проверяем временной интервал
    if (current_time - last_add_time < 2.0)
        return;

    last_add_time = current_time;

    // Добавляем машины на все доступные полосы

    switch (lines_count)
    {
    case 1:
    {
        if (count_cars(lane_1) < MAX_LANE_CAR)
        {
            CarNode new_car = create_highway_car(RIGHT, 1);
            insert_car(&lane_1, new_car);
        }
        if (count_cars(lane_m1) < MAX_LANE_CAR)
        {
            CarNode new_car = create_highway_car(LEFT, 1);
            insert_car(&lane_m1, new_car);
        }
    } 
    break;
    case 2:
    {
        if (count_cars(lane_1) < MAX_LANE_CAR)
        {
            CarNode new_car = create_highway_car(RIGHT, 1);
            insert_car(&lane_1, new_car);
        }

        if (count_cars(lane_2) < MAX_LANE_CAR)
        {
            CarNode new_car = create_highway_car(RIGHT, 2);
            insert_car(&lane_2, new_car);
        }

        if (count_cars(lane_m1) < MAX_LANE_CAR)
        {
            CarNode new_car = create_highway_car(LEFT, 1);
            insert_car(&lane_m1, new_car);
        }

        if (count_cars(lane_m2) < MAX_LANE_CAR)
        {
            CarNode new_car = create_highway_car(LEFT, 2);
            insert_car(&lane_m2, new_car);
        }
    }
    break;
    case 3:
    {
        if (count_cars(lane_1) < MAX_LANE_CAR)
        {
            CarNode new_car = create_highway_car(RIGHT, 1);
            insert_car(&lane_1, new_car);
        }

        if (count_cars(lane_2) < MAX_LANE_CAR)
        {
            CarNode new_car = create_highway_car(RIGHT, 2);
            insert_car(&lane_2, new_car);
        }

        if (count_cars(lane_3) < MAX_LANE_CAR)
        {
            CarNode new_car = create_highway_car(RIGHT, 3);
            insert_car(&lane_3, new_car);
        }

        if (count_cars(lane_m1) < MAX_LANE_CAR)
        {
            CarNode new_car = create_highway_car(LEFT, 1);
            insert_car(&lane_m1, new_car);
        }

        if (count_cars(lane_m2) < MAX_LANE_CAR)
        {
            CarNode new_car = create_highway_car(LEFT, 2);
            insert_car(&lane_m2, new_car);
        }

        if (count_cars(lane_m3) < MAX_LANE_CAR)
        {
            CarNode new_car = create_highway_car(LEFT, 3);
            insert_car(&lane_m3, new_car);
        }
    }
    break;
    case 4:
    {
        if (count_cars(lane_1) < MAX_LANE_CAR)
        {
            CarNode new_car = create_highway_car(RIGHT, 1);
            insert_car(&lane_1, new_car);
        }

        if (count_cars(lane_2) < MAX_LANE_CAR)
        {
            CarNode new_car = create_highway_car(RIGHT, 2);
            insert_car(&lane_2, new_car);
        }

        if (count_cars(lane_3) < MAX_LANE_CAR)
        {
            CarNode new_car = create_highway_car(RIGHT, 3);
            insert_car(&lane_3, new_car);
        }

        if (count_cars(lane_4) < MAX_LANE_CAR)
        {
            CarNode new_car = create_highway_car(RIGHT, 4);
            insert_car(&lane_4, new_car);
        }

        if (count_cars(lane_m1) < MAX_LANE_CAR)
        {
            CarNode new_car = create_highway_car(LEFT, 1);
            insert_car(&lane_m1, new_car);
        }

        if (count_cars(lane_m2) < MAX_LANE_CAR)
        {
            CarNode new_car = create_highway_car(LEFT, 2);
            insert_car(&lane_m2, new_car);
        }

        if (count_cars(lane_m3) < MAX_LANE_CAR)
        {
            CarNode new_car = create_highway_car(LEFT, 3);
            insert_car(&lane_m3, new_car);
        }

        if (count_cars(lane_m4) < MAX_LANE_CAR)
        {
            CarNode new_car = create_highway_car(LEFT, 4);
            insert_car(&lane_m4, new_car);
        }
    }
    break;
    case 5:
    {
        if (count_cars(lane_1) < MAX_LANE_CAR)
        {
            CarNode new_car = create_highway_car(RIGHT, 1);
            insert_car(&lane_1, new_car);
        }

        if (count_cars(lane_2) < MAX_LANE_CAR)
        {
            CarNode new_car = create_highway_car(RIGHT, 2);
            insert_car(&lane_2, new_car);
        }

        if (count_cars(lane_3) < MAX_LANE_CAR)
        {
            CarNode new_car = create_highway_car(RIGHT, 3);
            insert_car(&lane_3, new_car);
        }

        if (count_cars(lane_4) < MAX_LANE_CAR)
        {
            CarNode new_car = create_highway_car(RIGHT, 4);
            insert_car(&lane_4, new_car);
        }

        if (count_cars(lane_5) < MAX_LANE_CAR)
        {
            CarNode new_car = create_highway_car(RIGHT, 5);
            insert_car(&lane_5, new_car);
        }

        if (count_cars(lane_m1) < MAX_LANE_CAR)
        {
            CarNode new_car = create_highway_car(LEFT, 1);
            insert_car(&lane_m1, new_car);
        }

        if (count_cars(lane_m2) < MAX_LANE_CAR)
        {
            CarNode new_car = create_highway_car(LEFT, 2);
            insert_car(&lane_m2, new_car);
        }

        if (count_cars(lane_m3) < MAX_LANE_CAR)
        {
            CarNode new_car = create_highway_car(LEFT, 3);
            insert_car(&lane_m3, new_car);
        }

        if (count_cars(lane_m4) < MAX_LANE_CAR)
        {
            CarNode new_car = create_highway_car(LEFT, 4);
            insert_car(&lane_m4, new_car);
        }

        if (count_cars(lane_m5) < MAX_LANE_CAR)
        {
            CarNode new_car = create_highway_car(LEFT, 5);
            insert_car(&lane_m5, new_car);
        }
    }
    break;
    default: break;
    }
    
}

CarNode create_highway_car(CarDirection direction, char lane)
{
    CarNode car = {0}; // memset 0

    car.direction = direction;
    car.lane = lane;

    car.is_braking = false;
    car.max_speed = MIN_SPEED + (rand() % (int)((MAX_SPEED - MIN_SPEED) * 100)) * 0.01;
    car.speed = car.max_speed * 0.5;

    car.color[0] = (50 + rand() % 50) / 100.0f; // R: 0.5-1.0 (исключает 0.0)
    car.color[1] = (rand() % 100) / 100.0f;     // G: 0.0-1.0
    car.color[2] = (rand() % 100) / 100.0f;     // B: 0.0-1.0

    if (car.direction == RIGHT)
    {
        car.position = -WINDOW_BORDER - 1 - (rand() % 10);
    }
    else
    {
        car.position = WINDOW_BORDER + 1 + (rand() % 10);
        // car.lane = -car.lane;
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

/*
ВОТ ТУТ ИЗМЕНИТЬ НА СПИСОК + ВСТРОИТЬ
*/

// подсчет оптимальной скорости в торможении
float calculateSafeSpeed(CarNode car, float distance)
{
    // на основе дистанции мд машинами
    float safe_speed = car.max_speed * (distance / (SAFE_DISTANCE * 2));
    return fmax(MIN_SPEED, fmin(car.max_speed, safe_speed));
}



// проверка для предотвращения дтп
 // ЗДЕСЬ НАДО ЛИСТ_КАР ПЕРЕДАВАТЬ, ПОТОМУ ЧТО ПРОСМОТР В ТЕКУЩЕЙ ПОЛОСЕ
void checkCollisionAvoidance(ListCar* current)
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
            {
                current->car.speed = fmax(safe_speed, current->car.speed * BRAKING_FACTOR);
            }

            // Если очень близко - экстренное торможение
            if (path_distance < SAFE_DISTANCE * 0.7)
            {
                current->car.speed *= 0.8;
            }
        }
    }

    // Плавное ускорение, если нет препятствий
    if (!current->car.is_braking && current->car.speed < current->car.max_speed)
    {
        current->car.speed += ACCELERATION;
        current->car.speed = fmin(current->car.speed, current->car.max_speed);
    }
}