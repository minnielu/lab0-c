#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"
#include "list.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
/*創建並返回一個指向struct list_head類型的指針*/
{
    struct list_head *new_head = malloc(sizeof(struct list_head));
    /*分配足夠的記憶體來儲存一個struct list_head結構體的大小，並將返回的指針賦給new_qhead*/
    if (!new_head)
        return NULL;
    INIT_LIST_HEAD(new_head);
    return new_head;
    return NULL;
}

/* Free all storage used by queue */
void q_free(struct list_head *head) {
    if (!head)
        return;
    element_t *entry, *safe;
    /*entry：用來指向當前正在遍歷的queue node*/
    /*safe：用來存下個節點的指針，這樣在刪除當前節點時不會丟失list的連接*/
    list_for_each_entry_safe (entry, safe, head, list) {
        free(entry->value);
        free(entry);
    }
    free(head);
    return;
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    /*head：指向鏈表頭的指針（struct list_head），s：要插入元素的字符串。*/
    if (!head)
        return false;
    element_t *new_ele = malloc(sizeof(element_t));/*分配一塊內存來存放新的element_t，並將該內存的地址給 new_ele*/
    if (!new_ele)
        return false;
    INIT_LIST_HEAD(&new_ele->list);/*初始化 new_ele 的 list*/
    new_ele->value = strdup(s);
    if (!new_ele->value) {
        free(new_ele);
        return false;
    }
    list_add(&new_ele->list, head);/*list_add用來將new_ele->list插入到linked list的頭部*/
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new_ele = malloc(sizeof(element_t));
    if (!new_ele)
        return false;

    INIT_LIST_HEAD(&new_ele->list);
    new_ele->value = strdup(s);
    if (!new_ele->value) {
        free(new_ele);
        return false;
    }
    list_add_tail(&new_ele->list, head);/*list_add用來將new_ele->list插入到linked list的tail*/
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
/*char *sp: 用來接收刪除元素的值、size_t bufsize: 表示緩衝區的大小，用來限制複製的最大字元數*/
{
    if (!head || list_empty(head))/*檢查傳入的 head 是否為空*/
        return NULL;

    element_t *f = list_first_entry(head, element_t, list);/*list_first_entry 用來獲取queue中的第一個元素*/
    list_del(&f->list);/*list_del將元素從queue中移除，但不會釋放它的記憶體*/

    if (sp) {
        size_t copy_size =
            strlen(f->value) < (bufsize - 1) ? strlen(f->value) : (bufsize - 1);
        /*strlen(f->value) 計算元素值的長度，並將其與 bufsize-1 作比較，確保不會超過緩衝區的大小*/
        strncpy(sp, f->value, copy_size);/*strncpy將f->value複製到sp中，最多複製copy_size字元*/
        sp[copy_size] = '\0';
    }
    return f;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *f = list_last_entry(head, element_t, list);/*list_first_entry 用來獲取queue中的最後一個元素*/
    list_del(&f->list);/*list_del將元素從queue中移除，但不會釋放它的記憶體*/

    if (sp) {
        size_t copy_size =
            strlen(f->value) < (bufsize - 1) ? strlen(f->value) : (bufsize - 1);
        /*strlen(f->value) 計算元素值的長度，並將其與 bufsize-1 作比較，確保不會超過緩衝區的大小*/
        strncpy(sp, f->value, copy_size);/*strncpy將f->value複製到sp中，最多複製copy_size字元*/
        sp[copy_size] = '\0';
    }
    return f;
}


/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int size = 0;
    struct list_head *cur;
    list_for_each (cur, head)
        size++;
    return size;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;

    struct list_head *first = head->next;
    struct list_head *second = head->prev;
    while ((first != second) && (first->next != second)) {
        first = first->next;
        second = second->prev;
    }
    element_t *node = list_entry(first, element_t, list);
    list_del(first);
    free(node->value);
    free(node);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;

    bool dup = false;
    element_t *cur, *tmp;
    list_for_each_entry_safe (cur, tmp, head, list) {
        if (&tmp->list != head && !strcmp(cur->value, tmp->value)) {
            list_del(&cur->list);
            free(cur->value);
            free(cur);
            dup = true;
        } else if (dup) {
            list_del(&cur->list);
            free(cur->value);
            free(cur);
            dup = false;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head))
        return;

    struct list_head *first, *second;
    list_for_each_safe (first, second, head) {
        if (second == head)
            break;
        first->prev->next = second;
        second->prev = first->prev;
        first->next = second->next;
        first->prev = second;
        second->next->prev = first;
        second->next = first;

        second = first->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        node->next = node->prev;
        node->prev = safe;
    }
    node->next = node->prev;
    node->prev = safe;
    return;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head)
        return;

    int times = q_size(head) / k;
    struct list_head *tail;

    LIST_HEAD(tmp);
    LIST_HEAD(new_head);

    for (int i = 0; i < times; i++) {
        int j = 0;
        list_for_each (tail, head) {
            if (j >= k)
                break;
            j++;
        }
        list_cut_position(&tmp, head, tail->prev);
        q_reverse(&tmp);
        list_splice_tail_init(&tmp, &new_head);
    }
    list_splice_init(&new_head, head);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
