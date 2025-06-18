# 📎 Recursive Include Preprocessor

Утилита на C++ для рекурсивной обработки `#include`-директив, аналогично препроцессору C/C++.

## 📑 Содержание

- [Возможности](#возможности)
- [Пример структуры](#пример-структуры)
- [Пример использования](#пример-использования)
- [Встроенные тесты](#встроенные-тесты)
- [Сборка и запуск](#сборка-и-запуск)
- [Особенности реализации](#особенности-реализации)
- [Требования](#требования)
- [Пример результата](#пример-результата)

---

## Возможности

- Обработка `#include "file"` и `#include <file>` с рекурсией
- Поиск по текущей папке и include-директориям
- Поддержка вложенных файлов
- Сообщения об ошибке с указанием строки
- Тестирование через `assert`

---

## Пример структуры

```
sources/
├── a.cpp
├── a.in        # ← вывод
├── dir1/
│   ├── b.h
│   ├── d.h
│   └── subdir/
│       └── c.h
├── include1/
│   └── std1.h
└── include2/
    └── lib/
        └── std2.h
```

---

## Пример использования

```cpp
bool Preprocess(
    const std::filesystem::path& input_file,
    const std::filesystem::path& output_file,
    const std::vector<std::filesystem::path>& include_dirs
);

Preprocess("sources/a.cpp", "sources/a.in", {
    "sources/include1",
    "sources/include2"
});
```

---

## Встроенные тесты

`Test()`:

- Создаёт директории и файлы
- Вызывает `Preprocess(...)`
- Сравнивает результат с ожидаемым через `assert`

---

## Сборка и запуск

```bash
g++ -std=c++17 -o preprocessor main.cpp
./preprocessor
```

---

## Особенности реализации

- Используется `std::regex` для поиска include-строк
- Рекурсивное включение через `ProcessRecursInclude`
- Проверка всех путей по include-директориям
- Сообщения об ошибках с указанием строки и файла

---

## Требования

- C++17
- Подключаемые заголовки:
  - `<filesystem>`, `<fstream>`, `<regex>`, `<sstream>`
  - `<vector>`, `<string>`, `<iostream>`, `<cassert>`

---

## Пример результата

```cpp
// this comment before include
// text from b.h before include
// text from c.h before include
// std1
// text from c.h after include
// text from b.h after include
// text between b.h and c.h
// text from d.h before include
// std2
// text from d.h after include

int SayHello() {
    cout << "hello, world!" << endl;
```

---

> Используется для сборки монолитных исходников, анализа зависимостей или эмуляции препроцессора.