# Gate

В этой задачe вам нужно будет доделать ядро операционной системы RJOS64 (Rudimental JOS64).
ОС RJOS64 - однозадачная 64-битная ОС для процессоров x86\_64.
У RJOS64 есть два системных вызова: `sys_work` и `sys_retire`.
Их номера 1 и 2 соответственно. Имплементация `sys_work` и `sys_retire` уже готова и находится в файле `syscall.c`, декларации лежат в файле `syscall.h`.

В RJOS64 нет ни виртуальной памяти, ни файловой системы.
Единственное пользовательское приложение вкомпилированно прямо в бинарник.
Исходный код пользовательского приложения находится в файле `user.S`.
Посмотрите его. По исходному коду вы должны понять интерфейс системных вызовов RJOS64.

У вас должны получиться ответы на следующие вопросы:
  * Как указывается номер системного вызова?
  * Как передаются аргументы системного вызова?
  * Должен ли системный вызов возвращать управление в пользовательский код?
  * Если да, то какой код возврата означает успех?

В RJOS64 не хватает самого важного для работы пользовательского кода.
Во-первых управление на него никак не передаётся.
Во-вторых не настроена точка входа для syscall.

Вам нужно доделать RJOS64 так, чтобы получилось успешно выполнить код из `user.S` (т.е. код позвал бы все системные вызовы как надо, при этом начинать исполнение, конечно же, нужно с метки `_start_user`).
Разрешается менять файлы `sysgate.c` и  `sysgate_asm.S`.

<details>
<summary>Как настроить syscall?</summary>
Процессор x86 даст выполнить инструкцию `syscall` только если установлен бит `IA32_EFER.SCE` в регистре `IA32_EFER`.
`IA32_EFER` - это model specific register (MSR) с номером `0xC0000080`.
Бит `IA32_EFER.SCE` - нулевой бит этого регистра.
Чтение и запись таких регистров делается специальными инструкциями `rdmsr`/`wrmsr`.

Инструкция `rdmsr` читает содержимое MSR, номер которого указан на регистре `ecx` на пару регистров `edx` и `eax`.
Значения `edx` и `eax` нужно интерпретировать как 32-битные части одного 64-битного значения: `edx` содержит старшие 32 бита, а `eax` - младшие.

Инструкция `wrmsr` пишет в MSR, номер которого указан на регистре `ecx`, 64-битное значение, переданное как пара регистров `edx` и `eax`.
Как и в `rdmsr`, `edx` интерпретируется как старшие 32 бита 64-битного числа, а `eax` - как младшие 32 бита.

Вам будет полезно написать функции `long read_msr(int msr)` и `void write_msr(int msr, long value)`.
Для этого можете пользоваться ассемблерными вставками или написать целиком на ассемблере в файле `sysgate_asm.S`.

Далее, помимо того чтобы разрешить вызывать `syscall`, нужно указать точку входа в ядре RJOS64.
Точка входа - это адрес инструкции, на которую будет совершён переход при выполнении `syscall`.
Адрес должен быть записан в MSR `IA32_LSTAR` (номер `0xC0000082`).

Допустим, вы решили назвать точку входа `_syscall_enter`.
После того как пользовательский код выполнит `syscall` и управление будет передано на `_syscall_enter`, старый пользовательский `rip` будет сохранён на регистр `rcx`.
Помимо регистра `rip` сохраняется также `rflags` (на регистре `r11`).
При этом `rsp` не меняется - он указывает на вершину пользовательского стека.
В этой задаче предлагается оставить стек как есть.
То есть пользовательский код и ядро будут работать с одним стеком.
Мы считаем что пользователь у нас хороший и стек портить не будет.

Сохраните `rcx` (для этого можно использовать стек), поймите какой системный вызов запросил пользовательский код (`sys_work` или `sys_retire`) и позовите соответствующую функцию из `syscall.c`.

Возврат из обработки системного вызова выполняется инструкцией `sysretq` (q на конце означает что возврат будет совершён в 64-битный код).
Инструкция `sysretq` передаст управление на адрес, который записан в регистре `rcx`, а в `rflags` записывает значение из регистра `r11`.

Теперь у нас настроен вызов syscall, дело осталось за малым - передать управление пользовательскому коду в самом начале.
Может показаться странным, но это тоже делается через инструкцию `sysretq`. Она как раз делает что нам надо - передаёт управление с понижением привелегий.
После настройки окружения для syscall просто позовите `sysretq`, так чтобы передать управление функции `_start_user`.
</details>

Когда у вас заработает программа `user.S`, вы узнаете что же она хочет вам сказать.