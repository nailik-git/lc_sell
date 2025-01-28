#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "cli.h"

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
  int sum;
} array;

array solve(array* a, int quota, bool print) {
  array r = {0};

  printf("sum of all items: '%d\n", a->sum);
  if(a->sum < quota) {
    printf("\nquota not possible\n");
    return r;
  } else if(a->sum == quota) {
    printf("\nall items\n");
    return r;
  } else if(a->sum / 2 < quota) {
    printf("\nquota more than half of sum, now searching which items not to put in\n");
    quota = a->sum - quota;
  }

  // abomination because of integer overflow
  const uint64_t bound = a->count ? (a->count < 64 ? (((uint64_t) 1) << (a->count)) - 1 : UINT64_MAX) : 0;
  if(bound == 0) {
    printf("\ninvalid number of items\n");
    return r;
  }

  for(uint64_t c = bound; c > 0; c--) {
    for(int i = 0; i < a->count; i++) {

      const uint64_t pos = ((uint64_t) 1) << (a->count - 1 - i); // position of i'th element in bitmask

      if(c & pos) { // if element is in bitmask
        if(a->items[i].value > quota) goto l_break;

        da_append(&r, a->items[i]);
        r.sum += a->items[i].value;

        if(r.sum >= quota) goto l_break;

        continue; // continue, so that l_break is unreachable

        l_break:
          c -= pos; // remove element from bitmask
          break;
      }
    }

    if(c == 0) break; // if not done there may be an infinite loop

    if(r.sum != quota) {
      if(print) printf("found arrangement for: '%d\n", r.sum);
      r.count = 0;
      r.sum = 0;
    } else {
      break;
    }
  }
  return r;
}

int item_compare(const void* a, const void* b) {
  return ((item*) a)->value < ((item*) b)->value;
}

int main() {
  int quota = -1;
  bool print = false;
  bool quit = false;

  printf("welcome to lc_sell! type 'help' for a list of commands\n");

  while(true) {
    cli_fuer_julius(&quota, &print, &quit);
    if(quit) break;
    if(quota < 0) {printf("invalid quota\n"); continue;}

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
      a.sum += value;
    }
    fclose(input);

    qsort(a.items, a.count, sizeof(item), item_compare);

    // for(int i = 0; i < a.count; i++) {
    //   printf("%s: '%d\n", a.items[i].name, a.items[i].value);
    // }

    array r = solve(&a, quota, print);

    if(r.count == 0 && quota != a.sum)  printf("no arrangement found\n");
    if(r.count != 0) printf("\n");
    for(int i = 0; i < r.count; i++) {
      printf("%s: '%d\n", r.items[i].name, r.items[i].value);
    }
    da_free(r);

    for(int i = 0; i < a.count; i++) {
      free(a.items[i].name);
    }
    da_free(a);
  }
  return 0;
}
