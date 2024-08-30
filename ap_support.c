// ap_support.c 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "twl_list.h"
#include "ap_support.h"

/* ap_rank_aps is required by the linked list ADT for sorted lists. 
 *
 * This function returns 
 *     1 if rec_a should be closer to the front than rec_b,
 *    -1 if rec_b is to be considered closer to the front, and 
 *     0 if the records are equal.
 *
 * For the AP data we want to sort from largest mobile count down, so
 * closer to the front means a larger mobile count address.
 *
 * Also, if the mobile count is tied then the record with the smaller
 * eth_address is closer to the front.
 *
 * The function expects pointers to two record structures, and it is an error
 * if either is NULL
 *
 * THIS FUNCTION SHOULD NOT BE CHANGED
 */
int ap_rank_aps(const ap_info_t *record_a, const ap_info_t *record_b)
{
    assert(record_a != NULL && record_b !=NULL);

    if (record_a->mobile_count > record_b->mobile_count)
        return 1;
    else if (record_a->mobile_count < record_b->mobile_count)
        return -1;
    else {
        // the mobile counts are equal
        if (record_a->eth_address < record_b->eth_address)
            return 1;
        else if (record_a->eth_address > record_b->eth_address)
            return -1;
        else
            return 0;
    }
}

/* ap_match_eth is used by the list element find for searching for an access point. 
 *
 * This function returns 
 *     0 if the records have the same AP id.
 *    -1 otherwise
 *
 * THIS FUNCTION SHOULD NOT BE CHANGED
 */
int ap_match_eth(const ap_info_t *record_a, const ap_info_t *record_b)
{
    assert(record_a != NULL && record_b !=NULL);
    if (record_a->eth_address == record_b->eth_address)
        return 0;
    return -1;
}

/* print the records in a list 
 *
 * This function provides an example on how to iterate through
 * the list using the access function.
 */
void ap_print_list(twl_list_t *list_ptr, const char *type_of_list)
{
    assert(strcmp(type_of_list, "Leaderboard")==0 || strcmp(type_of_list, "Queue")==0);
    ap_info_t *rec_ptr;
    int num_in_list = twl_list_size(list_ptr); // Get the number of records in the list
    int counter = 0;

    if (num_in_list == 0) {
        printf("%s is empty\n", type_of_list);
    } else {
        printf("%s has %d records\n", type_of_list, num_in_list);
        rec_ptr = twl_list_access(list_ptr, counter);
        while (rec_ptr != NULL)
        {
            printf("%d: ", counter);
            ap_print_info(rec_ptr);
            counter++;
            rec_ptr = twl_list_access(list_ptr, counter);
        }
        assert(num_in_list == counter);
    }
    printf("\n");
}

/* This creates a list for storing AP records that are maintained
 * in sorted order based on mobile_count and eth_address.  There are 
 * no records with duplicate ethernet addresses.  
 *
 * Notice that the function pointer for the comparison
 * function is passed in as an agrument.  The comparison function is used 
 * by the sorted insert (and validate) function. 
 *
 */
twl_list_t *ap_create_leaderboard(void)
{
    return twl_list_construct(ap_rank_aps);
}

/* This function adds the AP record to the leaderboard list.  The list is
 * kept in sorted order based on the mobile_acount and eth_address.
 *
 * The order of the list should be first account for the mobile_count in
 * decreasing order.  If there are records with the same mobile_count value, 
 * these should be sorted based on the eth_address in increasing order.  
 * Note that there cannot be any ties for eth_address, because a duplicate 
 * eth_address cannot be added.
 *
 * There are two reasons a record may not be added to the list.  Check in this
 * order:
 *    first: if the list is full
 *    second: if a record already exists with the same eth_address
 *
 * Inputs:
 *     list_ptr: pointer to the list created by the construction function
 *     ap_id: the ethernet address of the AP
 *     max_list_size: the limit of how many records can be stored in the
 *                    leaderboard
 *
 * print one of three outcomes for the add operation:
 *     0: added a record in sorted order.
 *     1: did not insert because list is full.
 *     2: did not insert because eth_address already found in list (and list
 *        is not full)
 */
void ap_add(twl_list_t *list_ptr, int ap_id, int max_list_size)
{
    int add_result = 0; // Initialize add_result
    
    // Create and initialize a new AP record
    ap_info_t *new_record = ap_create_info(ap_id);
    
    // Check if the list is full (max_list_size)
    if (twl_list_size(list_ptr) >= max_list_size) {
        add_result = 1;
        free(new_record); // Free memory allocated for the new record
    } else {
        // Check if a record already exists with the same eth_address in the list
        ap_info_t *existing_record = twl_list_elem_find_data_ptr(list_ptr, new_record, ap_match_eth);
    
        if (existing_record != NULL) {
            add_result = 2;
            free(new_record); // Free memory allocated for the new record
        } else {
            // Insert the new record into the list in sorted order
            twl_list_insert_sorted(list_ptr, new_record);
        }
    }

    // Print message after determining the result of the operation.
    
    if (add_result == 0) {
        printf("Inserted %d\n", ap_id);
    } else if (add_result == 1) {
        printf("Rejected %d because list is full with %d entries\n", ap_id, max_list_size);
    } else if (add_result == 2) {
        printf("Rejected %d already in list\n", ap_id);
    } else {
        printf("Error with return value! Fix your code.\n");
    }
}




/* This function prints the record with the matching eth address in the
 * leaderboard list.  
 */
void ap_find(twl_list_t *list_ptr, int ap_id)
{
    ap_info_t *rec_ptr = NULL;
    // Create a temporary search record
    ap_info_t search_record;
    search_record.eth_address = ap_id;

    // hint: use index_ptr = twl_list_elem_find(list_ptr, comparison_ap_ptr, ap_match_eth);
    //  review driver.c for an example how to use twl_list_elem_find
    //
    //  notice ap_match_eth is a function found in this file
  
        rec_ptr = twl_list_elem_find_data_ptr(list_ptr, &search_record, ap_match_eth);

       if (rec_ptr == NULL) {
           printf("Did not find access point with id: %d\n", ap_id);
       } else {
           // Print information about the found access point
           printf("%d mobiles registered with AP %d\n", rec_ptr->mobile_count, ap_id);
           assert(rec_ptr->eth_address == ap_id); // Verify that the correct record was found
       }
   }


/* This function removes the record from the leaderboard list with the matching
 * AP ethernet address
 *
 * Inputs
 *     list_ptr: pointer to the leaderboard list 
 *     ap_id: eth_address that should be removed from the list
 */
void ap_remove(twl_list_t *list_ptr, int ap_id)
{
    ap_info_t search_record;  // Create a search record with the given ap_id
    search_record.eth_address = ap_id;
    ap_info_t *rec_ptr = NULL;

    // Use twl_list_elem_find_data_ptr with a pointer to the search record
    rec_ptr = twl_list_elem_find_data_ptr(list_ptr, &search_record, ap_match_eth);

    if (rec_ptr == NULL) {
         printf("Remove did not find: %d\n", ap_id);
    } else {
        ap_info_t *removed_record = twl_list_remove(list_ptr, twl_list_elem_find_position(list_ptr, rec_ptr, ap_match_eth));
        
        printf("Removed: %d\n", ap_id);
        ap_print_info(rec_ptr);
        assert(removed_record == rec_ptr);
        // Free the memory of the removed record
        free(removed_record);

        // Set the rec_ptr to NULL to avoid dangling pointer
        rec_ptr = NULL;

        
    }
}



/* Increment the mobile_count field for the access point record with
 * the matching eth_address.  If the record is found, print the new
 * value of mobile_count.  
 *
 * The record that is changed may need to have its position in the
 * list updated (to a lower index position or nearer the front of the list) so
 * the list is alwasy in sorted order.  The sorting order is the same as
 * defined in the ap_add function.
 *
 * Inputs
 *     list_ptr: pointer to the leaderboard 
 *     ap_id: eth_address that should have its mobile count incremented 
 */
void ap_inc(twl_list_t *list_ptr, int ap_id)
{
    ap_info_t comparison_ap;
    comparison_ap.eth_address = ap_id;

    ap_info_t *rec_ptr = twl_list_elem_find_data_ptr(list_ptr, &comparison_ap, ap_match_eth);

    int inc_result = -2;   // get the new mobile_count if found in list
    if (rec_ptr != NULL) {
        // Increment the mobile_count
        rec_ptr->mobile_count++;

        // Remove the old record from the list
        int position_to_remove = twl_list_elem_find_position(list_ptr, &comparison_ap, ap_match_eth);
        twl_list_remove(list_ptr, position_to_remove);

        // Insert the updated record back into the list
        twl_list_insert_sorted(list_ptr, rec_ptr);

        inc_result = rec_ptr->mobile_count; // Update the result
    }
    
    if (inc_result == -2) {
        printf("Increment failed for AP %d because not found\n", ap_id);
    } else if (inc_result > 0) {
        printf("AP %d incremented to %d\n", ap_id, inc_result);
    } else {
        printf("Increment return value %d invalid for AP %d.  Fix your code.\n", inc_result, ap_id);
    }
}

/* Decrement the mobile_count field for the access point record with
 * the matching eth_address.  If the record is found, print the new
 * value of mobile_count.  
 *
 * If the count is already zero it cannot be decremented.
 *
 * The record that is changed may need to have its position in the
 * list updated (to a larger index position or nearer the back of the list) so
 * the list is always in sorted order.  The sorting order is the same as
 * defined in the ap_add function.
 *
 * Inputs
 *     list_ptr: pointer to the leaderboard 
 *     ap_id: eth_address that should have its mobile count decremented 
 *
 */
void ap_dec(twl_list_t *list_ptr, int ap_id) {
    // Find the record with the matching eth_address
    ap_info_t comparison_ap;
    comparison_ap.eth_address = ap_id;

    ap_info_t *rec_ptr = twl_list_elem_find_data_ptr(list_ptr, &comparison_ap, ap_match_eth);

    int dec_result = -2 ; 

    if (rec_ptr != NULL) {
        if (rec_ptr->mobile_count > 0) {
            // Decrement the mobile_count
            rec_ptr->mobile_count--;

            // Remove the old record from the list
            int position_to_remove = twl_list_elem_find_position(list_ptr, &comparison_ap, ap_match_eth);
            twl_list_remove(list_ptr, position_to_remove);

            // Insert the updated record back into the list
            twl_list_insert_sorted(list_ptr, rec_ptr);

            dec_result = rec_ptr->mobile_count; // Update the result
        } else {
            // Count is already zero, cannot decrement further
            dec_result = -1;
        }
    }

    if (dec_result == -2) {
        printf("Decrement for AP %d failed because not found\n", ap_id);
    } else if (dec_result == -1) {
        printf("Decrement for AP %d failed.  Count is already zero\n", ap_id);
    } else if (dec_result >= 0) {
        printf("AP %d decremented to %d\n", ap_id, dec_result);
    } else {
        printf("Decrement return value %d invalid for AP %d. Fix your code.\n", dec_result, ap_id);
    }
}


/* prints size of each of the two lists
 */
void ap_stats(twl_list_t *leaderboard, twl_list_t *queue)
{
    // get the number in list and size of the list
    int num_leaderboard = twl_list_size(leaderboard);
    int num_queue = twl_list_size(queue);
    printf("Leaderboard list records:  %d, ", num_leaderboard);
    printf("Queue list records: %d\n", num_queue);
}


/* Remove all records from the leaderboard.
 */
void ap_removeall(twl_list_t *leaderboard)
{
    // Iterate through the leaderboard and remove each record
    int found = 0;  // how many records are removed
    while (twl_list_size(leaderboard) > 0) {
        ap_info_t *record = twl_list_remove(leaderboard, TWL_LIST_FRONT);
        if (record != NULL) {
            free(record); // Free the memory associated with the removed record
            found++;
        }
    }
     
    if (found == 0) {
        printf("No stations found\n");
    } else {
        printf("Removed %d station%s\n", found, found==1?"":"s");
    }
}


/* This function moves the AP record at the front of the queue and attempts
 * to insert into the leaderboard.
 *
 * Use the identical rules as for ap_add.  Don't insert if the leaderboard 
 * is full or if AP is already in the leaderboard.  If the move is rejected
 * the record is discarded (and not left in the queue). 
 *
 *  Inputs
 *     queue: pointer to the first-in first-out queue 
 *     leaderboard: pointer to the leaderboard list
 *     max_list_size: the limit of how many records can be stored in the
 *                    leaderboard
 *
 * print one of these outcomes for the move operation:
 *     0: there is no AP in queue
 *     1: moved a record from queue to leaderboard in sorted order.
 *     2: did not move because leaderboard is full.
 *     3: did not move because eth_address already found in leaderboard (and
 *        leaderboard is not full)
 *
 */

  void ap_dequeue(twl_list_t *queue, twl_list_t *leaderboard, int max_list_size)
{
    int move_result = -2; // Initialize move_result to an invalid value

    // Check if the queue is empty
    if (twl_list_size(queue) == 0) {
        printf("Queue is empty, no AP moved\n");
        return;
    }

    // Get the record from the front of the queue
    ap_info_t *rec_ptr = (ap_info_t *)twl_list_remove(queue, 0);
    
    // Check if the leaderboard is already full
    if (twl_list_size(leaderboard) >= max_list_size) {
        move_result = 2; // Move rejected due to leaderboard full
        
    } else {
        // Check if the record is already in the leaderboard
        int position_in_leaderboard = twl_list_elem_find_position(leaderboard, rec_ptr, ap_match_eth);
        if (position_in_leaderboard != -1) {
            move_result = 3; // Move rejected due to duplicate in leaderboard
            
        } else {
            // Attempt to insert the record into the leaderboard in sorted order
            twl_list_insert_sorted(leaderboard, rec_ptr);
            move_result = 1; // Moved successfully
        }
    }

    // Remove the first element from the queue
    

    // Determine the appropriate print message based on move_result
    if (move_result == 0) {
        printf("Queue is empty, no AP moved\n");
    } else if (move_result == 1) {
        printf("Moved %d\n", rec_ptr->eth_address);
    } else if (move_result == 2) {
        printf("Move rejected %d because leaderboard is full with %d entries\n", rec_ptr->eth_address, max_list_size);
        free(rec_ptr);

    } else if (move_result == 3) {
        printf("Move rejected %d already in leaderboard\n", rec_ptr->eth_address);
         free(rec_ptr);
    } else {
        printf("Error with return value for move! Fix your code.\n");
    }
}





/* This function adds a new AP record to the back of the queue.
 *
 * This function does NOT check for a duplicate entry and it does NOT
 * limit the size of the queue.
 *
 * Simply append to the back of the queue.
 *
 */
void ap_enqueue(twl_list_t *queue, int ap_id)
{
    // Create a new AP record using ap_create_info (you will need to implement this function)
    ap_info_t *rec_ptr = ap_create_info(ap_id);

    // Enqueue the new record at the back of the queue
    twl_list_insert(queue, rec_ptr, TWL_LIST_BACK);

    // Print a success message
    printf("Appended to back of queue %d\n", ap_id);
   
}

/* this function frees the memory for either a sorted or unsorted list.
 */
void ap_cleanup(twl_list_t *list_ptr)
{
    twl_list_destruct(list_ptr);
}

//ap_sort_mc for sorting based on mobile count i.e sortap x command
void ap_sort_mc(twl_list_t *list_ptr, int sort_type) {
    clock_t start, end;
    double elapse_time; /* time in milliseconds */
    int initialcount = twl_list_size(list_ptr);
    start = clock();
    twl_list_sort(list_ptr, sort_type, ap_rank_aps);
    end = clock();
    elapse_time = 1000.0 * ((double) (end - start)) / CLOCKS_PER_SEC;
    assert(twl_list_size (list_ptr) == initialcount);
    printf("%d\t%f\t%d\n", initialcount, elapse_time, sort_type);
    }
    
//ap_sort_eth for sorting based on eth address i.e sorteth x command
void ap_sort_eth(twl_list_t *list_ptr, int sort_type) {
    clock_t start, end;
    double elapse_time; /* time in milliseconds */
    int initialcount = twl_list_size(list_ptr);
    start = clock();
    twl_list_sort(list_ptr, sort_type, ap_compare_eth);
    end = clock();
    elapse_time = 1000.0 * ((double) (end - start)) / CLOCKS_PER_SEC;
    assert(twl_list_size (list_ptr) == initialcount);
    printf("%d\t%f\t%d\n", initialcount, elapse_time, sort_type);
    }

//ap_APPENDQ CODE
void ap_appendq(twl_list_t *queue, int eth_id, int mobile_cnt) {
    // Create a new ap_info_t structure with the provided values
    ap_info_t *new_ap = (ap_info_t *)calloc(1, sizeof(ap_info_t));
    if (new_ap == NULL) {
        //printf("# Error: Memory allocation failed\n");
        return;
    }

    new_ap->eth_address = eth_id;      
    new_ap->mobile_count = mobile_cnt;

    // Insert the new AP record into the queue
    twl_list_insert(queue, new_ap, twl_list_size(queue));
    
    // Mark the list as unsorted using the new function
    twl_mark_the_list_unsorted(queue);
}
    
// funtion to compare eth_address
int ap_compare_eth(const ap_info_t *record_a, const ap_info_t *record_b)
{
    assert(record_a != NULL && record_b != NULL);

    if (record_a->eth_address > record_b->eth_address)
        return -1;
    else if (record_a->eth_address < record_b->eth_address)
        return 1;
    else
        return 0;
}

/* Prompts user for AP record input starting with the Mobile's IP address.
 * The input is not checked for errors but will default to an acceptable value
 * if the input is incorrect or missing.
 *
 * This function creates the memory block for the record.
 *
 * DO NOT CHANGE THIS FUNCTION!
 */
ap_info_t *ap_create_info(int sta_id)
{
    char line[MAXLINE];
    char str[MAXLINE];
    char letter;
    ap_info_t *new = (ap_info_t *) calloc(1, sizeof(ap_info_t));
    assert(new != NULL);

    new->eth_address = sta_id;
    printf("AP IP address:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &new->ip_address);
    printf("AP location code:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &new->location_code);

    printf("Authenticated (T/F):");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%s", str);
    if (strcmp(str, "T")==0 || strcmp(str, "t")==0)
        new->authenticated = 1;
    else
        new->authenticated = 0;

    printf("Privacy (none|WEP|WPA|WPA2):");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%s", str);
    if (strcmp(str, "WEP")==0)
        new->privacy = 1;
    else if (strcmp(str, "WPA")==0)
        new->privacy = 2;
    else if (strcmp(str, "WPA2")==0)
        new->privacy = 3;
    else
        new->privacy = 0;


    printf("Standard letter (a b e g h n s):");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%c", &letter);
    if (letter < 'a' || letter > 'z')
        letter = 'a';
    new->standard_letter = letter - 'a';

    printf("Band (2.4|5.0):");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%f", &new->band);

    printf("Channel:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &new->channel);

    printf("Data rate:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%f", &new->data_rate);

    printf("Time received (int):");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &new->time_received);
    printf("\n");

    return new;
}

/* print the information for a particular access point record 
 *
 * Input is a pointer to a record, and no entries are changed.
 *
 * DO NOT CHANGE THIS FUNCTION!
 */
void ap_print_info(ap_info_t *rec)
{
    const char *pri_str[] = {"none", "WEP", "WPA", "WPA2"};
    assert(rec != NULL);
    printf("mc: %d,", rec->mobile_count);
    printf("eth: %d,", rec->eth_address);
    printf(" IP: %d, Loc: %d,", rec->ip_address, rec->location_code);
    printf(" Auth: %s,", rec->authenticated ? "T" : "F"); 
    printf(" Pri: %s, L: %c, B: %g, C: %d,", pri_str[rec->privacy], 
            (char) (rec->standard_letter + 'a'), rec->band, rec->channel);
    printf(" R: %g", rec->data_rate);
    printf(" Time: %d\n", rec->time_received);
}


/* commands specified to vim. ts: tabstop, sts: soft tabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */

