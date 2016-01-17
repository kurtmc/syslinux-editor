#include <stdlib.h>
#include <menu.h>
#include "config_handler.h"

#define CTRLD 	4

char *choices[] = {
	"Choice 1",
	"Choice 2",
	"Choice 3",
	"Choice 4",
	"Choice 5",
	"Choice 6",
	"Choice 7",
	"Exit",
};

int main(void)
{
	struct boot_option **boot_options = NULL;
	int size = 0;
	int line_number = 0;
	parse_config_file(&boot_options, &size, &line_number);

	//print_file(CONFIG_FILE, 0, line_number -1);

	//for (int i = 0; i < size; i++) {
		//print_boot_option(boot_options[i]);
		//printf("\n");
	//}
	
	char **choices = malloc((size + 1) * sizeof(char *));
	for (int i = 0; i < size; i++) {
		choices[i] = boot_options[i]->label;
	}

	choices[size] = strdup("Exit");

	ITEM **my_items;
	int c;				
	MENU *my_menu;
	int n_choices, i;
	ITEM *cur_item;

	/* Initialize curses */	
	initscr();
	start_color();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_MAGENTA, COLOR_BLACK);

	/* Initialize items */
	n_choices = size + 1;
	my_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
	for(i = 0; i < n_choices; ++i)
		my_items[i] = new_item(choices[i], choices[i]);
	my_items[n_choices] = (ITEM *)NULL;
	//item_opts_off(my_items[3], O_SELECTABLE);
	//item_opts_off(my_items[6], O_SELECTABLE);

	/* Create menu */
	my_menu = new_menu((ITEM **)my_items);

	/* Set fore ground and back ground of the menu */
	set_menu_fore(my_menu, COLOR_PAIR(1) | A_REVERSE);
	set_menu_back(my_menu, COLOR_PAIR(2));
	set_menu_grey(my_menu, COLOR_PAIR(3));

	/* Post the menu */
	mvprintw(LINES - 3, 0, "Press <ENTER> to see the option selected");
	mvprintw(LINES - 2, 0, "Up and Down arrow keys to naviage (F1 to Exit)");
	post_menu(my_menu);
	refresh();

	while((c = getch()) != KEY_F(1))
	{       switch(c)
		{	case KEY_DOWN:
				menu_driver(my_menu, REQ_DOWN_ITEM);
				break;
			case KEY_UP:
				menu_driver(my_menu, REQ_UP_ITEM);
				break;
			case 10: /* Enter */
				move(20, 0);
				clrtoeol();
				char *selected = item_name(current_item(my_menu));
				if (strcmp(selected, "Exit") == 0) {
					goto exit;
				}
				mvprintw(20, 0, "Item selected is : %s", selected);
				pos_menu_cursor(my_menu);
				break;
			case 'd':
				if (O_SELECTABLE & item_opts(current_item(my_menu)) == O_SELECTABLE) {
					item_opts_off(current_item(my_menu), O_SELECTABLE);
				} else {
					item_opts_on(current_item(my_menu), O_SELECTABLE);
				}
				break;
		}
	}	

exit:
	unpost_menu(my_menu);
	for(i = 0; i < n_choices; ++i)
		free_item(my_items[i]);
	free_menu(my_menu);
	endwin();



	return 0;
}
