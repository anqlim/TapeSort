# TapeSort

Сортировка данных с входной ленты на выходную с ограничением по RAM.

## Задача

- Определить интерфейс для работы с устройством типа лента.
- Написать класс, реализующий этот интерфейс и эмулирующий работу с лентой посредством обычного файла. Должно быть возможно сконфигурировать (без перекомпиляции – например, через внешний конфигурационный файл, который будет прочитан на старте приложения) задержки по записи/чтению элемента с ленты, перемотки ленты, и сдвига ленты на одну позицию.
- Файлы временных лент можно сохранять в директорию tmp.
- Написать класс, реализующий алгоритм сортировки данных с входной ленты на выходную.
- Консольное приложение должно принимать на вход имя входного и выходного файлов и производить сортировку.
- Желательно написать юнит-тесты.

## Стек

- C++20
- CMake 3.30
- [nlohmann/json](https://github.com/nlohmann/json) — парсинг конфига
- [GoogleTest](https://github.com/google/googletest) — юнит-тесты

## Архитектура

**Паттерн: Фабричный метод** — `TapeFactory` создаёт объекты через интерфейс `ITape`, скрывая конкретную реализацию. `TapeSorter` работает только с `ITape*` и не знает что за ним — файл или мок.

```
src/
├── tape/
│   ├── interface.h — ITape: чистый абстрактный интерфейс
│   ├── file_tape.h/.cpp — реализация через fstream
│   └── tape_factory.h/.cpp
├── config/
│   └── config.h/.cpp — загрузка задержек из config.json
├── sorter/
│   └── tape_sorter.h/.cpp — алгоритм сортировки
└── main.cpp
tests/
├── mock_tape.h/.cpp — in-memory реализация ITape для тестов
├── file_tape_test.cpp
└── tape_sorter_test.cpp
```

## Алгоритм

**External merge sort** — внешняя сортировка слиянием, два прохода:

1. **Split** — читаем блоки по `M / 4` элементов, сортируем в RAM (`std::sort`), пишем во временные ленты `tmp/chunk_N`
2. **Merge** — k-путевое слияние через `std::priority_queue`, k = `M / 12` одновременных лент (учитывается хранение указателей на текущие ленты и пары {элемент, лента-источник} в очереди). Если чанков больше k — слияние рекурсивно в несколько проходов

## Сложность

| Операция                 | Сложность по времени   |
|--------------------------|------------------------|
| `read`                   | $O(1)$                 |
| `write`                  | $O(1)$                 |
| `moveRight` / `moveLeft` | $O(1)$                 |
| `rewind`                 | $O(1)$                 |
| `split` (фаза 1)         | $O(N \log\frac{M}{4})$ |
| `merge` (фаза 2)         | $O(N \log N)$          |
| `sort`  | $O(N \log N)$          |

**Split** — читаем $N$ элементов, каждый чанк сортируем за $chunk_size * log(chunk_size)$, чанков $\frac{N}{chunk_size}$ штук, итого $O(N \log\frac{M}{4})$.

**Merge** — каждый элемент проходит через priority_queue за $\log k$, элементов $N$, проходов $\log_{k}(\frac{N}{chunk_size})$, итого $N * \log k * \log_{k}(\frac{N}{chunk_size})$ — что сводится к $O(N \log N)$ в итоге.

## Конфигурация

`config.json` в рабочей директории (корень проекта):

```json
{
  "read_delay_ms":   0,
  "write_delay_ms":  0,
  "shift_delay_ms":  0,
  "rewind_delay_ms": 0,
  "memory_limit_mb": 32
}
```

Если файл не найден — все задержки 0, лимит памяти 32 MB.

## Сборка

```bash
cmake -S . -B cmake-build-debug
cmake --build cmake-build-debug
```

## Запуск

```bash
cd cmake-build-debug
.\TapeSort.exe input.bin output.bin  # Windows
./TapeSort input.bin output.bin      # Linux/macOS
```

Входной файл — бинарный, последовательность `int32_t`. Создать пример через C++ утилиту (`gen.cpp` рядом с проектом):

```cpp
#include 
int main() {
    std::ofstream f("input.bin", std::ios::binary);
    int32_t data[] = {9,1,5,3,7,2,8,4,6,0};
    f.write(reinterpret_cast(data), sizeof(data));
}
```

## Тесты

```bash
cd cmake-build-debug
.\TapeSortTests.exe   # Windows
./TapeSortTests       # Linux/macOS
```