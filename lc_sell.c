#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#define DA_INIT_CAP 256

#define da_append(da, item)                                                          \
    do {                                                                             \
        if ((da)->count >= (da)->capacity) {                                         \
            (da)->capacity = (da)->capacity == 0 ? DA_INIT_CAP : (da)->capacity*2;   \
            (da)->items = realloc((da)->items, (da)->capacity*sizeof(*(da)->items)); \
            assert((da)->items != NULL && "Buy more RAM lol");                       \
        }                                                                            \
                                                                                     \
        (da)->items[(da)->count++] = (item);                                         \
    } while (0)

#define da_free(da) free((da).items)

typedef struct item {
  char* name;
  int value;
} item;

typedef struct array {
  item* items;
  int count;
  int capacity;
} array;

array solve(array* a, int quota, bool print) {
  array r = {0};

  int sum = 0;
  for(int i = 0; i < a->count; i++) {
    sum += a->items[i].value;
  }
  printf("sum of all items: '%d\n", sum);
  if(sum < quota) {
    printf("\nquota not possible\n");
    return r;
  } else if(sum == quota) {
    printf("\nall items\n");
    return r;
  } else if(sum / 2 < quota) {
    printf("\nquota more than half of sum, now searching which items not to put in\n");
    quota = sum - quota;
  }

  const uint64_t bound = a->count <= 64 ? ((uint64_t) 1) << (a->count - 1) : 0;
  if(bound == 0) {
    printf("\ntoo many items\n");
    return r;
  }

  for(uint64_t k = 0; k < bound; k++) {
    sum = 0;
    for(int i = 0; i < a->count; i++) {
      if(k & ((uint64_t) 1) << i) {
        da_append(&r, a->items[i]);
        sum += a->items[i].value;
      }
    }

    if(sum != quota) {
      if(print) printf("found arrangement for: '%d\n", sum);
      r.count = 0;
    } else {
      break;
    }
  }
  return r;
}

int item_compare(const void* a, const void* b) {
  return ((item*) a)->value < ((item*) b)->value;
}

int main(int argc, char* argv[]) {
  if(argc < 2) return -1;
  array a = {0};

  FILE* input = fopen("items", "r");

  while(true) {
    char buf[512];
    fgets(buf, 512, input);
    if(feof(input)) break;
    // printf("%s", buf);

    char* name = calloc(512, sizeof(char));
    int value = 0;
    sscanf(buf, "%s %d\n", name, &value);
    // printf("%s: %d\n", name, value);

    item item = {.name = name, .value = value};

    da_append(&a, item);
  }
  fclose(input);

  qsort(a.items, a.count, sizeof(item), item_compare);

  int quota;
  sscanf(argv[1], "%d", &quota);
  // printf("%d\n", quota);

  bool print;
  if(argc > 2 && (argv[2][0] == 'y' || argv[2][0] == 'Y')) print = true;
  else print = false;

  array r = solve(&a, quota, print);

  if(r.count > 0) printf("\n");
  for(int i = 0; i < r.count; i++) {
    printf("%s: '%d\n", r.items[i].name, r.items[i].value);
  }
  da_free(r);

  for(int i = 0; i < a.count; i++) {
    free(a.items[i].name);
  }
  da_free(a);
  return 0;
}
