#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config_handler.h"

struct boot_option *new_boot_option()
{
	struct boot_option *bo;
	bo = malloc(sizeof(struct boot_option));
	bo->label = NULL;
	bo->menu_label = NULL;
	bo->image = NULL;
	bo->root = NULL;
	bo->initrd = NULL;
	bo->com32 = NULL;
	return bo;
}

void fprint_boot_option(struct boot_option *b, char *path)
{
	FILE *fp;

	fp = fopen(path, "a");

	fprintf(fp, "LABEL %s\n", b->label);
	fprintf(fp, "\tMENU LABEL %s\n", b->menu_label);
	if (b->image)
		fprintf(fp, "\tLINUX %s\n", b->image);
	if (b->root)
		fprintf(fp, "\tAPPEND %s\n", b->root);
	if (b->initrd)
		fprintf(fp, "\tINITRD %s\n", b->initrd);
	if (b->com32)
		fprintf(fp, "\tCOM32 %s\n", b->com32);
	fclose(fp);
}
void print_boot_option(struct boot_option *b)
{
	printf("LABEL %s\n", b->label);
	printf("\tMENU LABEL %s\n", b->menu_label);
	if (b->image)
		printf("\tLINUX %s\n", b->image);
	if (b->root)
		printf("\tAPPEND %s\n", b->root);
	if (b->initrd)
		printf("\tINITRD %s\n", b->initrd);
	if (b->com32)
		printf("\tCOM32 %s\n", b->com32);
}

void add_to_string(char **str_ptr, char *str)
{
	if (*(str_ptr) == NULL) {
		*(str_ptr) = strdup(str);
	} else {
		*(str_ptr) = realloc(*(str_ptr), strlen(*(str_ptr)) + strlen(str) + 1); /* +1 for null termination */

		strcat(*(str_ptr), str);
	}
}

void fprint_file(char *path, int start_line, int end_line, char *output)
{
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	FILE * out;
	out = fopen(output, "a");


	fp = fopen(path, "r");
	if (fp == NULL)
		exit(EXIT_FAILURE);


	int line_count = -1;
	while ((read = getline(&line, &len, fp)) != -1) {
		line_count++;
		if (start_line <= line_count && end_line >= line_count)
			fprintf(out, "%s", line);
	}

	fclose(out);
	fclose(fp);
	if (line)
		free(line);
}
void print_file(char *path, int start_line, int end_line)
{

	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen(path, "r");
	if (fp == NULL)
		exit(EXIT_FAILURE);


	int line_count = -1;
	while ((read = getline(&line, &len, fp)) != -1) {
		line_count++;
		if (start_line <= line_count && end_line >= line_count)
			printf("%s", line);
	}

	fclose(fp);
	if (line)
		free(line);
}

void parse_config_file(struct boot_option ***boot_options, int *size, int *line_number)
{
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	char *config_file = NULL;
	add_to_string(&config_file, BOOT_DIR);
	add_to_string(&config_file, "/syslinux/syslinux.cfg");
	fp = fopen(config_file, "r");
	if (fp == NULL)
		exit(EXIT_FAILURE);

	struct boot_option *boot;
	*size = 0;

	*line_number = -1;
	int line_count = -1;

	while ((read = getline(&line, &len, fp)) != -1) {
		line_count++;
		if (line[0] == '#')
			continue;

		char *line_copy;
		line_copy = strdup(line);
		char *token = strtok(line_copy, " \t\n");
		while (token) {
			if (strcmp(token, "LABEL") == 0) {
				token = strtok(NULL, " \t\n");
				boot = new_boot_option();
				(*boot_options) = realloc((*boot_options), ++(*size) * sizeof(struct boot_option *));
				(*boot_options)[(*size) - 1] = boot;
				boot->label = strdup(token);

				if (*line_number < 0)
					*line_number = line_count;
				break;
			} else if (strcmp(token, "MENU") == 0) {
				token = strtok(NULL, " \t\n");
				if (strcmp(token, "LABEL") == 0) {
					token = strtok(NULL, " \t\n");
					add_to_string(&boot->menu_label, token);
					token = strtok(NULL, " \t\n");
					while (token) {
						add_to_string(&boot->menu_label, " ");
						add_to_string(&boot->menu_label, token);
						token = strtok(NULL, " \t\n");
					}
					break;
				} else {
					break;
				}
			} else if (strcmp(token, "LINUX") == 0) {
				token = strtok(NULL, " \t\n");
				add_to_string(&boot->image, token);
				break;
			} else if (strcmp(token, "APPEND") == 0) {
				token = strtok(NULL, " \t\n");
				add_to_string(&boot->root, token);
				token = strtok(NULL, " \t\n");
				while(token) {
					add_to_string(&boot->root, " ");
					add_to_string(&boot->root, token);
					token = strtok(NULL, " \t\n");
				}
				break;
			} else if (strcmp(token, "INITRD") == 0) {
				token = strtok(NULL, " \t\n");
				add_to_string(&boot->initrd, token);
				break;
			} else if (strcmp(token, "COM32") == 0) {
				token = strtok(NULL, " \t\n");
				add_to_string(&boot->com32, token);
				break;
			} else {
				break;
			}
			token = strtok(NULL, " \t\n");
		}
		free(line_copy);
	}

	fclose(fp);
	if (line)
		free(line);
}

void delete_configuration(struct boot_option ***boot_options, int *size, int index) {
	for (int i = index; i < *size - 1; i++) {
		(*boot_options)[i] = (*boot_options)[i + 1];
	}

	(*size)--;

	(*boot_options) = realloc(*boot_options, *size * sizeof(struct boot_option *));

}

void output_config_file(struct boot_option **boot_options, int size, int line_number, char *path, char *input_file)
{
	fprint_file(input_file, 0, line_number -1, path);

	for (int i = 0; i < size; i++) {
		fprint_boot_option(boot_options[i], path);
		FILE *fp;
		fp = fopen(path, "a");
		fprintf(fp, "\n");
		fclose(fp);
	}
}
