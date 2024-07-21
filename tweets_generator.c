#include "markov_chain.h"
#include <string.h>
#define TOKEN_DETERMINE " \n\r\t"
#define MAX_LINE 1000
#define NO_WORDS_LIMIT 4
#define WORDS_LIMIT 5
#define SUCCESS 0
#define FAILED 1
#define INVALID_ARGS_ERROR_MESSAGE "Usage: invalid number of arguments"
#define FILE_ERROR_MESSAGE "Error: couldn't open file"
#define MAX_TWEET 20
#define READ_ALL_FILE (-1)
#define BASE_10 10

#define SEED_PLACE 1
#define FILE_PLACE 3
#define TWEETS_PLACE 2
#define WORDS_PLACE 4
/**
 * receives a pointer to the file, the number of words to read and a markov
 * chain with a valid database in it, and fills it with the words from the file
 * @param fp a file pointer
 * @param words_to_read number of words to read
 * @param markov_chain a pointer to the markov chain
 * @return 0 upon success, 1 if failed
 */
static int fill_database(FILE *fp, int words_to_read, MarkovChain
*markov_chain);

/**
 * checks if a word is the last word in a sentence
 * @param word a generic pointer that points to a word
 * @return true if its the last word, false if not
 */
static bool is_word_last(void * word);

/**
 * prints the word
 * @param word a generic pointer to a word
 */
static void my_print(void *word);

/**
 * gets two generic pointers to two words, returns a negative value if the
 * first should appear before the second, 0 if they are the same and 1
 * otherwise
 * @param first a pointer to the first word
 * @param second a pointer to the second word
 * @return a number
 */
static int my_compare(void *first, void *second);

/**
 * copies a pointer of a word to a new allocated generic pointer
 * @param word a pointer to a word
 * @return the newly allocated pointer
 */
static void* my_copy (const void* word);

/**
 * checks if the program receives a valid amount of arguments
 * @param argc the number of arguments
 * @return true if valid, false if not
 */
static bool is_valid_args(int argc);

/**
 * prints random tweets
 * @param markov_chain a pointer to the markov chain
 * @param tweets_num number of tweets to print
 */
static void print_tweets(MarkovChain *markov_chain, int tweets_num);

static int fill_database(FILE *fp, int words_to_read, MarkovChain
*markov_chain)
{
  char line[MAX_LINE] = {0};
  int word_counter = 0;
  while (word_counter != words_to_read)
  {
    if (!fgets(line, MAX_LINE, fp))
    {
      return SUCCESS;
    }
    void *token = (void*)strtok(line, TOKEN_DETERMINE); char* word_pointer =
        NULL;
    while (token != NULL)
    {
      if (word_counter == words_to_read)
      {
        return SUCCESS;
      }
      if (!markov_chain->is_last(token))
      {
        word_pointer = token;
      }
      else
      {
        if (!add_to_database (markov_chain,token))
        {
          return FAILED;
        }
        word_pointer = NULL;
      }
      token = strtok(NULL, TOKEN_DETERMINE);
      if (word_pointer && token)
      {
        Node *last_node = add_to_database (markov_chain,
                                           word_pointer);
        Node *cur_node = add_to_database (markov_chain, token);
        if (!last_node || !cur_node)
        {
          return FAILED;
        }
        if (!add_node_to_frequencies_list (last_node->data, cur_node->data, markov_chain))
        {
          return FAILED;
        }
      }
      word_counter++;
    }
  }
  return EXIT_SUCCESS;
}

static bool is_word_last(void * word)
{
  char* new_word = (char*) word;
  if (new_word[strlen (new_word) - 1] == '.')
  {
    return true;
  }
  return false;
}

static void my_print(void *word)
{
  if (is_word_last (word))
  {
    printf ("%s", (char*)word);
  }
  else
  {
    printf ("%s ", (char*)word);
  }

}

static int my_compare(void *first, void *second)
{
  return strcmp ((char*)first, (char*)second);
}

static void* my_copy (const void* word)
{
  char* cur = (char*)word;
  size_t num = strlen (cur) + 1;
  void* new = calloc (num, sizeof (char));
  if (!new)
  {
    return NULL;
  }
  memcpy (new, word, num);
  return new;
}

static bool is_valid_args(int argc)
{
  if (argc != WORDS_LIMIT && argc != NO_WORDS_LIMIT)
  {
    printf ("%s\n", INVALID_ARGS_ERROR_MESSAGE);
    return false;
  }
  return true;
}

static void print_tweets(MarkovChain *markov_chain, int tweets_num)
{
  for (int i = 1; i <= tweets_num; i++)
  {
    printf ( "Tweet %d: ", i);
    generate_tweet (markov_chain, NULL, MAX_TWEET);
    printf("\n");
  }
}

int main (int argc, char *argv[])
{
  if (!is_valid_args (argc))
  {
    return EXIT_FAILURE;
  }
  unsigned seed = (unsigned)strtol(argv[SEED_PLACE], NULL, BASE_10);
  srand (seed);
  FILE *input = fopen (argv[FILE_PLACE], "r");
  if (input == NULL)
  {
    printf ("%s", FILE_ERROR_MESSAGE);
    return EXIT_FAILURE;
  }
  MarkovChain *my_chain = calloc (1, sizeof (MarkovChain));
  if (!my_chain)
  {
    printf ("%s", ALLOCATION_ERROR_MASSAGE);
    fclose (input);
    return EXIT_FAILURE;
  }
  my_chain->database = calloc(1,sizeof (LinkedList));
  if (!my_chain->database)
  {
    printf ("%s", ALLOCATION_ERROR_MASSAGE);
    free(my_chain);
    my_chain = NULL;
    fclose (input);
    return EXIT_FAILURE;
  }
  update_funcs (&my_chain, my_print, my_compare, free, my_copy, is_word_last);
  int words_to_read = READ_ALL_FILE;
  if (argc == WORDS_LIMIT)
  {
    words_to_read = (int) strtol (argv[WORDS_PLACE],NULL, BASE_10);
  }
  if (fill_database (input, words_to_read,my_chain))
  {
    free_database (&my_chain);
    fclose (input);
    printf ("%s", ALLOCATION_ERROR_MASSAGE);
    return EXIT_FAILURE;
  }
  int tweets_num = (int) strtol (argv[TWEETS_PLACE], NULL, BASE_10);
  print_tweets (my_chain, tweets_num);
  free_database (&my_chain);
  fclose (input);
  return EXIT_SUCCESS;
}
