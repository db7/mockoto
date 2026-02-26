#ifndef RUNTIME_H
#define RUNTIME_H

void *memmgr_runtime_alloc(unsigned long nbytes);
void memmgr_runtime_free(void *ptr);

#endif
