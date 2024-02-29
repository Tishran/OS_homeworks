        .global main

        .data

errmsg_sum:
        .ascii "error in sum\0"
errmsg_sum_struct:
        .ascii "error in sum_struct\0"
errmsg_sum_array:
        .ascii "error in sum_array\0"
errmsg_sum_args:
        .ascii "error in sum_args\0"

structA:
        .quad 5
        .quad 7
        .quad 0
structB:
        .quad -11
        .quad 20
        .quad 0

array:
        .quad 11
        .quad -22
        .quad 33
        .quad -44
        .quad 55


        .text

panic_test_sum:
        mov $errmsg_sum, %rdi
        call panic

panic_test_sum_struct:
        mov $errmsg_sum_struct, %rdi
        call panic

panic_test_sum_array:
        mov $errmsg_sum_array, %rdi
        call panic

panic_test_sum_args:
        mov $errmsg_sum_args, %rdi
        call panic

panic:
        call puts
        int $3
        nop


test_sum:
        mov $1, %rdi
        mov $2, %rsi
        call sum
        cmp $3, %rax
        jne panic_test_sum

        mov $-10, %rdi
        mov $20, %rsi
        call sum
        cmp $10, %rax
        jne panic_test_sum

        ret

test_sum_struct:
        mov $structA, %rdi
        call sum_struct
        mov $structA, %rdi
        cmpq $12, 0x10(%rdi)
        jne panic_test_sum_struct

        mov $structB, %rdi
        call sum_struct
        mov $structB, %rdi
        cmpq $9, 0x10(%rdi)
        jne panic_test_sum_struct

        ret

test_sum_array:
        mov $array, %rdi
        mov $0, %rsi
        call sum_array
        cmp $0, %rax
        jne panic_test_sum_array

        mov $array, %rdi
        mov $1, %rsi
        call sum_array
        cmp $11, %rax
        jne panic_test_sum_array

        mov $array, %rdi
        mov $2, %rsi
        call sum_array
        cmp $-11, %rax
        jne panic_test_sum_array

        mov $array, %rdi
        add $0x10, %rdi
        mov $3, %rsi
        call sum_array
        cmp $44, %rax
        jne panic_test_sum_array

        ret

test_sum_args:
        sub $0x50, %rsp

        mov $0, %rdi
        call sum_args
        cmp $0, %rax
        jne panic_test_sum_args

        movq $5, (%rsp)
        movq $-7, 0x8(%rsp)
        movq $100, 0x10(%rsp)
        mov $3, %rdi
        call sum_args
        cmp $98, %rax
        jne panic_test_sum_args

        movq $5, (%rsp)
        movq $-1, 0x8(%rsp)
        movq $-100, 0x10(%rsp)
        mov $3, %rdi
        call sum_args
        cmp $-96, %rax
        jne panic_test_sum_args

        add $0x50, %rsp
        ret


main:
        call test_sum
        call test_sum_struct
        call test_sum_array
        call test_sum_args
        xor %rax, %rax
        ret
