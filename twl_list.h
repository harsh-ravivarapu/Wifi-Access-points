// twl_list.h 

#include "datatypes.h"   // defines data_t 

#define TWL_LIST_FRONT -2023
#define TWL_LIST_BACK  -914

typedef struct ll_node_tag {
    // twl_list.c private members 
    mydata_t *data_ptr;
    struct ll_node_tag *prev;
    struct ll_node_tag *next;
} ll_node_t;

typedef struct twl_list_tag {
    // twl_list.c private members
    ll_node_t *ll_front;
    ll_node_t *ll_back;
    ll_node_t *ll_rover;
    int ll_count;
    int ll_is_sorted;
    // twl_list.c private procedure for sorted insert 
    int (*ll_comp_function)(const mydata_t *, const mydata_t *);
} twl_list_t;

/* public prototype definitions */

/* build and cleanup lists */
twl_list_t *twl_list_construct(int (*fcomp)(const mydata_t *, const mydata_t *));
void twl_list_destruct(twl_list_t *list_ptr);

mydata_t * twl_list_access(twl_list_t *list_ptr, int pos_index);
mydata_t * twl_list_elem_find_data_ptr(twl_list_t *list_ptr, mydata_t *elem_ptr,
        int (*fcomp)(const mydata_t *, const mydata_t *));
int twl_list_elem_find_position(twl_list_t *list_ptr, mydata_t *elem_ptr,
        int (*fcomp)(const mydata_t *, const mydata_t *));

void twl_list_insert(twl_list_t *list_ptr, mydata_t *elem_ptr, int pos_index);
void twl_list_insert_sorted(twl_list_t *list_ptr, mydata_t *elem_ptr);

mydata_t * twl_list_remove(twl_list_t *list_ptr, int pos_index);

int twl_list_size(twl_list_t *list_ptr);
void twl_list_sort(twl_list_t *list_ptr, int sort_type, int (*fcomp)(const mydata_t *, const mydata_t *));
void twl_mark_the_list_unsorted(twl_list_t *list_ptr);

//sorting algoritms
void InsertionSort(twl_list_t *list_ptr, twl_list_t *sorted_list, int (*fcomp)(const mydata_t *, const mydata_t *));
void RecursiveSelectionSort(twl_list_t* list_ptr, ll_node_t* start, ll_node_t* end,  int (*fcomp)(const mydata_t *, const mydata_t *));
void IterativeSelectionSort(twl_list_t* list_ptr, ll_node_t* start, ll_node_t* end,  int (*fcomp)(const mydata_t *, const mydata_t *));

void MergeSort(twl_list_t *list_ptr, int (*fcomp)(const mydata_t *, const mydata_t *));
/* commands for vim. ts: tabstop, sts: soft tabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */
