[BITS 64]
mov rcx, rdi
mov rdi, rsi
mov rsi, rdx
xor rax, rax
call rcx
ret
