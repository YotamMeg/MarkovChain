CFLAGS = -Wall -Wextra -Wvla -std=c99

tweets: tweets_generator.o markov_chain.o linked_list.o
	gcc -o tweets_generator tweets_generator.o markov_chain.o linked_list.o

snake: snakes_and_ladders.o markov_chain.o linked_list.o
	gcc -o snakes_and_ladders snakes_and_ladders.o markov_chain.o linked_list.o

tweets_generator.o: tweets_generator.c markov_chain.h linked_list.h
	gcc $(CFLAGS) -c tweets_generator.c

snakes_and_ladders.o: snakes_and_ladders.c markov_chain.h linked_list.h
	gcc $(CFLAGS) -c snakes_and_ladders.c

markov_chain.o: markov_chain.c markov_chain.h linked_list.h
	gcc $(CFLAGS) -c markov_chain.c

linked_list.o: linked_list.c linked_list.h
	gcc $(CLAGS) -c linked_list.c
