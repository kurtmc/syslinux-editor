#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

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

void free_boot_option(struct boot_option *bo)
{
	free(bo->label);
	free(bo->menu_label);
	free(bo->image);
	free(bo->root);
	free(bo->initrd);
	free(bo->com32);
	free(bo);
}

void fprint_boot_option(FILE *fp, struct boot_option *b)
{
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
}

char *add_to_string(char *str, int num, ...)
{
	va_list valist;

	va_start(valist, num);

	for (int i = 0; i < num; i++) {
		char *str_arg = va_arg(valist, char *);

		if (str == NULL) {
			str = strdup(str_arg);
		} else {
			/* +1 for null termination */
			str = realloc(str, strlen(str) + strlen(str_arg) + 1);
			strcat(str, str_arg);

		}
	}
	va_end(valist);
	return str;
}

struct node *parse_config_file(char *config_file)
{
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen(config_file, "r");
	if (fp == NULL)
		exit(EXIT_FAILURE);

	struct node *head = NULL;
	struct node *tail = NULL;
	struct boot_option *boot;


	while ((read = getline(&line, &len, fp)) != -1) {

		char *line_copy;

		line_copy = strdup(line);
		char *token = strtok(line_copy, " \t\n");

		if (token) {
			if (strcmp(token, "LABEL") == 0) {
				token = strtok(NULL, " \t\n");
				boot = new_boot_option();
				struct node *current;

				if (head == NULL) {
					head = malloc(sizeof(struct node));
					current = head;
				} else {
					tail->next =
						malloc(sizeof(struct node));
					current = tail->next;
				}
				current->type = BOOT_OPTION;
				current->data = boot;
				current->next = NULL;
				tail = current;
				boot->label = strdup(token);

			} else if (strcmp(token, "MENU") == 0) {
				token = strtok(NULL, " \t\n");
				if (strcmp(token, "LABEL") == 0) {
					token = strtok(NULL, " \t\n");
					boot->menu_label = add_to_string(boot->menu_label, 1, token);
					token = strtok(NULL, " \t\n");
					while (token) {
						boot->menu_label = add_to_string(boot->menu_label, 2, " ", token);
						token = strtok(NULL, " \t\n");
					}
				} else {
					struct node *current;

					if (head == NULL) {
						head =
						malloc(sizeof(struct node));
						current = head;
					} else {
						tail->next =
						malloc(sizeof(struct node));
						current = tail->next;
					}
					current->type = TEXT_BLOCK;
					current->data = strdup(line);
					current->next = NULL;
					tail = current;
				}
			} else if (strcmp(token, "LINUX") == 0) {
				token = strtok(NULL, " \t\n");
				boot->image = add_to_string(boot->image, 1, token);
			} else if (strcmp(token, "APPEND") == 0) {
				token = strtok(NULL, " \t\n");
				boot->root = add_to_string(boot->root, 1, token);
				token = strtok(NULL, " \t\n");
				while (token) {
					boot->root = add_to_string(boot->root, 2, " ", token);
					token = strtok(NULL, " \t\n");
				}
			} else if (strcmp(token, "INITRD") == 0) {
				token = strtok(NULL, " \t\n");
				boot->initrd = add_to_string(boot->initrd, 1, token);
			} else if (strcmp(token, "COM32") == 0) {
				token = strtok(NULL, " \t\n");
				boot->com32 = add_to_string(boot->com32, 1, token);
			} else {
				struct node *current;

				if (head == NULL) {
					head = malloc(sizeof(struct node));
					current = head;
				} else {
					tail->next =
						malloc(sizeof(struct node));
					current = tail->next;
				}
				current->type = TEXT_BLOCK;
				current->data = strdup(line);
				current->next = NULL;
				tail = current;
			}
		} else {
			struct node *current;

			if (head == NULL) {
				head = malloc(sizeof(struct node));
				current = head;
			} else {
				tail->next = malloc(sizeof(struct node));
				current = tail->next;
			}
			current->type = TEXT_BLOCK;
			current->data = strdup(line);
			current->next = NULL;
			tail = current;
		}
		free(line_copy);
	}

	fclose(fp);
	if (line)
		free(line);
	return head;
}

void delete_configuration(struct node **head, struct boot_option *to_delete,
		char *boot_dir)
{
	/* Handle to_delete is the head node */
	if (to_delete == (struct boot_option *)(*head)->data)
		*head = (*head)->next;

	struct node *current = (*head)->next;
	struct node *previous = *head;

	while (current) {
		if ((struct boot_option *)current->data == to_delete)
			previous->next = current->next;
		current = current->next;
		previous = previous->next;
	}

	char *image;

	if (to_delete->image != NULL) {
		image = NULL;
		image = add_to_string(image, 3, boot_dir, "/", basename(to_delete->image));
		remove(image);
		free(image);
	}
	char *initrd;

	if (to_delete->initrd != NULL) {
		initrd = NULL;
		initrd = add_to_string(initrd, 3, boot_dir, "/", basename(to_delete->initrd));
		remove(initrd);
		free(initrd);
	}
	free_boot_option(to_delete);
}

void output_config_file(struct node *head, char *path)
{
	/* clean up trailing whitespace */
	struct node *current;

	current = head;
	struct node *last_non_whitespace;

	while (current) {
		if (current->type == TEXT_BLOCK) {
			if (strcmp((char *)current->data, "\n") != 0)
				last_non_whitespace = current;
		} else {
			last_non_whitespace = current;
		}
		current = current->next;
	}
	current = last_non_whitespace->next;
	last_non_whitespace->next = NULL;

	while (current) {
		free(current->data);
		struct node *to_free = current;

		current = current->next;
		free(to_free);
	}

	FILE *fp;

	fp = fopen(path, "w+");

	current = head;
	while (current) {
		if (current->type == BOOT_OPTION) {
			fprint_boot_option(fp,
					(struct boot_option *)current->data);
		} else {
			fprintf(fp, (char *)current->data);
		}
		current = current->next;
	}
	fclose(fp);
}

/* Get an array or struct boot_option * from a mixed linked list
 * Returns the number of boot_options allocated
 */
int get_boot_options_list(struct boot_option ***boot_options,
		struct node *head)
{
	int size = 0;
	struct node *current;

	current = head;
	while (current) {
		if (current->type == BOOT_OPTION) {
			(*boot_options) =
			realloc(*boot_options,
			++size * sizeof(struct boot_option *));
			(*boot_options)[size - 1] =
				(struct boot_option *)current->data;
		}
		current = current->next;
	}
	return size;
}
