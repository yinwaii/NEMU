#include "watchpoint.h"
#include "expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
	strcpy(wp_pool[i].expr,"");
	wp_pool[i].tmp_value = 0;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

WP* new_wp() {
  assert(free_ != NULL);
  WP* tmp = free_;
  free_ = tmp->next;
  tmp->next = NULL;
  if(head != NULL) {
	WP *tail = head;
	for(; tail->next != NULL; tail = tail->next)
	  ;
	tail->next = tmp;
  }
  else
	head = tmp;
  return tmp;
}

void free_wp(WP *wp) {
  assert(wp != NULL);
  assert(head !=NULL);
  if(head == wp)
	head = head->next;
  else
  {
	WP *tmp = head;
	for(; tmp != NULL && tmp->next != wp; tmp = tmp->next)
	  ;
	assert(tmp != NULL);
	tmp->next = tmp->next->next;
  }
  wp->next = free_;
  free_ = wp;
}

WP* get_wp_head()
{
  return head;
}

/* TODO: Implement the functionality of watchpoint */

