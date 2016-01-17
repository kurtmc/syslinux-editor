#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CONFIG_FILE "/boot/syslinux/syslinux.cfg"

struct boot_option {
	char *label;
	char *menu_label;
	char *image;
	char *root;
	char *initrd;
};

struct boot_option *new_boot_option()
{
	struct boot_option *bo;
	bo = malloc(sizeof(struct boot_option));
	bo->label = NULL;
	bo->menu_label = NULL;
	bo->image = NULL;
	bo->root = NULL;
	bo->initrd = NULL;
	return bo;
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
	

int main(void)
{

	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen(CONFIG_FILE, "r");
	if (fp == NULL)
		exit(EXIT_FAILURE);

	struct boot_option *boot;
	struct boot_option **boot_options = NULL;
	int size = 0;

	while ((read = getline(&line, &len, fp)) != -1) {
		if (line[0] == '#')
			continue;

		char *line_copy;
		line_copy = strdup(line);
		char *token = strtok(line_copy, " \t\n");
		while (token) {
			if (strcmp(token, "LABEL") == 0) {
				token = strtok(NULL, " \t\n");
				printf("LABEL: %s\n", token);
				boot = new_boot_option();
				boot_options = realloc(boot_options, ++size * sizeof(struct boot_option *));
				boot_options[size - 1] = boot;
				boot->label = strdup(token);
				break;
			} else if (strcmp(token, "MENU") == 0) {
				token = strtok(NULL, " \t\n");
				if (strcmp(token, "LABEL") == 0) {
					token = strtok(NULL, " \t\n");
					printf("MENU LABEL:");
					add_to_string(&boot->menu_label, token);
					token = strtok(NULL, " \t\n");
					while (token) {
						add_to_string(&boot->menu_label, " ");
						add_to_string(&boot->menu_label, token);
						printf(" %s", token);
						token = strtok(NULL, " \t\n");
					}
					printf("\n");
					break;
				} else {
					break;
				}
			} else if (strcmp(token, "LINUX") == 0) {
				token = strtok(NULL, " \t\n");
				printf("LINUX: %s\n", token);
				add_to_string(&boot->image, token);
				break;
			} else if (strcmp(token, "APPEND") == 0) {
				token = strtok(NULL, " \t\n");
				printf("APPEND:");
				add_to_string(&boot->root, token);
				token = strtok(NULL, " \t\n");
				while(token) {
					add_to_string(&boot->root, " ");
					add_to_string(&boot->root, token);
					printf(" %s", token);
					token = strtok(NULL, " \t\n");
				}
				printf("\n");
				break;
			} else if (strcmp(token, "INITRD") == 0) {
				token = strtok(NULL, " \t\n");
				printf("INITRD: %s\n", token);
				add_to_string(&boot->initrd, token);
				break;

			} else {
				break;
			}
			token = strtok(NULL, " \t\n");
		}
		printf("\n");
		free(line_copy);
	}

	fclose(fp);
	if (line)
		free(line);


	for (int i = 0; i < size; i++) {
		printf("Boot Option %d\n", i);
		printf("LABEL %s\n", boot_options[i]->label);
		printf("MENU LABEL %s\n", boot_options[i]->menu_label);
		printf("LINUX %s\n", boot_options[i]->image);
		printf("APPEND %s\n", boot_options[i]->root);
		printf("INITRD %s\n", boot_options[i]->initrd);
		printf("\n");
	}

	exit(EXIT_SUCCESS);
}
