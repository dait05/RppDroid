/*
 *  chardev.h - the header file with the ioctl definitions.
 *
 *  The declarations here have to be in a header file, because
 *  they need to be known both to the kernel module
 *  (in chardev.c) and the process calling ioctl (ioctl.c)
 */

#ifndef POLICY_H
#define POLICY_H

#include <linux/ioctl.h>
#include "vector.h"

struct sec_policy {
	char rule[99];
};

struct sec_external_storage {
	char storage[64];
};

typedef struct _policy
{
	char pname[64];
	char permission[3];
	char zone[32];
} policy;

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
#define IOCTL_GET_NTH_RULE _IOWR(MAJOR_NUM, 5, int)

//clear rules
#define IOCTL_CLEAR_RULES _IO(MAJOR_NUM, 6)

//pass external storage root directory to device file
#define IOCTL_EXTERNAL_STORAGE _IOR(MAJOR_NUM, 7, struct sec_external_storage *)


/* 
 * The name of the device file 
 */
#define DEVICE_FILE_NAME "/dev/policy"

#endif
