#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* pa1.3
   * 2020-11-28
   * add: expr, val, count
   * expr: the expression of watchpoint
   * val: the value of expression
   * count: hit count
   */
  char expr[32];
  uint32_t val;
  uint32_t count;
  
} WP;

WP* new_wp();
void free_wp(WP*);
void display_wp();
bool watch_wp();
void delete_wp(int);

#endif
