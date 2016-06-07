#include "minunit.h"
#include "config_handler.h"

#include <string.h>
#include <stdlib.h>

/*
 * https://github.com/siu/minunit
 *
 * This test suite and the test directory are highly coupled.
 */

void test_setup() {
	system("git checkout test");
}

void test_teardown() {
	system("git checkout test");
}

MU_TEST(test_delete_item) {
	char *config_file_path = "./test/test-boot/syslinux/syslinux.cfg";	
	struct node *head = parse_config_file(config_file_path);

	mu_assert(boot_option_count(head) == 13, "There should be 13 boot options");

	struct node *current = head;
	while (current->data) {
		if (current->type == BOOT_OPTION) {
			if (strcmp(((struct boot_option*)current->data)->label, "4.4.0-rc7-ARCH-00076-g9c982e8-dirty") == 0) {
				delete_configuration(&head, (struct boot_option*)current->data, "./test/test-boot");
				break;
			}
		}
		current = current->next;
	}

	mu_assert(boot_option_count(head) == 12, "There should be 12 boot options");
}

MU_TEST_SUITE(test_suite) {
	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

	MU_RUN_TEST(test_delete_item);
}

int main(void) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return 0;
}

