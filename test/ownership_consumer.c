#include "hashmap.h"

#include <stdlib.h>

int main(void) {
  struct hashmap_s hashmap;
  int *first = NULL;
  int *second = NULL;
  void *old_value = NULL;
  int result = 1;

  if (0 != hashmap_create(1, &hashmap)) {
    return 1;
  }

  first = (int *)malloc(sizeof(*first));
  second = (int *)malloc(sizeof(*second));
  if ((NULL == first) || (NULL == second)) {
    goto cleanup;
  }
  *first = 1;
  *second = 2;

  if (0 != hashmap_put_and_return_old(&hashmap, "owned", 5, first,
                                      &old_value)) {
    goto cleanup;
  }
  if (NULL != old_value) {
    goto cleanup;
  }

  if (0 != hashmap_put_and_return_old(&hashmap, "owned", 5, second,
                                      &old_value)) {
    goto cleanup;
  }
  if (first != old_value) {
    goto cleanup;
  }
  free(first);
  first = NULL;

  if (0 != hashmap_remove_and_return_value(&hashmap, "owned", 5,
                                           &old_value)) {
    goto cleanup;
  }
  if (second != old_value) {
    goto cleanup;
  }
  free(second);
  second = NULL;

  if (0 != hashmap_put(&hashmap, "null", 4, NULL)) {
    goto cleanup;
  }
  old_value = (void *)first;
  if (0 != hashmap_remove_and_return_value(&hashmap, "null", 4,
                                           &old_value)) {
    goto cleanup;
  }
  if (NULL != old_value) {
    goto cleanup;
  }

  old_value = (void *)second;
  if (0 == hashmap_remove_and_return_value(&hashmap, "missing", 7,
                                           &old_value)) {
    goto cleanup;
  }
  if (NULL != old_value) {
    goto cleanup;
  }

  result = 0;

cleanup:
  hashmap_destroy(&hashmap);
  free(first);
  free(second);
  return result;
}
