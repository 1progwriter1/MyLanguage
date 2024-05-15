#!/bin/bash

# Переменная для управления выводом команд в консоль
VERBOSE=true

# Функция для выполнения команды с возможностью вывода в консоль и проверкой кода завершения
execute_command() {
    local command="$1"
    if [ "$VERBOSE" = true ]; then
        echo "Выполняется команда: $command"
    fi
    eval "$command"
    local exit_code=$?
    if [ $exit_code -ne 0 ]; then
        echo "Ошибка: Команда завершилась с кодом $exit_code"
        exit $exit_code
    fi
}

# Путь к исполняемым файлам
EXECUTABLES_DIR="$(dirname "$0")/my_g--"
# Путь к папке с исходным кодом
SOURCE_DIR="$(dirname "$0")/MyLanguage"

# Путь к файлу dump.txt
DUMP_FILE="dump.txt"

# Проверяем, переданы ли один или два аргумента в скрипт
if [ $# -lt 1 ] || [ $# -gt 2 ]; then
    echo "Использование: $0 <имя_файла.fly> [<-mout>]"
    exit 1
fi

# Получаем имя файла и расширение
filename=$(basename -- "$1")
extension="${filename##*.}"

# Проверяем, является ли расширение .fly
if [ "$extension" != "fly" ]; then
    echo "Некорректное расширение файла. Ожидается .fly"
    exit 1
fi

# Получаем имя файла без расширения
filename="${filename%.*}"

DATA_DIR="$filename.data"

# Проверяем, существует ли файл
if [ ! -e "$1" ]; then
    echo "Файл не найден: $1"
    exit 1
fi

# Перемещаем файл графа в папку с остальными выходными файлами

# Запускаем ./front.out с файлом .fly в качестве аргумента
execute_command "$EXECUTABLES_DIR/front.out '$1'"

# Проверяем, был ли создан файл .mo
if [ ! -e "$filename.mo" ]; then
    echo "Не удалось создать файл .mo"
    exit 1
fi

# Создаем папку с именем файла и перемещаем файл .mo в эту папку
execute_command "mkdir -p '$DATA_DIR'"
execute_command "mv '$filename.mo' '$DATA_DIR/'"
execute_command "mv 'graphviz/graph.png' '$DATA_DIR/'"

# Перемещаем файл dump.txt в папку с остальными созданными файлами
execute_command "mv '$DUMP_FILE' '$DATA_DIR/'"

# Проверяем, передан ли второй аргумент
if [ $# -eq 2 ]; then
    # Проверяем второй аргумент
    if [ "$2" != "-mout" ]; then
        echo "Некорректный второй аргумент. Ожидается -mout"
        exit 1
    fi

    # Запускаем ./back.out с файлом .mo в качестве аргумента
    execute_command "$EXECUTABLES_DIR/back.out '$DATA_DIR/$filename.mo'"

    # Проверяем, был ли создан файл .ms
    if [ ! -e "$DATA_DIR/$filename.ms" ]; then
        echo "Не удалось создать файл .ms"
        exit 1
    fi

    # Запускаем ./asm.out с файлом .ms в качестве аргумента и указываем имя выходного файла .mout
    execute_command "$EXECUTABLES_DIR/asm.out '$DATA_DIR/$filename.ms' '$filename.mout'"

    # Проверяем, был ли создан файл .mout
    if [ ! -e "$filename.mout" ]; then
        echo "Не удалось создать файл .mout"
        exit 1
    fi

else
    # Второго аргумента нет, вызываем ./intel.out
    execute_command "$EXECUTABLES_DIR/intel.out '$DATA_DIR/$filename.mo'"

    # Проверяем, был ли создан файл .s
    if [ ! -e "$DATA_DIR/$filename.s" ]; then
        echo "Ошибка: Файл .s не был создан"
        exit 1
    fi

    # Запускаем make {file}.out
    execute_command "make '$DATA_DIR/$filename.out'"

    # Перемещаем файл .out в папку MyLanguage
    execute_command "mv '$DATA_DIR/$filename.out' '../'"
fi

echo "Процесс завершен"
