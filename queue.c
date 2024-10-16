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

    int size = 0;/*用來存節點數量*/
    struct list_head *cur;/*cur 是當前節點指針*/
    list_for_each (cur, head)
        size++;
    return size;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;/*true或false表示是否成功刪除中間節點*/

    struct list_head *first = head->next;
    struct list_head *second = head->prev;
    /*初始化兩個指標，first 從頭部開始，second 從尾部開始*/
    while ((first != second) && (first->next != second)) {
        first = first->next;
        second = second->prev;
    }
    element_t *node = list_entry(first, element_t, list);
    /*利用list_entry來獲取first所指的節點對應的element_t結構*/
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

    bool dup = false;/*用來記錄當前是否存在重複的節點*/
    element_t *cur, *tmp;/*cur 和 tmp 分別指向當前節點和下一個節點*/
    list_for_each_entry_safe (cur, tmp, head, list) {
        /*檢查當前cur和下一個tmp是否相同。如果是則刪除操作。*/
        if (&tmp->list != head && !strcmp(cur->value, tmp->value)) {
            list_del(&cur->list);
            free(cur->value);
            free(cur);
            dup = true;
        } else if (dup) {
            /*如果在前面找到過重複節點，並且當前節點沒有與下一個節點相同，則繼續刪除*/
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

    struct list_head *first, *second;/*first是當前節點，second是下一個節點*/
    list_for_each_safe (first, second, head) {
        if (second == head)
            break;/*如果second到了頭，沒有成對的節點需要交換*/
        first->prev->next = second;/*first的前一個節點的next指向second*/
        second->prev = first->prev;/*second的prev指向first的前一個節點*/
        first->next = second->next;/*first的next指向second的下一個節點*/
        first->prev = second;/*first的prev設置為second，這樣second成為first的前一個節點*/
        second->next->prev = first;/*second->next的前一個指針設置為first*/
        second->next = first;/*second的next指向first，完成交換*/

        second = first->next;/*second指向first的下一個節點，進行下一對*/
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {/*node 是當前節點，safe 是下一個節點*/
        node->next = node->prev;/*將當前節點node的next指針設置為它的前一個節點*/
        node->prev = safe;/*將當前節點的prev指針設置為下一個節點safe*/
    }
    /*迴圈完node會指向鏈表的最後一個節點，再進行一次反轉指針操作*/
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

    /*計算能夠反轉的組數。只有當鏈表中的元素數量達到k的倍數時，才能反轉這些節點*/
    int times = q_size(head) / k;
    struct list_head *tail;

    LIST_HEAD(tmp);/*用來暫時存每組反轉的節點*/
    LIST_HEAD(new_head);/*用來存最終結果的*/

    for (int i = 0; i < times; i++) {
        int j = 0;
        list_for_each (tail, head) {/*找到當前這一組的結尾節點*/
            if (j >= k)
                break;
            j++;
        }
        list_cut_position(&tmp, head, tail->prev);/*將當前這一組k個節點剪出放入tmp*/
        q_reverse(&tmp);
        list_splice_tail_init(&tmp, &new_head);/*將反轉後的這組接到最終結果的new_head中*/
    }
    list_splice_init(&new_head, head);/*將已經反轉的節點和鏈表中剩的未反轉節點接起來*/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_is_singular(head) || list_empty(head))
        return;

    head->prev->next = NULL;/*從頭部斷開，只處理從head->next開始的節點，不包含head本身*/
    head->next = MergeSort(head->next, descend);/*MergeSort進行排序，descend決定是升還是降*/

    /*prev重新設置*/
    struct list_head *front = head, *cur = head->next;
    while (cur) {
        cur->prev = front;
        // front = cur;
        cur = cur->next;
        front = front->next;
    }
    /*list重新閉合*/
    front->next = head;
    head->prev = front;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    element_t *cur = list_entry(head->prev->prev, element_t, list);/*cur指向倒數第二個節點，因為要比較每個節點與右邊的節點值*/
    element_t *small = list_entry(head->prev, element_t, list);/*small指向最後一個節點，初始認為這個節點是最大的節點*/
    while (&cur->list != head) {
        /*small->value大於等於cur->value，不需要刪除。更新small為cur，移動到下一個節點比較*/
        /*cur的值比右邊的節點值大，用list_del刪除這個節點，釋放內存。將cur更新為下一個要比較的節點*/
        if (strcmp(small->value, cur->value) >= 0) {
            small = cur;
            cur = list_entry(cur->list.prev, element_t, list);
        } else {
            list_del(&cur->list);
            q_release_element(cur);
            cur = list_entry(small->list.prev, element_t, list);
        }
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    element_t *cur = list_entry(head->prev->prev, element_t, list);
    element_t *large = list_entry(head->prev, element_t, list);
    while (&cur->list != head) {
        if (strcmp(large->value, cur->value) <= 0) {
            large = cur;
            cur = list_entry(cur->list.prev, element_t, list);
        } else {
            list_del(&cur->list);
            q_release_element(cur);
            cur = list_entry(large->list.prev, element_t, list);
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;
    else if (list_is_singular(head))/*如果鏈表只有一個節點不用合併*/
        return q_size(list_first_entry(head, queue_contex_t, chain)->q);
    int size = q_size(head);/*計算兩兩合併需要的次數。如果是奇數，則會多出一個需要額外處理*/
    int count = (size % 2) ? size / 2 + 1 : size / 2;
    int queue_size = 0;
    for (int i = 0; i < count; i++) {
        queue_contex_t *first = list_first_entry(head, queue_contex_t, chain);
        queue_contex_t *second =
            list_entry(first->chain.next, queue_contex_t, chain);
        /*使用 __merge 函式來合併*/
        /*合併完second會被移動到尾部，然後first和second更新為下一對繼續進行合併*/
        while (!list_empty(first->q) && !list_empty(second->q)) {
            queue_size = __merge(first->q, second->q);
            list_move_tail(&second->chain, head);
            first = list_entry(first->chain.next, queue_contex_t, chain);
            second = list_entry(first->chain.next, queue_contex_t, chain);
        }
    }
    return queue_size;
}
