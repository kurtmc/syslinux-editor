#ifndef CONFIG_HANDLER_H
#define CONFIG_HANDLER_H
#include <stdarg.h>


struct boot_option {
	char *label;
	char *menu_label;
	char *image;
	char *root;
	char *initrd;
	char *com32;
};

typedef enum {BOOT_OPTION, TEXT_BLOCK} data_type;

struct node {
	data_type type;
	void *data;
	struct node *next;
};

struct boot_option *new_boot_option();
void fprint_boot_option(FILE *fp, struct boot_option *b);
char *add_to_string(char *str, int num, ...);
struct node *parse_config_file(char *config_file);
void delete_configuration(struct node **head, struct boot_option *to_delete,
		char *boot_dir);
void output_config_file(struct node *head, char *path);
int get_boot_options_list(struct boot_option ***boot_options,
		struct node *head);
void free_boot_option(struct boot_option *bo);
void print_list(struct node *head);


#endif
