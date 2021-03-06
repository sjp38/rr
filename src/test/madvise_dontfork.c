/* -*- Mode: C; tab-width: 8; c-basic-offset: 2; indent-tabs-mode: nil; -*- */

#include "rrutil.h"

static void breakpoint(void) {}

int main(void) {
  char* page;
  pid_t pid;
  int status;

  page = mmap(NULL, PAGE_SIZE * 2, PROT_READ | PROT_WRITE,
              MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  test_assert(page != MAP_FAILED);

  test_assert(0 == madvise(page, PAGE_SIZE, MADV_DONTFORK));

  breakpoint();

  page[0] = 1;

  pid = fork();
  if (!pid) {
    test_assert(-1 == madvise(page, PAGE_SIZE, MADV_NORMAL));
    test_assert(ENOMEM == errno);

    page[PAGE_SIZE] = 2;

    atomic_puts("EXIT-SUCCESS");
    return 77;
  }

  test_assert(pid == wait(&status));
  test_assert(WIFEXITED(status) && WEXITSTATUS(status) == 77);

  return 0;
}
