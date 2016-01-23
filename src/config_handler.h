#ifndef CONFIG_HANDLER_H
#define CONFIG_HANDLER_H



struct boot_option {
	char *label;
	char *menu_label;
	char *image;
	char *root;
	char *initrd;
	char *com32;
};
struct boot_option *new_boot_option();
void print_boot_option(struct boot_option *b);
void fprint_boot_option(struct boot_option *b, char *path);
void add_to_string(char **str_ptr, char *str);
void print_file(char *path, int start_line, int end_line);
void fprint_file(char *path, int start_line, int end_line, char *output);
void parse_config_file(struct boot_option ***boot_options, int *size,
		int *line_number, char *config_file);
void delete_configuration(struct boot_option ***boot_options, int *size,
		int index, char *boot_dir);
void output_config_file(struct boot_option **boot_options, int size,
		int line_number, char *path, char *input_file);

#endif
