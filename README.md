# Checkers
This is a project for the imperative programming course at the NSU.
# Шашки: Игра для курса императивного программирования в НГУ

## Описание проекта

Это консольная реализация классической игры в шашки, разработанная в рамках курса императивного программирования в Новосибирском государственном университете. Проект демонстрирует применение структурного подхода к программированию на языке C.

## Особенности

- 🎮 **Режим игры**: человек против компьютера
- ♟️ **Правил**: Амереканские шашки
- ⚔️ **Обязательное взятие**: реализовано правило обязательного взятия фишек
- 👑 **Автоматическое превращение**: пешки становятся дамками при достижении последней линии
- 📊 **Подсветка ходов**: визуализация возможных ходов для выбранной фишки

## Требования

- Компилятор C (gcc, clang или аналогичный)

## Установка и запуск

1. Клонируйте репозиторий:
```bash
git clone https://github.com/SollyMor/Shashki.git
```

2. Скомпилируйте программу:
```bash
gcc -o main main.c
```

3. Запустите игру:
```bash
./main
```

## Управление

1. При запуске выберите цвет фишек (белые или черные)
2. Вводите ходы в формате `БукваЦифра` (например, `B3`)
3. Для выбора хода из доступных введите соответствующий номер

## Структура проекта

```
checkers/
├── main.c          # Основной исходный код игры
└── README.md           # Этот файл
```

## Лицензия

Этот проект распространяется под лицензией MIT. Подробнее см. в файле `LICENSE`.

---

*Разработано в рамках курса "Императивное программирование" в НГУ, 2025*