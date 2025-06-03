/**
 * @file checkers.c
 * @brief Программа для игры в шашки между человеком и компьютером
 * 
 * Программа реализует классическую игру в шашки с консольным интерфейсом.
 * Поддерживаются стандартные правила игры, включая превращение в дамки и обязательное взятие.
 */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 35                       /**< Ширина игрового поля в символах */
#define BOARD_SIZE 18                 /**< Высота игрового поля в символах */
#define MAX_MOVES 12                  /**< Максимальное количество возможных ходов для анализа */


/**
 * @struct GameState
 * @brief Состояние игры для отслеживания количества фишек
 */

// Структура нужна для отслеживания конца игры
typedef struct
{
  int count_white;         /**< Количество белых фишек */
  int count_black;         /**< Количество черных фишек */
  int count_white_king;    /**< Количество белых дамок */
  int count_black_king;    /**< Количество черных дамок */
} GameState;

/**
 * @struct Position
 * @brief Координаты фишки на поле
 */
typedef struct
{
  short x;                  /**< Ряд (A,B,C) на графическом поле */
  short y;                  /**< Строка (1,2,3) на графическом поле */
  short x_8;                /**< Координата x на логическом поле (0-7) */
  short y_8;                /**< Координата y на логическом поле (0-7) */
  bool damka;               /**< Флаг, является ли фишка дамкой */
} Position;


/**
 * @struct Valid_Hod
 * @brief Валидные ходы для фишки
 */
typedef struct
{
  bool l_h;     /**< Ход слева-вверх */
  short lh_y;   /**< Координата y для хода слева-вверх */
  short lh_x;   /**< Координата x для хода слева-вверх */
  bool r_h;     /**< Ход справа-вверх */
  short rh_y;   /**< Координата y для хода справа-вверх */
  short rh_x;   /**< Координата x для хода справа-вверх */
  bool l_s;     /**< Ход слева-вниз */
  short ls_y;   /**< Координата y для хода слева-вниз */
  short ls_x;   /**< Координата x для хода слева-вниз */
  bool r_s;     /**< Ход справа-вниз */
  short rs_y;   /**< Координата y для хода справа-вниз */
  short rs_x;   /**< Координата x для хода справа-вниз */
} Valid_Hod;

/**
 * @struct Valid_Kill
 * @brief Валидные взятия для фишки
 */
typedef struct {
    bool kill_l_h;   /**< Взятие слева-вверх */
    short kill_lh_y; /**< Координата y для взятия слева-вверх */
    short kill_lh_x; /**< Координата x для взятия слева-вверх */
    bool kill_r_h;   /**< Взятие справа-вверх */
    short kill_rh_y; /**< Координата y для взятия справа-вверх */
    short kill_rh_x; /**< Координата x для взятия справа-вверх */
    bool kill_l_s;   /**< Взятие слева-вниз */
    short kill_ls_y; /**< Координата y для взятия слева-вниз */
    short kill_ls_x; /**< Координата x для взятия слева-вниз */
    bool kill_r_s;   /**< Взятие справа-вниз */
    short kill_rs_y; /**< Координата y для взятия справа-вниз */
    short kill_rs_x; /**< Координата x для взятия справа-вниз */
} Valid_Kill;

bool is_player_turn = false;           // Флаг, ход игрока
bool player_is_white = false;          // Флаг, игрок играет за белых
char player_piece;                     // Фишка игрока
char computer_piece;                   // Фишка компьютера
GameState game_state = {12, 12, 0, 0}; // Состояние поля

// Функции
/**
 * @brief Преобразует логические координаты в графические символы
 * @param i_x Логическая координата x (0-7)
 * @param i_y Логическая координата y (0-7)
 * @param[out] x Символьное обозначение столбца (A-H)
 * @param[out] y Символьное обозначение строки (1-8)
 * @return true если преобразование успешно, false в противном случае
 */
bool reverse_graph_out_koordinaty(short i_x, short i_y, char *x, char *y);

/**
 * @brief Очищает фишку на графическом поле
 * @param board Игровое поле
 * @param x Координата x на графическом поле
 * @param y Координата y на графическом поле
 */
void wash_piece(char board[BOARD_SIZE][SIZE + 1], short x, short y);

/**
 * @brief Преобразует логические координаты в графические
 * @param i_x Логическая координата x (0-7)
 * @param i_y Логическая координата y (0-7)
 * @param[out] x Графическая координата x
 * @param[out] y Графическая координата y
 */
void reverse_graph_koordinaty(short i_x, short i_y, short *x, short *y);

/**
 * @brief Получает возможные взятия для фишки
 * @param where Позиция фишки
 * @param[out] velian Структура с возможными взятиями
 * @param lodic Логическое представление доски
 * @return Количество возможных взятий
 */
int get_valid_kill(Position where, Valid_Kill *velian, char lodic[8][8]);

/**
 * @brief Получает возможные ходы для фишки
 * @param pos Позиция фишки
 * @param[out] moves Структура с возможными ходами
 * @param lodic Логическое представление доски
 * @return Количество возможных ходов
 */
int get_valid_moves(Position pos, Valid_Hod *moves, char lodic[8][8]);

/**
 * @brief Инициализирует начальное расположение фишек
 * @param board Игровое поле
 * @param white_on_bottom Флаг, белые фишки внизу
 */
void setup_pieces(char board[BOARD_SIZE][SIZE + 1], bool white_on_bottom);

/**
 * @brief Инициализирует пустое игровое поле
 * @param board Игровое поле
 */
void initialize_board(char board[BOARD_SIZE][SIZE + 1]);

/**
 * @brief Выводит игровое поле в консоль
 * @param board Игровое поле
 */
void print_board(const char board[BOARD_SIZE][SIZE + 1]);

/**
 * @brief Основной игровой цикл
 * @param board Игровое поле
 */
void play_game(char board[BOARD_SIZE][SIZE + 1]);

/**
 * @brief Подсвечивает выбранную фишку
 * @param x Координата x на графическом поле
 * @param y Координата y на графическом поле
 * @param board Игровое поле
 */
void highlight_piece(short x, short y, char board[BOARD_SIZE][SIZE + 1]);

/**
 * @brief Выводит информацию о текущем ходе
 */
void print_turn();

/**
 * @brief Проверяет условие окончания игры
 * @return true если игра окончена, false в противном случае
 */
bool check_game_over();

/**
 * @brief Меняет текущего игрока
 */
void switch_turn();

/**
 * @brief Обрабатывает ход игрока
 * @param board Игровое поле
 * @return true если ход выполнен успешно, false в противном случае
 */
bool player_move(char board[BOARD_SIZE][SIZE + 1]);

/**
 * @brief Преобразует символьные координаты в числовые
 * @param x Символьное обозначение столбца (A-H)
 * @param y Символьное обозначение строки (1-8)
 * @param[out] i_x Графическая координата x
 * @param[out] i_y Графическая координата y
 * @param[out] i_x_8 Логическая координата x (0-7)
 * @param[out] i_y_8 Логическая координата y (0-7)
 * @return true если преобразование успешно, false в противном случае
 */
bool koordinaty(char x, char y, short *i_x, short *i_y, short *i_x_8, short *i_y_8);

/**
 * @brief Перемещает фишку на новую позицию
 * @param where Текущая позиция фишки
 * @param x Новая координата x
 * @param y Новая координата y
 */
void Hod(Position *where, int x, int y);

/**
 * @brief Подсвечивает возможные ходы
 * @param board Игровое поле
 * @param x Координата x на графическом поле
 * @param y Координата y на графическом поле
 * @param not_chose Флаг выбора
 */
void light(char board[BOARD_SIZE][SIZE + 1], short x, short y, bool not_chose);

/**
 * @brief Превращает фишку в дамку при достижении последней линии
 * @param where Позиция фишки
 * @param lodic Логическое представление доски
 */
void becameQueen(Position where, char lodic[8][8]);

/**
 * @brief Обрабатывает ход компьютера
 * @param board Игровое поле
 * @return true если ход выполнен успешно, false в противном случае
 */
bool computer_move(char board[BOARD_SIZE][SIZE + 1]);

/**
 * @brief Оценивает текущее состояние доски для компьютера
 * @param lodic Логическое представление доски
 * @param is_white Флаг, играет ли компьютер белыми
 * @return Оценка состояния доски
 */
int evaluate_board_pc(char lodic[8][8], bool is_white);

/**
 * @brief Находит лучшую последовательность взятий
 * @param pos Текущая позиция
 * @param vks Возможные взятия
 * @param vks_count Количество возможных взятий
 * @param mx_score Текущая максимальная оценка
 * @param lodic Логическое представление доски
 * @param[out] best_lodic Лучшее состояние доски
 * @param[out] best_game_state Лучшее состояние игры
 * @return Максимальная оценка
 */
int get_best_sequent_kills(Position pos, Valid_Kill vks, int vks_count, int mx_score, char lodic[8][8], char best_lodic[8][8], GameState *best_game_state);

/**
 * @brief Вычисляет возможные ходы со взятием
 * @param pos Текущая позиция
 * @param lodic Логическое представление доски
 * @param[out] move_i Количество возможных ходов
 * @param[out] move_buffer Буфер для хранения возможных ходов
 * @param[out] lodic_buffer Буфер для состояний доски
 * @param[out] game_states Буфер для состояний игры
 */
void calculate_kill_moves(Position pos, char lodic[8][8], int *move_i, short move_buffer[10][2], char lodic_buffer[10][8][8], GameState *game_states);

/**
 * @brief Главная функция программы
 * @return Код завершения программы
 */
int main();
char lodic[8][8];

char board[BOARD_SIZE][SIZE + 1] = { // Интерфейс поля
    {'+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', ' ', ' '},
    {'|', ' ', ' ', ' ', '|', ' ', '*', ' ', '|', ' ', ' ', ' ', '|', ' ', '*', ' ', '|', ' ', ' ', ' ', '|', ' ', '*', ' ', '|', ' ', ' ', ' ', '|', ' ', '*', ' ', '|', ' ', '8'},
    {'+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', ' ', ' '},
    {'|', ' ', '*', ' ', '|', ' ', ' ', ' ', '|', ' ', '*', ' ', '|', ' ', ' ', ' ', '|', ' ', '*', ' ', '|', ' ', ' ', ' ', '|', ' ', '*', ' ', '|', ' ', ' ', ' ', '|', ' ', '7'},
    {'+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', ' ', ' '},
    {'|', ' ', ' ', ' ', '|', ' ', '*', ' ', '|', ' ', ' ', ' ', '|', ' ', '*', ' ', '|', ' ', ' ', ' ', '|', ' ', '*', ' ', '|', ' ', ' ', ' ', '|', ' ', '*', ' ', '|', ' ', '6'},
    {'+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', ' ', ' '},
    {'|', ' ', '*', ' ', '|', ' ', ' ', ' ', '|', ' ', '*', ' ', '|', ' ', ' ', ' ', '|', ' ', '*', ' ', '|', ' ', ' ', ' ', '|', ' ', '*', ' ', '|', ' ', ' ', ' ', '|', ' ', '5'},
    {'+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', ' ', ' '},
    {'|', ' ', ' ', ' ', '|', ' ', '*', ' ', '|', ' ', ' ', ' ', '|', ' ', '*', ' ', '|', ' ', ' ', ' ', '|', ' ', '*', ' ', '|', ' ', ' ', ' ', '|', ' ', '*', ' ', '|', ' ', '4'},
    {'+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', ' ', ' '},
    {'|', ' ', '*', ' ', '|', ' ', ' ', ' ', '|', ' ', '*', ' ', '|', ' ', ' ', ' ', '|', ' ', '*', ' ', '|', ' ', ' ', ' ', '|', ' ', '*', ' ', '|', ' ', ' ', ' ', '|', ' ', '3'},
    {'+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', ' ', ' '},
    {'|', ' ', ' ', ' ', '|', ' ', '*', ' ', '|', ' ', ' ', ' ', '|', ' ', '*', ' ', '|', ' ', ' ', ' ', '|', ' ', '*', ' ', '|', ' ', ' ', ' ', '|', ' ', '*', ' ', '|', ' ', '2'},
    {'+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', ' ', ' '},
    {'|', ' ', '*', ' ', '|', ' ', ' ', ' ', '|', ' ', '*', ' ', '|', ' ', ' ', ' ', '|', ' ', '*', ' ', '|', ' ', ' ', ' ', '|', ' ', '*', ' ', '|', ' ', ' ', ' ', '|', ' ', '1'},
    {'+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', '-', '-', '-', '+', ' ', ' '},
    {' ', ' ', 'A', ' ', ' ', ' ', 'B', ' ', ' ', ' ', 'C', ' ', ' ', ' ', 'D', ' ', ' ', ' ', 'E', ' ', ' ', ' ', 'F', ' ', ' ', ' ', 'G', ' ', ' ', ' ', 'H', ' ', ' ', ' ', ' '}};

// Функция для начала игры
int main()
{
  char choice[10];
  bool valid_choice = false; // Флаг поднимаеться когда игрок выберает цвет фишек

  printf("\nДобро пожаловать в игру шашки!\n"); // Преветсвие

  while (!valid_choice)
  {
    printf("\nВыберите цвет фишек:\n");
    printf("1. White (белые)\n");
    printf("2. Black (черные)\n");
    printf("Ваш выбор: ");

    if (fgets(choice, sizeof(choice), stdin) == NULL)
    {
      printf("Ошибка ввода!\n");
      continue;
    }

    // Удаляем символ перевода строки
    choice[strcspn(choice, "\n")] = '\0';

    // Приводим к нижнему регистру для сравнения
    for (int i = 0; choice[i]; i++)
    {
      choice[i] = tolower(choice[i]);
    }

    // initialize_board(board); // Создаем доску

    if (strcmp(choice, "white") == 0)
    {
      setup_pieces(board, true);
      printf("\nВы выбрали белые фишки (O). Вы ходите первым.\n");
      player_is_white = true;
      player_piece = 'O';
      computer_piece = '0';
      is_player_turn = true;
      valid_choice = true;
    }
    else if (strcmp(choice, "black") == 0)
    {
      setup_pieces(board, false);
      printf("\nВы выбрали черные фишки (0). Компьютер ходит первым.\n");
      player_is_white = false;
      player_piece = '0';
      computer_piece = 'O';
      is_player_turn = false;
      valid_choice = true;
    }
    else
      printf("Некорректный ввод. Пожалуйста, введите 'White' или 'Black'.\n");
  }

  print_board(board);

  play_game(board);

  return 0;
}

void initialize_board(char board[BOARD_SIZE][SIZE + 1])
{ // Инициализация поля
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    for (int j = 0; j < SIZE; j++)
    {
      if (board[i][j] == '*' || board[i][j] == 'O' || board[i][j] == '0' ||
          board[i][j] == 'Q' || board[i][j] == 'K')
      {
        board[i][j] = '*';
      }
    }
  }
}

void print_board(const char board[BOARD_SIZE][SIZE + 1])
{ // Печать интерфейса поля
  printf("\n");
  printf("0 - Фишка черного игрока\n");
  printf("O - Фишка белого игрока\n");
  printf("W, B - Дамки\n");
  printf("Текущее состояние доски:\n");
  printf("Белые: %d (%d дамок), Черные: %d (%d дамок)\n", game_state.count_white, game_state.count_white_king,
         game_state.count_black, game_state.count_black_king);

  for (int i = 0; i < BOARD_SIZE; i++)
  {
    for (int j = 0; j < SIZE; j++)
      printf("%c", board[i][j]);
    printf("\n");
  }
}

void setup_pieces(char board[BOARD_SIZE][SIZE + 1], bool white_on_bottom)
{ // Располежение поля в стороны игрока
  if (white_on_bottom)
  {
    for (int i = 11; i < BOARD_SIZE - 1; i += 2)
    {
      for (int j = 2; j < SIZE - 2; j += 8)
      {
        if (i % 4 == 1)
        {
          if (j + 4 < SIZE - 2)
            board[i][j + 4] = 'O';
        }
        else
          board[i][j] = 'O';
      }
    }

    for (int i = 1; i < 6; i += 2)
    {
      for (int j = 2; j < SIZE - 2; j += 8)
      {
        if (i % 4 == 1)
        {
          if (j + 4 < SIZE - 2)
            board[i][j + 4] = '0';
        }
        else
          board[i][j] = '0';
      }
    }
  }
  else
  {
    for (int i = 11; i < BOARD_SIZE - 1; i += 2)
    {
      for (int j = 2; j < SIZE - 2; j += 8)
      {
        if (i % 4 == 1)
        {
          if (j + 4 < SIZE - 2)
            board[i][j + 4] = '0';
        }
        else
          board[i][j] = '0';
      }
    }

    for (int i = 1; i <= 6; i += 2)
    {
      for (int j = 2; j < SIZE - 2; j += 8)
      {
        if (i % 4 == 1)
        {
          if (j + 4 < SIZE - 2)
            board[i][j + 4] = 'O';
        }
        else
          board[i][j] = 'O';
      }
    }
  }
}

void play_game(char board[BOARD_SIZE][SIZE + 1])
{
  bool has_moves = true;
  while (true)
  {
    print_turn();

    if (check_game_over())
      break;

    if (is_player_turn)
      has_moves = player_move(board);

    else
      has_moves = computer_move(board);
    if (!has_moves) break;

    switch_turn();
    for (short x = 0; x < 8; x++)
      for (short y = 0; y < 8; y++)
      {
        short bx, by;
        reverse_graph_koordinaty(x, y, &bx, &by);
        if (player_is_white)
          board[by][bx] = (lodic[y][x] == '0' ? '*' : (lodic[y][x] == '1' ? '0' : (lodic[y][x] == '2' ? 'O' : (lodic[y][x] == '3' ? 'B' : (lodic[y][x] == '4' ? 'W' : ' ')))));
        else
          board[by][bx] = (lodic[y][x] == '0' ? '*' : (lodic[y][x] == '1' ? 'O' : (lodic[y][x] == '2' ? '0' : (lodic[y][x] == '3' ? 'W' : (lodic[y][x] == '4' ? 'B' : ' ')))));
      }
    print_board(board);
  }
  printf("Конец. Парам-парам-пам");
}

bool player_move(char board[BOARD_SIZE][SIZE + 1])
{
  printf("\nВаш ход. Введите координаты фишки (например, B3): ");

  char x, y;
  Position where;
  Valid_Hod motion;
  Valid_Kill veliam;

  short can_kill[12][2];
  int can_kill_i = 0;
  bool no_moves = true;
  memset(can_kill, 0, 12 * 2 * sizeof(short));
  for (int y = 0; y < 8; y++)
    for (int x = y % 2 == 0; x < 8; x += 2)
    {
      if (lodic[y][x] != '2' && lodic[y][x] != '4')
        continue;
      Position pos;
      pos.x_8 = x;
      pos.y_8 = y;
      reverse_graph_koordinaty(x, y, &pos.x, &pos.y);
      Valid_Kill vks;
      int vks_count = get_valid_kill(pos, &vks, lodic);
      int vhs_count = get_valid_moves(pos, &motion, lodic);
      if (vhs_count > 0)
        no_moves = false;
      if (vks_count > 0)
      {
        no_moves = false;
        can_kill[can_kill_i][0] = x;
        can_kill[can_kill_i][1] = y;
        can_kill_i++;
      }
    }
  int killer_i = -1;
  if (no_moves) return false;
  // Ввод координат фишки
  while (true)
  {
    if (scanf(" %c%c", &x, &y) != 2)
    {
      printf("Ошибка ввода. Попробуйте еще раз: ");
      while (getchar() != '\n')
        ;
      continue;
    }

    x = toupper(x);

    if (!koordinaty(x, y, &where.x, &where.y, &where.x_8, &where.y_8))
    {
      printf("Неверные координаты! Попробуйте еще раз: ");
      continue;
    }

    char piece = board[where.y][where.x];
    if ((player_is_white && (piece != 'O' && piece != 'W')) ||
        (!player_is_white && (piece != '0' && piece != 'B')))
    {
      printf("Это не ваша фишка! Попробуйте еще раз: ");
      continue;
    }

    if (can_kill_i > 0)
    {
      bool correct_pos = false;
      for (int i = 0; i <= can_kill_i; i++)
        if (can_kill[i][0] == where.x_8 && can_kill[i][1] == where.y_8)
        {
          killer_i = i;
          correct_pos = true;
          break;
        }
      if (!correct_pos)
      {
        printf("Вы обязаны рубить! Пожалуйста, выберите фишку, которая рубит фишку протвиника в этом ходу\n");
        continue;
      }
    }

    highlight_piece(where.x, where.y, board);

    if (killer_i != -1)
    {
      Position pos;
      pos.x_8 = can_kill[killer_i][0];
      pos.y_8 = can_kill[killer_i][1];
      reverse_graph_koordinaty(can_kill[killer_i][0], can_kill[killer_i][1], &pos.x, &pos.y);
      short available_kills[10][2];
      int move_i = 0;
      char lodic_buffer[10][8][8];
      GameState game_states[10];
      calculate_kill_moves(pos, lodic, &move_i, available_kills, lodic_buffer, game_states);
      int kol = 1;
      char out_x = 0, out_y = 0;
      short big_x = 0, big_y = 0;
      for (int i = 0; i < move_i; i++)
      {
        reverse_graph_koordinaty(available_kills[i][0], available_kills[i][1], &big_x, &big_y);
        light(board, big_x, big_y, true);
      }
      highlight_piece(pos.x, pos.y, board);
      for (int i = 0; i < move_i; i++)
      {
        reverse_graph_out_koordinaty(available_kills[i][0], available_kills[i][1], &out_x, &out_y);
        printf("%d. %c%c\n", kol, out_x, out_y);
        kol++;
        reverse_graph_koordinaty(available_kills[i][0], available_kills[i][1], &big_x, &big_y);
        light(board, big_x, big_y, false);
      }
      int vsbor = 1;
      while (true)
      {
        printf("Введите номер хода:\n");
        scanf("%d", &vsbor);
        if (vsbor < 1 || vsbor > move_i)
        {
          printf("Ошибка ввода");
          continue;
        }
        break;
      }
      where.x_8 = available_kills[vsbor - 1][0];
      where.y_8 = available_kills[vsbor - 1][1];
      reverse_graph_koordinaty(where.x_8, where.y_8, &where.x, &where.y);
      memcpy(lodic, lodic_buffer[vsbor - 1], 8 * 8 * sizeof(char));
      game_state = game_states[vsbor - 1];
    }
    else
    {
      int sum = get_valid_moves(where, &motion, lodic);
      if (get_valid_moves(where, &motion, lodic) > 0) 
      {
        printf("\nМожно походить в:\n");

        short go_x = 0, go_y = 0;
        int output[4][2] = {0};
        int off[4][2] = {0};
        short id = -1;

        bool not_chose = true;

        if (motion.l_h == true)
        {
          id++;
          reverse_graph_koordinaty(motion.lh_x, motion.lh_y, &go_x, &go_y);
          light(board, go_x, go_y, not_chose);
          output[id][0] = motion.lh_x;
          output[id][1] = motion.lh_y;
          off[0][0] = go_x;
          off[0][1] = go_y;
        }

        if (motion.r_h == true)
        {
          id++;
          reverse_graph_koordinaty(motion.rh_x, motion.rh_y, &go_x, &go_y);
          light(board, go_x, go_y, not_chose);
          output[id][0] = motion.rh_x;
          output[id][1] = motion.rh_y;
          off[1][0] = go_x;
          off[1][1] = go_y;
        }

        if (motion.l_s == true)
        {
          id++;
          reverse_graph_koordinaty(motion.ls_x, motion.ls_y, &go_x, &go_y);
          light(board, go_x, go_y, not_chose);
          output[id][0] = motion.ls_x;
          output[id][1] = motion.ls_y;
          off[2][0] = go_x;
          off[2][1] = go_y;
        }

        if (motion.r_s == true)
        {
          id++;
          reverse_graph_koordinaty(motion.rs_x, motion.rs_y, &go_x, &go_y);
          light(board, go_x, go_y, not_chose);
          output[id][0] = motion.rs_x;
          output[id][1] = motion.rs_y;
          off[3][0] = go_x;
          off[3][1] = go_y;
        }

        highlight_piece(where.x, where.y, board);

        int kol = 1;
        char out_x = 0, out_y = 0;
        for (int i = 0; i < sum; i++)
        {
          reverse_graph_out_koordinaty(output[i][0], output[i][1], &out_x, &out_y);
          printf("%d. %c%c\n", kol, out_x, out_y);
          kol++;
          not_chose = false;
        }
        for (int i = 0; i < 4; i++)
        {
          light(board, off[i][0], off[i][1], not_chose);
        }
        int vsbor = 1;

        while (true)
        {
          printf("Введите номер хода:\n");
          scanf("%d", &vsbor);
          if (vsbor < 1 || vsbor >= kol)
          {
            printf("Ошибка ввода\n");
            continue;
          }
          break;
        }
        int hod_x = output[vsbor - 1][0];
        int hod_y = output[vsbor - 1][1];
        Hod(&where, hod_x, hod_y);
        reverse_graph_koordinaty(where.x_8, where.y_8, &where.x, &where.y);
      }
      else
      {
        printf("Этой фишкой походить нельзя");
      }
    }
    becameQueen(where, lodic);
    break;
  }
  return true;
}

void Hod(Position *where, int x, int y)
{
  char bslo = lodic[where->y_8][where->x_8];
  lodic[where->y_8][where->x_8] = '0';
  where->x_8 = x;
  where->y_8 = y;
  lodic[where->y_8][where->x_8] = bslo;
}

bool reverse_graph_out_koordinaty(short i_x, short i_y, char *x, char *y)
{
  // Вычисляем координаты lodic из графических координат
  *x = 'A' + i_x;
  *y = '8' - i_y;

  return true;
}

void reverse_graph_koordinaty(short i_x, short i_y, short *x, short *y)
{
  *x = (i_x * 4) + 2;
  *y = 1 + (i_y * 2);
}

char lodic[8][8] = { // для просчета ходов.
    {' ', '1', ' ', '1', ' ', '1', ' ', '1'},
    {'1', ' ', '1', ' ', '1', ' ', '1', ' '},
    {' ', '1', ' ', '1', ' ', '1', ' ', '1'},
    {'0', ' ', '0', ' ', '0', ' ', '0', ' '},
    {' ', '0', ' ', '0', ' ', '0', ' ', '0'},
    {'2', ' ', '2', ' ', '2', ' ', '2', ' '},
    {' ', '2', ' ', '2', ' ', '2', ' ', '2'},
    {'2', ' ', '2', ' ', '2', ' ', '2', ' '}};

int get_valid_moves(Position where, Valid_Hod *motion, char lodic[8][8])
{
  // Инициализация всех возможных ходов как недопустимых
  motion->l_h = false;
  motion->r_h = false;
  motion->l_s = false;
  motion->r_s = false;

  int count = 0;

  char piece = lodic[where.y_8][where.x_8];

  // Проверка ходов для обычной фишки
  if (piece == '1' || piece == '2' || piece == '3' || piece == '4')
  {
    // Проверка хода влево-вниз
    if (where.y_8 + 1 < 8 && where.x_8 - 1 >= 0 &&
        lodic[where.y_8 + 1][where.x_8 - 1] == '0')
    {
      if (piece == '2')
      {
        motion->l_s = false;
      }
      else
      {
        motion->l_s = true;
        motion->ls_y = where.y_8 + 1;
        motion->ls_x = where.x_8 - 1;
        count++;
      }
    }
    // Проверка хода вправо-вниз
    if (where.y_8 + 1 < 8 && where.x_8 + 1 < 8 &&
        lodic[where.y_8 + 1][where.x_8 + 1] == '0')
    {
      if (piece == '2')
      {
        motion->r_s = false;
      }
      else
      {
        motion->r_s = true;
        motion->rs_y = where.y_8 + 1;
        motion->rs_x = where.x_8 + 1;
        count++;
      }
    }

    // Проверка хода влево-вниз
    if (where.y_8 - 1 >= 0 && where.x_8 - 1 >= 0 &&
        lodic[where.y_8 - 1][where.x_8 - 1] == '0')
    {
      if (piece == '1')
        motion->l_h = false;
      else
      {
        motion->l_h = true;
        motion->lh_y = where.y_8 - 1;
        motion->lh_x = where.x_8 - 1;
        count++;
      }
    }

    // Проверка хода вправо-вниз
    if (where.y_8 - 1 >= 0 && where.x_8 + 1 < 8 &&
        lodic[where.y_8 - 1][where.x_8 + 1] == '0')
    {
      if (piece == '1')
        motion->r_h = false;
      else
      {
        motion->r_h = true;
        motion->rh_y = where.y_8 - 1;
        motion->rh_x = where.x_8 + 1;
        count++;
      }
    }
  }

  return count;
}

int get_valid_kill(Position where, Valid_Kill *velian, char lodic[8][8])
{ // Клетки которые можно срубить
  // Инициализация всех возможных ходов как недопустимых
  velian->kill_l_h = false;
  velian->kill_r_h = false;
  velian->kill_l_s = false;
  velian->kill_r_s = false;

  int count = 0;

  char piece = lodic[where.y_8][where.x_8];

  if (piece == '1' || piece == '2' || piece == '3' || piece == '4')
  {
    if (is_player_turn)
    {
      if ((where.y_8 + 1 < 8 && where.x_8 - 1 >= 0) &&
          ((lodic[where.y_8 + 1][where.x_8 - 1] == '1') || (lodic[where.y_8 + 1][where.x_8 - 1] == '3')) &&
          (where.y_8 + 2 < 8 && where.x_8 - 2 >= 0) &&
          (lodic[where.y_8 + 2][where.x_8 - 2] == '0'))
      {
        velian->kill_l_s = true;
        velian->kill_ls_y = where.y_8 + 2;
        velian->kill_ls_x = where.x_8 - 2;
        count++;
      }
      if ((where.y_8 + 1 < 8 && where.x_8 + 1 < 8) &&
          ((lodic[where.y_8 + 1][where.x_8 + 1] == '1') || (lodic[where.y_8 + 1][where.x_8 + 1] == '3')) &&
          (where.y_8 + 2 < 8 && where.x_8 + 2 < 8) &&
          (lodic[where.y_8 + 2][where.x_8 + 2] == '0'))
      {
        velian->kill_r_s = true;
        velian->kill_rs_y = where.y_8 + 2;
        velian->kill_rs_x = where.x_8 + 2;
        count++;
      }
      if ((where.y_8 - 1 >= 0 && where.x_8 - 1 >= 0) &&
          ((lodic[where.y_8 - 1][where.x_8 - 1] == '1') || (lodic[where.y_8 - 1][where.x_8 - 1] == '3')) &&
          (where.y_8 - 2 >= 0 && where.x_8 - 2 >= 0) &&
          (lodic[where.y_8 - 2][where.x_8 - 2] == '0'))
      {
        velian->kill_l_h = true;
        velian->kill_lh_y = where.y_8 - 2;
        velian->kill_lh_x = where.x_8 - 2;
        count++;
      }
      if ((where.y_8 - 1 >= 0 && where.x_8 + 1 < 8) &&
          ((lodic[where.y_8 - 1][where.x_8 + 1] == '1') || (lodic[where.y_8 - 1][where.x_8 + 1] == '3')) &&
          (where.y_8 - 2 >= 0 && where.x_8 + 2 < 8) &&
          (lodic[where.y_8 - 2][where.x_8 + 2] == '0'))
      {
        velian->kill_r_h = true;
        velian->kill_rh_y = where.y_8 - 2;
        velian->kill_rh_x = where.x_8 + 2;
        count++;
      }
    }
    else
    {
      if ((where.y_8 + 1 < 8 && where.x_8 - 1 >= 0) &&
          ((lodic[where.y_8 + 1][where.x_8 - 1] == '2') || (lodic[where.y_8 + 1][where.x_8 - 1] == '4')) &&
          (where.y_8 + 2 < 8 && where.x_8 - 2 >= 0) &&
          (lodic[where.y_8 + 2][where.x_8 - 2] == '0'))
      {
        velian->kill_l_s = true;
        velian->kill_ls_y = where.y_8 + 2;
        velian->kill_ls_x = where.x_8 - 2;
        count++;
      }

      if ((where.y_8 + 1 < 8 && where.x_8 + 1 < 8) &&
          ((lodic[where.y_8 + 1][where.x_8 + 1] == '2') || (lodic[where.y_8 + 1][where.x_8 + 1] == '4')) &&
          (where.y_8 + 2 < 8 && where.x_8 + 2 < 8) &&
          (lodic[where.y_8 + 2][where.x_8 + 2] == '0'))
      {
        velian->kill_r_s = true;
        velian->kill_rs_y = where.y_8 + 2;
        velian->kill_rs_x = where.x_8 + 2;
        count++;
      }

      if ((where.y_8 - 1 >= 0 && where.x_8 - 1 >= 0) &&
          ((lodic[where.y_8 - 1][where.x_8 - 1] == '2') || (lodic[where.y_8 - 1][where.x_8 - 1] == '4')) &&
          (where.y_8 - 2 >= 0 && where.x_8 - 2 >= 0) &&
          (lodic[where.y_8 - 2][where.x_8 - 2] == '0'))
      {
        velian->kill_l_h = true;
        velian->kill_lh_y = where.y_8 - 2;
        velian->kill_lh_x = where.x_8 - 2;
        count++;
      }

      if ((where.y_8 - 1 >= 0 && where.x_8 + 1 < 8) &&
          ((lodic[where.y_8 - 1][where.x_8 + 1] == '2') || (lodic[where.y_8 - 1][where.x_8 + 1] == '4')) &&
          (where.y_8 - 2 >= 0 && where.x_8 + 2 < 8) &&
          (lodic[where.y_8 - 2][where.x_8 + 2] == '0'))
      {
        velian->kill_r_h = true;
        velian->kill_rh_y = where.y_8 - 2;
        velian->kill_rh_x = where.x_8 + 2;
        count++;
      }
    }
  }

  return count;
}

bool koordinaty(char x, char y, short *i_x, short *i_y, short *i_x_8, short *i_y_8)
{ // Расположение фишки на поле
  if (x < 'A' || x > 'H' || y < '1' || y > '8')
    return false;

  short in_x = (short)(x - 'A');

  short in_y = (short)(y - '0');

  *i_x_8 = in_x;
  *i_y_8 = 8 - in_y;
  reverse_graph_koordinaty(*i_x_8, *i_y_8, i_x, i_y);

  return true;
}

void print_turn()
{ // Печать флага чей сейчас ход
  printf("\nСейчас ход: %s\n", is_player_turn ? "игрока" : "компьютера");
}

void switch_turn()
{ // Поменять ход
  is_player_turn = !is_player_turn;
}

void highlight_piece(short x, short y, char board[BOARD_SIZE][SIZE + 1])
{ // Выделить клетку
  if (x > 1)
    board[y][x - 1] = '#';
  if (x < SIZE - 2)
    board[y][x + 1] = '#';
  print_board(board);

  if (x > 1)
    board[y][x - 1] = ' ';
  if (x < SIZE - 2)
    board[y][x + 1] = ' ';
}

bool check_game_over()
{ // Проверка оканчания игры
  if (game_state.count_white + game_state.count_white_king == 0)
  {
    printf("\nЧерные победили! У белых не осталось фишек.\n");
    return true;
  }

  if (game_state.count_black + game_state.count_black_king == 0)
  {
    printf("\nБелые победили! У черных не осталось фишек.\n");
    return true;
  }

  // нет ходов
  return false;
}

void wash_piece(char board[BOARD_SIZE][SIZE + 1], short x, short y)
{
  board[y][x] = '*';
}

void light(char board[BOARD_SIZE][SIZE + 1], short x, short y, bool not_chose)
{

  if (not_chose)
  {
    board[y][x - 1] = '(';
    board[y][x + 1] = ')';
  }
  else
  {
    board[y][x - 1] = ' ';
    board[y][x + 1] = ' ';
  }
}

void becameQueen(Position where, char lodic[8][8])
{
  if (player_is_white)
  {
    if (where.y_8 == 0 && is_player_turn)
    {
      if (board[where.y][where.x] != 'W')
      {
        where.damka = true;
        board[where.y][where.x] = 'W';
        lodic[where.y_8][where.x_8] = '4';
        game_state.count_white_king++;
        game_state.count_white--;
        return;
      }
    }
    if (where.y_8 == 7 && !is_player_turn)
    {
      if (board[where.y][where.x] != 'B')
      {
        where.damka = true;
        board[where.y][where.x] = 'B';
        lodic[where.y_8][where.x_8] = '3';
        game_state.count_black_king++;
        game_state.count_black--;
        return;
      }
    }
  }
  else
  {
    if (where.y_8 == 0 && is_player_turn)
    {
      if (board[where.y][where.x] != 'B')
      {
        where.damka = true;
        board[where.y][where.x] = 'B';
        lodic[where.y_8][where.x_8] = '4';
        game_state.count_black_king++;
        game_state.count_black--;
        return;
      }
    }
    if (where.y_8 == 7 && !is_player_turn)
    {
      if (board[where.y][where.x] != 'W')
      {
        where.damka = true;
        board[where.y][where.x] = 'W';
        lodic[where.y_8][where.x_8] = '3';
        game_state.count_white_king++;
        game_state.count_white--;
        return;
      }
    }
  }
}

void calculate_kill_moves(Position pos, char lodic[8][8], int *move_i, short move_buffer[10][2], char lodic_buffer[10][8][8], GameState *game_states)
{
  Valid_Kill vks;
  int vks_count = get_valid_kill(pos, &vks, lodic);
  if (vks_count == 0)
  {
    move_buffer[*move_i][0] = pos.x_8;
    move_buffer[*move_i][1] = pos.y_8;
    memcpy(lodic_buffer[*move_i], lodic, sizeof(char) * 8 * 8);
    game_states[*move_i] = game_state;
    (*move_i)++;
    return;
  }
  GameState gs_copy = game_state;
  if (vks.kill_l_h)
  {
    Position new_pos = pos;
    new_pos.x_8 = vks.kill_lh_x;
    new_pos.y_8 = vks.kill_lh_y;
    char piece = lodic[pos.y_8 - 1][pos.x_8 - 1];
    if (piece == '1')
      if (!player_is_white)
        game_state.count_white--;
      else
        game_state.count_black--;
    else if (piece == '3')
      if (!player_is_white)
        game_state.count_white_king--;
      else
        game_state.count_black_king--;
    reverse_graph_koordinaty(new_pos.x_8, new_pos.y_8, &new_pos.x, &new_pos.y);
    Valid_Kill new_vks;
    char lodic_copy[8][8];
    memcpy(lodic_copy, lodic, sizeof(char) * 8 * 8);
    lodic_copy[pos.y_8 - 1][pos.x_8 - 1] = '0';
    lodic_copy[new_pos.y_8][new_pos.x_8] = lodic[pos.y_8][pos.x_8];
    lodic_copy[pos.y_8][pos.x_8] = '0';
    calculate_kill_moves(new_pos, lodic_copy, move_i, move_buffer, lodic_buffer, game_states);
    game_state = gs_copy;
  }
  if (vks.kill_r_h)
  {
    Position new_pos = pos;
    new_pos.x_8 = vks.kill_rh_x;
    new_pos.y_8 = vks.kill_rh_y;
    char piece = lodic[pos.y_8 - 1][pos.x_8 + 1];
    if (piece == '1')
      if (!player_is_white)
        game_state.count_white--;
      else
        game_state.count_black--;
    else if (piece == '3')
      if (!player_is_white)
        game_state.count_white_king--;
      else
        game_state.count_black_king--;
    reverse_graph_koordinaty(new_pos.x_8, new_pos.y_8, &new_pos.x, &new_pos.y);
    Valid_Kill new_vks;
    char lodic_copy[8][8];
    memcpy(lodic_copy, lodic, sizeof(char) * 8 * 8);
    lodic_copy[pos.y_8 - 1][pos.x_8 + 1] = '0';
    lodic_copy[new_pos.y_8][new_pos.x_8] = lodic[pos.y_8][pos.x_8];
    lodic_copy[pos.y_8][pos.x_8] = '0';
    calculate_kill_moves(new_pos, lodic_copy, move_i, move_buffer, lodic_buffer, game_states);
    game_state = gs_copy;
  }
  if (vks.kill_l_s)
  {
    Position new_pos = pos;
    new_pos.x_8 = vks.kill_ls_x;
    new_pos.y_8 = vks.kill_ls_y;
    char piece = lodic[pos.y_8 + 1][pos.x_8 - 1];
    if (piece == '1')
      if (!player_is_white)
        game_state.count_white--;
      else
        game_state.count_black--;
    else if (piece == '3')
      if (!player_is_white)
        game_state.count_white_king--;
      else
        game_state.count_black_king--;
    reverse_graph_koordinaty(new_pos.x_8, new_pos.y_8, &new_pos.x, &new_pos.y);
    Valid_Kill new_vks;
    char lodic_copy[8][8];
    memcpy(lodic_copy, lodic, sizeof(char) * 8 * 8);
    lodic_copy[pos.y_8 + 1][pos.x_8 - 1] = '0';
    lodic_copy[new_pos.y_8][new_pos.x_8] = lodic[pos.y_8][pos.x_8];
    lodic_copy[pos.y_8][pos.x_8] = '0';
    calculate_kill_moves(new_pos, lodic_copy, move_i, move_buffer, lodic_buffer, game_states);
    game_state = gs_copy;
  }
  if (vks.kill_r_s)
  {
    Position new_pos = pos;
    new_pos.x_8 = vks.kill_rs_x;
    new_pos.y_8 = vks.kill_rs_y;
    char piece = lodic[pos.y_8 + 1][pos.x_8 + 1];
    if (piece == '1')
      if (!player_is_white)
        game_state.count_white--;
      else
        game_state.count_black--;
    else if (piece == '3')
      if (!player_is_white)
        game_state.count_white_king--;
      else
        game_state.count_black_king--;
    reverse_graph_koordinaty(new_pos.x_8, new_pos.y_8, &new_pos.x, &new_pos.y);
    Valid_Kill new_vks;
    char lodic_copy[8][8];
    memcpy(lodic_copy, lodic, sizeof(char) * 8 * 8);
    lodic_copy[pos.y_8 + 1][pos.x_8 + 1] = '0';
    lodic_copy[new_pos.y_8][new_pos.x_8] = lodic[pos.y_8][pos.x_8];
    lodic_copy[pos.y_8][pos.x_8] = '0';
    calculate_kill_moves(new_pos, lodic_copy, move_i, move_buffer, lodic_buffer, game_states);
    game_state = gs_copy;
  }
}

bool computer_move(char board[BOARD_SIZE][SIZE + 1])
{
  int mx_score = -1;
  char best_lodic[8][8];
  GameState best_gs = game_state;
  bool has_to_kill = false;
  bool no_moves = true;
  for (int y = 0; y < 8; y++)
    for (int x = y % 2 == 0; x < 8; x += 2)
    {
      if (lodic[y][x] != '1' && lodic[y][x] != '3')
        continue;
      Position pos;
      pos.x_8 = x;
      pos.y_8 = y;
      reverse_graph_koordinaty(x, y, &pos.x, &pos.y);
      Valid_Kill vks;
      int vks_count = get_valid_kill(pos, &vks, lodic);

      if (vks_count > 0)
      {
        char local_best_lodic[8][8];
        GameState best_game_state;
        int new_score = get_best_sequent_kills(pos, vks, vks_count, -1, lodic, local_best_lodic, &best_game_state);
        if (new_score > mx_score || !has_to_kill)
        {
          mx_score = new_score;
          memcpy(best_lodic, local_best_lodic, sizeof(char) * 8 * 8);
          best_gs = best_game_state;
          no_moves = false;
        }
        has_to_kill = true;
        continue;
      }
      if (has_to_kill)
        continue;
      Valid_Hod vhs;
      int vhs_count = get_valid_moves(pos, &vhs, lodic);
      if (vhs_count == 0)
        continue;
      if (vhs.l_h)
      {
        char lodic_copy[8][8];
        memcpy(lodic_copy, lodic, sizeof(char) * 8 * 8);
        lodic_copy[y][x] = '0';
        lodic_copy[vhs.lh_y][vhs.lh_x] = '1';
        Position new_pos;
        new_pos.x_8 = vhs.lh_x;
        new_pos.y_8 = vhs.lh_y;
        reverse_graph_koordinaty(new_pos.x_8, new_pos.y_8, &new_pos.x, &new_pos.y);
        GameState gs_copy = game_state;
        becameQueen(new_pos, lodic_copy);
        int score = evaluate_board_pc(lodic_copy, !player_is_white);
        if (score > mx_score)
        {
          mx_score = score;
          memcpy(best_lodic, lodic_copy, sizeof(char) * 8 * 8);
          best_gs = game_state;
          no_moves = false;
        }
        game_state = gs_copy;
      }
      if (vhs.r_h)
      {
        char lodic_copy[8][8];
        memcpy(lodic_copy, lodic, sizeof(char) * 8 * 8);
        lodic_copy[y][x] = '0';
        lodic_copy[vhs.rh_y][vhs.rh_x] = '1';
        Position new_pos;
        new_pos.x_8 = vhs.rh_x;
        new_pos.y_8 = vhs.rh_y;
        reverse_graph_koordinaty(new_pos.x_8, new_pos.y_8, &new_pos.x, &new_pos.y);
        GameState gs_copy = game_state;
        becameQueen(new_pos, lodic_copy);
        int score = evaluate_board_pc(lodic_copy, !player_is_white);
        if (score > mx_score)
        {
          mx_score = score;
          memcpy(best_lodic, lodic_copy, sizeof(char) * 8 * 8);
          best_gs = game_state;
          no_moves = false;
        }
        game_state = gs_copy;
      }
      if (vhs.l_s)
      {
        char lodic_copy[8][8];
        memcpy(lodic_copy, lodic, sizeof(char) * 8 * 8);
        lodic_copy[y][x] = '0';
        lodic_copy[vhs.ls_y][vhs.ls_x] = '1';
        Position new_pos;
        new_pos.x_8 = vhs.ls_x;
        new_pos.y_8 = vhs.ls_y;
        reverse_graph_koordinaty(new_pos.x_8, new_pos.y_8, &new_pos.x, &new_pos.y);
        GameState gs_copy = game_state;
        becameQueen(new_pos, lodic_copy);
        int score = evaluate_board_pc(lodic_copy, !player_is_white);
        if (score > mx_score)
        {
          mx_score = score;
          memcpy(best_lodic, lodic_copy, sizeof(char) * 8 * 8);
          best_gs = game_state;
          no_moves = false;
        }
        game_state = gs_copy;
      }
      if (vhs.r_s)
      {
        char lodic_copy[8][8];
        memcpy(lodic_copy, lodic, sizeof(char) * 8 * 8);
        lodic_copy[y][x] = '0';
        lodic_copy[vhs.rs_y][vhs.rs_x] = '1';
        Position new_pos;
        new_pos.x_8 = vhs.rs_x;
        new_pos.y_8 = vhs.rs_y;
        reverse_graph_koordinaty(new_pos.x_8, new_pos.y_8, &new_pos.x, &new_pos.y);
        GameState gs_copy = game_state;
        becameQueen(new_pos, lodic_copy);
        int score = evaluate_board_pc(lodic_copy, !player_is_white);
        if (score > mx_score)
        {
          mx_score = score;
          memcpy(best_lodic, lodic_copy, sizeof(char) * 8 * 8);
          best_gs = game_state;
          no_moves = false;
        }
        game_state = gs_copy;
      }
    }
  memcpy(lodic, best_lodic, sizeof(char) * 8 * 8);
  game_state = best_gs;
  return !no_moves;
  // for (short x = 0; x < 8; x++)
  //   for (short y = 0; y < 8; y++)
  //   {
  //     short bx, by;
  //     reverse_graph_koordinaty(x, y, &bx, &by);
  //     board[by][bx] = (lodic[y][x] == '0' ? '*' : (lodic[y][x] == '1' ? '0' : (lodic[y][x] == '2' ? 'O' : ' ')));
  //   }
}

int evaluate_board_pc(char lodic[8][8], bool is_white)
{
  int score = 0;
  int our_count = is_white ? game_state.count_white : game_state.count_black;
  int opponent_count = is_white ? game_state.count_black : game_state.count_white;
  int inc = (our_count - opponent_count) * 10;
  score += inc > 0 ? inc : 0;
  our_count = is_white ? game_state.count_white_king : game_state.count_black_king;
  opponent_count = is_white ? game_state.count_black_king : game_state.count_white_king;
  inc = (our_count - opponent_count) * 15;
  score += inc > 0 ? inc : 0;
  score += (lodic[3][4] == '1') * 2;
  score += (lodic[3][4] == '1') * 2;
  score += (lodic[3][4] == '1') * 2;
  score += (lodic[3][4] == '1') * 2;
  return score;
}

int get_best_sequent_kills(Position pos, Valid_Kill vks, int vks_count, int mx_score, char lodic[8][8], char best_lodic[8][8], GameState *best_game_state)
{
  if (vks_count == 0)
  {
    int new_score = evaluate_board_pc(lodic, !player_is_white);
    if (new_score > mx_score)
    {
      mx_score = new_score;
      memcpy(best_lodic, lodic, sizeof(char) * 8 * 8);
      *best_game_state = game_state;
    }
    return mx_score;
  }
  GameState game_state_copy = game_state;
  if (vks.kill_l_h)
  {
    Position new_pos = pos;
    new_pos.x_8 = vks.kill_lh_x;
    new_pos.y_8 = vks.kill_lh_y;
    char piece = lodic[pos.y_8 - 1][pos.x_8 - 1];
    if (piece == '2')
      if (player_is_white)
        game_state.count_white--;
      else
        game_state.count_black--;
    else if (piece == '4')
      if (player_is_white)
        game_state.count_white_king--;
      else
        game_state.count_black_king--;
    reverse_graph_koordinaty(new_pos.x_8, new_pos.y_8, &new_pos.x, &new_pos.y);
    Valid_Kill new_vks;
    char lodic_copy[8][8];
    memcpy(lodic_copy, lodic, sizeof(char) * 8 * 8);
    lodic_copy[pos.y_8 - 1][pos.x_8 - 1] = '0';
    lodic_copy[pos.y_8][pos.x_8] = '0';
    lodic_copy[new_pos.y_8][new_pos.x_8] = '1';
    becameQueen(new_pos, lodic_copy);
    int new_vks_count = get_valid_kill(new_pos, &new_vks, lodic_copy);
    mx_score = get_best_sequent_kills(new_pos, new_vks, new_vks_count, mx_score, lodic_copy, best_lodic, best_game_state);
    game_state = game_state_copy;
  }
  if (vks.kill_r_h)
  {
    Position new_pos = pos;
    new_pos.x_8 = vks.kill_rh_x;
    new_pos.y_8 = vks.kill_rh_y;
    char piece = lodic[pos.y_8 - 1][pos.x_8 + 1];
    if (piece == '2')
      if (player_is_white)
        game_state.count_white--;
      else
        game_state.count_black--;
    else if (piece == '4')
      if (player_is_white)
        game_state.count_white_king--;
      else
        game_state.count_black_king--;
    reverse_graph_koordinaty(new_pos.x_8, new_pos.y_8, &new_pos.x, &new_pos.y);
    Valid_Kill new_vks;
    char lodic_copy[8][8];
    memcpy(lodic_copy, lodic, sizeof(char) * 8 * 8);
    lodic_copy[pos.y_8 - 1][pos.x_8 + 1] = '0';
    lodic_copy[pos.y_8][pos.x_8] = '0';
    lodic_copy[new_pos.y_8][new_pos.x_8] = '1';
    becameQueen(new_pos, lodic_copy);
    int new_vks_count = get_valid_kill(new_pos, &new_vks, lodic_copy);
    mx_score = get_best_sequent_kills(new_pos, new_vks, new_vks_count, mx_score, lodic_copy, best_lodic, best_game_state);
    game_state = game_state_copy;
  }
  if (vks.kill_l_s)
  {
    Position new_pos = pos;
    new_pos.x_8 = vks.kill_ls_x;
    new_pos.y_8 = vks.kill_ls_y;
    char piece = lodic[pos.y_8 + 1][pos.x_8 - 1];
    if (piece == '2')
      if (player_is_white)
        game_state.count_white--;
      else
        game_state.count_black--;
    else if (piece == '4')
      if (player_is_white)
        game_state.count_white_king--;
      else
        game_state.count_black_king--;
    reverse_graph_koordinaty(new_pos.x_8, new_pos.y_8, &new_pos.x, &new_pos.y);
    Valid_Kill new_vks;
    char lodic_copy[8][8];
    memcpy(lodic_copy, lodic, sizeof(char) * 8 * 8);
    lodic_copy[pos.y_8 + 1][pos.x_8 - 1] = '0';
    lodic_copy[pos.y_8][pos.x_8] = '0';
    lodic_copy[new_pos.y_8][new_pos.x_8] = '1';
    becameQueen(new_pos, lodic_copy);
    int new_vks_count = get_valid_kill(new_pos, &new_vks, lodic_copy);
    mx_score = get_best_sequent_kills(new_pos, new_vks, new_vks_count, mx_score, lodic_copy, best_lodic, best_game_state);
    game_state = game_state_copy;
  }
  if (vks.kill_r_s)
  {
    Position new_pos = pos;
    new_pos.x_8 = vks.kill_rs_x;
    new_pos.y_8 = vks.kill_rs_y;
    char piece = lodic[pos.y_8 + 1][pos.x_8 + 1];
    if (piece == '2')
      if (player_is_white)
        game_state.count_white--;
      else
        game_state.count_black--;
    else if (piece == '4')
      if (player_is_white)
        game_state.count_white_king--;
      else
        game_state.count_black_king--;
    reverse_graph_koordinaty(new_pos.x_8, new_pos.y_8, &new_pos.x, &new_pos.y);
    Valid_Kill new_vks;
    char lodic_copy[8][8];
    memcpy(lodic_copy, lodic, sizeof(char) * 8 * 8);
    lodic_copy[pos.y_8 + 1][pos.x_8 + 1] = '0';
    lodic_copy[pos.y_8][pos.x_8] = '0';
    lodic_copy[new_pos.y_8][new_pos.x_8] = '1';
    becameQueen(new_pos, lodic_copy);
    int new_vks_count = get_valid_kill(new_pos, &new_vks, lodic_copy);
    mx_score = get_best_sequent_kills(new_pos, new_vks, new_vks_count, mx_score, lodic_copy, best_lodic, best_game_state);
    game_state = game_state_copy;
  }
  return mx_score;
}