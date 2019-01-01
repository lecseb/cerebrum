/*
 * This file is part of cerebrum.
 *
 * cerebrum is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cerebrum is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with cerebrum.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "daemon-alloc.h"
#include "daemon-cond.h"
#include "daemon-list.h"

/**
 * @brief Check if a specific cell is contained by the list
 * @param list[in] : a list
 * @param llist[in] : a specific cell
 * @return 1 if the list contain the cell, 0 if not.
 */
static int _s_list_check(struct s_list *list, struct s_list *llist)
{
  daemon_return_val_if_fail(list, 0);
  daemon_return_val_if_fail(llist, 0);

  struct s_list *_list = list;
  while (_list && _list != llist)
    _list = _list->next;

  return _list == llist;
}

/**
 * @brief Allocates space for one list element. It is called by
 * s_list_append(), s_list_prepend(), s_list_insert() and
 * s_list_insert_sorted() and so is rarely used on its own.
 * @param prev[in] : previous list element
 * @param data[in] : element data
 * @param next[in] : next list element
 * @return a pointer to the newly-allocated list element
 */
static struct s_list *_s_list_new(struct s_list *prev, void *data,
  struct s_list *next)
{
  struct s_list *list = daemon_malloc(sizeof(struct s_list));
  list->data = data;
  list->previous = prev;
  list->next = next;

  return list;
}

void s_list_free(struct s_list *list)
{
  if (list) {
    if (list->next)
      s_list_free(list->next);
    daemon_free(list);
  }
}

void s_list_free_full(struct s_list *list, s_destroy_cbk func)
{
  daemon_return_if_fail(func);

  if (list) {
    struct s_list *_list = s_list_first(list);
    while (_list) {
      struct s_list *_next = s_list_next(_list);
      s_list_remove_link(list, _list);
      if (func)
        func((_list)->data);
      s_list_free(_list);
      _list = _next;
    }
  }
}

static struct s_list *_s_list_append(struct s_list *list,
  s_copy_cbk func, void *data, void *user_data)
{
  daemon_return_val_if_fail(func, list);

  struct s_list *_list = s_list_last(list);
  struct s_list *_new = _s_list_new(_list,
    func(data, user_data), NULL);

  if (_list)
    _list->next = _new;
  else
    return _new;
  return list;
}

static void *_s_list_default_copy(void *data, void *user)
{
  (void)user;
  return data;
}

struct s_list *s_list_append(struct s_list *list, void *data)
{
  daemon_return_val_if_fail(data, list);

  return _s_list_append(list, _s_list_default_copy, data, NULL);
}

struct s_list *s_list_prepend(struct s_list *list, void *data)
{
  daemon_return_val_if_fail(data, list);

  struct s_list *_list = _s_list_new(NULL, data, list);

  if (list)
    list->previous = _list;

  return _list;
}

struct s_list *s_list_insert(struct s_list *list, void *data,
  uint32_t position)
{
  daemon_return_val_if_fail(data, list);

  uint32_t current = 0;
  struct s_list *_list = list;

  if (position == 0)
    return s_list_prepend(_list, data);

  if (list) {
    for (; current < position && _list->next; current++)
      _list = _list->next;
    struct s_list *_next = _list->next;
    struct s_list *_prev = _list;
    struct s_list *_new = _s_list_new(_prev, data, _next);
    if (_prev)
      _prev->next = _new;
    if (_next)
      _next->previous = _new;
    return list;
  }
  return s_list_prepend(list, data);
}

struct s_list *s_list_insert_before(struct s_list *list,
  struct s_list *sibling, void *data)
{
  daemon_return_val_if_fail(data, list);
  daemon_return_val_if_fail(_s_list_check(list, sibling), list);

  if (!list || !sibling)
    return s_list_append(list, data);

  if (sibling->previous) {
    struct s_list *_prev = sibling->previous;
    struct s_list *elt = _s_list_new(_prev, data, sibling);
    _prev->next = elt;
    sibling->previous = elt;
  } else {
    struct s_list *elt = _s_list_new(NULL, data, sibling);
    return elt;
  }
  return list;
}

struct s_list *s_list_remove(struct s_list *list, void *data)
{
  daemon_return_val_if_fail(data, list);
  daemon_return_val_if_fail(list, list);

  struct s_list *llist = s_list_find(list, data);
  list = s_list_remove_link(list, llist);
  s_list_free(llist);
  return list;
}

struct s_list *s_list_remove_link(struct s_list *list,
  struct s_list *llist)
{
  daemon_return_val_if_fail(list, list);
  daemon_return_val_if_fail(llist, list);
  daemon_return_val_if_fail(_s_list_check(list, llist), list);

  struct s_list *_prev = llist->previous;
  struct s_list *_next = llist->next;

  if (_prev)
    _prev->next = _next;
  else
    list = _next;

  if (_next)
    _next->previous = _prev;

  llist->next = NULL;
  llist->previous = NULL;
  return list;
}

struct s_list *s_list_delete_link(struct s_list *list,
  struct s_list *link)
{
  list = s_list_remove_link(list, link);
  s_list_free(link);
  return list;
}

struct s_list *s_list_remove_all(struct s_list *list, void *data)
{
  daemon_return_val_if_fail(data, list);
  daemon_return_val_if_fail(data, list);

  struct s_list *_list = list;
  while (_list) {
    if (_list->data == data) {
      struct s_list *next = _list->next;
      s_list_delete_link(list, _list);
      _list = next;
      continue;
    }
    _list = _list->next;
  }
  return list;
}

uint32_t s_list_length(struct s_list *list)
{
  if (!list)
    return 0;

  uint32_t size = 1;
  struct s_list *_list = list;

  while (_list->next) {
    _list = _list->next;
    size++;
  }
  return size;
}

struct s_list *s_list_copy(struct s_list *list)
{
  daemon_return_val_if_fail(list && list->previous == NULL, list);

  struct s_list *_list = list;
  struct s_list *_new_list = NULL;

  while (_list) {
    _new_list = s_list_append(_new_list, _list->data);
    _list = _list->next;
  }
  return _new_list;
}

struct s_list *s_list_deep_copy(struct s_list *list,
  s_copy_cbk func, void *data)
{
  daemon_return_val_if_fail(list && list->previous == NULL, list);
  daemon_return_val_if_fail(func, list);

  struct s_list *_list = list;
  struct s_list *_new_list = NULL;

  while (_list) {
    _new_list = _s_list_append(_new_list, func, _list->data, data);
    _list = _list->next;
  }
  return _new_list;
}

struct s_list *s_list_concat(struct s_list *list1,
  struct s_list *list2)
{
  daemon_return_val_if_fail(list1 && list1->previous == NULL, list1);
  daemon_return_val_if_fail(list2 && list2->previous == NULL, list1);

  struct s_list *_list = list2;
  while (_list) {
    list1 = s_list_append(list1, _list->data);
    _list = _list->next;
  }
  return list1;
}

void s_list_foreach(struct s_list *list, s_foreach_cbk func,
  void *user_data)
{
  daemon_return_if_fail(func);

  struct s_list *_list = list;
  while (_list) {
    func(user_data, _list->data);
    _list = _list->next;
  }
}

struct s_list *s_list_first(struct s_list *list)
{
  daemon_return_val_if_fail(list, NULL);

  return list;
}

struct s_list *s_list_last(struct s_list *list)
{
  if (list) {
    struct s_list *_list = list;

    if (!_list->next)
      return _list;

    while (_list->next)
      _list = _list->next;
    return _list;
  }
  return list;
}

struct s_list *s_list_nth(struct s_list *list, uint32_t nth)
{
  daemon_return_val_if_fail(list && list->previous == NULL, NULL);

  uint32_t current = 0;
  struct s_list *_list = list;

  while (_list->next && current != nth) {
    _list = _list->next;
    current++;
  }

  if (current != nth)
    return NULL;

  return _list;
}

struct s_list *s_list_find(struct s_list *list, void *data)
{
  daemon_return_val_if_fail(data, NULL);

  struct s_list *_list = list;

  while (_list->next && _list->data != data)
    _list = _list->next;

  if (_list->data == data)
    return _list;
  return NULL;
}
