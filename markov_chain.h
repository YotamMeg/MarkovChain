#ifndef _MARKOV_CHAIN_H
#define _MARKOV_CHAIN_H
#define FAILED_ADD 1

#include "linked_list.h"
#include <stdio.h>  // For printf(), sscanf()
#include <stdlib.h> // For exit(), malloc()
#include <stdbool.h> // for bool

#define ALLOCATION_ERROR_MASSAGE "Allocation failure: Failed to allocate new memory\n"


/***************************/
/*   insert typedefs here  */
typedef void (*print_function) (void*);
typedef int (*compare_function) (void*, void*);
typedef void (*free_function) (void*);
typedef void* (*copy_function) (const void*);
typedef bool (*is_last_function) (void*);
/***************************/


/***************************/



/***************************/
/*        STRUCTS          */
/***************************/

typedef struct MarkovNode {
    void *data;
    struct MarkovNodeFrequency *frequencies_list;
    int frequencies_list_length;
} MarkovNode;

typedef struct MarkovNodeFrequency {
    struct MarkovNode *markov_node;
    int frequency;
} MarkovNodeFrequency;

/* DO NOT ADD or CHANGE variable names in this struct */
typedef struct MarkovChain {
    LinkedList *database;

    // pointer to a func that receives data from a generic type and prints it
    // returns void.
    print_function print_func;

    // pointer to a func that gets 2 pointers of generic data type(same one)
    // and compare between them */
    // returns: - a positive value if the first is bigger
    //          - a negative value if the second is bigger
    //          - 0 if equal
    compare_function comp_func;

    // a pointer to a function that gets a pointer of generic data type and
    // frees it.
    // returns void.
    free_function free_data;

    // a pointer to a function that  gets a pointer of generic data type and
    // returns a newly allocated copy of it
    // returns a generic pointer.
    copy_function copy_func;

    //  a pointer to function that gets a pointer of generic data type
    //  and returns:
    //      - true if it's the last state.
    //      - false otherwise.
    is_last_function is_last;
} MarkovChain;

/**
 * returns a random number
 * @param max_number the max possible number
 * @return
 */
int get_random_number(int max_number);

/**
 * counter the frequencies of all markov nodes in a node's frequency list
 * @param state_struct_ptr
 * @return
 */
int get_num_appearances(MarkovNode *state_struct_ptr);

/**
 * looks for a markov node on a frequency list of another and updates it's
 * frequency if found
 * @param first_node the first markov node
 * @param second_node the second
 * @param markov_chain the markov chain
 * @return true if found, false if not
 */
bool update_frequency_if_found(MarkovNode *first_node, MarkovNode
*second_node, MarkovChain *markov_chain);




/**
 * Get one random state from the given markov_chain's database.
 * @param markov_chain
 * @return
 */
MarkovNode* get_first_random_node(MarkovChain *markov_chain);

/**
 * Choose randomly the next state, depend on it's occurrence frequency.
 * @param state_struct_ptr MarkovNode to choose from
 * @return MarkovNode of the chosen state
 */
MarkovNode* get_next_random_node(MarkovNode *state_struct_ptr);

/**
 * Receive markov_chain, generate and print random sentence out of it. The
 * sentence most have at least 2 words in it.
 * @param markov_chain
 * @param first_node markov_node to start with, if NULL- choose a random markov_node
 * @param  max_length maximum length of chain to generate
 */
void generate_tweet(MarkovChain *markov_chain, MarkovNode *
first_node, int max_length);

/**
 * Free markov_chain and all of it's content from memory
 * @param markov_chain markov_chain to free
 */
void free_database(MarkovChain **markov_chain);

/**
 * Add the second markov_node to the counter list of the first markov_node.
 * If already in list, update it's counter value.
 * @param first_node
 * @param second_node
 * @param markov_chain
 * @return success/failure: true if the process was successful, false if in
 * case of allocation error.
 */
bool add_node_to_frequencies_list(MarkovNode *first_node, MarkovNode
*second_node, MarkovChain *markov_chain);

/**
* Check if data_ptr is in database. If so, return the markov_node wrapping it in
 * the markov_chain, otherwise return NULL.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return Pointer to the Node wrapping given state, NULL if state not in
 * database.
 */
Node* get_node_from_database(MarkovChain *markov_chain, void *data_ptr);

/**
* If data_ptr in markov_chain, return it's node. Otherwise, create new
 * node, add to end of markov_chain's database and return it.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return node wrapping given data_ptr in given chain's database
 */
Node* add_to_database(MarkovChain *markov_chain, void *data_ptr);

/**
 * receives 5 functions and a pointer to a pointer to markov chain and
 * updates the fields of the markov chain.
 * @param markov_chain
 * @param print_func
 * @param comp_func
 * @param free_func
 * @param copy_func
 * @param is_last_func
 */
void update_funcs(MarkovChain **markov_chain, print_function print_func,
                  compare_function comp_func, free_function free_func,
                  copy_function copy_func, is_last_function is_last_func);

#endif /* MARKOV_CHAIN_H */
