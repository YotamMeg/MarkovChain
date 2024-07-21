
#include "markov_chain.h"


int get_random_number(int max_number)
{
  return rand() % max_number;
}

int get_num_appearances(MarkovNode *state_struct_ptr)
{
  int total_appearances = 0;
  MarkovNodeFrequency *cur_node = state_struct_ptr->frequencies_list;
  for (int counter = 0; counter < state_struct_ptr->frequencies_list_length;
       counter++)
  {
    total_appearances += cur_node->frequency;
    cur_node++;
  }
  return total_appearances;
}

MarkovNode* get_first_random_node(MarkovChain *markov_chain)
{
  Node *temp = NULL;
  do
  {
    temp = markov_chain->database->first;
    int node_number = get_random_number (markov_chain->database->size);
    for (int node_counter = 0; node_counter < node_number; node_counter++)
    {
      temp = temp->next;
    }
  }
  while (markov_chain->is_last(temp->data->data));
  return temp->data;
}


MarkovNode* get_next_random_node(MarkovNode *state_struct_ptr)
{
  MarkovNodeFrequency *cur_node = state_struct_ptr->frequencies_list;
  int num = get_random_number (get_num_appearances (state_struct_ptr));
  while (num >= cur_node->frequency)
  {
    num -= cur_node->frequency;
    cur_node++;
  }
  return cur_node->markov_node;
}

void generate_tweet(MarkovChain *markov_chain, MarkovNode *
first_node, int max_length)
{
  if (!first_node)
  {
    first_node = get_first_random_node (markov_chain);
  }
  MarkovNode *next_node = first_node;
  markov_chain->print_func(next_node->data);
  for (int i = 1; i < max_length; i++)
  {
    next_node = get_next_random_node (next_node);
    markov_chain->print_func(next_node->data);
    if (markov_chain->is_last(next_node->data))
    {
      break;
    }
  }
}

void free_database(MarkovChain **markov_chain)
{
  Node *temp = (*markov_chain)->database->first;
  while (temp)
  {
    free(temp->data->frequencies_list);
    temp->data->frequencies_list = NULL;
    (*markov_chain)->free_data(temp->data->data);
    temp->data->data = NULL;
    free(temp->data);
    temp->data = NULL;
    Node *prev = temp;
    temp = temp->next;
    free(prev);
    prev = NULL;
  }
  free ((*markov_chain)->database);
  (*markov_chain)->database = NULL;
  free ((*markov_chain));
  *markov_chain = NULL;
}

bool update_frequency_if_found(MarkovNode *first_node, MarkovNode
*second_node, MarkovChain *markov_chain)
{
  if (!first_node || !second_node || !markov_chain)
  {
    return false;
  }
  if (!first_node->frequencies_list)
  {
    return false;
  }
  for (int i = 0; i < first_node->frequencies_list_length; i++)
  {
    MarkovNodeFrequency *cur_marc_node = first_node->frequencies_list +
                                         i;
    if (markov_chain->comp_func(cur_marc_node->markov_node->data,
                                second_node->data) == 0)
    {
      cur_marc_node->frequency++;
      return true;
    }
  }
  return false;
}

bool add_node_to_frequencies_list(MarkovNode *first_node, MarkovNode
*second_node, MarkovChain *markov_chain)
{
  if (!first_node || !second_node || !markov_chain)
  {
    return false;
  }
  if (update_frequency_if_found (first_node, second_node, markov_chain))
  {
    return true;
  }
  MarkovNodeFrequency *temp = realloc (first_node->frequencies_list,
                                       (first_node->frequencies_list_length
                                        + 1) * sizeof (MarkovNodeFrequency));
  if (!temp)
  {
    printf ("%s", ALLOCATION_ERROR_MASSAGE);
    return false;
  }
  first_node->frequencies_list = temp;
  temp = NULL;
  MarkovNodeFrequency *new_node_location = first_node->frequencies_list +
                                           first_node->frequencies_list_length;
  new_node_location->markov_node = second_node;
  new_node_location->frequency = 1;
  first_node->frequencies_list_length++;
  return true;
}

Node* get_node_from_database(MarkovChain *markov_chain, void *data_ptr)
{
  Node *temp = markov_chain->database->first;
  while (temp)
  {
    if (markov_chain->comp_func(temp->data->data, data_ptr) == 0)
    {
      return temp;
    }
    temp = temp->next;
  }
  return NULL;
}

Node* add_to_database(MarkovChain *markov_chain, void *data_ptr)
{
  Node *new_node = (get_node_from_database (markov_chain, data_ptr));
  if (new_node)
  {
    return new_node;
  }
  MarkovNodeFrequency *new_freq_list = NULL;
  MarkovNode *new_marc_node = calloc (1, sizeof (MarkovNode));
  if (!new_marc_node)
  {
    return NULL;
  }
  new_marc_node->data = markov_chain->copy_func(data_ptr);
  if (!new_marc_node->data)
  {
    free(new_marc_node);
    new_marc_node = NULL;
    return NULL;
  }
  new_marc_node->frequencies_list = new_freq_list;
  new_marc_node->frequencies_list_length = 0;
  int is_success = add (markov_chain->database, new_marc_node);
  if (is_success == FAILED_ADD)
  {
    markov_chain->free_data(new_marc_node->data);
    new_marc_node->data = NULL;
    free(new_marc_node);
    new_marc_node = NULL;
    return NULL;
  }
  return markov_chain->database->last;
}

void update_funcs(MarkovChain **markov_chain, print_function print_func,
                         compare_function comp_func, free_function free_func,
                         copy_function copy_func, is_last_function
                         is_last_func)
{
  (*markov_chain)->print_func = print_func;
  (*markov_chain)->comp_func = comp_func;
  (*markov_chain)->free_data = free_func;
  (*markov_chain)->copy_func = copy_func;
  (*markov_chain)->is_last = is_last_func;
}