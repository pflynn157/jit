#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>


// Allocates RWX memory of given size and returns a pointer to it. On failure,
// prints out the error and returns NULL.
void* alloc_executable_memory(size_t size) {
  void* ptr = mmap(0, size,
                   PROT_READ | PROT_WRITE | PROT_EXEC,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (ptr == (void*)-1) {
    perror("mmap");
    return NULL;
  }
  return ptr;
}

void emit_mov1(uint8_t *m, int *index) {
    m[*index] = 0x48;
    m[*index+1] = 0x89;
    m[*index+2] = 0xf2;
    *index += 3;
}

void emit_mov2(uint8_t *m, int *index) {
    m[*index] = 0x48;
    m[*index+1] = 0x89;
    m[*index+2] = 0xfe;
    *index += 3;
}

void emit_mov32_eax(uint8_t *m, int *index, uint32_t value) {
    m[*index] = 0xb8;
    *index += 1;
    memcpy(&m[*index], &value, sizeof(uint32_t));
    *index += 4;
}

void emit_mov32_edi(uint8_t *m, int *index, uint32_t value) {
    m[*index] = 0xbf;
    *index += 1;
    memcpy(&m[*index], &value, sizeof(uint32_t));
    *index += 4;
}

void emit_syscall(uint8_t *m, int *index) {
    m[*index] = 0x0f;
    m[*index+1] = 0x05;
    *index += 2;
}

void emit_ret(uint8_t *m, int *index) {
    m[*index] = 0xc3;
    *index += 1;
}

void emit_code(uint8_t *m) {
    puts("Emitting code...");
    int index = 0;
    emit_mov1(m, &index);
    emit_mov2(m, &index);
    emit_mov32_eax(m, &index, 1);
    emit_mov32_edi(m, &index, 1);
    emit_syscall(m, &index);
    emit_ret(m, &index);
}

void emit_code_from_file(unsigned char* m) {
  /*unsigned char code[] = {
    0x48, 0x89, 0xf8,                   // mov %rdi, %rax
    0x48, 0x83, 0xc0, 0x04,             // add $4, %rax
    0xc3                                // ret
  };
  memcpy(m, code, sizeof(code));*/
  FILE *f = fopen("flat.bin", "rb");
  int idx = 0;
  while (!feof(f)) {
    uint8_t byte = 0;
    fread(&byte, sizeof(uint8_t), 1, f);
    m[idx] = byte;
    ++idx;
  }
  fclose(f);
}

const size_t SIZE = 1024;
typedef long (*JittedFunc)(void *s, char *msg, int x);

void test1() {
    puts("Hello!!!");
}

// Allocates RWX memory directly.
void run_from_rwx() {
  void* m = alloc_executable_memory(SIZE);
  emit_code_from_file(m);
  //emit_code(m);
  
  char *s = "hi!\nhello\n\0";

  JittedFunc func = m;
  //int result = func(s, 3);
  int result = func(&test1, 0, 0);
  printf("\nresult = %d\n", result);
  func(&printf, "Number: %d\n", 20);
}

int main() {
    run_from_rwx();
    return 0;
}

