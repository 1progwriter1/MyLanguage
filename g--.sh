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

# Путь к файлу dump.txt
DUMP_FILE="dump.txt"

# Проверяем, передан ли аргумент в скрипт
if [ $# -ne 1 ]; then
    echo "Использование: $0 <имя_файла.fly>"
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
execute_command "mkdir -p '$filename'"
execute_command "mv '$filename.mo' '$filename/'"
execute_command "mv 'graphviz/graph.png' '$filename/'"

# Перемещаем файл dump.txt в папку с остальными созданными файлами
execute_command "mv '$DUMP_FILE' '$filename/'"

# Запускаем ./back.out с файлом .mo в качестве аргумента
execute_command "$EXECUTABLES_DIR/back.out '$filename/$filename.mo'"

# Проверяем, был ли создан файл .ms
if [ ! -e "$filename/$filename.ms" ]; then
    echo "Не удалось создать файл .ms"
    exit 1
fi

# Перемещаем файл .ms в папку с остальными промежуточными файлами
execute_command "mv '$filename.ms' '$filename/'"

# Запускаем ./asm.out с файлом .ms в качестве аргумента и указываем имя выходного файла .mout
execute_command "$EXECUTABLES_DIR/asm.out '$filename/$filename.ms' '$filename.mout'"

# Проверяем, был ли создан файл .mout
if [ ! -e "$filename.mout" ]; then
    echo "Не удалось создать файл .mout"
    exit 1
fi

echo "Процесс завершен"
