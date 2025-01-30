#include "cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline void print_usage() {
  printf("\
help:                  see this message\n\
list:                  list all items\n\
new:                   make new item file\n\
add <name> <value>:    add an item\n\
delete <name> <value>: delete an item\n\
quota [int]:           show/set quota\n\
print:                 toggle printing\n\
run:                   begin program\n\
quit:                  quit program\n");
}

static inline void list_items() {
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

static inline void new_items() {
  FILE* items = fopen("items", "w");
  fprintf(items, "");
  fclose(items);
}

static inline void add_item(char buf[]) {
  FILE* items = fopen("items", "a");
  fprintf(items, "%s\n", buf);
  fclose(items);
}

inline void delete_item(char buf[]) {
  FILE* input = fopen("items", "r");
  if(!input) {
    printf("file either doesnt exist or you have insufficient permission\n");
    return;
  }
  FILE* output = fopen("items.new", "w");

  int buf_len = strlen(buf);
  bool w = true;
  while(true) {
    char line[512];
    fgets(line, 511, input);

    if(feof(input)) break;

    if(strncmp(buf, line, buf_len) == 0) {
      if(w) w = false;
      else fprintf(output, "%s", line);
    } else {
      fprintf(output, "%s", line);
    }
  }

  fclose(input);
  fclose(output);
  system("mv items.new items");
}

static inline void change_quota(int* quota, char buf[]) {
  sscanf(buf, "%d", quota);
}

static inline void toggle_print(bool* print) {
  *print = !*print;
  if(*print) printf("print is now on\n");
  else printf("print is now off\n");
}

static inline void toggle_sell(bool* sell) {
  *sell = !*sell;
  if(*sell) printf("selling is now on\n");
  else printf("selling is now off\n");
}

void cli_fuer_julius(int* quota, bool* print, bool* quit, bool* sell) {
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
    else if(strncmp(buf, "sell", 4) == 0) toggle_sell(sell); 
    else if(strncmp(buf, "run", 3) == 0) break;
    else if(strncmp(buf, "quit", 4) == 0) {*quit = true; break;}
    else printf("unknown command\n");
  }
}
