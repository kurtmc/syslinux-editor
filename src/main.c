#include <stdlib.h>
#include <menu.h>
#include <string.h>
#include "config_handler.h"

#ifdef DEBUG
#define BOOT_DIR  "./test/test-boot"
#else
#define BOOT_DIR "/boot"
#endif


MENU *build_menu(int size, struct boot_option **boot_options)
{
	MENU *my_menu;
	ITEM **my_items;

	clear();
	/* Initialize items */
	my_items = calloc(size + 1, sizeof(ITEM *));
	char *str;

	for (int i = 0; i < size; i++) {
		if (boot_options[i]->root)
			str = boot_options[i]->root;
		else
			str = "";
		my_items[i] = new_item(boot_options[i]->menu_label, str);
	}
	my_items[size] = NULL;

	/* Create menu */
	my_menu = new_menu(my_items);

	/* Set fore ground and back ground of the menu */
	set_menu_fore(my_menu, COLOR_PAIR(1) | A_REVERSE);
	set_menu_back(my_menu, COLOR_PAIR(2));
	set_menu_grey(my_menu, COLOR_PAIR(3));
	set_menu_format(my_menu, 40, 1);

	/* Post the menu */
	mvprintw(LINES - 4, 0,
			"Hightlight option and press 'd' to select it for deletion");
	mvprintw(LINES - 3, 0,
			"J and K or arrow keys to navigate, 'q' to quit");
	mvprintw(LINES - 2, 0,
			"After selecting options press 'a' to apply changes");
	post_menu(my_menu);

	return my_menu;
}

void update_menu(MENU *menu, int size, ITEM **old_items, struct boot_option **boot_options)
{
	ITEM **new_items;

	new_items = realloc(old_items, (size + 1) * sizeof(ITEM *));

	char *str;
	for (int i = 0; i < size; i++) {
		if (boot_options[i]->root)
			str = boot_options[i]->root;
		else
			str = "";
		new_items[i] = new_item(boot_options[i]->menu_label, str);
	}
	new_items[size] = NULL;

	unpost_menu(menu);
	set_menu_items(menu, new_items);
	post_menu(menu);
}

int main(void)
{
	MENU *my_menu;
	int size;
	struct boot_option **boot_options = NULL;
	ITEM **my_items;

	/* Get path to config file */
	char *config_file = NULL;

	config_file = add_to_string(config_file, 2, BOOT_DIR, "/syslinux/syslinux.cfg");

	struct node *head = parse_config_file(config_file);

	size = get_boot_options_list(&boot_options, head);

	/* Initialize curses */
	initscr();
	start_color();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_MAGENTA, COLOR_BLACK);

	my_menu = build_menu(size, boot_options);
	my_items = menu_items(my_menu);
	refresh();

	struct boot_option **to_delete_array;
	int num_indexes;
	int c;
	while ((c = getch()) != KEY_F(1)) {
		switch (c) {
		case KEY_DOWN:
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
			const char *selected = item_name(current_item(my_menu));

			if (strcmp(selected, "Exit") == 0)
				goto exit;
			mvprintw(20, 0, "Item selected is : %s", selected);
			pos_menu_cursor(my_menu);
			break;
		case 'd':
			if ((O_SELECTABLE & item_opts(current_item(my_menu)))
					== O_SELECTABLE) {
				item_opts_off(current_item(my_menu),
						O_SELECTABLE);
			} else {
				item_opts_on(current_item(my_menu),
						O_SELECTABLE);
			}
			break;
		case 'a':
			to_delete_array = NULL;
			num_indexes = 0;
			for (int i = 0; i < size; i++) {
				if ((O_SELECTABLE & item_opts(my_items[i])) !=
						O_SELECTABLE) {
					to_delete_array =
						realloc(to_delete_array,
						++num_indexes *
						sizeof(struct boot_option *));
					to_delete_array[num_indexes - 1] =
						boot_options[i];


				}
			}
			for (int i = num_indexes - 1; i >= 0; i--) {
				delete_configuration(&head, to_delete_array[i],
						BOOT_DIR);
				size = get_boot_options_list(&boot_options, head);
			}
			update_menu(my_menu, size, my_items, boot_options);
			refresh();
			output_config_file(head, config_file);

			break;


		case 'q':
			goto exit;
		}
	}

exit:
	unpost_menu(my_menu);
	for (int i = 0; i < size; i++)
		free_item(my_items[i]);
	free_menu(my_menu);
	endwin();
	free(config_file);



	return 0;
}
