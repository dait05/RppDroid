/*
*
*	Added by Benjamin
*
*/

#ifndef POLICY_H
#define POLICY_H

struct sec_policy {
	char rule[99];
};

struct sec_external_storage {
	char storage[64];
};

/* 
 * The name of the device file 
 */
#define DEVICE_FILE_NAME "/dev/policy"

//void ioctl_set_msg(int file_desc, char *message);
//void ioctl_get_msg(int file_desc);
//void ioctl_get_nth_byte(int file_desc);

//add rule sp into kernel module
void ioctl_add_rule(int file_desc, struct sec_policy * sp);

//check how many rules stored in kernel module
void ioctl_get_rule_num(int file_desc, int * rule_num);

void ioctl_external_storage(int file_desc, struct sec_external_storage * sp);

//read nth rule in the kernel module into sp
//int ioctl_get_nth_rule(int file_desc, int nth, struct sec_policy * sp);

//clear rules
void ioctl_clear_rules(int file_desc);

#endif