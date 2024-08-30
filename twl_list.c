// twl_list.c              
 

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "twl_list.h"   // defines public functions for two-way linked list ADT

#define TRUE  1
#define FALSE 0

// prototypes for private functions used in twl_list.c only
void list_debug_validate(twl_list_t *L);

/* ----- below are the functions  ----- */

/* Allocates a new, empty list 
 *
 * If the comparison function is NULL, then the list is unsorted.
 *
 * Otherwise, the list is initially assumed to be sorted.  Note that if 
 * twl_list_insert is used the list is changed to unsorted.  
 *
 * Use linked_destruct to remove and deallocate all elements on a list 
 * and the header block.
 *
 * (This function is written and no changes needed. It provides an example
 *  of how save the comparison function pointer.  See other examples in this
 *  file.)
 */
twl_list_t *twl_list_construct(int (*compare_function)(const mydata_t *, const mydata_t *))
{
    twl_list_t *L;
    L = (twl_list_t *)malloc(sizeof(twl_list_t));
        L->ll_front = NULL;
        L->ll_back = NULL;
        L->ll_count = 0;
        L->ll_comp_function = compare_function;
        
        if (compare_function == NULL) {
            L->ll_is_sorted = FALSE;
        } else {
            L->ll_is_sorted = TRUE;
        };
    /* the last line of this function must call validate */
    //list_debug_validate(L);
        return L;
    }
/* Deallocates the contents of the specified list, releasing associated memory
 * resources for other purposes.
 *
 * Free all elements in the list and the header block.
 */
void twl_list_destruct(twl_list_t *list_ptr)
    /* the first line must validate the list */
{
    //list_debug_validate(list_ptr);
    assert(list_ptr != NULL);

    // Free all elements in the list and the header block

    while (list_ptr->ll_front != NULL) {
        ll_node_t *temp = list_ptr->ll_front;
        list_ptr->ll_front = list_ptr->ll_front->next;
        free(temp->data_ptr);
        free(temp);
    }

    free(list_ptr);
}


/* Obtains a pointer to an element stored in the specified list, at the
 * specified list position
 * 
 * list_ptr: pointer to list-of-interest.  A pointer to an empty list is
 *           obtained from twl_list_construct.
 *
 * pos_index: position of the element to be accessed.  Index starts at 0 at
 *            front of the list, and incremented by one until the back is
 *            reached.  Can also specify TWL_LIST_FRONT and TWL_LIST_BACK
 *
 * return value: pointer to the mydata_t element accessed in the list at the
 * index position.  A value NULL is returned if the pos_index does not 
 * correspond to an element in the list.
 */
mydata_t * twl_list_access(twl_list_t *list_ptr, int pos_index)
{
    assert(list_ptr != NULL);
    /* debugging function to verify that the structure of the list is valid */
   // list_debug_validate(list_ptr);

    /* handle special cases.
     *   1.  The list is empty
     *   2.  Asking for the front 
     *   3.  Asking for the back 
     *   4.  specifying a position that is out of range.  This is not defined
     *       to be an error in this function, but instead it is assumed the 
     *       calling function is looping to find an end of the list 
     */
    // the list is empty
    if (list_ptr->ll_count == 0) {
        return NULL;
    }
    else if (pos_index == TWL_LIST_FRONT || pos_index == 0) {
        return list_ptr->ll_front->data_ptr;
    }
    else if (pos_index == TWL_LIST_BACK || pos_index == list_ptr->ll_count - 1) {
        return list_ptr->ll_back->data_ptr;
    }
    else if (pos_index < 0 || pos_index >= list_ptr->ll_count){
        return NULL;   // does not correspond to position in list
}

ll_node_t *current = list_ptr->ll_front;
int position = 0;

   while (current != NULL && position < pos_index) {
       current = current->next;
       position++;
   }
// Check if the position was found and return the data pointer
if (current != NULL) {
    return current->data_ptr;
} else {
    // Return NULL if the position does not correspond to an element in the list
    return NULL;
}
}


/* Finds an element in a list and returns a pointer to the mydata_t memory
 * block.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * elem_ptr: element against which other elements in the list are compared.
 *
 * compare_function: function to test if elem_ptr is a match to an item in the list.
 *        Returns 0 if they match and any value not equal to 0 if they do not match.
 *
 * The function returns a pointer to the mydata_t memory block for the ll_node_t 
 * that contains the first matching element if a match if found.  If a match is 
 * not found the return value is NULL.
 */
mydata_t * twl_list_elem_find_data_ptr(twl_list_t *list_ptr, mydata_t *elem_ptr,
        int (*compare_function)(const mydata_t *, const mydata_t *))
{
    assert(list_ptr != NULL);
    assert(compare_function != NULL);
    
    //list_debug_validate(list_ptr);

    ll_node_t *current = list_ptr->ll_front;

    while (current != NULL) {
        if (compare_function(current->data_ptr, elem_ptr) == 0) {
            return current->data_ptr;
        }
        current = current->next;
    }

    return NULL;
}
/* Similar to twl_list_elem_find_data_ptr, this function finds an element in 
 * a list.  However, the return value is the integer position of the matching 
 * element with the lowest index.  If a matching element is not found, the 
 * position index that is returned should be -1. 
 *
 */
int twl_list_elem_find_position(twl_list_t *list_ptr, mydata_t *elem_ptr,
        int (*compare_function)(const mydata_t *, const mydata_t *))
{
    assert(list_ptr != NULL);
    assert(compare_function != NULL);
    
    //list_debug_validate(list_ptr);

    ll_node_t *current = list_ptr->ll_front;
    int position = 0;

    while (current != NULL) {
        if (compare_function(current->data_ptr, elem_ptr) == 0) {
            return position;
        }
        current = current->next;
        position++;
    }

    return -1; // Element not found
}

/* Inserts the data element into the specified list at the specified
 * position.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * elem_ptr: pointer to the memory block to be inserted into list.
 *
 * pos_index: numeric position index of the element to be inserted into the 
 *            list.  Index starts at 0 at front of the list, and incremented by 
 *            one until the back is reached.  The index can also be equal
 *            to TWL_LIST_FRONT or TWL_LIST_BACK (these are special negative 
 *            values use to provide a short cut for adding to the front 
 *            or back of the list).
 *
 * If pos_index is greater than the number of elements currently in the list, 
 * the element is simply appended to the end of the list (no additional elements
 * are inserted).
 *
 * Note that use of this function results in the list to be marked as unsorted,
 * even if the element has been inserted in the correct position.  That is, on
 * completion of this subroutine the list_ptr->ll_is_sorted must be equal 
 * to FALSE.
 */
void twl_list_insert(twl_list_t *list_ptr, mydata_t *elem_ptr, int pos_index)
{
    assert(list_ptr != NULL);

    // Create a new node
    ll_node_t *new_node = (ll_node_t *)malloc(sizeof(ll_node_t));
    new_node->data_ptr = elem_ptr;
    new_node->prev = NULL;
    new_node->next = NULL;

    if (list_ptr->ll_count == 0) {
        // List is empty, insert as the only element
        list_ptr->ll_front = new_node;
        list_ptr->ll_back = new_node;
    } else if (pos_index == TWL_LIST_FRONT || pos_index == 0) {
        // Insert at the front
        new_node->next = list_ptr->ll_front;
        list_ptr->ll_front->prev = new_node;
        list_ptr->ll_front = new_node;
    } else if (pos_index == TWL_LIST_BACK || pos_index >= list_ptr->ll_count) {
        // Insert at the back
        new_node->prev = list_ptr->ll_back;
        list_ptr->ll_back->next = new_node;
        list_ptr->ll_back = new_node;
    } else {
        // Insert at a specific position
        ll_node_t *current = list_ptr->ll_front;
        int index = 0;

        while (current != NULL && index < pos_index) {
            current = current->next;
            index++;
        }

        new_node->prev = current->prev;
        new_node->next = current;
        if (current->prev != NULL) {
            current->prev->next = new_node;
        }
        current->prev = new_node;
    }

    list_ptr->ll_count++;
    list_ptr->ll_is_sorted = FALSE; // Mark the list as unsorted
    list_debug_validate(list_ptr);
}

/* Inserts the element into the specified sorted list at the proper position,
 * as defined by the ll_comp_function.  This function is defined in the header:
 *     list_ptr->ll_comp_function(A, B)
 *
 * The comparison procedure must accept two arguments (A and B) which are both
 * pointers to elements of type mydata_t.  The comparison procedure returns an
 * integer code which indicates the precedence relationship between the two
 * elements.  The integer code takes on the following values:
 *    1: A should be closer to the front of the list than B
 *   -1: B should be closer to the front of the list than A
 *    0: A and B are equal in rank
 *
 * If the element to be inserted is equal in rank to an element already
 * in the list, the newly inserted element will be placed _after all_ the
 * elements of equal rank that are already in the list.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * elem_ptr: pointer to the element to be inserted into list.
 *
 * If you use twl_list_insert_sorted, the list preserves its sorted nature.
 *
 * If you use twl_list_insert, the list will be considered to be unsorted, even
 * if the element has been inserted in the correct position.
 *
 * If the list is not sorted and you call twl_list_insert_sorted, this subroutine
 * must generate a system error and the program should immediately stop.
 *
 */
void twl_list_insert_sorted(twl_list_t *list_ptr, mydata_t *elem_ptr)
{
    assert(list_ptr != NULL);
    assert(list_ptr->ll_is_sorted == TRUE);

    /* Create a new node for the new element */
    ll_node_t *new_node = (ll_node_t *)malloc(sizeof(ll_node_t));
    new_node->data_ptr = elem_ptr;
    new_node->prev = NULL;
    new_node->next = NULL;

    /* Handle the case of an empty list */
    if (list_ptr->ll_count == 0) {
        list_ptr->ll_front = new_node;
        list_ptr->ll_back = new_node;
        list_ptr->ll_count++;
        return; // No need for further processing
    }

    /* Iterate through the list to find the proper position */
    ll_node_t *current = list_ptr->ll_front;
    ll_node_t *previous = NULL;

    while (current != NULL && list_ptr->ll_comp_function(elem_ptr, current->data_ptr) < 0) {
        previous = current;
        current = current->next;
    }

    /* Insert the new element before the existing element */
    if (previous != NULL) {
        previous->next = new_node;
        new_node->prev = previous;
    } else {
        /* Insert at the front */
        new_node->next = list_ptr->ll_front;
        list_ptr->ll_front->prev = new_node;
        list_ptr->ll_front = new_node;
    }

    if (current != NULL) {
        current->prev = new_node;
        new_node->next = current;
    } else {
        /* Insert at the back */
        list_ptr->ll_back->next = new_node;
        new_node->prev = list_ptr->ll_back;
        list_ptr->ll_back = new_node;
    }

    /* Update the list count */
    list_ptr->ll_count++;

    
   list_debug_validate(list_ptr);
}




/* Removes the element from the specified list that is found at the 
 * specified list position.  A pointer to the data element is returned.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * pos_index: position of the element to be removed.  Index starts at 0 at
 *            front of the list, and incremented by one until the back is
 *            reached.  Can also specify TWL_LIST_FRONT and TWL_LIST_BACK
 *
 * Attempting to remove an element at a position index that is not contained in
 * the list will result in no element being removed, and a NULL pointer will be
 * returned.
 */
mydata_t *twl_list_remove(twl_list_t *list_ptr, int pos_index)
{
    assert(list_ptr != NULL);

    //list_debug_validate(list_ptr);
    if (pos_index == TWL_LIST_FRONT){
        pos_index = 0;}
       
       else if (pos_index == TWL_LIST_BACK){
       pos_index=list_ptr->ll_count - 1;}
        

    if (list_ptr->ll_count == 0 || pos_index < 0 || pos_index >= list_ptr->ll_count) {
        // List is empty or the specified position is out of range
        return NULL;
    }

    mydata_t *removed_data = NULL;

    if (pos_index == TWL_LIST_FRONT || pos_index == 0) {
        // Remove from the front
        ll_node_t *temp = list_ptr->ll_front;
        removed_data = temp->data_ptr;
        list_ptr->ll_front = temp->next;
        if (list_ptr->ll_front != NULL) {
            list_ptr->ll_front->prev = NULL;
        } else {
            // The list is now empty
            list_ptr->ll_back = NULL;
        }
        free(temp);
        temp = NULL; // Set the freed pointer to NULL
    } else if (pos_index == TWL_LIST_BACK || pos_index == list_ptr->ll_count - 1) {
        // Remove from the back
        ll_node_t *temp = list_ptr->ll_back;
        removed_data = temp->data_ptr;
        list_ptr->ll_back = temp->prev;
        if (list_ptr->ll_back != NULL) {
            list_ptr->ll_back->next = NULL;
        } else {
            // The list is now empty
            list_ptr->ll_front = NULL;
        }
        free(temp);
        temp = NULL; // Set the freed pointer to NULL
    } else {
        // Remove from a specific position
        ll_node_t *current = list_ptr->ll_front;
        int index = 0;

        while (current != NULL && index < pos_index) {
            current = current->next;
            index++;
        }

        if (current != NULL) {
            removed_data = current->data_ptr;
            current->prev->next = current->next;
            if (current->next != NULL) {
                current->next->prev = current->prev;
            }
            free(current);
            current = NULL; // Set the freed pointer to NULL
        }
    }

    if (removed_data != NULL) {
        list_ptr->ll_count--;
    }

    //list_debug_validate(list_ptr);
    return removed_data;
}




/* Obtains the length of the specified list, that is, the number of elements
 * that the list contains.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * Returns an integer equal to the number of elements stored in the list.  An
 * empty list has a size of zero.
 *
 * (This function is already written, so no changes necessary.)
 */
int twl_list_size(twl_list_t *list_ptr)
{
    assert(list_ptr != NULL);
    assert(list_ptr->ll_count >= 0);
    return list_ptr->ll_count;
}



//sorting funtion (twl_sort)
void twl_list_sort(twl_list_t *list_ptr, int sort_type, int (*fcomp)(const mydata_t *, const mydata_t *)){
    // Check if the list is empty or contains only one element (no need to sort)
    list_ptr->ll_comp_function = fcomp;
    ll_node_t *start = list_ptr->ll_front;
    ll_node_t *end = list_ptr->ll_back;
    if (twl_list_size(list_ptr) <= 1) {
        return;
    }

    // Initialize a new sorted list
    twl_list_t *sorted_list = twl_list_construct(fcomp);

    // Implement different sorting algorithms based on sort_type
    if (sort_type == 1) {
        InsertionSort(list_ptr, sorted_list, fcomp);
        *list_ptr = *sorted_list;
    } else if (sort_type == 2) {
        RecursiveSelectionSort(list_ptr, start, end, fcomp);
    } else if (sort_type == 3) {
         IterativeSelectionSort(list_ptr, start, end, fcomp);
    } else if (sort_type == 4) {
        MergeSort(list_ptr, fcomp);   
    } else {
        // Invalid sort_type
        printf("Invalid sort_type: %d\n", sort_type);
    }

    // Update the original list to point to the sorted list
    free(sorted_list);// Free the temporary sorted list structure
    list_ptr->ll_is_sorted = TRUE;
    list_debug_validate(list_ptr);
}




// Function for marking the list as unsorted
void twl_mark_the_list_unsorted(twl_list_t *list) {
    if (list != NULL) {
        list->ll_is_sorted = FALSE;
    }
}


/* This function verifies that the pointers for the two-way linked list are
 * valid, and that the list size matches the number of items in the list.
 *
 * If the linked list is sorted it also checks that the elements in the list
 * appear in the proper order.
 *
 * The function produces no output if the two-way linked list is correct.  It
 * causes the program to terminate and print a line beginning with "Assertion
 * failed:" if an error is detected.
 *
 * The checks are not exhaustive, so an error may still exist in the
 * list even if these checks pass.
 *
 * YOU MUST NOT CHANGE THIS FUNCTION.  WE USE IT DURING GRADING TO VERIFY THAT
 * YOUR LIST IS CONSISTENT.
 */
void list_debug_validate(twl_list_t *L)
{
    ll_node_t *N;
    int count = 0;
    assert(NULL != L); 
    if (NULL == L->ll_back) 
	assert(NULL == L->ll_front && 0 == L->ll_count);
    else
	assert(NULL == L->ll_back->next);
    if (L->ll_front != NULL) 
	assert(L->ll_front->prev == NULL);
    else
	assert(L->ll_back == NULL && L->ll_count == 0);
    if (0 == L->ll_count) assert(NULL == L->ll_front && NULL == L->ll_back);
    if (L->ll_front == L->ll_back && L->ll_back != NULL) assert(L->ll_count == 1);
    if (1 == L->ll_count) {
        assert(L->ll_front == L->ll_back && L->ll_back != NULL);
        assert(NULL == L->ll_back->next && NULL == L->ll_back->prev);
        assert(NULL != L->ll_back->data_ptr);
    }
    assert(L->ll_is_sorted == TRUE || L->ll_is_sorted == FALSE);
    if (1 < L->ll_count) {
        assert(L->ll_front != L->ll_back && NULL != L->ll_front && NULL != L->ll_back);
        N = L->ll_back;
        while (N != NULL) {
            assert(NULL != N->data_ptr);
            if (NULL != N->prev) assert(N->prev->next == N);
            else assert(N == L->ll_front);
            count++;
            N = N->prev;
        }
        assert(count == L->ll_count);
    }
    if (L->ll_is_sorted && NULL != L->ll_front) {
        N = L->ll_front;
        while (N->next != NULL) {
           assert(L->ll_comp_function(N->data_ptr, N->next->data_ptr) != -1);
            N = N->next;
        }
    }
}

// insertion sort
void InsertionSort(twl_list_t *list_ptr, twl_list_t *sorted_list, int (*fcomp)(const mydata_t *, const mydata_t *)) {
    while (twl_list_size(list_ptr) > 0) {
        mydata_t *elem = twl_list_remove(list_ptr, 0); // Remove the first element from the unsorted list
        twl_list_insert_sorted(sorted_list, elem); // Insert it into the sorted list
    }
}


//find max function
ll_node_t* FindMax(twl_list_t *list_ptr, ll_node_t *start, ll_node_t *end, int (*fcomp)(const mydata_t *, const mydata_t *)) 
{
    ll_node_t* i = start; 
    ll_node_t* j = start; 


    do 
    {
        i = i->next; 

        
        if (i != NULL && fcomp(i->data_ptr, j->data_ptr) > 0)  
        {
            j = i; 
        }
    } 
    while(i != end && i != NULL); 

    return j; 
}



void RecursiveSelectionSort(twl_list_t* list_ptr, ll_node_t* start, ll_node_t* end, int (*fcomp)(const mydata_t *, const mydata_t *) )
{
    if(start != end) 
    {   
        ll_node_t* MaxPosition = FindMax(list_ptr,start, end, fcomp);
        mydata_t *temp = start->data_ptr;
        start->data_ptr = MaxPosition->data_ptr;
        MaxPosition->data_ptr = temp;

        RecursiveSelectionSort(list_ptr, start->next, end, fcomp);
    }
}



void IterativeSelectionSort(twl_list_t* list_ptr, ll_node_t* start, ll_node_t* end, int (*fcomp)(const mydata_t *, const mydata_t *))
{
    ll_node_t* current = start;

    do 
    {
        // Find the maximum position in the remaining unsorted list using FindMax
        ll_node_t* MaxPosition = FindMax(list_ptr, current, end, fcomp);

        // Swap the data values of nodes current and MaxPosition
        mydata_t *temp = current->data_ptr;
        current->data_ptr = MaxPosition->data_ptr;
        MaxPosition->data_ptr = temp;

        // Move to the next node
        current = current->next;
    } 
    while (current != NULL && current != end->next);
}


//merge sort
void MergeSort(twl_list_t *list_ptr, int (*fcomp)(const mydata_t *, const mydata_t *)) {
    // Base case: If the list has one element or is empty, it's already sorted
    if (twl_list_size(list_ptr) <= 1) {
       return;
    }

    int mid = twl_list_size(list_ptr) / 2;

    // Create LeftList containing the first half of the elements
    twl_list_t *LeftList = twl_list_construct(fcomp);
    for (int i = 0; i < mid; i++) {
        mydata_t *data = twl_list_remove(list_ptr, 0);
        twl_list_insert(LeftList, data, twl_list_size(LeftList));
    }

    // Create RightList containing the second half of the elements
    twl_list_t *RightList = twl_list_construct(fcomp);
    while (twl_list_size(list_ptr) != 0) {
        mydata_t *data = twl_list_remove(list_ptr, 0);
        twl_list_insert(RightList, data, twl_list_size(RightList));
    }


// Recursively sort LeftList and RightList
    MergeSort(LeftList, fcomp);
    MergeSort(RightList, fcomp);

// Merge the sorted LeftList and RightList back into the SortedList
    while (twl_list_size(LeftList) != 0 && twl_list_size(RightList) != 0) {
        mydata_t *left_data = twl_list_access(LeftList, 0);
        mydata_t *right_data = twl_list_access(RightList, 0);

        if (fcomp(left_data, right_data) >= 0) {
            mydata_t *data = twl_list_remove(LeftList, 0);
            twl_list_insert(list_ptr, data, twl_list_size(list_ptr));
        } else {
            mydata_t *data = twl_list_remove(RightList, 0);
            twl_list_insert(list_ptr, data, twl_list_size(list_ptr));
        }
    }

// Add any remaining elements from LeftList or RightList
    while (twl_list_size(LeftList) > 0) {
        mydata_t *data = twl_list_remove(LeftList, 0);
        twl_list_insert(list_ptr, data, twl_list_size(list_ptr));
    }

    while (twl_list_size(RightList) > 0) {
        mydata_t *data = twl_list_remove(RightList, 0);
        twl_list_insert(list_ptr, data, twl_list_size(list_ptr));
    }

    // Destruct the temporary lists
    twl_list_destruct(LeftList);
    twl_list_destruct(RightList);
}


/* commands for vim. ts: tabstop, sts: softtabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */

