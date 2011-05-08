#include <stddef.h>
#include <malloc.h>

// error handler for pure virtual functions
extern "C" void __cxa_pure_virtual() { while (1); }

void* __dso_handle = (void*) &__dso_handle;

void *operator new(size_t size) {
	return malloc(size);
}

void *operator new[](size_t size) {
    return malloc(size);
}

void operator delete(void *ptr) {
	free(ptr);
}

void operator delete[](void *ptr) {
    free(ptr);
}
