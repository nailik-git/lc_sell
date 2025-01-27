#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

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

inline void print_usage() {
  printf("\
help:                  see this message\n\
list:                  list all items\n\
new:                   make new item file\n\
add <name> <value>:    add an item\n\
delete <name> <value>: delete an item\n\
quota <int>:           set quota\n\
print:                 toggle printing\n\
run:                   begin program\n\
quit:                  quit program\n");
}

inline void list_items() {
  FILE* items = fopen("items", "r");
  if(!items) {
    printf("file either doesnt exist or you have insufficient permission\n");
    return;
  }
  while(true) {
    char buf[512];
    fgets(buf, 512, items);
    if(feof(items)) break;
    printf("%s", buf);
  }
  fclose(items);
}

inline void new_items() {
  FILE* items = fopen("items", "w");
  fprintf(items, "");
  fclose(items);
}

inline void add_item(char buf[]) {
  FILE* items = fopen("items", "a");
  fprintf(items, "%s\n", buf);
  fclose(items);
}

inline void delete_item(char buf[]) {
  char cmd[1024];
  sprintf(cmd, "cat items | grep -v \"%s\" > items.new", buf);
  system(cmd);
  system("mv items.new items");
}

inline void change_quota(int* quota, char buf[]) {
  sscanf(buf, "%d", quota);
}

inline void toggle_print(bool* print) {
  *print = !*print;
  if(print) printf("print is now on\n");
  else printf("print is now off\n");
}

void cli_fuer_julius(int* quota, bool* print, bool* quit) {
  while(true) {
    printf("> ");
    char buf[512];
    fgets(buf, 511, stdin);
    for(int i = 0; i < 511; i++) {
      if(buf[i] == '\n') buf[i] = 0;
    }
    if(strncmp(buf, "help", 4) == 0) print_usage();
    else if(strncmp(buf, "list", 4) == 0) list_items();
    else if(strncmp(buf, "new", 3) == 0) new_items();
    else if(strncmp(buf, "add", 3) == 0) add_item(buf + 4);
    else if(strncmp(buf, "delete", 6) == 0) delete_item(buf + 7);
    else if(strncmp(buf, "quota", 5) == 0) change_quota(quota, buf + 6);
    else if(strncmp(buf, "print", 5) == 0) toggle_print(print);
    else if(strncmp(buf, "run", 3) == 0) break;
    else if(strncmp(buf, "quit", 4) == 0) {*quit = true; break;}
    else printf("unknown command:");
  }
}

int main() {
  int quota = -1;
  bool print = false;
  bool quit = false;

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
    }
    fclose(input);

    qsort(a.items, a.count, sizeof(item), item_compare);


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
  }
  return 0;
}
