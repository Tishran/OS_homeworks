Домашние задачи для курса МИФИ "Архитектура компьютера и операционных систем".

## Решаем задачи

В каждой из папок находится условие и файлы очередной задачи.

Файлы main.cpp и makefile в каждой задаче отвечают за тестирование. При проверке решения они перезаписываются, так что их изменения не учитываются.

Остальные файлы менять можно и нужно.

Для тестирования в каждой задаче достаточно вызвать:
```bash
$ make
```

Команда соберет бинарные файлы из исходных и запустит тесты в main.cpp.

Как только убедились, что решение работает, отправить его можно коммитом в репозиторний:
```bash
$ git add <измененный файл>
$ git commit -m "<комментарий, описывающий изменения>"
$ git push origin main
```

Процесс проверки автоматизирован, результаты проверки можно увидеть либо в CI/CD коммита (зеленая/красная/желтая галочка напротив коммита), либо во вкладках SUBMITS и GDOC главной страницы курса https://mephi-dap-os.manytask.org/.

Закрытых тестов в задачах нет, все тесты публичные и доступны в main.cpp. Сообщения, записанные в консоль в процессе тестирования, можно увидеть в результатах проверки.

## Запуск в докере

Если у вас нет linux или x86_64 процессора, то вы можете воспользоваться докером.

1) [Скачать](https://www.docker.com/products/docker-desktop/) приложение докера.

2) Создаем образ `mephi-os` (требуется сделать всего один раз):
```bash
cd <ваш репозиторий>
docker build --platform linux/x86_64 . -t mephi-os
```

3) После того как вы создали образ `mephi-os`. Можно создать контейнер:
```bash
docker run --platform linux/x86_64 -it --rm -v <путь до вашего репозитория>:/workspace mephi-os bash
```

Эта команда переключает вас в контейнер в интерактивном режиме, а так же синхронизирует вашу текущую директорию с директорией `/workspace` в контейнере. Вы можете менять файлы как в контейнере, так и вне, все изменения будут видны в обоих местах. 
В контейнере можно запускать код с помощью команды make, а так же тестировать с помощью gdb.

## Запуск gdb на не x86_64 архитектурах

1) Запуститься докере

2) Запустить бинарник `main` в эмуляторе qemu в фоновом режиме
```bash
qemu-x86_64-static -g 1234 main &
```

3) Подключится в gdb
```bash
gdb main
...
(gdb) target remote localhost:1234
```

Ваш бинарник уже запущен, можно начинать дебажить.
