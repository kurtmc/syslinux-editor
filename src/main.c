#include <stdlib.h>
#include <menu.h>
#include <string.h>
#include "config_handler.h"

#define CTRLD 	4

ITEM **my_items;
int c;				
MENU *my_menu;
int n_choices, i;
ITEM *cur_item;
int size = 0;
struct boot_option **boot_options = NULL;

void build_menu()
{
	clear();
	/* Initialize items */
	n_choices = size;
	my_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
	char *str;
	for(i = 0; i < n_choices; ++i) {
		if (boot_options[i]->root) {
			str = boot_options[i]->root;
		} else {
			str = "";
		}
		my_items[i] = new_item(boot_options[i]->menu_label, str);
	}
	my_items[n_choices] = (ITEM *)NULL;

	/* Create menu */
	my_menu = new_menu((ITEM **)my_items);

	/* Set fore ground and back ground of the menu */
	set_menu_fore(my_menu, COLOR_PAIR(1) | A_REVERSE);
	set_menu_back(my_menu, COLOR_PAIR(2));
	set_menu_grey(my_menu, COLOR_PAIR(3));
	set_menu_format(my_menu, 40, 1);

	/* Post the menu */
	mvprintw(LINES - 4, 0, "Hightlight option and press 'd' to select it for deletion");
	mvprintw(LINES - 3, 0, "J and K arrow keys to naviage, q to quit");
	mvprintw(LINES - 2, 0, "After selecting options press a to apply changes");
	post_menu(my_menu);
}

int main(void)
{
	char *config_file = NULL;
	add_to_string(&config_file, BOOT_DIR);
	add_to_string(&config_file, "/syslinux/syslinux.cfg");
	int line_number = 0;
	parse_config_file(&boot_options, &size, &line_number);
	

	/* Initialize curses */	
	initscr();
	start_color();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_MAGENTA, COLOR_BLACK);

	build_menu();
	refresh();

	int count = 0;
	while((c = getch()) != KEY_F(1))
	{       switch(c)
		{	case KEY_DOWN:
				menu_driver(my_menu, REQ_DOWN_ITEM);
				break;
			case 'j':
				menu_driver(my_menu, REQ_DOWN_ITEM);
				break;
			case KEY_UP:
				menu_driver(my_menu, REQ_UP_ITEM);
				break;
			case 'k':
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
			case 'a':
				count = 0;
				int *indexes = NULL;
				int num_indexes = 0;
				for (int i = 0; i < n_choices; i++) {
					if (O_SELECTABLE & item_opts(my_items[i]) != O_SELECTABLE) {
						indexes = realloc(indexes, ++num_indexes * sizeof(int));
						indexes[num_indexes - 1] = i;


					}
				}
				for (int i = num_indexes - 1; i >= 0; i--) {
					delete_configuration(&boot_options, &size, indexes[i]);
				}
				build_menu();
				refresh();
				output_config_file(boot_options, size, line_number, "test.txt", config_file);

				break;
				

			case 'q':
				goto exit;
		}
	}	

exit:
	unpost_menu(my_menu);
	for(i = 0; i < n_choices; ++i)
		free_item(my_items[i]);
	free_menu(my_menu);
	endwin();
	free(config_file);



	return 0;
}
