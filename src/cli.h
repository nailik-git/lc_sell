#ifndef CLI_H
#define CLI_H

#include <stdbool.h>

extern char items_path[0x1000];

void cli_fuer_julius(int* quota, bool* print, bool* quit, bool* sell);
void delete_item(char buf[]);

#endif // CLI_H
