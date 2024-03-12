        .global sum
        .global sum_struct
        .global sum_array
        .global sum_args

// input:
//    rdi: первое int64_t число
//    rsi: второе int64_t число
//
// output:
//    rax: сумма чисел
//
sum:
    mov $0, %rax
    add %rdi, %rax
    add %rsi, %rax
    ret

// input:
//    rdi: указатель на структуру
//
// struct {
//     int64_t a;
//     int64_t b;
//     int64_t sum;
// };
//
// output:
//    в поле sum нужно записать сумму a и b
//
sum_struct:
    push %rax
    mov (%rdi), %rax
    add 8(%rdi), %rax
    add 16(%rdi), %rax
    mov %rax, 16(%rdi)
    pop %rax
    ret

// input: 
//    rdi: указатель на массив int64_t 
//    rsi: количество элементов в массиве
//
// output:
//    rax: сумма элементов массива
//
sum_array:
    xor %rax, %rax

    loop_array:
    cmp $0, %rsi
    je break_loop_array
    add (%rdi), %rax
    add $8, %rdi
    dec %rsi
    jmp loop_array

    break_loop_array:
    ret

// input:
//    rdi: количество элементов на стеке (сразу после адреса возврата из функции)
//
// output:
//    rax: сумма элементов со стека
//
sum_args:
    mov $8, %rax
    mul %rdi
    mov %rax, %rdi
    xor %rax, %rax

    arg_loop:
    cmp $0, %rdi
    je break_arg_array
    add (%rsp, %rdi), %rax
    sub $8, %rdi
    jmp arg_loop

    break_arg_array:
    ret