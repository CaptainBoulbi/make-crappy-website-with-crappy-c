#include <stdio.h>
#include <sys/mman.h>

int main() {
  int N = 5;
  int *ptr = mmap(NULL, N * sizeof(int), PROT_READ | PROT_WRITE,
                  MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

  if (ptr == MAP_FAILED) {
    printf("Mapping Failed\n");
    return 1;
  }

  for (int i = 0; i < N; i++)
      ptr[i] = i * 10;

  for (int i = 0; i < N; i++)
      printf("[%d] ", ptr[i]);

  printf("\n");

  int err = munmap(ptr, N * sizeof(int));
  if (err != 0) {
    printf("Unmapping Failed\n");
    return 1;
  }

  return 0;
}
