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

#ifndef _DAEMON_LIST_H_
# define _DAEMON_LIST_H_

# include <stdint.h>

/**
 * @brief Specifies the type of function which is called when a data element is
 * destroyed. It is passed the pointer to the data element and should free any
 * memory and resources allocated for it.
 * @param data[in] : the data element
 */
typedef void (*s_destroy_cbk)(void *data);

/**
 * @brief A function of this signature is used to copy the node data when doing
 * a deep-copy of a data.
 * @param data[in] : a pointer to the data which should be copied
 * @param user_data[in] : additional data
 * @return A pointer to the copy
 */
typedef void *(*s_simple_copy_cbk)(void *data);

/**
 * @brief A function of this signature is used to copy the node data when doing
 * a deep-copy of a data.
 * @param data[in] : a pointer to the data which should be copied
 * @param user_data[in] : additional data
 * @return A pointer to the copy
 */
typedef void *(*s_copy_cbk)(void *data, void *user_data);

/**
 * @brief Specifies the type of functions passed throw *_foreach()
 * @param data[in] : the element's data
 * @param user_data[in] : additional data
 */
typedef int (*s_foreach_cbk)(void *data, void *user_data);

/**
 * @brief The list struct is used for each element in a doubly-linked list
 * @param data : holds the element's data, which can be a pointer to any kind of
 * data, or any integer value using the Type Conversion Macros?
 * @param previous : contains the link to the previous element in the list
 * @param next : contains the link to the next element in the list
 */
struct s_list {
  void *data;
  struct s_list *previous;
  struct s_list *next;
};

/**
 * @brief A convenience macro to get the previous element in a list. Note that
 * it is considered perfectly acceptable to access list->previous directly.
 */
# define s_list_previous(list) (list->previous)

/**
 * @brief A convenience macro to get the next element in a list. Note that
 * it is considered perfectly acceptable to access list->next directly.
 */
# define s_list_next(list) (list->next)

/**
 * @brief A convenience macro to get the data in an element. Note that
 * it is considered perfectly acceptable to access list->data directly.
 */
# define s_list_data(list) (list->data)

/**
 * @brief Frees all of the memory used by a list. If list elements contain
 * dynamically-allocated memory, you should either use s_list_free_full() or
 * free them manually first.
 * @param list[out] : list instance
 * @return if succeed 0 is returned else other value (see errno)
 * @note the pointer can be set to null if there is no more element.
 */
void s_list_free(struct s_list *list);

/**
 * @brief Convenience method, which frees all the memory used by a list and
 * calls func on every element's data.
 * @param list[out] : list instance
 * @param func[in] : function pointer
 * @return if succeed 0 is returned else other value (see errno)
 */
void s_list_free_full(struct s_list *list, s_destroy_cbk func);

/**
 * @brief Adds a new element on to the end of the list. Note that the return
 * value is the new start of the list, if list was empty; make sure you store
 * the new value. s_list_append() has to traverse the entire list to find the
 * end, which is inefficient when adding multiple elements. A common idiom to
 * avoid the inefficiency is to use s_list_prepend() and reverse the list
 * with s_list_reverse() when all elements have been added.
 * @param list[in] : list instance
 * @param data[in] : the data for the new element
 * @return either list or the new start of the s_list if list was NULL.
 */
struct s_list *s_list_append(struct s_list *list, void *data);

/**
 * @brief Prepends a new element on to the start of the list. Note that the
 * return value is the new start of the list, which will have changed, so make
 * sure you store the new value.
 * @param list[in] : list instance
 * @param data[in]  the data for the new element
 * @return a pointer to the newly prepended element, which is the new start of
 * the list
 * @note Do not use this function to prepend a new element to a different
 * element than the start of the list. Use s_list_insert_before() instead.
 */
struct s_list *s_list_prepend(struct s_list *list, void *data);

/**
 * @brief Inserts a new element into the list at the given position.
 * @param list[in] : list instance
 * @param data[in] : the data for the new element
 * @param position[in] : the position to insert the element. If this is
 * negative, or is larger than the number of elements in the list, the new
 * element is added on to the end of the list.
 * @return the (possibly changed) start of the list
 */
struct s_list *s_list_insert(struct s_list *list, void *data,
  uint32_t position);

/**
 * @brief Inserts a new element into the list before the given position.
 * @param list[in] : list instance
 * @param sibling[in] : the list element before which the new element is
 * inserted or NULL to insert at the end of the list
 * @param data[in] : the data for the new element
 * @return the (possibly changed) start of the list
 * @note no check done on the sibling pointer. User had to be sure about the
 * pointer.
 */
struct s_list *s_list_insert_before(struct s_list *list,
  struct s_list *sibling, void *data);

/**
 * @brief Removes an element from a list. If two elements contain the same
 * data, only the first is removed. If none of the elements contain the data,
 * the list is unchanged.
 * @param list[in] : list instance
 * @param data[in] : the data for the element to remove
 * @return the (possibly changed) start of the list
 */
struct s_list *s_list_remove(struct s_list *list, void *data);

/**
 * @brief Removes an element from a list, without freeing the element. The
 * removed element's prev and next links are set to NULL, so that it becomes a
 * self-contained list with one element.
 * @param list[in] : list instance
 * @param llist[in] : an element in the list
 * @return the (possibly changed) start of the list
 * @note This function is for example used to move an element in the list (see
 * the example for s_list_concat()) or to remove an element in the list
 * before freeing its data.
 */
struct s_list *s_list_remove_link(struct s_list *list,
  struct s_list *llist);

/**
 * @brief Removes the node link from the list and frees it. Compare this to
 * s_list_remove_link() which removes the node without freeing it.
 * @param list[in] : list instance
 * @param link[in] : node to delete from the list
 * @return the (possibly changed) start of the list
 */
struct s_list *s_list_delete_link(struct s_list *list,
  struct s_list *link);

/**
 * @brief Removes all list nodes with data equal to data . Returns the new head
 * of the list. Contrast with s_list_remove() which removes only the first
 * node matching the given data.
 * @param list[in] : list instance
 * @param data[in] : data to remove
 * @return the (possibly changed) start of the list
 */
struct s_list *s_list_remove_all(struct s_list *list, void *data);

/**
 * @brief Gets the number of elements in a list. This function iterates over the
 * whole list to count its elements. Use a s_queue instead of a s_list if
 * you regularly need the number of items. To check whether the list is
 * non-empty, it is faster to check list against NULL.
 * @param list[in] : list instance
 * @return the number of elements in the list
 */
uint32_t s_list_length(struct s_list *list);

/**
 * @brief Copies a list
 * @param list[in] : list instance
 * @return the start of the new list that holds the same data as list
 * @note Note that this is a "shallow" copy. If the list elements consist of
 * pointers to data, the pointers are copied but the actual data is not.
 * See s_list_copy_deep() if you need to copy the data as well.
 */
struct s_list *s_list_copy(struct s_list *list);

/**
 * @brief Makes a full (deep) copy of a list. In contrast with s_list_copy(),
 * this function uses func to make a copy of each list element, in addition to
 * copying the list container itself. func , as a s_copy_func, takes two
 * arguments, the data to be copied and a user_data pointer.
 * It's safe to pass NULL as user_data, if the copy function takes only one
 * argument.
 * @param list[in] : list instance
 * @param func[in] : the function which is called to copy the data inside each
 * node, or NULL to use the original data.
 * @param data[in] : to pass to copy_func
 * @return the start of the new list that holds the same data as list
*/
struct s_list *s_list_deep_copy(struct s_list *list,
  s_copy_cbk func, void *data);

/**
 * @brief Adds the second list onto the end of the first list. Note that the
 * elements of the second list are not copied. They are used directly.
 * @param list1[in] : a list, this must point to the top of the list
 * @param list2[in] : the list to add to the end of the first list, this must
 * point to the top of the list
 * @return the start of the new list, which equals list1 if not NULL
 */
struct s_list *s_list_concat(struct s_list *list1, struct s_list *list2);

/**
 * @brief Calls a function for each element of a list
 * @param list[in] : a list
 * @param func[in] : the function to call with each element's data
 * @param user_data[in] : user data to pass to the function
 */
void s_list_foreach(struct s_list *list, s_foreach_cbk func,
  void *user_data);

/**
 * @brief Get the first element in a list
 * @param list[in] : a list
 * @return the first element in the list, or NULL if the list has no elements
 */
struct s_list *s_list_first(struct s_list *list);

/**
 * @brief Get the last element in a list
 * @param list[in] : a list
 * @return the last element in the list, or NULL if the list has no elements
 */
struct s_list *s_list_last(struct s_list *list);

/**
 * @brief Gets the element at the given position in a list. This iterates over
 * the list until it reaches the n -th position.
 * @param list[in] : a list, this must be the top of the list
 * @param nth[in] : the position of the element, counting from 0
 * @return the element, or NULL if the position is off the end of the list
 */
struct s_list *s_list_nth(struct s_list *list, uint32_t nth);

/**
 * @brief Finds the element in a list which contains the given data.
 * @param list[in] : a list
 * @param data[in] : the element data to find
 * @return the found list element, or NULL if it is not found
 */
struct s_list *s_list_find(struct s_list *list, void *data);

#endif /* !_DAEMON_LIST_H_ */
