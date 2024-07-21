#include <string.h> // For strlen(), strcmp(), strcpy()
#include "markov_chain.h"

#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))
#define FIRST_NODE "Random Walk"
#define EMPTY -1
#define BOARD_SIZE 100
#define MAX_GENERATION_LENGTH 60

#define DICE_MAX 6
#define NUM_OF_TRANSITIONS 20

#define VALID_ARGS 3

#define BASE_10 10
/**
 * checkes if the number of arguments is invalid
 * @param argc number of arguments
 * @return true if invalid number of args, false if valid
 */
static bool invalid_args(int argc);

/**
 * prints the content of a cell as described
 * @param cell a generic pointer to a cell
 */
static void my_print(void *cell);

/**
 * compared to cells based on their values
 * @param first the first cell
 * @param second the second cell
 * @return negative value if the first cell is smaller, 0 if equal and
 * positive otherwise
 */
static int my_compare(void* first, void* second);

/**
 * copies the content of a cell to a newly allocated generic pointer
 * @param cell a pointer to a cell
 * @return the newly allocated pointer
 */
static void* my_copy(const void* cell);

/**
 * checks if a cell is the last one on the board
 * @param cell a pointer to cell
 * @return true if last, false if not
 */
static bool my_is_last(void* cell);



/**
 * represents the transitions by ladders and snakes in the game
 * each tuple (x,y) represents a ladder from x to if x<y or a snake otherwise
 */
const int transitions[][2] = {{13, 4},
                              {85, 17},
                              {95, 67},
                              {97, 58},
                              {66, 89},
                              {87, 31},
                              {57, 83},
                              {91, 25},
                              {28, 50},
                              {35, 11},
                              {8,  30},
                              {41, 62},
                              {81, 43},
                              {69, 32},
                              {20, 39},
                              {33, 70},
                              {79, 99},
                              {23, 76},
                              {15, 47},
                              {61, 14}};

/**
 * struct represents a Cell in the game board
 */
typedef struct Cell {
    int number; // Cell number 1-100
    int ladder_to;  // ladder_to represents the jump of the ladder in case there is one from this square
    int snake_to;  // snake_to represents the jump of the snake in case there is one from this square
    //both ladder_to and snake_to should be -1 if the Cell doesn't have them
} Cell;

/** Error handler **/
static int handle_error(char *error_msg, MarkovChain **database)
{
    printf("%s", error_msg);
    if (database != NULL)
    {
      free_database (database);
    }
    return EXIT_FAILURE;
}


static int create_board(Cell *cells[BOARD_SIZE])
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        cells[i] = malloc(sizeof(Cell));
        if (cells[i] == NULL)
        {
            for (int j = 0; j < i; j++) {
                free(cells[j]);
            }
            handle_error(ALLOCATION_ERROR_MASSAGE,NULL);
            return EXIT_FAILURE;
        }
        *(cells[i]) = (Cell) {i + 1, EMPTY, EMPTY};
    }

    for (int i = 0; i < NUM_OF_TRANSITIONS; i++)
    {
        int from = transitions[i][0];
        int to = transitions[i][1];
        if (from < to)
        {
            cells[from - 1]->ladder_to = to;
        }
        else
        {
            cells[from - 1]->snake_to = to;
        }
    }
    return EXIT_SUCCESS;
}

/**
 * fills database
 * @param markov_chain
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
static int fill_database(MarkovChain *markov_chain)
{
    Cell* cells[BOARD_SIZE];
    if(create_board(cells) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }
    MarkovNode *from_node = NULL, *to_node = NULL;
    size_t index_to;
    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        add_to_database(markov_chain, cells[i]);
    }

    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        from_node = get_node_from_database(markov_chain,cells[i])->data;

        if (cells[i]->snake_to != EMPTY || cells[i]->ladder_to != EMPTY)
        {
            index_to = MAX(cells[i]->snake_to,cells[i]->ladder_to) - 1;
            to_node = get_node_from_database(markov_chain, cells[index_to])
                    ->data;
          add_node_to_frequencies_list (from_node, to_node, markov_chain);
        }
        else
        {
            for (int j = 1; j <= DICE_MAX; j++)
            {
                index_to = ((Cell*) (from_node->data))->number + j - 1;
                if (index_to >= BOARD_SIZE)
                {
                    break;
                }
                to_node = get_node_from_database(markov_chain, cells[index_to])
                        ->data;
              add_node_to_frequencies_list (from_node, to_node, markov_chain);
            }
        }
    }
    // free temp arr
    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        free(cells[i]);
    }
    return EXIT_SUCCESS;
}

static bool invalid_args(int argc)
{
  if (argc != VALID_ARGS)
  {
    return true;
  }
  return false;
}

static void my_print(void *cell)
{
  Cell *cur_cell = (Cell*)cell;
  if (my_is_last (cell))
  {
    printf ("[%d]", cur_cell->number);
  }
  else
  {
    printf ("[%d]", cur_cell->number);
    if (cur_cell->ladder_to != EMPTY)
    {
      printf ("-ladder to %d ->", cur_cell->ladder_to);
    }
    else if (cur_cell->snake_to != EMPTY)
    {
      printf ("-snake to %d ->", cur_cell->snake_to);
    }
    else if (cur_cell->number != BOARD_SIZE)
    {
      printf (" ->");
    }
    printf (" ");
  }
}

static int my_compare(void* first, void* second)
{
  Cell *first_cell = (Cell*)first;
  Cell *second_cell = (Cell*)second;
  if (first_cell->number < second_cell->number)
  {
    return -1;
  }
  else if (first_cell->number == second_cell->number)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

static void* my_copy(const void* cell)
{
  Cell *new_cell = calloc (1, sizeof (Cell));
  if (!new_cell)
  {
    return NULL;
  }
  Cell *cur_cell = (Cell*)cell;
  new_cell->number = cur_cell->number;
  new_cell->snake_to = cur_cell->snake_to;
  new_cell->ladder_to = cur_cell->ladder_to;
  return (void*)new_cell;
}

static bool my_is_last(void* cell)
{
  Cell *cur_cell = (Cell*)cell;
  if (cur_cell->number == BOARD_SIZE)
  {
    return true;
  }
  return false;
}

/**
 * @param argc num of arguments
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int main(int argc, char *argv[])
{
  if (invalid_args(argc))
  {
    printf ("%s", ALLOCATION_ERROR_MASSAGE);
    return EXIT_FAILURE;
  }
  unsigned seed = (unsigned)strtol(argv[1], NULL, BASE_10);
  srand (seed);

  MarkovChain *markov_chain = calloc (1, sizeof (MarkovChain));
  if (!markov_chain)
  {
    printf ("%s", ALLOCATION_ERROR_MASSAGE);
    return EXIT_FAILURE;
  }
  markov_chain->database = calloc (1, sizeof (LinkedList));
  if (!markov_chain->database)
  {
    printf ("%s", ALLOCATION_ERROR_MASSAGE);
    free(markov_chain);
    return EXIT_FAILURE;
  }
  update_funcs (&markov_chain, my_print, my_compare, free, my_copy,
                my_is_last);
  if (fill_database (markov_chain) == EXIT_FAILURE)
  {
    free_database (&markov_chain);
    printf ("%s", ALLOCATION_ERROR_MASSAGE);
    return EXIT_FAILURE;
  }
  int num_tracks = (int)strtol(argv[2], NULL, BASE_10);
  MarkovNode *first = markov_chain->database->first->data;
  for (int i = 1; i <= num_tracks; i++)
  {
    printf ( "%s %d: ",FIRST_NODE, i);
    generate_tweet (markov_chain, first, MAX_GENERATION_LENGTH);
    printf("\n");
  }
  free_database (&markov_chain);
  return EXIT_SUCCESS;
}
