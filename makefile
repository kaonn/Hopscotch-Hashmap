all : tests

tests : hopscotch.c tests.c
	gcc -Wall -Werror hopscotch.c tests.c -o tests.out

gen: gen.c
	gcc -Wall -Werror -Wextra gen.c -o gen.out
	
debug: debug.c
	gcc -Wall -Werror -Wextra debug.c -o debug.out
