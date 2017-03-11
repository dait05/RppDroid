/*
 *  chardev.c - Create an input/output character device
 */

#include <linux/kernel.h>	/* We're doing kernel work */
#include <linux/module.h>	/* Specifically, a module */
#include <linux/fs.h>
#include <asm/uaccess.h>	/* for get_user and put_user */
#include <linux/unistd.h>     /* The list of system calls    */
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include "android_filesystem_config.h"
#include "sec_env.h"
#define SUCCESS 0
#define DEVICE_NAME "sec_env"
#define BUF_LEN 150

//use system call table to make system call in LKM
/*
#define SYS_CALL_TABLE_ADDRESS 0xc0024ee4;
#ifdef SYS_CALL_TABLE_ADDRESS
   void **sys_call_table = ( void * )SYS_CALL_TABLE_ADDRESS;
#else
   extern void *sys_call_table[];
#endif
*/
#define	major(x)	((int)(((u_int)(x) >> 8)&0xff))	/* major number */
#define	minor(x)	((int)((x)&0xff))		/* minor number */
#define	makedev(x,y)	((dev_t)(((x)<<8) | (y)))	/* create dev_t */


/* 
 * Is the device open right now? Used to prevent
 * concurent access into the same device 
 */
static int Device_Open = 0;

/* 
 * The message the device will give when asked 
 */
static char Message[BUF_LEN];

/* 
 * How far did the process reading the message get?
 * Useful if the message is larger than the size of the
 * buffer we get to fill in device_read. 
 */
static char *Message_Ptr;

/*
 * This is the number of current stored rules
 */
static int rule_num = 0;

struct rule_node
{
	int pid;
	struct rule_node* next;
};

struct rule_node* rule_link = NULL;

bool rule_synchronized = false;

//copy definition from /fs/proc/base.c
int proc_pid_cmdline(struct task_struct *task, char * buffer)
{
	int res = 0;
	unsigned int len;
	struct mm_struct *mm = get_task_mm(task);
	if (!mm)
		goto out;
	if (!mm->arg_end)
		goto out_mm;	/* Shh! No looking before we're done */

 	len = mm->arg_end - mm->arg_start;
 
	if (len > PAGE_SIZE)
		len = PAGE_SIZE;
 
	res = access_process_vm(task, mm->arg_start, buffer, len, 0);

	// If the nul at the end of args has been overwritten, then
	// assume application is using setproctitle(3).
	if (res > 0 && buffer[res-1] != '\0' && len < PAGE_SIZE) {
		len = strnlen(buffer, res);
		if (len < res) {
		    res = len;
		} else {
			len = mm->env_end - mm->env_start;
			if (len > PAGE_SIZE - res)
				len = PAGE_SIZE - res;
			res += access_process_vm(task, mm->env_start, buffer+res, len, 0);
			res = strnlen(buffer, res);
		}
	}
out_mm:
	mmput(mm);
out:
	return res;
}

struct file* file_open(const char* path, int flags, int rights) {
    struct file* filp = NULL;
    mm_segment_t oldfs;
    int err = 0;

    oldfs = get_fs();
    set_fs(get_ds());
    filp = filp_open(path, flags, rights);
    set_fs(oldfs);
    if(IS_ERR(filp)) {
    	err = PTR_ERR(filp);
    	return NULL;
    }
    return filp;
}

void file_close(struct file* file) {
    filp_close(file, NULL);
}

int file_read(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size) {
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());

    ret = vfs_read(file, data, size, &offset);

    set_fs(oldfs);
    return ret;
}


int get_name_by_pid(pid_t pid,char *name)
{
         int fd;
         char buf[1024];

         snprintf(buf,1024,"/proc/%d/cmdline",pid);
         if ( (fd = file_open(buf,O_RDONLY,0)) == -1)
         {
                 printk("Open file :%s,error!\n",buf);
                 return -1;
         }

         if( (file_read(fd,0, buf,1024)) == -1)
         {
                 printk("Read file error,fd = %d!\n",fd);
                 return -1;
         }
	 file_close(fd);
         strncpy(name,buf,1024);

         return 0;
}

/* 
 * This is called whenever a process attempts to open the device file 
 */
static int device_open(struct inode *inode, struct file *file)
{
#ifdef DEBUG
	printk(KERN_INFO "device_open(%p)\n", file);
#endif

	/* 
	 * We don't want to talk to two processes at the same time 
	 */
	if (Device_Open)
		return -EBUSY;

	Device_Open++;
	/*
	 * Initialize the message 
	 */
	Message_Ptr = Message;
	try_module_get(THIS_MODULE);
	return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file)
{
#ifdef DEBUG
	printk(KERN_INFO "device_release(%p,%p)\n", inode, file);
#endif

	/* 
	 * We're now ready for our next caller 
	 */
	Device_Open--;

	module_put(THIS_MODULE);
	return SUCCESS;
}

/* 
 * This function is called whenever a process which has already opened the
 * device file attempts to read from it.
 */
static ssize_t device_read(struct file *file,	/* see include/linux/fs.h   */
			   char __user * buffer,	/* buffer to be
							 * filled with data */
			   size_t length,	/* length of the buffer     */
			   int offset)
{
	/* 
	 * Number of bytes actually written to the buffer 
	 */
	int bytes_read = 0;

#ifdef DEBUG
	printk(KERN_INFO "device_read(%p,%p,%d)\n", file, buffer, length);
#endif

	/* 
	 * If we're at the end of the message, return 0
	 * (which signifies end of file) 
	 */
	if (*Message_Ptr == 0)
		return 0;

	/* 
	 * Actually put the data into the buffer 
	 */
	while (length && *Message_Ptr) {

		/* 
		 * Because the buffer is in the user data segment,
		 * not the kernel data segment, assignment wouldn't
		 * work. Instead, we have to use put_user which
		 * copies data from the kernel data segment to the
		 * user data segment. 
		 */
		put_user(*(Message_Ptr++), buffer++);
		length--;
		bytes_read++;
	}

#ifdef DEBUG
	printk(KERN_INFO "Read %d bytes, %d left\n", bytes_read, length);
#endif

	/* 
	 * Read functions are supposed to return the number
	 * of bytes actually inserted into the buffer 
	 */
	return bytes_read;
}

/* 
 * This function is called when somebody tries to
 * write into our device file. 
 */
static ssize_t
device_write(struct file *file,
	     const char __user * buffer, size_t length, int offset)
{
	int i;

#ifdef DEBUG
	printk(KERN_INFO "device_write(%p,%s,%d)", file, buffer, length);
#endif

	for (i = 0; i < length && i < BUF_LEN; i++)
		get_user(Message[i+offset], buffer + i);

	Message_Ptr = Message;

	/* 
	 * Again, return the number of input characters used 
	 */
	return i;
}

/* 
 * This function is called whenever a process tries to do an ioctl on our
 * device file. We get two extra parameters (additional to the inode and file
 * structures, which all device functions get): the number of the ioctl called
 * and the parameter given to the ioctl function.
 *
 * If the ioctl is write or read/write (meaning output is returned to the
 * calling process), the ioctl call returns the output of this function.
 *
 */
int device_ioctl(struct inode *inode,	/* see include/linux/fs.h */
		 struct file *file,	/* ditto */
		 unsigned int ioctl_num,	/* number and param for ioctl */
		 unsigned long ioctl_param)
{
	int i;
	char *temp;
	char ch;
	
	int* s;
	int t;

	struct rule_node *temp_node;
	int len=0;
	char buf[50];
	char buf2[50];
	bool mark = false;
	
	int count=0;
	struct task_struct *task_list; 
	/* 
	 * Switch according to the ioctl called 
	 */
	switch (ioctl_num) {
	case IOCTL_SET_MSG:
		/* 
		 * Receive a pointer to a message (in user space) and set that
		 * to be the device's message.  Get the parameter given to 
		 * ioctl by the process. 
		 */
		temp = (char *)ioctl_param;

		/* 
		 * Find the length of the message 
		 */
		get_user(ch, temp);
		for (i = 0; ch && i < BUF_LEN; i++, temp++)
			get_user(ch, temp);

		device_write(file, (char *)ioctl_param, i, 0);
		break;

	case IOCTL_GET_MSG:
		/* 
		 * Give the current message to the calling process - 
		 * the parameter we got is a pointer, fill it. 
		 */
		i = device_read(file, (char *)ioctl_param, 99, 0);

		/* 
		 * Put a zero at the end of the buffer, so it will be 
		 * properly terminated 
		 */
		put_user('\0', (char *)ioctl_param + i);
		break;

	case IOCTL_GET_NTH_BYTE:
		/* 
		 * This ioctl is both input (ioctl_param) and 
		 * output (the return value of this function) 
		 */
		return Message[ioctl_param];
		break;
	case IOCTL_ADD_RULE:
		device_write(file, (char *)ioctl_param, sizeof(struct sec_policy), sizeof(struct sec_policy)*rule_num);
		rule_num++;
		//add a node
		temp_node = kmalloc(sizeof(struct rule_node), GFP_KERNEL);
		temp_node->pid = -1;
		temp_node->next = rule_link;
		rule_link = temp_node;
		//check ps, compare the package name in the rule buf[1] and cmdline in /proc buf[2]
		temp = (char*)ioctl_param;
		get_user(ch, temp);
		for (i = 0; ch && i < 50; i++, temp++)
		{
			get_user(ch, temp);
			buf[i] = ch;
		}
		//comm length is 16, here use un-accurate comparasion
		if(i>16)
		{
			for(t=0;t<15;t++)
			{
				//printk("%d %c -> %c\n",t,buf[t+i-16], buf[t]);
				buf[t] = buf[t+i-16];
				
			}
			buf[t] = '\0';
		}
		//printk("Traversal module is working..\n");
		for_each_process(task_list) {     
			count++;
			//printk("[process %d]: %s\'s pid is %d\n",count,buf,len);
			//printk("[process %d]: %s\'s pid is %d\n",count,task_list->comm,task_list->pid);
			//printk("%s\n",buf);
			mark = true;
			for(i=0;i<15;i++)
			{
				if(buf[i]!=task_list->comm[i])
					mark = false;
			}
			if(mark)
			{
				temp_node->pid = task_list->pid;
				printk("[process %d]: %s\'s pid is %d\n",count,task_list->comm,task_list->pid);
			}
		}
		rule_synchronized = true;  
	      	//printk(KERN_ALERT"The number of process is:%d\n",count);

		break;
	case IOCTL_GET_RULE_NUM:
		return rule_num;
		break;
	case IOCTL_GET_NTH_RULE:
		s = (int*)ioctl_param;
		t = *s;
		if(t >= rule_num)
			return -1;
		temp = (char *)ioctl_param;
		for(i = 0; i< sizeof(struct sec_policy); i++)
		{
			put_user(Message[t*sizeof(struct sec_policy)+i], temp+i);
		}
		return 0;
		break;
	case IOCTL_CLEAR_RULES:
		if(rule_num > 0)
		{
			for(i=0;i<rule_num;i++)
			{
				temp = rule_link;
				rule_link = rule_link->next;
				kfree(temp);
			}
		}
		rule_num = 0;
		rule_link = NULL;
		break;
	}
	

	return SUCCESS;
}



/* Module Declarations */

/* 
 * This structure will hold the functions to be called
 * when a process does something to the device we
 * created. Since a pointer to this structure is kept in
 * the devices table, it can't be local to
 * init_module. NULL is for unimplemented functions. 
 */
struct file_operations Fops = {
	.owner = THIS_MODULE,
	.read = device_read,
	.write = device_write,
	.ioctl = device_ioctl,
	.open = device_open,
	.release = device_release,	/* a.k.a. close */
};


// int (*sys_unlink)(const char __user *name); /* save the original system call */
// int (*sys_mknod)(const char __user *filename, int mode, unsigned dev); /* save the original system call */
// int (*sys_kill)(int pid, int sig); /* save the original system call */
// int (*sys_chmod)(const char __user *filename, mode_t mode); /* save the original system call */
// int (*sys_chown)(const char __user *filename, uid_t user, gid_t group); /* save the original system call */
/*
 * provide interfaces to sched.h
 */
bool mark = false;
int sec_check_valid_pid(int pid)
{
	//Testing write Message with 3 chars
	//int unvalid_pid = ((int)Message[0]-48)*100+((int)Message[1]-48)*10+(int)Message[2]-48;
	//printk("<1>\nunvalid_pid \'%d\' module\n", unvalid_pid);
	if(rule_synchronized == true)
	{
		struct rule_node* temp = rule_link;
		while(temp)
		{
			//check whether it is allowed
			if(temp->pid == pid)
			{
				printk("<1>\nmatching unvalid_pid!\n");
				mm_segment_t fs = get_fs();     /* save previous value */
				set_fs (get_ds()); /* use kernel limit */
				if(!mark)
				{
					sys_kill(pid,9);
					mark = true;
				}
				set_fs(fs); /* restore before returning to user space */
				return 0;
				//break;
			}
			temp = temp->next;
		};
	}
	return 1;
}


static const struct {
	unsigned int		minor;
	char			*name;
	umode_t			mode;
	const struct file_operations	*fops;
} devlist[] = { /* list of minor devices */
	{0, DEVICE_NAME,     S_IFCHR, &Fops},
};

static struct class *sec_env_class;

/* 
 * Initialize the module - Register the character device 
 */
int sec_env_init()
{
	int ret_val;
	mm_segment_t fs;

	/* 
	 * Register the character device (atleast try) 
	 */
        printk( "<1>%s: module loaded\n", DEVICE_NAME );

	ret_val = register_chrdev(MAJOR_NUM, DEVICE_NAME, &Fops);

	/* 
	 * Negative values signify an error 
	 */
	if (ret_val < 0) {
		printk(KERN_ALERT "%s failed with %d\n",
		       "Sorry, registering the character device ", ret_val);
		return ret_val;
	}
	
	//sys_mknod = sys_call_table[__NR_mknod];
	//sys_unlink = sys_call_table[__NR_unlink];
	//sys_kill = sys_call_table[__NR_kill];
	//sys_chmod = sys_call_table[__NR_chmod];
	//sys_chown = sys_call_table[__NR_chown];

	sec_env_class = class_create(THIS_MODULE, DEVICE_NAME);
	device_create(sec_env_class, NULL, MKDEV(MAJOR_NUM, devlist[0].minor), NULL,devlist[0].name);

      	//fs = get_fs();     /* save previous value */
	//set_fs (KERNEL_DS); /* use kernel limit */
	//sys_unlink(DEVICE_FILE_NAME);
	//ret1 = sys_mknod( DEVICE_FILE_NAME, S_IFCHR, makedev(MAJOR_NUM,0));
	//ret2 = sys_chown( DEVICE_FILE_NAME, AID_SYSTEM, AID_SYSTEM);
	//ret3 = sys_chmod( DEVICE_FILE_NAME, 600);
	//set_fs(fs); /* restore before returning to user space */
	//printk( "<1>%s: module loaded %d %d %d %d\n", DEVICE_NAME, ret_val, ret1, ret2, ret3 );
	/*
	 * init function pointer in sched.h
	 */	
	sched_check_valid_pid = sec_check_valid_pid;

	/*
	set_fs( get_ds() );
	sys_unlink( DEVICE_FILE_NAME ); 

	sys_mknod( DEVICE_FILE_NAME, S_IFCHR, MAJOR_NUM);

	sys_chmod( DEVICE_FILE_NAME, 666 );
	*/
	/*
	printk(KERN_INFO "%s The major device number is %d.\n",
	       "Registeration is a success", MAJOR_NUM);
	printk(KERN_INFO "If you want to talk to the device driver,\n");
	printk(KERN_INFO "you'll have to create a device file. \n");
	printk(KERN_INFO "We suggest you use:\n");
	printk(KERN_INFO "mknod %s c %d 0\n", DEVICE_FILE_NAME, MAJOR_NUM);
	printk(KERN_INFO "The device file name is important, because\n");
	printk(KERN_INFO "the ioctl program assumes that's the\n");
	printk(KERN_INFO "file you'll use.\n");
	*/

	return 0;
}

/* 
 * Cleanup - unregister the appropriate file from /proc 
 */
void sec_env_cleanup()
{
	//int ret;
	mm_segment_t fs;

	/*
	 * clear function pointer in sched.h
	 */	
	sched_check_valid_pid = NULL;

      	//fs = get_fs();     /* save previous value */
	//set_fs (get_ds()); /* use kernel limit */
	//sys_unlink( DEVICE_FILE_NAME ); 
	//set_fs(fs); /* restore before returning to user space */

	/* 
	 * Unregister the device 
	 */
	device_destroy(sec_env_class, MKDEV(MAJOR_NUM, devlist[0].minor));
	class_destroy(sec_env_class);
	unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
	printk( "<1>%s: module unloaded\n", DEVICE_NAME );

	/* 
	 * If there's an error, report it 
	 */
	//if (ret < 0)
	//	printk(KERN_ALERT "Error: unregister_chrdev: %d\n", ret);
}

module_init(sec_env_init)
module_exit(sec_env_cleanup)
MODULE_LICENSE("GPL");
