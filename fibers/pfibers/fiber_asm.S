.global SwitchContext

// switch context works wrongly
SwitchContext:
        pop %rbx
        pop %rax //rsi
        pop %rcx //rdi
        push %rbx
        movq        %rcx, 64(%rdi)
        movq        %rax, 72(%rdi)

        xor %rax, %rax

        // Store callee-preserved registers
        movq        %rbx, 0(%rdi) /* FIBER_REG_RBX */
        movq        %rbp, 8(%rdi) /* FIBER_REG_RBP */
        movq        %r12, 16(%rdi) /* FIBER_REG_R12 */
        movq        %r13, 24(%rdi) /* FIBER_REG_R13 */
        movq        %r14, 32(%rdi) /* FIBER_REG_R14 */
        movq        %r15, 40(%rdi) /* FIBER_REG_R15 */

        //what should i do with rdi and rsi

        /* skip the pushed return address */
        leaq        8(%rsp), %rcx
        movq        %rcx, 48(%rdi) /* FIBER_REG_RSP */

        /* call stores the return address on the stack before jumping */
        movq        (%rsp), %rcx
        movq        %rcx, 56(%rdi) /* FIBER_REG_RIP */

        // Load context 'to'
        movq        0(%rsi), %rbx /* FIBER_REG_RBX */
        movq        8(%rsi), %rbp /* FIBER_REG_RBP */
        movq        16(%rsi), %r12 /* FIBER_REG_R12 */
        movq        24(%rsi), %r13 /* FIBER_REG_R13 */
        movq        32(%rsi), %r14 /* FIBER_REG_R14 */
        movq        40(%rsi), %r15 /* FIBER_REG_R15 */

        // Load stack pointer
        movq        48(%rsi), %rsp /* FIBER_REG_RSP */

        // Load instruction pointer, and jump
        push $0
        push        56(%rsi) /* FIBER_REG_RIP */

        // load func and its arguments
        movq        64(%rsi), %rdi
        movq        72(%rsi), %rsi
        ret
