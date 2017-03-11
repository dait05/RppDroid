/*
*
*	Added by Benjamin
*
*/

#include "Policy.h"
#include <stdio.h>
#include <fcntl.h>		/* open */
#include <unistd.h>		/* exit */
#include <stdlib.h>
#include <linux/ioctl.h>

/* 
 * The major device number. We can't rely on dynamic 
 * registration any more, because ioctls need to know 
 * it. 
 */
#define MAJOR_NUM 200

/* 
 * Set the message of the device driver 
 */
#define IOCTL_SET_MSG _IOR(MAJOR_NUM, 0, char *)
/*
 * _IOR means that we're creating an ioctl command 
 * number for passing information from a user process
 * to the kernel module. 
 *
 * The first arguments, MAJOR_NUM, is the major device 
 * number we're using.
 *
 * The second argument is the number of the command 
 * (there could be several with different meanings).
 *
 * The third argument is the type we want to get from 
 * the process to the kernel.
 */

/* 
 * Get the message of the device driver 
 */
#define IOCTL_GET_MSG _IOR(MAJOR_NUM, 1, char *)
/* 
 * This IOCTL is used for output, to get the message 
 * of the device driver. However, we still need the 
 * buffer to place the message in to be input, 
 * as it is allocated by the process.
 */

/* 
 * Get the n'th byte of the message 
 */
#define IOCTL_GET_NTH_BYTE _IOWR(MAJOR_NUM, 2, int)
/* 
 * The IOCTL is used for both input and output. It 
 * receives from the user a number, n, and returns 
 * Message[n]. 
 */

//add new rule into device file
#define IOCTL_ADD_RULE _IOR(MAJOR_NUM, 3, struct sec_policy *)

//get rule_num in device file
#define IOCTL_GET_RULE_NUM _IOR(MAJOR_NUM, 4, int *)

//read one rule in device file
//#define IOCTL_GET_NTH_RULE _IOWR(MAJOR_NUM, 5, int)

//clear rules
#define IOCTL_CLEAR_RULES _IO(MAJOR_NUM, 6)

//pass external storage root directory to device file
#define IOCTL_EXTERNAL_STORAGE _IOR(MAJOR_NUM, 7, struct sec_external_storage *)

/* 
 * Functions for the ioctl calls 
 */

void ioctl_set_msg(int file_desc, char *message)
{
	int ret_val;

	ret_val = ioctl(file_desc, IOCTL_SET_MSG, message);

	if (ret_val < 0) {
		printf("ioctl_set_msg failed:%d\n", ret_val);
		exit(-1);
	}
}

void ioctl_get_msg(int file_desc)
{
	int ret_val;
	char message[100];

	/* 
	 * Warning - this is dangerous because we don't tell
	 * the kernel how far it's allowed to write, so it
	 * might overflow the buffer. In a real production
	 * program, we would have used two ioctls - one to tell
	 * the kernel the buffer length and another to give
	 * it the buffer to fill
	 */
	ret_val = ioctl(file_desc, IOCTL_GET_MSG, message);

	if (ret_val < 0) {
		printf("ioctl_get_msg failed:%d\n", ret_val);
		exit(-1);
	}

	printf("get_msg message:%s\n", message);
}

void ioctl_get_nth_byte(int file_desc)
{
	int i;
	char c;

	printf("get_nth_byte message:");

	i = 0;
	do {
		c = ioctl(file_desc, IOCTL_GET_NTH_BYTE, i++);

		if (c < 0) {
			printf
			    ("ioctl_get_nth_byte failed at the %d'th byte:\n",
			     i);
			exit(-1);
		}

		putchar(c);
	} while (c != 0);
	putchar('\n');
}

void ioctl_add_rule(int file_desc, struct sec_policy * sp)
{
  ioctl(file_desc, IOCTL_ADD_RULE, sp);
}

void ioctl_external_storage(int file_desc, struct sec_external_storage * sp)
{
  ioctl(file_desc, IOCTL_EXTERNAL_STORAGE, sp);
}

void ioctl_get_rule_num(int file_desc, int * rule_num)
{
  *rule_num = ioctl(file_desc, IOCTL_GET_RULE_NUM, 0);
}
/*
int ioctl_get_nth_rule(int file_desc, int nth, struct sec_policy * sp)
{
  int * index = (int*) sp;
  *index = nth;
  return ioctl(file_desc, IOCTL_GET_NTH_RULE, sp);
}
*/
void ioctl_clear_rules(int file_desc)
{
  ioctl(file_desc, IOCTL_CLEAR_RULES);
}
