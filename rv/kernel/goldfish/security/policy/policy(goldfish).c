/*
 *  chardev.c - Create an input/output character device
 */

#include <linux/kernel.h>	/* We're doing kernel work */
#include <linux/module.h>	/* Specifically, a module */
#include <linux/fs.h>
#include <linux/fs_struct.h> 
#include <linux/fdtable.h> 
#include <linux/rcupdate.h> 
#include <linux/moduleparam.h>  
#include <asm/uaccess.h>	/* for get_user and put_user */
#include <asm/stat.h>
#include <asm/segment.h>	
#include <linux/unistd.h>     /* The list of system calls    */
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/string.h>
#include <linux/dcache.h> 
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/netlink.h>
#include <linux/init.h>
#include <linux/buffer_head.h>
#include <linux/mm.h>
#include "android_filesystem_config.h"
#include "policy.h"
#define SUCCESS 0
#define DEVICE_NAME "policy"
#define BUF_LEN 150

//use system call table to make system call in LKM
#define SYS_CALL_TABLE_ADDRESS 0xc0022f24;

void __user *sample;

void **sys_call_table_pointer;

#define	major(x)	((int)(((u_int)(x) >> 8)&0xff))	/* major number */
#define	minor(x)	((int)((x)&0xff))		/* minor number */
#define	makedev(x,y)	((dev_t)(((x)<<8) | (y)))	/* create dev_t */

/* getuid() prototype */

asmlinkage int (*getuid_call)();


/* Original system call pointers for SD card*/

asmlinkage long (*original_sys_read)(unsigned int fd, char __user *buf, size_t count); 
asmlinkage long (*original_sys_write)(unsigned int fd, const char __user *buf, size_t count);
asmlinkage long (*original_sys_open)(const char __user *filename, int flags, int mode);
asmlinkage long (*original_sys_link)(const char __user *oldpath, const char __user *newpath);
asmlinkage long (*original_sys_unlink)(const char __user *filename);
asmlinkage long (*original_sys_execve)(char __user *filename, char __user * __user *argv, char __user * __user *envp, struct pt_regs *regs);
asmlinkage long (*original_sys_chdir)(const char __user *filename);
asmlinkage long (*original_sys_mknod)(const char __user *filename, int mode, unsigned dev);
asmlinkage long (*original_sys_chmod)(const char __user *filename, mode_t mode);
asmlinkage long (*original_sys_lchown)(const char __user *filename, uid_t user, gid_t group);
asmlinkage long (*original_sys_lchown32)(const char __user *filename, old_uid_t user, old_gid_t group);
asmlinkage long (*original_sys_chown32)(const char __user *filename, old_uid_t user, old_gid_t group);
asmlinkage long (*original_sys_stat64)(char __user *filename, struct stat64 __user *statbuf);
asmlinkage long (*original_sys_lstat64)(char __user *filename, struct stat64 __user *statbuf);
asmlinkage long (*original_sys_utimes)(char __user *filename, struct timeval __user *utimes);
asmlinkage long (*original_sys_access)(const char __user *filename, int mode); 
asmlinkage long (*original_sys_rename)(const char __user *oldpath, const char __user *newpath);
asmlinkage long (*original_sys_mkdir)(const char __user *filename, int mode);
asmlinkage long (*original_sys_rmdir)(const char __user *filename);
asmlinkage long (*original_sys_chroot)(const char __user *filename);
asmlinkage long (*original_sys_symlink)(const char __user *oldpath, const char __user *newpath);
asmlinkage long (*original_sys_readlink)(const char __user *filename, char __user *buf, int bufsiz);
asmlinkage long (*original_sys_truncate)(const char __user *filename, unsigned long length);


asmlinkage long (*original_sys_creat)(const char __user *pathname, int mode);
asmlinkage long (*original_sys_openat)(int dfd, const char __user *filename, int flags, int mode);
asmlinkage long (*original_sys_mkdirat)(int dfd, const char __user * pathname, int mode);
asmlinkage long (*original_sys_mknodat)(int dfd, const char __user * filename, int mode, unsigned dev);
asmlinkage long (*original_sys_fchownat)(int dfd, const char __user *filename, uid_t user, gid_t group, int flag);
asmlinkage long (*original_sys_futimesat)(int dfd, char __user *filename, struct timeval __user *utimes);
asmlinkage long (*original_sys_fstatat64)(int dfd, char __user *filename, struct stat64 __user *statbuf, int flag);
asmlinkage long (*original_sys_unlinkat)(int dfd, const char __user * pathname, int flag);
asmlinkage long (*original_sys_renameat)(int olddfd, const char __user * oldname, int newdfd, const char __user * newname);
asmlinkage long (*original_sys_linkat)(int olddfd, const char __user *oldname, int newdfd, const char __user *newname, int flags);
asmlinkage long (*original_sys_symlinkat)(const char __user * oldname, int newdfd, const char __user * newname);
asmlinkage long (*original_sys_readlinkat)(int dfd, const char __user *path, char __user *buf, int bufsiz);
asmlinkage long (*original_sys_fchmodat)(int dfd, const char __user * filename, mode_t mode);
asmlinkage long (*original_sys_faccessat)(int dfd, const char __user *filename, int mode);
asmlinkage long (*original_sys_statfs)(const char __user * path, struct statfs __user *buf);
asmlinkage long (*original_sys_statfs64)(const char __user *path, size_t sz, struct statfs64 __user *buf);
asmlinkage long (*original_sys_fstatfs)(unsigned int fd, struct statfs __user *buf);
asmlinkage long (*original_sys_fstatfs64)(unsigned int fd, size_t sz, struct statfs64 __user *buf);
asmlinkage long (*original_sys_stat)(char __user *filename, struct __old_kernel_stat __user *statbuf);
asmlinkage long (*original_sys_lstat)(char __user *filename, struct __old_kernel_stat __user *statbuf);
asmlinkage long (*original_sys_fstat)(unsigned int fd, struct __old_kernel_stat __user *statbuf);
asmlinkage long (*original_sys_fstat64)(unsigned long fd, struct stat64 __user *statbuf);
asmlinkage long (*original_sys_truncate64)(const char __user *path, loff_t length);
asmlinkage long (*original_sys_ftruncate64)(unsigned int fd, loff_t length);
asmlinkage long (*original_sys_ftruncate)(unsigned int fd, unsigned long length);
asmlinkage long (*original_sys_ustat)(unsigned dev, struct ustat __user *ubuf);


/* Original system call pointers for Internet access*/

asmlinkage long (*original_sys_socket)(int, int, int);
asmlinkage long (*original_sys_bind)(int, struct sockaddr __user *, int);
asmlinkage long (*original_sys_connect)(int, struct sockaddr __user *, int);

asmlinkage long (*original_sys_listen)(int,int);
asmlinkage long (*original_sys_accept)(int, struct sockaddr __user *, int __user *);
asmlinkage long (*original_sys_getsockname)(int, struct sockaddr __user *, int __user *);
asmlinkage long (*original_sys_getpeername)(int, struct sockaddr __user *, int __user *);
asmlinkage long (*original_sys_socketpair)(int, int, int, int __user *);
asmlinkage long (*original_sys_send)(int, void __user *, size_t, unsigned);
asmlinkage long (*original_sys_sendto)(int, void __user *, size_t, unsigned, struct sockaddr __user *, int);
asmlinkage long (*original_sys_recv)(int, void __user *, size_t, unsigned);
asmlinkage long (*original_sys_recvfrom)(int, void __user *, size_t, unsigned, struct sockaddr __user *, int __user *);
asmlinkage long (*original_sys_shutdown)(int, int);
asmlinkage long (*original_sys_setsockopt)(int fd, int level, int optname, char __user *optval, int optlen);
asmlinkage long (*original_sys_getsockopt)(int fd, int level, int optname, char __user *optval, int __user *optlen);
asmlinkage long (*original_sys_sendmsg)(int fd, struct msghdr __user *msg, unsigned flags);
asmlinkage long (*original_sys_recvmsg)(int fd, struct msghdr __user *msg, unsigned flags);


int counter=0;
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

static int capacity = sizeof(struct sec_policy);

struct rule_node
{
	int pid;
	struct rule_node* next;
};


static vector *tableptr, table;

static policy *p, pelem;

static char *dummy_file = "/dev/null";


struct rule_node* rule_link = NULL;

bool rule_synchronized = false;

bool read_once = false;
bool write_once = false;
bool open_once = false;
bool stat64_once = false;
bool mkdir_once = false;
bool rename_once = false;
bool access_once = false;
bool chmod_once = false;
bool lstat64_once = false;
bool unlink_once = false;
bool changed_old = false;
bool changed_new = false;
bool fstat64_once = false;

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
         struct file* fd;	 
         char buf[sizeof(p->pname)];

         snprintf(buf,sizeof(p->pname),"/proc/%d/cmdline",pid);
         if ( (fd = file_open(buf,O_RDONLY,0)) == -1)
         {
                 printk("Open file :%s,error!\n",buf);
                 return -1;
         }
	 	int ret = file_read(fd,0, buf,sizeof(p->pname));
         if( ret == -1)
         {
                 printk("Read file error!\n");
                 return -1;
         }
	 file_close(fd);
         strncpy(name,buf,ret);
	 if(ret<sizeof(p->pname))
		buf[ret] = '\0';

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
	char buf[capacity];
	struct task_struct *task_list; 

	policy *pptr, p1;
    pptr = &p1;

    char packagename[sizeof(pptr->pname)];
	char access[sizeof(pptr->permission)];
	char zone[sizeof(pptr->zone)];

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
		printk("@@@@@@@@@@@@@@@@@@@@@Enter add rules***************************\n");
		
		//device_write(file, (char *)ioctl_param, capacity, capacity*rule_num);
		
		
		//add a node
		temp_node = kmalloc(sizeof(struct rule_node), GFP_KERNEL);
		temp_node->pid = -1;
		temp_node->next = rule_link;
		rule_link = temp_node;
		//check ps, compare the package name in the rule buf[1] and cmdline in /proc buf[2]
		temp = (char*)ioctl_param;
		
			
		get_user(ch, temp);
		for (i = 0; ch && i < capacity; i++, temp++)
		{
			get_user(ch, temp);
			buf[i] = ch;
		}
		
		int index = -1;
		for (i = 0; i < capacity; i++)
		{
			if (buf[i] == ',')
			{
				index = i;	
				break;			
			}	
		}

		memset(packagename, '\0', sizeof(packagename));
		memset(access, '\0', sizeof(access));
		memset(zone,'\0', sizeof(zone));

		if (index != -1)
		{
			strncpy(packagename, buf, index);
			access[0] = buf[index+1];
			access[1] = buf[index+2];
		}

		strncpy(zone, buf+index+4, sizeof(zone));

		// test if add rule works
		printk("\nThe package and permission and zone = %s ;; %s ;; %s\n\n", packagename,access,zone);

		memcpy(pptr->pname, packagename, sizeof(pptr->pname));
		memcpy(pptr->permission, access, sizeof(pptr->permission));	
		memcpy(pptr->zone, zone, sizeof(pptr->zone));

		vector_push(tableptr,pptr);
		
		rule_num++;


		/*		
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
		*/
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
		for(i = 0; i< capacity; i++)
		{
			put_user(Message[t*capacity+i], temp+i);
		}
		return 0;
		break;
	case IOCTL_CLEAR_RULES:
		if (rule_num > 0)
		{
			vector_remove_all(tableptr);
		}	

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
/*
bool mark = false;
int check_valid_pid(int pid)
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
				mm_segment_t fs = get_fs();     // save previous value 
				set_fs (get_ds());  //use kernel limit 
				if(!mark)
				{
					sys_kill(pid,9);
					mark = true;
				}
				set_fs(fs); // restore before returning to user space 
				return 0;
				//break;
			}
			temp = temp->next;
		};
	}
	return 1;
}
*/


void init_policy()
{
	memset(p->pname, '\0', sizeof(p->pname));
	memset(p->permission, '\0', sizeof(p->permission));	
	memset(p->zone, '\0', sizeof(p->zone));
}

void check_policy_exist(policy *pptr) 
{
	int size = vector_length(tableptr);
	int i;
	policy temp_policy, *tempptr;
	tempptr = &temp_policy;

	for (i=0;i<size;i++)
	{
		vector_get(tableptr,i,tempptr);

		if (strcmp(tempptr->pname,pptr->pname)==0)
		{
			strncpy(pptr->permission,tempptr->permission,sizeof(pptr->permission));
			strncpy(pptr->zone,tempptr->zone,sizeof(pptr->zone));
			break;
		}
	}
}

void directory_mapping(char *file_name, const char *original_filename, policy *p)
{
	strncpy(file_name,original_filename,11);
	strncpy(file_name+11,"/",1);
	strncpy(file_name+12, p->zone,strlen(p->zone));
	if (strlen(original_filename) > 11)
		strncpy(file_name+12+strlen(p->zone), original_filename+11, strlen(original_filename)-11);
}

void directory_mapping_sdcard(char *file_name, const char *original_filename, policy *p)
{
	strncpy(file_name,original_filename,7);
	strncpy(file_name+7,"/",1);
	strncpy(file_name+8, p->zone,strlen(p->zone));
	if (strlen(original_filename) > 7)
		strncpy(file_name+8+strlen(p->zone), original_filename+7, strlen(original_filename)-7);
}

static const struct {
	unsigned int		minor;
	char			*name;
	umode_t			mode;
	const struct file_operations	*fops;
} devlist[] = { /* list of minor devices */
	{0, DEVICE_NAME,     S_IFCHR, &Fops},
};

static struct class *policy_class;


/* Hooked sys_read() method definition */

asmlinkage long our_sys_read(unsigned int fd, char __user *buf, size_t count)
{
	init_policy();

	pid_t pid = current->tgid;

	// avoid deadlock with root pid
	if (pid>100)
		get_name_by_pid(pid, p->pname);;

	if(p->pname != NULL)
	{
		check_policy_exist(p);
		if (strncmp("com.estrongs.android.pop", p->pname,24) == 0 || strncmp("com.rhmsoft.fm",p->pname,14) == 0 )
		{
			if (read_once == false)
				printk("\nSYS_READ\n");
			read_once = true;
		}
			
	}
	
	// deny access
	if (p->permission[0] == '0')
	{
		printk("\nSD card access denied!SYS_READ\n");
		//return;
	}

    return original_sys_read(fd,buf,count);
}

asmlinkage long our_sys_write(unsigned int fd, const char __user *buf, size_t count)
{
	init_policy();

	pid_t pid = current->tgid;

	// avoid deadlock with root pid
	if (pid>100)
		get_name_by_pid(pid, p->pname);;

	if(p->pname != NULL)
	{
		check_policy_exist(p);
		if (strncmp("com.estrongs.android.pop", p->pname,24) == 0 || strncmp("com.rhmsoft.fm",p->pname,14) == 0 )
		{
			if (write_once == false)
				printk("\nSYS_WRITE\n");
			write_once = true;
		}
			
	}

	// deny access
	if (p->permission[0] == '0')
	{
		printk("\nSD card access denied!SYS_WRITE\n");
		//return;
	}

    return original_sys_write(fd,buf,count);
}


asmlinkage long our_sys_open(const char __user *filename, int flags, int mode)
{
	init_policy();

	pid_t pid = current->tgid;
	
	// avoid deadlock with root pid
	if (pid>100)
		get_name_by_pid(pid, p->pname);
	
	
	if(p->pname != NULL)
	{
		check_policy_exist(p);
		if (strncmp("com.estrongs.android.pop", p->pname,24) == 0 || strncmp("com.rhmsoft.fm",p->pname,14) == 0 )
		{
			if (open_once == false)
				printk("\nSYS_OPEN\n");
			open_once = true;
		}
	}
	
	if (p->permission[0] == '1')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0)
		{
			//printk("\nStrlen of filename: %d\n", strlen(filename));
			//printk("\n Filename is: %s\n", filename);
				
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping(file_name,filename, p);

			printk("\n filename AFTER change: %s\n",file_name);
    		mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_open(file_name, flags, mode);
			set_fs(oldfs);
			return res;
		}
		
		if (strncmp("/sdcard", filename,7) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping_sdcard(file_name,filename, p);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_open(file_name, flags, mode);
			set_fs(oldfs);
			return res;
		}
	}
	
	// deny access
	if (p->permission[0] == '0')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0 || strncmp("/sdcard", filename,7) == 0)
		{
			mm_segment_t oldfs;
	    	oldfs = get_fs();
	   	 	set_fs(get_ds());

	    	long res = original_sys_open(dummy_file, flags, mode);
	    	set_fs(oldfs);
	    	printk("\nSD card access denied!SYS_OPEN\n");
			return res;
		}
	}
	
	return original_sys_open(filename,flags,mode);
}

asmlinkage long our_sys_link(const char __user *oldpath, const char __user *newpath)
{
	init_policy();

	pid_t pid = current->tgid;

	// avoid deadlock with root pid
	if (pid>100)
		get_name_by_pid(pid, p->pname);

	if(p->pname != NULL)
	{
		check_policy_exist(p);
		if (strncmp("com.estrongs.android.pop", p->pname,24) == 0 || strncmp("com.rhmsoft.fm",p->pname,14) == 0 )
			printk("\nSYS_LINK\n");
	}

	if (p->permission[0] == '1')
	{
		char old_file_name[strlen(oldpath)+strlen(p->zone)+2];
		char new_file_name[strlen(newpath)+strlen(p->zone)+2];
		memset(old_file_name, '\0', sizeof(old_file_name));
		memset(new_file_name, '\0', sizeof(new_file_name));

		strncpy(old_file_name, oldpath, strlen(oldpath));
		strncpy(new_file_name, newpath, strlen(newpath));

		if (strncmp("/mnt/sdcard",oldpath,11) == 0)
		{
			changed_old = true;
			directory_mapping(old_file_name, oldpath, p);
		}

		if (strncmp("/mnt/sdcard",newpath,11) == 0)
		{
			changed_new = true;
			directory_mapping(new_file_name, newpath, p);
		}

		if (strncmp("/sdcard",oldpath,7) == 0)
		{
			changed_old = true;
			directory_mapping_sdcard(old_file_name, oldpath, p);
		}

		if (strncmp("/sdcard",newpath,7) == 0)
		{
			changed_new = true;
			directory_mapping_sdcard(new_file_name, newpath, p);
		}
		
		if (changed_new && changed_old)
		{
			changed_old = false;
			changed_new = false;	

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_link(old_file_name, new_file_name);
			set_fs(oldfs);
			return res;
		}

		if (changed_old && !changed_new)
		{
			changed_old = false;
			
			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_link(old_file_name, newpath);
			set_fs(oldfs);
			return res;
		}
		if (changed_new && !changed_old)
		{
			changed_new = false;
			
			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_link(oldpath, new_file_name);
			set_fs(oldfs);
			return res;
		}
		

		return original_sys_link(oldpath,newpath);
	}
	
	// deny access
	if (p->permission[0] == '0')
	{
		if (strncmp("/mnt/sdcard",oldpath,11) == 0 || strncmp("/sdcard", oldpath,7) == 0 
			||strncmp("/mnt/sdcard", newpath,7) == 0 || strncmp("/sdcard", newpath,7) == 0)
		{
			printk("\nSD card access denied!SYS_LINK\n");
			mm_segment_t oldfs;
	    	oldfs = get_fs();
	   	 	set_fs(get_ds());

	    	long res = original_sys_link(dummy_file, dummy_file);
	    	set_fs(oldfs);
	    	return res;
	    }
	}
	
	return original_sys_link(oldpath,newpath);
}

asmlinkage long our_sys_unlink(const char __user *filename) 
{
	init_policy();

	pid_t pid = current->tgid;

	// avoid deadlock with root pid
	if (pid>100)
		get_name_by_pid(pid, p->pname);
	
	
	if(p->pname != NULL)
	{
		check_policy_exist(p);
		if (strncmp("com.estrongs.android.pop", p->pname,24) == 0 || strncmp("com.rhmsoft.fm",p->pname,14) == 0 )
		{
			if (unlink_once == false)
				printk("\nSYS_UNLINK\n");
			unlink_once = true;
		}
	}
		
	
	if (p->permission[0] == '1')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping(file_name,filename, p);
			
			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res =  original_sys_unlink(file_name);
			set_fs(oldfs);
			return res;
		}

		if (strncmp("/sdcard", filename,7) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping_sdcard(file_name,filename, p);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_unlink(file_name);
			set_fs(oldfs);
			return res;
		}
	}
	
	// deny access
	if (p->permission[0] == '0')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0 || strncmp("/sdcard", filename,7) == 0)
		{
			printk("\nSD card access denied!SYS_UNLINK\n");
			mm_segment_t oldfs;
	    	oldfs = get_fs();
	   	 	set_fs(get_ds());

	    	long res = original_sys_unlink(dummy_file);
	    	set_fs(oldfs);
	    	return res;
	    }
	}
	
	return original_sys_unlink(filename);
}

asmlinkage long our_sys_execve(char __user *filename, char __user * __user *argv, char __user * __user *envp, struct pt_regs *regs)
{
	
	init_policy();

	pid_t pid = current->tgid;

	if (pid>100)
		get_name_by_pid(pid, p->pname);
	
	
	if(p->pname != NULL)
	{
		check_policy_exist(p);
		if (strncmp("com.estrongs.android.pop", p->pname,24) == 0 || strncmp("com.rhmsoft.fm",p->pname,14) == 0 )
				printk("\nSYS_EXECVE\n");
	}
	
	if (p->permission[0] == '1')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping(file_name,filename, p);
			return original_sys_execve(file_name, argv, envp, regs);
		}
	}
	
	// deny access
	if (p->permission[0] == '0')
	{
		printk("\nSD card access denied!\n");
		return;
	}
	
	
	return original_sys_execve(filename, argv, envp, regs);
}

asmlinkage long our_sys_chdir(const char __user *filename)
{
	init_policy();

	pid_t pid = current->tgid;

	// avoid deadlock with root pid
	if (pid>100)
		get_name_by_pid(pid, p->pname);
	
	
	if(p->pname != NULL)
	{
		check_policy_exist(p);
		if (strncmp("com.estrongs.android.pop", p->pname,24) == 0 || strncmp("com.rhmsoft.fm",p->pname,14) == 0 )
				printk("\nSYS_CHDIR\n");
	}
	
	if (p->permission[0] == '1')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping(file_name,filename, p);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_chdir(file_name);
			set_fs(oldfs);
			return res;
		}
		
		if (strncmp("/sdcard", filename,7) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping_sdcard(file_name,filename, p);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_chdir(file_name);
			set_fs(oldfs);
			return res;
		}
	}
	
	// deny access
	if (p->permission[0] == '0')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0 || strncmp("/sdcard", filename,7) == 0)
		{
			printk("\nSD card access denied!SYS_CHDIR\n");
			mm_segment_t oldfs;
	    	oldfs = get_fs();
	   	 	set_fs(get_ds());

	    	long res = original_sys_chdir(dummy_file);
	    	set_fs(oldfs);
	    	return res;
	    }
	}
	
	return original_sys_chdir(filename);
}

asmlinkage long our_sys_mknod(const char __user *filename, int mode, unsigned dev)
{
	init_policy();

	pid_t pid = current->tgid;
	
	// avoid deadlock with root pid
	if (pid>100)
		get_name_by_pid(pid, p->pname);
	
	
	if(p->pname != NULL)
	{
		check_policy_exist(p);
		if (strncmp("com.estrongs.android.pop", p->pname,24) == 0 || strncmp("com.rhmsoft.fm",p->pname,14) == 0 )
				printk("\nSYS_MKNOD\n");
	}
	
	if (p->permission[0] == '1')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping(file_name,filename, p);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_mknod(file_name, mode, dev);
			set_fs(oldfs);
			return res;
		}

		if (strncmp("/sdcard", filename,7) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping_sdcard(file_name,filename, p);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_mknod(file_name, mode, dev);
			set_fs(oldfs);
			return res;
		}
	}
	
	// deny access
	if (p->permission[0] == '0')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0 || strncmp("/sdcard", filename,7) == 0)
		{
			printk("\nSD card access denied!SYS_MKNOD\n");
			mm_segment_t oldfs;
	    	oldfs = get_fs();
	   	 	set_fs(get_ds());

	    	long res = original_sys_mknod(dummy_file, mode, dev);
	    	set_fs(oldfs);
	    	return res;
	    }
	}
	
	return original_sys_mknod(filename, mode, dev);
}


asmlinkage long our_sys_chmod(const char __user *filename, mode_t mode) 
{
	init_policy();

	pid_t pid = current->tgid;
	
	// avoid deadlock with root pid
	if (pid>100)
		get_name_by_pid(pid, p->pname);
	
	
	if(p->pname != NULL)
	{
		check_policy_exist(p);
		if (strncmp("com.estrongs.android.pop", p->pname,24) == 0 || strncmp("com.rhmsoft.fm",p->pname,14) == 0 )
		{		
			if(chmod_once == false)
				printk("\nSYS_CHMOD\n");
			chmod_once = true;
		}
	}
	
	if (p->permission[0] == '1')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping(file_name,filename, p);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_chmod(file_name, mode);
			set_fs(oldfs);
			return res;
		}

		if (strncmp("/sdcard", filename,7) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping_sdcard(file_name,filename, p);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_chmod(file_name, mode);
			set_fs(oldfs);
			return res;
		}
	}
	
	// deny access
	if (p->permission[0] == '0')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0 || strncmp("/sdcard", filename,7) == 0)
		{
			printk("\nSD card access denied!SYS_CHMOD\n");
			mm_segment_t oldfs;
	    	oldfs = get_fs();
	   	 	set_fs(get_ds());

	    	long res = original_sys_chmod(dummy_file, mode);
	    	set_fs(oldfs);
	    	return res;
	    }
	}
	
	return original_sys_chmod(filename, mode);
}

asmlinkage long our_sys_lchown(const char __user *filename, uid_t user, gid_t group)
{
	init_policy();

	pid_t pid = current->tgid;
	
	// avoid deadlock with root pid
	if (pid>100)
		get_name_by_pid(pid, p->pname);
	
	
	if(p->pname != NULL)
	{
		check_policy_exist(p);
		if (strncmp("com.estrongs.android.pop", p->pname,24) == 0 || strncmp("com.rhmsoft.fm",p->pname,14) == 0 )
			printk("\nSYS_LCHOWN\n");
	}
	
	if (p->permission[0] == '1')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping(file_name,filename, p);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_lchown(file_name, user, group);
			set_fs(oldfs);
			return res;
		}

		if (strncmp("/sdcard", filename,7) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping_sdcard(file_name,filename, p);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_lchown(file_name, user, group);
			set_fs(oldfs);
			return res;
		}
	}
	
	// deny access
	if (p->permission[0] == '0')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0 || strncmp("/sdcard", filename,7) == 0)
		{
			printk("\nSD card access denied!SYS_LCHOWN\n");
			mm_segment_t oldfs;
	    	oldfs = get_fs();
	   	 	set_fs(get_ds());

	    	long res = original_sys_lchown(dummy_file, user, group);
	    	set_fs(oldfs);
	    	return res;
	    }
	}
	
	return original_sys_lchown(filename, user, group);
}

asmlinkage long our_sys_lchown32(const char __user *filename, old_uid_t user, old_gid_t group)
{
	init_policy();

	pid_t pid = current->tgid;
	
	// avoid deadlock with root pid
	if (pid>100)
		get_name_by_pid(pid, p->pname);
	
	
	if(p->pname != NULL)
	{
		check_policy_exist(p);
		if (strncmp("com.estrongs.android.pop", p->pname,24) == 0 || strncmp("com.rhmsoft.fm",p->pname,14) == 0 )
			printk("\nSYS_LCHOWN32\n");
	}
	
	if (p->permission[0] == '1')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping(file_name,filename, p);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_lchown32(file_name, user, group);
			set_fs(oldfs);
			return res;
		}

		if (strncmp("/sdcard", filename,7) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping_sdcard(file_name,filename, p);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_lchown32(file_name, user, group);
			set_fs(oldfs);
			return res;
		}
	}
	
	// deny access
	if (p->permission[0] == '0')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0 || strncmp("/sdcard", filename,7) == 0)
		{
			printk("\nSD card access denied!SYS_LCHOWN32\n");
			mm_segment_t oldfs;
	    	oldfs = get_fs();
	   	 	set_fs(get_ds());

	    	long res = original_sys_lchown32(dummy_file, user, group);
	    	set_fs(oldfs);
	    	return res;
	    }
	}
	
	return original_sys_lchown32(filename, user, group);
}

asmlinkage long our_sys_chown32(const char __user *filename, old_uid_t user, old_gid_t group)
{
	init_policy();

	pid_t pid = current->tgid;
	
	// avoid deadlock with root pid
	if (pid>100)
		get_name_by_pid(pid, p->pname);
	
	
	if(p->pname != NULL)
	{
		check_policy_exist(p);
		if (strncmp("com.estrongs.android.pop", p->pname,24) == 0 || strncmp("com.rhmsoft.fm",p->pname,14) == 0 )
				printk("\nSYS_CHOWN32\n");
	}
	
	if (p->permission[0] == '1')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping(file_name,filename, p);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_chown32(file_name, user, group);
			set_fs(oldfs);
			return res;
		}

		if (strncmp("/sdcard", filename,7) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping_sdcard(file_name,filename, p);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_chown32(file_name, user, group);
			set_fs(oldfs);
			return res;
		}
	}
	
	// deny access
	if (p->permission[0] == '0')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0 || strncmp("/sdcard", filename,7) == 0)
		{
			printk("\nSD card access denied!\n");
			mm_segment_t oldfs;
	    	oldfs = get_fs();
	   	 	set_fs(get_ds());

	    	long res = original_sys_chown32(dummy_file, user, group);
	    	set_fs(oldfs);
	    	return res;
    	}
	}
	
	return original_sys_chown32(filename, user, group);
}


asmlinkage long our_sys_stat64(char __user *filename, struct stat64 __user *statbuf)
{
	init_policy();

	pid_t pid = current->tgid;
	
	// avoid deadlock with root pid
	if (pid>100)
		get_name_by_pid(pid, p->pname);
	
	
	if(p->pname != NULL)
	{
		check_policy_exist(p);
		if (strncmp("com.estrongs.android.pop", p->pname,24) == 0 || strncmp("com.rhmsoft.fm",p->pname,14) == 0 )
		{		
			if(stat64_once == false)
				printk("\nSYS_STAT64\n");
			stat64_once = true;

		}
	}
	
	if (p->permission[0] == '1')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping(file_name,filename, p);

			//printk("\n STAT filename AFTER change: %s\n",file_name);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_stat64(file_name, statbuf);
			set_fs(oldfs);
			return res;
		}

		if (strncmp("/sdcard", filename,7) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping_sdcard(file_name,filename, p);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_stat64(file_name, statbuf);
			set_fs(oldfs);
			return res;
		}
	}
	
	// deny access
	if (p->permission[0] == '0')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0 || strncmp("/sdcard", filename,7) == 0)
		{
			printk("\nSD card access denied!SYS_STAT64\n");
			mm_segment_t oldfs;
	    	oldfs = get_fs();
	    	set_fs(get_ds());

			long res = original_sys_stat64(dummy_file, statbuf);
			set_fs(oldfs);
			return res;
		}
	}
	
	return original_sys_stat64(filename, statbuf);
}

asmlinkage long our_sys_lstat64(char __user *filename, struct stat64 __user *statbuf)
{
	init_policy();

	pid_t pid = current->tgid;
	
	// avoid deadlock with root pid
	if (pid>100)
		get_name_by_pid(pid, p->pname);
	
	
	if(p->pname != NULL)
	{
		check_policy_exist(p);
		if (strncmp("com.estrongs.android.pop", p->pname,24) == 0 || strncmp("com.rhmsoft.fm",p->pname,14) == 0 )
		{	
			if(lstat64_once == false)
				printk("\nSYS_LSTAT64\n");
			lstat64_once = true;
		}
	}
	
	if (p->permission[0] == '1')
	{

		if (strncmp("/mnt/sdcard",filename,11) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping(file_name,filename, p);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_lstat64(file_name, statbuf);
			set_fs(oldfs);
			return res;
		}

		if (strncmp("/sdcard", filename,7) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping_sdcard(file_name,filename, p);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_lstat64(file_name, statbuf);
			set_fs(oldfs);
			return res;
		}
	}
	
	// deny access
	if (p->permission[0] == '0')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0 || strncmp("/sdcard", filename,7) == 0)
		{
			printk("\nSD card access denied!SYS_LSTAT64\n");
			mm_segment_t oldfs;
	    	oldfs = get_fs();
	   	 	set_fs(get_ds());

	    	long res = original_sys_lstat64(dummy_file, statbuf);
	    	set_fs(oldfs);
	    	return res;
	    }
	}
	
	return original_sys_lstat64(filename, statbuf);
}

asmlinkage long our_sys_utimes(char __user *filename, struct timeval __user *utimes)
{
	init_policy();

	pid_t pid = current->tgid;
	
	// avoid deadlock with root pid
	if (pid>100)
		get_name_by_pid(pid, p->pname);
	
	
	if(p->pname != NULL)
	{
		check_policy_exist(p);
		if (strncmp("com.estrongs.android.pop", p->pname,24) == 0 || strncmp("com.rhmsoft.fm",p->pname,14) == 0 )
				printk("\nSYS_UTIMES\n");
	}
	
	if (p->permission[0] == '1')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping(file_name,filename, p);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_utimes(file_name, utimes);
			set_fs(oldfs);
			return res;
		}

		if (strncmp("/sdcard", filename,7) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping_sdcard(file_name,filename, p);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_utimes(file_name, utimes);
			set_fs(oldfs);
			return res;
		}
	}
	
	// deny access
	if (p->permission[0] == '0')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0 || strncmp("/sdcard", filename,7) == 0)
		{
			printk("\nSD card access denied!SYS_UTIMES\n");
			mm_segment_t oldfs;
	    	oldfs = get_fs();
	   	 	set_fs(get_ds());

	    	long res = original_sys_utimes(dummy_file, utimes);
	    	set_fs(oldfs);
	    	return res;
	    }
	}
	
	return original_sys_utimes(filename, utimes);
}


asmlinkage long our_sys_access(const char __user *filename, int mode) 
{
		
	init_policy(); 
	pid_t pid = current->tgid;
	
	// avoid deadlock with root pid
	if (pid>100)
		get_name_by_pid(pid, p->pname);
	
	if(p->pname != NULL)
	{
		check_policy_exist(p);
		if (strncmp("com.estrongs.android.pop", p->pname,24) == 0 || strncmp("com.rhmsoft.fm",p->pname,14) == 0 )
		{	
			if (access_once == false)	
				printk("\nSYS_ACCESS\n");
			access_once = true;
		}
	}
	
	
	if (p->permission[0] == '1')
	{
		
		if (strncmp("/mnt/sdcard",filename,11) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping(file_name,filename, p);
			
			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_access(file_name,mode);
			set_fs(oldfs);
			return res;
		}

		if (strncmp("/sdcard", filename,7) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping_sdcard(file_name,filename, p);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_access(file_name,mode);
			set_fs(oldfs);
			return res;
		}

	}
	
	// deny access
	if (p->permission[0] == '0')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0 || strncmp("/sdcard", filename,7) == 0)
		{	
			printk("\nSD card access denied!SYS_ACCESS\n");
			mm_segment_t oldfs;
	    	oldfs = get_fs();
	    	set_fs(get_ds());

			long res = original_sys_access(dummy_file,mode);
			set_fs(oldfs);
			return res;
		}
	}
	
	return original_sys_access(filename,mode);
}


asmlinkage long our_sys_rename(const char __user *oldpath, const char __user *newpath)
{
	init_policy();

	pid_t pid = current->tgid;

	// avoid deadlock with root pid
	if (pid>100)
		get_name_by_pid(pid, p->pname);

	if(p->pname != NULL)
	{
		check_policy_exist(p);
		if (strncmp("com.estrongs.android.pop", p->pname,24) == 0 || strncmp("com.rhmsoft.fm",p->pname,14) == 0 )
		{
			if(rename_once == false)
				printk("\nSYS_RENAME\n");
			rename_once = true;

		}
	}

	if (p->permission[0] == '1')
	{
		char old_file_name[strlen(oldpath)+strlen(p->zone)+2];
		char new_file_name[strlen(newpath)+strlen(p->zone)+2];
		memset(old_file_name, '\0', sizeof(old_file_name));
		memset(new_file_name, '\0', sizeof(new_file_name));

		strncpy(old_file_name, oldpath, strlen(oldpath));
		strncpy(new_file_name, newpath, strlen(newpath));

		if (strncmp("/mnt/sdcard",oldpath,11) == 0)
		{
			changed_old = true;
			directory_mapping(old_file_name, oldpath, p);
		}

		if (strncmp("/mnt/sdcard",newpath,11) == 0)
		{
			changed_new = true;
			directory_mapping(new_file_name, newpath, p);
		}
		
		if (strncmp("/sdcard",oldpath,7) == 0)
		{
			changed_old = true;
			directory_mapping_sdcard(old_file_name, oldpath, p);
		}

		if (strncmp("/sdcard",newpath,7) == 0)
		{
			changed_new = true;
			directory_mapping_sdcard(new_file_name, newpath, p);
		}

		if (changed_new && changed_old)
		{
			changed_old = false;
			changed_new = false;	

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_rename(old_file_name, new_file_name);
			set_fs(oldfs);
			return res;
		}

		if (changed_old && !changed_new)
		{
			changed_old = false;
			
			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_rename(old_file_name, newpath);
			set_fs(oldfs);
			return res;
		}
		if (changed_new && ! changed_old)
		{
			changed_new = false;
			
			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_rename(oldpath, new_file_name);
			set_fs(oldfs);
			return res;
		}
		

		return original_sys_rename(oldpath,newpath);
	}
	
	// deny access
	if (p->permission[0] == '0')
	{
		if (strncmp("/mnt/sdcard",oldpath,11) == 0 || strncmp("/sdcard", oldpath,7) == 0 
			||strncmp("/mnt/sdcard", newpath,7) == 0 || strncmp("/sdcard", newpath,7) == 0)
		{
			printk("\nSD card access denied!\n");
			mm_segment_t oldfs;
	    	oldfs = get_fs();
	   	 	set_fs(get_ds());

	    	long res = original_sys_rename(dummy_file, dummy_file);
	    	set_fs(oldfs);
	    	return res;
	    }
	}
	
	return original_sys_rename(oldpath,newpath);
}


asmlinkage long our_sys_mkdir(const char __user *filename, int mode)
{
	init_policy();

	pid_t pid = current->tgid;
	
	// avoid deadlock with root pid
	if (pid>100)
		get_name_by_pid(pid, p->pname);
	
	if(p->pname != NULL)
	{
		check_policy_exist(p);
		if (strncmp("com.estrongs.android.pop", p->pname,24) == 0 || strncmp("com.rhmsoft.fm",p->pname,14) == 0 )
		{	
			if (mkdir_once == false)	
				printk("\nSYS_MKDIR\n");
			mkdir_once = true;
		}
	}
	
	if (p->permission[0] == '1')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping(file_name,filename, p);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_mkdir(file_name, mode);
			set_fs(oldfs);
			return res;
		}

		if (strncmp("/sdcard", filename,7) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping_sdcard(file_name,filename, p);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_mkdir(file_name, mode);
			set_fs(oldfs);
			return res;
		}
	}
	
	// deny access
	if (p->permission[0] == '0')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0 || strncmp("/sdcard", filename,7) == 0)
		{
			printk("\nSD card access denied!SYS_MKDIR\n");
			mm_segment_t oldfs;
	    	oldfs = get_fs();
	   	 	set_fs(get_ds());

	    	long res = original_sys_mkdir(dummy_file, mode);
	    	set_fs(oldfs);
	    	return res;
	    }
	}
	
	return original_sys_mkdir(filename, mode);
}

asmlinkage long our_sys_rmdir(const char __user *filename)
{
	init_policy();

	pid_t pid = current->tgid;
	
	// avoid deadlock with root pid
	if (pid>100)
		get_name_by_pid(pid, p->pname);
	
	if(p->pname != NULL)
	{
		check_policy_exist(p);
		if (strncmp("com.estrongs.android.pop", p->pname,24) == 0 || strncmp("com.rhmsoft.fm",p->pname,14) == 0 )
				printk("\nSYS_RMDIR\n");
	}
	
	if (p->permission[0] == '1')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping(file_name,filename, p);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_rmdir(file_name);
			set_fs(oldfs);
			return res;
		}

		if (strncmp("/sdcard", filename,7) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping_sdcard(file_name,filename, p);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_rmdir(file_name);
			set_fs(oldfs);
			return res;
		}
	}
	
	// deny access
	if (p->permission[0] == '0')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0 || strncmp("/sdcard", filename,7) == 0)
		{
			printk("\nSD card access denied!SYS_RMDIR\n");
			mm_segment_t oldfs;
	    	oldfs = get_fs();
	   	 	set_fs(get_ds());

	    	long res = original_sys_rmdir(dummy_file);
	    	set_fs(oldfs);
	    	return res;
	    }
	}
	
	return original_sys_rmdir(filename);
}

asmlinkage long our_sys_chroot(const char __user *filename)
{
	init_policy();

	pid_t pid = current->tgid;
	
	// avoid deadlock with root pid
	if (pid>100)
		get_name_by_pid(pid, p->pname);
	
	if(p->pname != NULL)
	{
		check_policy_exist(p);
		if (strncmp("com.estrongs.android.pop", p->pname,24) == 0 || strncmp("com.rhmsoft.fm",p->pname,14) == 0 )
				printk("\nSYS_CHROOT\n");
	}
	
	if (p->permission[0] == '1')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping(file_name,filename, p);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_chroot(file_name);
			set_fs(oldfs);
			return res;
		}

		if (strncmp("/sdcard", filename,7) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping_sdcard(file_name,filename, p);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_chroot(file_name);
			set_fs(oldfs);
			return res;
		}
	}
	
	// deny access
	if (p->permission[0] == '0')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0 || strncmp("/sdcard", filename,7) == 0)
		{	
			printk("\nSD card access denied!\n");
			mm_segment_t oldfs;
	    	oldfs = get_fs();
	   	 	set_fs(get_ds());

	    	long res = original_sys_chroot(dummy_file);
	    	set_fs(oldfs);
	    	return res;
	    }
	}
	
	return original_sys_chroot(filename);
}

asmlinkage long our_sys_symlink(const char __user *oldpath, const char __user *newpath)
{
	init_policy();

	pid_t pid = current->tgid;

	// avoid deadlock with root pid
	if (pid>100)
		get_name_by_pid(pid, p->pname);

	if(p->pname != NULL)
	{
		check_policy_exist(p);
		if (strncmp("com.estrongs.android.pop", p->pname,24) == 0 || strncmp("com.rhmsoft.fm",p->pname,14) == 0 )
				printk("\nSYS_SYMLINK\n");
	}

	if (p->permission[0] == '1')
	{
		char old_file_name[strlen(oldpath)+strlen(p->zone)+2];
		char new_file_name[strlen(newpath)+strlen(p->zone)+2];
		memset(old_file_name, '\0', sizeof(old_file_name));
		memset(new_file_name, '\0', sizeof(new_file_name));

		strncpy(old_file_name, oldpath, strlen(oldpath));
		strncpy(new_file_name, newpath, strlen(newpath));

		if (strncmp("/mnt/sdcard",oldpath,11) == 0)
		{
			changed_old = true;
			directory_mapping(old_file_name, oldpath, p);
		}

		if (strncmp("/mnt/sdcard",newpath,11) == 0)
		{
			changed_new = true;
			directory_mapping(new_file_name, newpath, p);
		}
		
		if (strncmp("/sdcard",oldpath,7) == 0)
		{
			changed_old = true;
			directory_mapping_sdcard(old_file_name, oldpath, p);
		}

		if (strncmp("/sdcard",newpath,7) == 0)
		{
			changed_new = true;
			directory_mapping_sdcard(new_file_name, newpath, p);
		}

		if (changed_new && changed_old)
		{
			changed_old = false;
			changed_new = false;	

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_symlink(old_file_name, new_file_name);
			set_fs(oldfs);
			return res;
		}

		if (changed_old && !changed_new)
		{
			changed_old = false;
			
			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_symlink(old_file_name, newpath);
			set_fs(oldfs);
			return res;
		}
		if (changed_new && !changed_old)
		{
			changed_new = false;
			
			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_symlink(oldpath, new_file_name);
			set_fs(oldfs);
			return res;
		}
	

		return original_sys_symlink(oldpath,newpath);
	}
	
	// deny access
	if (p->permission[0] == '0')
	{
		if (strncmp("/mnt/sdcard",oldpath,11) == 0 || strncmp("/sdcard", oldpath,7) == 0 
			||strncmp("/mnt/sdcard", newpath,7) == 0 || strncmp("/sdcard", newpath,7) == 0)
		{	
			printk("\nSD card access denied!\n");
			mm_segment_t oldfs;
	    	oldfs = get_fs();
	   	 	set_fs(get_ds());

	    	long res = original_sys_symlink(dummy_file, dummy_file);
	    	set_fs(oldfs);
	    	return res;
	    }
	}
	
	return original_sys_symlink(oldpath,newpath);
}

asmlinkage long our_sys_readlink(const char __user *filename, char __user *buf, int bufsiz)
{
	init_policy();

	pid_t pid = current->tgid;
	
	// avoid deadlock with root pid
	if (pid>100)
		get_name_by_pid(pid, p->pname);
	
	if(p->pname != NULL)
	{
		check_policy_exist(p);
		if (strncmp("com.estrongs.android.pop", p->pname,24) == 0 || strncmp("com.rhmsoft.fm",p->pname,14) == 0 )
				printk("\nSYS_READLINK\n");
	}
	
	if (p->permission[0] == '1')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping(file_name,filename, p);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_readlink(file_name, buf, bufsiz);
			set_fs(oldfs);
			return res;
		}

		if (strncmp("/sdcard", filename,7) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping_sdcard(file_name,filename, p);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_readlink(file_name, buf, bufsiz);
			set_fs(oldfs);
			return res;
		}
	}
	
	// deny access
	if (p->permission[0] == '0')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0 || strncmp("/sdcard", filename,7) == 0)
		{	
			printk("\nSD card access denied!\n");
			mm_segment_t oldfs;
	    	oldfs = get_fs();
	   	 	set_fs(get_ds());

	    	long res = original_sys_readlink(dummy_file, buf, bufsiz);
	    	set_fs(oldfs);
	    	return res;
	    }
	}
	
	return original_sys_readlink(filename, buf, bufsiz);
}

asmlinkage long our_sys_truncate(const char __user *filename, unsigned long length)
{
	init_policy();

	pid_t pid = current->tgid;
	
	// avoid deadlock with root pid
	if (pid>100)
		get_name_by_pid(pid, p->pname);
	
	if(p->pname != NULL)
	{
		check_policy_exist(p);
		if (strncmp("com.estrongs.android.pop", p->pname,24) == 0 || strncmp("com.rhmsoft.fm",p->pname,14) == 0 )
			printk("\nSYS_TRUNCATE\n");
	}
	
	if (p->permission[0] == '1')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping(file_name,filename, p);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_truncate(file_name, length);
			set_fs(oldfs);
			return res;
		}

		if (strncmp("/sdcard", filename,7) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping_sdcard(file_name,filename, p);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_truncate(file_name, length);
			set_fs(oldfs);
			return res;
		}
	}
	
	// deny access
	if (p->permission[0] == '0')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0 || strncmp("/sdcard", filename,7) == 0)
		{	
			printk("\nSD card access denied!\n");
			mm_segment_t oldfs;
	    	oldfs = get_fs();
	   	 	set_fs(get_ds());

	    	long res = original_sys_truncate(dummy_file, length);
	    	set_fs(oldfs);
	    	return res;
	    }
	}
	
	return original_sys_truncate(filename, length);
}


asmlinkage long our_sys_statfs64(const char __user *filename, size_t sz, struct statfs64 __user *buf)
{
	init_policy();

	pid_t pid = current->tgid;
	
	// avoid deadlock with root pid
	if (pid>100)
		get_name_by_pid(pid, p->pname);
	
	if(p->pname != NULL)
	{
		check_policy_exist(p);
		if (strncmp("com.estrongs.android.pop", p->pname,24) == 0 || strncmp("com.rhmsoft.fm",p->pname,14) == 0 )
			printk("\nSYS_STATFS64\n");
	}
	
	if (p->permission[0] == '1')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping(file_name,filename, p);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_statfs64(file_name, sz, buf);
			set_fs(oldfs);
			return res;
		}

		if (strncmp("/sdcard", filename,7) == 0)
		{
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping_sdcard(file_name,filename, p);

			mm_segment_t oldfs;
    		oldfs = get_fs();
    		set_fs(get_ds());

			long res = original_sys_statfs64(file_name, sz, buf);
			set_fs(oldfs);
			return res;
		}
	}

	// deny access
	if (p->permission[0] == '0')
	{
		if (strncmp("/mnt/sdcard",filename,11) == 0 || strncmp("/sdcard", filename,7) == 0)
		{	
			printk("\nSD card access denied!SYS_STATFS64\n");
			mm_segment_t oldfs;
	    	oldfs = get_fs();
	   	 	set_fs(get_ds());

	    	long res = original_sys_statfs64(dummy_file, sz, buf);
	    	set_fs(oldfs);
	    	return res;
	    }
	}
	
	return original_sys_statfs64(filename, sz, buf);
}


asmlinkage long our_sys_fstat64(unsigned long fd, struct stat64 __user *statbuf)
{
	init_policy();

	pid_t pid = current->tgid;
	
	// avoid deadlock with root pid
	if (pid>100)
		get_name_by_pid(pid, p->pname);
	
	if(p->pname != NULL)
	{
		check_policy_exist(p);
		if (strncmp("com.estrongs.android.pop", p->pname,24) == 0 || strncmp("com.rhmsoft.fm",p->pname,14) == 0 )
		{		
			if(fstat64_once == false)
				printk("\nSYS_FSTAT64\n");
			fstat64_once = true;
		}
	}
	
	// deny access
	if (p->permission[0] == '0')
	{
		printk("\nSD card access denied!SYS_FSTAT64\n");
		//return;
	}
	
	return original_sys_fstat64(fd, statbuf);
}


asmlinkage long our_sys_ftruncate(unsigned int fd, unsigned long length)
{
	init_policy();

	pid_t pid = current->tgid;
	
	// avoid deadlock with root pid
	if (pid>100)
		get_name_by_pid(pid, p->pname);
	
	if(p->pname != NULL)
	{
		check_policy_exist(p);
		if (strncmp("com.estrongs.android.pop", p->pname,24) == 0 || strncmp("com.rhmsoft.fm",p->pname,14) == 0 )
			printk("\nSYS_FTRUNCATE\n");
	}
	
	// deny access
	if (p->permission[0] == '0')
	{
		printk("\nSD card access denied!\n");
		//return;
	}
	
	return original_sys_ftruncate(fd, length);
}


/*
*	Internet access system calls
*/

asmlinkage long our_sys_socket(int p1, int p2, int p3)
{
	init_policy();

	pid_t pid = current->tgid;
	
	// avoid deadlock with root pid
	if (pid>100)
		get_name_by_pid(pid, p->pname);
	
	if(p->pname != NULL)
		check_policy_exist(p);

	// deny access
	if (p->permission[1] == '0')
	{
		printk("\nInternet access denied!\n");
		return;
	}

	return original_sys_socket(p1,p2,p3);
}

asmlinkage long our_sys_bind(int n1, struct sockaddr __user * addr, int n2)
{
	init_policy();

	pid_t pid = current->tgid;
	
	// avoid deadlock with root pid
	if (pid>100)
		get_name_by_pid(pid, p->pname);
	
	if(p->pname != NULL)
		check_policy_exist(p);

	// deny access
	if (p->permission[1] == '0')
	{
		printk("\nInternet access denied!\n");
		return;
	}
	
	return original_sys_bind(n1, addr, n2);
}

asmlinkage long our_sys_connect(int n1, struct sockaddr __user * addr, int n2)
{
	init_policy();

	pid_t pid = current->tgid;
	
	// avoid deadlock with root pid
	if (pid>100)
		get_name_by_pid(pid, p->pname);

	if(p->pname != NULL)
		check_policy_exist(p);
		
	// deny access
	if (p->permission[1] == '0')
	{
		printk("\nInternet access denied!\n");
		return;
	}
	
	return original_sys_connect(n1, addr, n2);
}


/* 
 * Initialize the module - Register the character device 
 */
int policy_init()
{
	int ret_val;

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

	policy_class = class_create(THIS_MODULE, DEVICE_NAME);
	device_create(policy_class, NULL, MKDEV(MAJOR_NUM, devlist[0].minor), NULL,devlist[0].name);

	/*
	* init function for vector table
	*/
	tableptr = &table;
	vector_init(tableptr, sizeof(struct _policy), 0);

	p = &pelem;

	
	/*
	 * init function pointer in sched.h
	 */	
	//sched_check_valid_pid = check_valid_pid;


	printk(KERN_INFO "---> Changing syscall table pointer ========== \n");
		
	sys_call_table_pointer = (void*) SYS_CALL_TABLE_ADDRESS;

	/*SD card system calls*/

    original_sys_read = sys_call_table_pointer[__NR_read];
	original_sys_write = sys_call_table_pointer[__NR_write];
	original_sys_open = sys_call_table_pointer[__NR_open];
    
    original_sys_link = sys_call_table_pointer[__NR_link];
    original_sys_unlink = sys_call_table_pointer[__NR_unlink];
    //original_sys_execve = sys_call_table_pointer[__NR_execve]; 
    original_sys_chdir = sys_call_table_pointer[__NR_chdir];
    original_sys_mknod = sys_call_table_pointer[__NR_mknod];
    original_sys_chmod = sys_call_table_pointer[__NR_chmod];
    original_sys_lchown = sys_call_table_pointer[__NR_lchown];
    original_sys_lchown32 = sys_call_table_pointer[__NR_lchown32];
    original_sys_chown32 = sys_call_table_pointer[__NR_chown32];
    original_sys_stat64 = sys_call_table_pointer[__NR_stat64];
    original_sys_lstat64 = sys_call_table_pointer[__NR_lstat64];
    original_sys_utimes = sys_call_table_pointer[__NR_utimes];
	original_sys_access = sys_call_table_pointer[__NR_access];
	original_sys_rename = sys_call_table_pointer[__NR_rename];
	original_sys_mkdir = sys_call_table_pointer[__NR_mkdir];
	original_sys_rmdir = sys_call_table_pointer[__NR_rmdir];
	original_sys_chroot = sys_call_table_pointer[__NR_chroot];
	original_sys_symlink = sys_call_table_pointer[__NR_symlink];
	original_sys_readlink = sys_call_table_pointer[__NR_readlink];
	original_sys_truncate = sys_call_table_pointer[__NR_truncate];
	original_sys_ftruncate = sys_call_table_pointer[__NR_ftruncate];
	original_sys_fstat64 = sys_call_table_pointer[__NR_fstat64];
	original_sys_statfs64 = sys_call_table_pointer[__NR_statfs64];

	/*
	original_sys_creat = sys_call_table_pointer[__NR_creat];
	original_sys_openat = sys_call_table_pointer[__NR_openat];
	original_sys_mkdirat = sys_call_table_pointer[__NR_mkdirat];
	original_sys_mknodat = sys_call_table_pointer[__NR_mknodat];
	original_sys_fchownat = sys_call_table_pointer[__NR_fchownat];
    original_sys_futimesat = sys_call_table_pointer[__NR_futimesat];
    original_sys_fstatat64 = sys_call_table_pointer[__NR_fstatat64];
    original_sys_unlinkat = sys_call_table_pointer[__NR_unlinkat];
    original_sys_renameat = sys_call_table_pointer[__NR_renameat];
    original_sys_linkat = sys_call_table_pointer[__NR_linkat];
    original_sys_symlinkat = sys_call_table_pointer[__NR_symlinkat];
    original_sys_readlinkat = sys_call_table_pointer[__NR_readlinkat];
    original_sys_fchmodat = sys_call_table_pointer[__NR_fchmodat];
    original_sys_faccessat = sys_call_table_pointer[__NR_faccessat];
    original_sys_statfs = sys_call_table_pointer[__NR_statfs];
    original_sys_statfs64 = sys_call_table_pointer[__NR_statfs64];
    original_sys_fstatfs = sys_call_table_pointer[__NR_fstatfs];
    original_sys_fstatfs64 = sys_call_table_pointer[__NR_fstatfs64];
    original_sys_stat = sys_call_table_pointer[__NR_stat];
    original_sys_lstat = sys_call_table_pointer[__NR_lstat];
    original_sys_fstat = sys_call_table_pointer[__NR_fstat];
    original_sys_fstat64 = sys_call_table_pointer[__NR_fstat64];
    original_sys_truncate64 = sys_call_table_pointer[__NR_truncate64];
    original_sys_ftruncate64 = sys_call_table_pointer[__NR_ftruncate64];
    original_sys_ftruncate = sys_call_table_pointer[__NR_ftruncate];
    original_sys_ustat = sys_call_table_pointer[__NR_ustat];
    */
    

	/*Internet access system calls*/

	original_sys_socket = sys_call_table_pointer[__NR_socket];	
	original_sys_bind = sys_call_table_pointer[__NR_bind];
	original_sys_connect = sys_call_table_pointer[__NR_connect];	

	printk(KERN_INFO "---> Done. \n");
	

	/*SD card system calls*/

    sys_call_table_pointer[__NR_read]  = our_sys_read;
    sys_call_table_pointer[__NR_write] = our_sys_write;
    sys_call_table_pointer[__NR_open] = our_sys_open;
    
    sys_call_table_pointer[__NR_link] = our_sys_link;
    sys_call_table_pointer[__NR_unlink] = our_sys_unlink;
    //sys_call_table_pointer[__NR_execve] = our_sys_execve;
    sys_call_table_pointer[__NR_chdir] = our_sys_chdir;
    sys_call_table_pointer[__NR_mknod] = our_sys_mknod;
    sys_call_table_pointer[__NR_chmod] = our_sys_chmod;
    sys_call_table_pointer[__NR_lchown] = our_sys_lchown;
    sys_call_table_pointer[__NR_lchown32] = our_sys_lchown32;
    sys_call_table_pointer[__NR_chown32] = our_sys_chown32;
    sys_call_table_pointer[__NR_stat64] = our_sys_stat64;
    sys_call_table_pointer[__NR_lstat64] = our_sys_lstat64;
    sys_call_table_pointer[__NR_utimes] = our_sys_utimes;
	sys_call_table_pointer[__NR_access] = our_sys_access;
	sys_call_table_pointer[__NR_rename] = our_sys_rename;
	sys_call_table_pointer[__NR_mkdir] = our_sys_mkdir;
	sys_call_table_pointer[__NR_rmdir] = our_sys_rmdir;
	sys_call_table_pointer[__NR_chroot] = our_sys_chroot;
	sys_call_table_pointer[__NR_symlink] = our_sys_symlink;
	sys_call_table_pointer[__NR_readlink] = our_sys_readlink;
	sys_call_table_pointer[__NR_truncate] = our_sys_truncate;
	sys_call_table_pointer[__NR_ftruncate] = our_sys_ftruncate;
	sys_call_table_pointer[__NR_fstat64] = our_sys_fstat64;
	sys_call_table_pointer[__NR_statfs64] = our_sys_statfs64;

	/*
	sys_call_table_pointer[__NR_creat] = our_sys_creat;
	sys_call_table_pointer[__NR_openat] = our_sys_openat;
	sys_call_table_pointer[__NR_mkdirat] = our_sys_mkdirat;
	sys_call_table_pointer[__NR_mknodat] = our_sys_mknodat;
	sys_call_table_pointer[__NR_fchownat] = our_sys_fchownat;
    sys_call_table_pointer[__NR_futimesat] = our_sys_futimesat;
    sys_call_table_pointer[__NR_fstatat64] = our_sys_fstatat64;
    sys_call_table_pointer[__NR_unlinkat] = our_sys_unlinkat;
    sys_call_table_pointer[__NR_renameat] = our_sys_renameat;
    sys_call_table_pointer[__NR_linkat] = our_sys_linkat;
    sys_call_table_pointer[__NR_symlinkat] = our_sys_symlinkat;
    sys_call_table_pointer[__NR_readlinkat] = our_sys_readlinkat;
    sys_call_table_pointer[__NR_fchmodat] = our_sys_fchmodat;
    sys_call_table_pointer[__NR_faccessat] = our_sys_faccessat;
    sys_call_table_pointer[__NR_statfs] = our_sys_statfs;
    sys_call_table_pointer[__NR_statfs64] = our_sys_statfs64;
    sys_call_table_pointer[__NR_fstatfs] = our_sys_fstatfs;
    sys_call_table_pointer[__NR_fstatfs64] = our_sys_fstatfs64;
    sys_call_table_pointer[__NR_stat] = our_sys_stat;
    sys_call_table_pointer[__NR_lstat] = our_sys_lstat;
    sys_call_table_pointer[__NR_fstat] = our_sys_fstat;
    sys_call_table_pointer[__NR_fstat64] = our_sys_fstat64;
    sys_call_table_pointer[__NR_truncate64] = our_sys_truncate64;
    sys_call_table_pointer[__NR_ftruncate64] = our_sys_ftruncate64;
    sys_call_table_pointer[__NR_ftruncate] = our_sys_ftruncate;
    sys_call_table_pointer[__NR_ustat] = our_sys_ustat;
    */
	

	/*Internet access system calls*/

	sys_call_table_pointer[__NR_socket] = our_sys_socket;
	sys_call_table_pointer[__NR_bind] = our_sys_bind;
	sys_call_table_pointer[__NR_connect] = our_sys_connect;

 	getuid_call = sys_call_table_pointer[__NR_getuid];


	return 0;
}

/* 
 * Cleanup - unregister the appropriate file from /proc 
 */
void policy_cleanup()
{

	/*
	 * clear function pointer in sched.h
	 */	
	//sched_check_valid_pid = NULL;


	/* 
	 * Unregister the device 
	 */
	device_destroy(policy_class, MKDEV(MAJOR_NUM, devlist[0].minor));
	class_destroy(policy_class);
	unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
	printk( "<1>%s: module unloaded\n", DEVICE_NAME );

	printk(KERN_INFO  "Clean up syscall_read module!!\n");

	/*SD card system calls*/

	sys_call_table_pointer[__NR_read]  = original_sys_read;
	sys_call_table_pointer[__NR_write] = original_sys_write;
	sys_call_table_pointer[__NR_open] = original_sys_open;
	
	
	sys_call_table_pointer[__NR_link] = original_sys_link;
	sys_call_table_pointer[__NR_unlink] = original_sys_unlink;	
	//sys_call_table_pointer[__NR_execve] = original_sys_execve;	
	sys_call_table_pointer[__NR_chdir] = original_sys_chdir;
	sys_call_table_pointer[__NR_mknod] = original_sys_mknod;	
	sys_call_table_pointer[__NR_chmod] = original_sys_chmod;
	sys_call_table_pointer[__NR_lchown] = original_sys_lchown;
	sys_call_table_pointer[__NR_lchown32] = original_sys_lchown32;
	sys_call_table_pointer[__NR_chown32] = original_sys_chown32;
	sys_call_table_pointer[__NR_stat64] = original_sys_stat64;
	sys_call_table_pointer[__NR_lstat64] = original_sys_lstat64;
	sys_call_table_pointer[__NR_utimes] = original_sys_utimes;
	sys_call_table_pointer[__NR_access] = original_sys_access;
	sys_call_table_pointer[__NR_rename] = original_sys_rename;
	sys_call_table_pointer[__NR_mkdir] = original_sys_mkdir;
	sys_call_table_pointer[__NR_rmdir] = original_sys_rmdir;
	sys_call_table_pointer[__NR_chroot] = original_sys_chroot;
	sys_call_table_pointer[__NR_symlink] = original_sys_symlink;
	sys_call_table_pointer[__NR_readlink] = original_sys_readlink;
	sys_call_table_pointer[__NR_truncate] = original_sys_truncate;
	sys_call_table_pointer[__NR_ftruncate] = original_sys_ftruncate;
	sys_call_table_pointer[__NR_fstat64] = original_sys_fstat64;
	sys_call_table_pointer[__NR_statfs64] = original_sys_statfs64;

	/*
	sys_call_table_pointer[__NR_creat] = original_sys_creat;
	sys_call_table_pointer[__NR_openat] = original_sys_openat;
	sys_call_table_pointer[__NR_mkdirat] = original_sys_mkdirat;
	sys_call_table_pointer[__NR_mknodat] = original_sys_mknodat;
	sys_call_table_pointer[__NR_fchownat] = original_sys_fchownat;
    sys_call_table_pointer[__NR_futimesat] = original_sys_futimesat;
    sys_call_table_pointer[__NR_fstatat64] = original_sys_fstatat64;
    sys_call_table_pointer[__NR_unlinkat] = original_sys_unlinkat;
    sys_call_table_pointer[__NR_renameat] = original_sys_renameat;
    sys_call_table_pointer[__NR_linkat] = original_sys_linkat;
    sys_call_table_pointer[__NR_symlinkat] = original_sys_symlinkat;
    sys_call_table_pointer[__NR_readlinkat] = original_sys_readlinkat;
    sys_call_table_pointer[__NR_fchmodat] = original_sys_fchmodat;
    sys_call_table_pointer[__NR_faccessat] = original_sys_faccessat;
    sys_call_table_pointer[__NR_statfs] = original_sys_statfs;
    sys_call_table_pointer[__NR_statfs64] = original_sys_statfs64;
    sys_call_table_pointer[__NR_fstatfs] = original_sys_fstatfs;
    sys_call_table_pointer[__NR_fstatfs64] = original_sys_fstatfs64;
    sys_call_table_pointer[__NR_stat] = original_sys_stat;
    sys_call_table_pointer[__NR_lstat] = original_sys_lstat;
    sys_call_table_pointer[__NR_fstat] = original_sys_fstat;
    sys_call_table_pointer[__NR_fstat64] = original_sys_fstat64;
    sys_call_table_pointer[__NR_truncate64] = original_sys_truncate64;
    sys_call_table_pointer[__NR_ftruncate64] = original_sys_ftruncate64;
    sys_call_table_pointer[__NR_ftruncate] = original_sys_ftruncate;
    sys_call_table_pointer[__NR_ustat] = original_sys_ustat;
    */
	

	/*Internet access system calls*/

	sys_call_table_pointer[__NR_socket] = original_sys_socket;
	sys_call_table_pointer[__NR_bind] = original_sys_bind;
	sys_call_table_pointer[__NR_connect] = original_sys_connect;

}

module_init(policy_init)
module_exit(policy_cleanup)
MODULE_LICENSE("GPL");
