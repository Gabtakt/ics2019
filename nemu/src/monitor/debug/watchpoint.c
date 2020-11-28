#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* pa1.3
 * 2020-11-28
 * finished new_wp(), free_wp(), display_wp(), watch_wp()
 * */
WP* new_wp() {
  if (free_ == NULL) {
    printf("No more free watchpoint node.\n");
    return NULL;
  }
  WP *wp = free_;
  free_ = free_->next;
  wp->next = head;
  head = wp;
  return wp;
}

void free_wp(WP *wp) {
  if (head == NULL) {
    printf("No watchpoint to free.\n");
  }
  else if (wp == NULL) {
    printf("Parameter 'wp' is invalid.\n");
  }
  else if (wp == head) {
    head = head->next;
    wp->next = free_;
    free_ = wp;
  }
  else {
    WP *p = head;
    while (p != NULL && p->next != wp) {
      p = p->next;
    }
    if (p == NULL) {
      printf("Watchpoint not found.\n");
    }
    else {
      p->next = wp->next;
      wp->next = free_;
      free_ = wp;
    }
  }
}

void display_wp() {
  if (head == NULL) {
    printf("No watchpoint be setting.\n");
  }
  else {
    printf("Num\tWhat\tVal\n");
    WP *p = head;
    while (p != NULL) {
      printf("%d\t%s\t0x%x(%u)\n", p->NO, p->expr, p->val, p->val);
      if (p->count > 0) {
        printf("Watchpoint already hit %u times.\n", p->count);
      }
      p = p->next;
    }
  }
}

bool watch_wp() {
  WP *p = head;
  bool update = false;
  while (p != NULL) {
    bool success = true;
    uint32_t new_val = expr(p->expr, &success);
    assert(success);
    if (new_val != p->val) {
      printf("Watchpoint %d %s\nOld val: %u\nNew val: %u\n", p->NO, p->expr, p->val, new_val);
      p->val = new_val;
      p->count++;
      update = true;
    }
    p = p->next;
  }
  return update;
}

void delete_wp(int NO) {
  WP *p = head;
  while (p != NULL && p->NO != NO) {
    p = p->next;
  }
  if (p == NULL) {
    printf("watchpoint %d is not be setting.\n", NO);
  }
  else {
    free_wp(p);
  }
}