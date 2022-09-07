.PHONY: main
main:
	gcc main.c headers.c prompt.c init_shell.c input.c execute.c cd.c discover.c ls.c history.c pinfo.c process_list.c bg_handler.c -o shell -g