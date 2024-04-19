[BITS 64]
mov rdx, rsi
mov rsi, rdi
mov rax, 1
mov rdi, 1
syscall
ret
