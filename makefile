.PHONY: main
main:
	gcc main.c headers.c prompt.c init_shell.c input.c execute.c cd.c ls.c -o shell