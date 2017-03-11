/*
 *  chardev.c - Create an input/output character device
 */
//#include <stdlib.c>
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
#include <linux/slab.h>
#define SUCCESS 0
#define DEVICE_NAME "policy"
#define BUF_LEN 150

//use system call table to make system call in LKM
#define SYS_CALL_TABLE_ADDRESS 0xc000f304; //new
//#define SYS_CALL_TABLE_ADDRESS 0xc0022f24; old kernel


void __user *sample;

void **sys_call_table_pointer;

#define	major(x)	((int)(((u_int)(x) >> 8)&0xff))	/* major number */
#define	minor(x)	((int)((x)&0xff))		/* minor number */
#define	makedev(x,y)	((dev_t)(((x)<<8) | (y)))	/* create dev_t */

/* getuid() prototype */

asmlinkage int (*getuid_call)();


/* Original system call pointers for SD card*/

asmlinkage long (*original_sys_open)(const char __user *filename, int flags, int mode);
//added by daiting
asmlinkage long (*original_sys_mkdir)(const char __user *filename, int mode);


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


static char external_storage_path[sizeof(struct sec_external_storage)];
static int length_of_external_storage_path = 0;

static vector *tableptr, table;

static policy *p, pelem;
static policy *p1, pelem1;

//static char *dummy_file = "/dev/null";


//struct rule_node* rule_link = NULL;

bool rule_synchronized = false;

/*bool read_once = false;
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
bool fstat64_once = false;*/


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

//change p to p1
int get_name_by_pid(pid_t pid,char *name)
{
         struct file* fd;	 
         char buf[sizeof(p1->pname)];

         snprintf(buf,sizeof(p1->pname),"/proc/%d/cmdline",pid);
         if ( (fd = file_open(buf,O_RDONLY,0)) == -1)
         {
                 printk("daiting Open file :%s,error!\n",buf);
                 return -1;
         }
	 	int ret = file_read(fd,0, buf,sizeof(p1->pname));
         if( ret == -1)
         {
                 printk("daiting Read file error!\n");
                 return -1;
         }
	 file_close(fd);
         strncpy(name,buf,ret);
	 if(ret<sizeof(p1->pname))
		buf[ret] = '\0';

         return 0;
}

//helper function
void list_policy()
{
	int size = vector_length(tableptr);
	int i;
	policy temp_policy, *tempptr;
	tempptr = &temp_policy;

	for (i=0;i<size;i++)
	{
		vector_get(tableptr,i,tempptr);
		
		printk(KERN_INFO "daiting policy %d : %s %s %s. \n", i, tempptr->pname, tempptr->permission, tempptr->zone);

		/*if (strcmp(tempptr->pname,pptr->pname)==0)
		{
			strncpy(pptr->permission,tempptr->permission,sizeof(pptr->permission));
			strncpy(pptr->zone,tempptr->zone,sizeof(pptr->zone));
			break;
		}*/
	}
}

//
void rm_substr(char* s, const char* toremove)
{	
	//while( s = strstr(s, toremove))
		memmove(s, s+strlen(toremove), 1+strlen(s+strlen(toremove)));
}

//for string split helper
/*char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

   
    count += last_comma < (a_str + strlen(a_str) - 1);

   
     
    count++;

    result = kmalloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strsep(a_str, delim);

        while (token)
        {
            //assert(idx < count);
		if(idx >= count) break;
            *(result + idx++) = strdup(token);
            token = strsep(0, delim);
        }
        //assert(idx == count - 1);
	if(idx != count - 1)return 0;
        *(result + idx) = 0;
    }

    return result;
}*/

/* 
 * This is called whenever a process attempts to open the device file 
 */
static int device_open(struct inode *inode, struct file *file)
{
#ifdef DEBUG
	printk(KERN_INFO "daiting device_open(%p)\n", file);
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
	printk(KERN_INFO "daiting device_release(%p,%p)\n", inode, file);
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
	printk(KERN_INFO "daiting device_read(%p,%p,%d)\n", file, buffer, length);
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
	printk(KERN_INFO "daiting Read %d bytes, %d left\n", bytes_read, length);
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
	printk(KERN_INFO "daiting device_write(%p,%s,%d)", file, buffer, length);
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
int device_ioctl(	/* see include/linux/fs.h  important for the new kernel modification daiting no inode anymore*/
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
	char buf2[sizeof(struct sec_external_storage)];
	struct task_struct *task_list; 

	policy *pptr, p2;
    pptr = &p2;

    char packagename[sizeof(pptr->pname)];
	char access[sizeof(pptr->permission)];
	char zone[sizeof(pptr->zone)];

printk("@@@@@@@@@@@@@@@@@@@@@ daiting in device ioctl ioctl_num %08x : %08x***************************\n", IOCTL_ADD_RULE, ioctl_num);

//printk("@daiting in device ioctl parse num (dir type nr size) %08x  %08x  %08x %08x\n", _IOC_DIR(ioctl_num),_IOC_TYPE(ioctl_num), _IOC_NR(ioctl_num), _IOC_SIZE(ioctl_num));
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
		printk("@@@@@@@@@@@@@@@@@@@@@ daiting Enter add rules***************************\n");
		
		//device_write(file, (char *)ioctl_param, capacity, capacity*rule_num);
		
		
		/*//add a node
		temp_node = kmalloc(sizeof(struct rule_node), GFP_KERNEL);
		temp_node->pid = -1;
		temp_node->next = rule_link;
		rule_link = temp_node;*/
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
			//access[0] = buf[index+1];
			//access[1] = buf[index+2];
		}

		int index2 = -1;
		for (i = index+1; i < capacity; i++)
		{
			if (buf[i] == ',')
			{
				index2 = i;	
				break;			
			}	
		}

		if (index2 != -1)
		{
			strncpy(access, buf+index+1, index2-index-1);
			//access[0] = buf[index+1];
			//access[1] = buf[index+2];
		}

		strncpy(zone, buf+index2+1, sizeof(zone));

		//we may need to parse zone

		// test if add rule works
		printk("\ndaiting The reserved, caller and callee in kernel = %s ;; %s ;; %s\n\n", packagename,access,zone);

		
		//memcpy(cur, access, sizeof(access));
		/*tokens = str_split(access, "+");
		if (tokens) {
			int ii;
			for (ii = 0; *(tokens + ii); ii++) {
				printk("daiting split: %s\n", *(tokens+ii));
				free(*(tokens+ii));
			}
			free(tokens);
		}*/
		/*char *token, *cur = access;
		while (token = strsep(&cur, "+")) {
			printk("daiting split: %s\n", token);
		}*/
		

		memcpy(pptr->pname, packagename, sizeof(pptr->pname));
		memcpy(pptr->permission, access, sizeof(pptr->permission));	
		memcpy(pptr->zone, zone, sizeof(pptr->zone));

		vector_push(tableptr,pptr);
		
		rule_num++;

		rule_synchronized = true;  
	      	//printk(KERN_ALERT"The number of process is:%d\n",count);
		list_policy();

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

		/*if(rule_num > 0)
		{
			for(i=0;i<rule_num;i++)
			{
				temp = rule_link;
				rule_link = rule_link->next;
				kfree(temp);
			}
		}*/
		rule_num = 0;
		//rule_link = NULL;
		break;

	case IOCTL_EXTERNAL_STORAGE:
		printk("@@@@@@@@@@@@@@@@@@@@@ daiting Enter External Storage***************************\n");
		memset(external_storage_path, '\0', sizeof(external_storage_path) );
		temp = (char*)ioctl_param;

		get_user(ch, temp);
		for (i = 0; ch && i < sizeof(struct sec_external_storage); i++, temp++)
		{
			get_user(ch, temp);
			buf2[i] = ch;
		}

		strncpy(external_storage_path, buf2, sizeof(external_storage_path));
		length_of_external_storage_path = strlen(external_storage_path);

		printk("\nThe external storage path is::: %s \n",external_storage_path);
		printk("\nLength of path is === %d \n", length_of_external_storage_path);
		break;
	}

return 0;
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
	//.ioctl = device_ioctl,
	.unlocked_ioctl = device_ioctl,//newer kernel 3.4
.compat_ioctl = device_ioctl,//newer kernel 3.4 not working
	.open = device_open,
	.release = device_release,	/* a.k.a. close */
};


void init_policy()
{
//printk(KERN_INFO "daiting---> init policy start. \n");
	memset(p->pname, '\0', sizeof(p->pname));
	memset(p->permission, '\0', sizeof(p->permission));	
	memset(p->zone, '\0', sizeof(p->zone));

	memset(p1->pname, '\0', sizeof(p1->pname));
	memset(p1->permission, '\0', sizeof(p1->permission));	
	memset(p1->zone, '\0', sizeof(p1->zone));
//printk(KERN_INFO "daiting---> init policy finish. \n");
}

//void check_policy_exist(policy *pptr) 
int check_policy_exist(policy *pptr) 
{
	//modified by daiting, need more modification on how to locate the rules, only one rule for each app?
	int size = vector_length(tableptr);
	int i;
	policy temp_policy, *tempptr;
	tempptr = &temp_policy;

	//for (i=0;i<size;i++)
	for (i=size-1;i>=0;i--)//search from the latest
	{
		vector_get(tableptr,i,tempptr);

		//if (strcmp(tempptr->pname,pptr->pname)==0)
		if (strcmp(tempptr->zone,p1->pname)==0) // checking callee zone
		{
			strncpy(pptr->permission,tempptr->permission,sizeof(pptr->permission));
			strncpy(pptr->zone,tempptr->zone,sizeof(pptr->zone));
			strncpy(pptr->pname,tempptr->pname,sizeof(pptr->pname));
			//break;
			return 1;
		}
	}

	return 0;
}

void directory_mapping(char *file_name, const char *original_filename, policy *p)
{
	strncpy(file_name,original_filename,length_of_external_storage_path);
	strncpy(file_name+length_of_external_storage_path,"/",1);
	strncpy(file_name+length_of_external_storage_path+1, p->zone,strlen(p->zone));
	if (strlen(original_filename) > length_of_external_storage_path)
		strncpy(file_name+length_of_external_storage_path+1+strlen(p->zone), original_filename+length_of_external_storage_path, strlen(original_filename)-length_of_external_storage_path);
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




asmlinkage long our_sys_open(const char __user *filename, int flags, int mode)
{
	init_policy();

	pid_t pid = current->tgid;
	
	// avoid deadlock with root pid
	if (pid>100)
		get_name_by_pid(pid, p1->pname);
	
	
	/*if(p->pname != NULL)
	{
		check_policy_exist(p);
	}
	
	if (p->permission[0] == '1')
	{
		if (strncmp(external_storage_path,filename,length_of_external_storage_path) == 0)
		{		
			char file_name[strlen(filename)+strlen(p->zone)+2];
			memset(file_name, '\0', sizeof(file_name));

			directory_mapping(file_name,filename, p);

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
		if (strncmp(external_storage_path,filename,length_of_external_storage_path) == 0)
		{
			mm_segment_t oldfs;
	    	oldfs = get_fs();
	   	 	set_fs(get_ds());

	    	long res = original_sys_open(dummy_file, flags, mode);
	    	set_fs(oldfs);
			return res;
		}
	}*/
//if(pid == 2323)
	//printk(KERN_INFO "daiting---> %d open file %s. \n", pid, filename);
/*const char* str = "/storage/sdcard/fileprivate/test.txt";
if (!strcmp(filename, str)) {
	printk(KERN_INFO "daiting---> %d write file %s. \n", pid, filename);
//must have
		mm_segment_t oldfs;
	    	oldfs = get_fs();
	   	 	set_fs(get_ds());
//
	long new_ret = original_sys_open("/storage/sdcard/Android/data/com.example.myapptest/test.txt",flags,mode);
//		
			set_fs(oldfs);//must have for write new file name
//
	printk(KERN_INFO "daiting---> %d ret file %s. %08x\n", pid, filename, new_ret);
	return new_ret;
} else {
	return original_sys_open(filename,flags,mode);
	}*/

//const char* str = "/storage/sdcard/fileprivate/test.txt";
if (strstr(filename, "storage/sdcard")) {
	printk(KERN_INFO "daiting---> %d write file hit %s. \n", pid, filename);

if(!(p1->pname))
	return original_sys_open(filename,flags,mode);	

	printk(KERN_INFO "daiting---> %d write file  %s. \n", pid, p1->pname);//who is writing file
	if(!check_policy_exist(p)){//for process such as system/bin/sh or ls 
		printk(KERN_INFO "daiting---> %d write file  %s policy not found. \n", pid, p1->pname);
		return original_sys_open(filename,flags,mode);	
	}

	if (strstr(p->permission, "com.cyanogenmod.trebuchet") && !strstr(p->permission, "+"))
	{
printk(KERN_INFO "daiting---> write file started by launcher. \n");
		return original_sys_open(filename,flags,mode);
	}
//must have, file write pointer index, important
		mm_segment_t oldfs;
	    	oldfs = get_fs();
	   	 	set_fs(get_ds());
//
	//long new_ret = original_sys_open("/storage/sdcard/Android/data/com.example.myapptest/test.txt",flags,mode);
	/*	char access[sizeof(p->permission)];
		char *token, *cur = access;
		while (token = strsep(&cur, "+")) {
			printk("daiting split: %s\n", token);
		}*/

//char real_filename[612];
		char tempstr[1024];
		strncpy(tempstr, filename, strlen(filename));
		tempstr[strlen(filename)] = '\0';	
printk(KERN_INFO "daiting 1 %s. \n", tempstr);
		rm_substr(tempstr, "/storage/sdcard");//asumptions on the directory
printk(KERN_INFO "daiting 2 %s. \n", tempstr);

	char temp_filename[612];
	const char* str = "/storage/sdcard/Android/data/";
	//const char* str2 = "/test.txt";
	strncpy(temp_filename, str, strlen(str));
	strncpy(temp_filename+strlen(str), p->pname, strlen(p->pname));
	strncpy(temp_filename+strlen(str)+strlen(p->pname), tempstr, strlen(tempstr));
	temp_filename[strlen(str)+strlen(tempstr)+strlen(p->pname)] = '\0';
	//strncpy(temp_filename, strcat(str, p->permission, ), sizeof(p->permission)+39);
printk(KERN_INFO "daiting  %d write pname permission %s %s. \n", pid, p->pname, p->permission);
printk(KERN_INFO "daiting  %d write file name %s. \n", pid, temp_filename);
	
//original_sys_mkdir(temp_filename, mode);//creating app directory, may need to create directory another time?


long new_ret = original_sys_open(temp_filename,flags,mode);
//		
			set_fs(oldfs);//must have for write new file name
//
	printk(KERN_INFO "daiting---> %d ret file %s. %08x\n", pid, filename, new_ret);
	return new_ret;
} else {
	return original_sys_open(filename,flags,mode);
	}


}

asmlinkage long our_sys_mkdir(const char __user *filename, int mode)
{
	init_policy();

	pid_t pid = current->tgid;
	
	// avoid deadlock with root pid
	if (pid>100)
		get_name_by_pid(pid, p1->pname);
	
	/*if(p->pname != NULL)
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
	}*/
//printk(KERN_INFO "daiting---> %d mkdir %s. \n", pid, filename);	

/*const char* str = "/storage/sdcard/fileprivate";
if (!strcmp(filename, str)) {
	printk(KERN_INFO "daiting---> %d mkdir hit %s. \n", pid, filename);

		//switch to kernel call from userspace, switch ds:fs
		mm_segment_t oldfs;
	    	oldfs = get_fs();
	   	 	set_fs(get_ds());

	long new_ret = 	original_sys_mkdir("/storage/sdcard/Android/data/com.example.myapptest", mode);

		set_fs(oldfs);

	printk(KERN_INFO "daiting---> %d mkdir ret %s. %08x\n", pid, filename, new_ret);
	return new_ret;
} else {
	return original_sys_mkdir(filename, mode);
	}*/

//const char* str = "/storage/sdcard/fileprivate";
if (strstr(filename, "storage/sdcard")) {
	printk(KERN_INFO "daiting---> %d mkdir hit %s. \n", pid, filename);
if(!(p1->pname))
	return original_sys_mkdir(filename, mode);
	
	printk(KERN_INFO "daiting---> %d mkdir  %s. \n", pid, p1->pname);
	if(!check_policy_exist(p))
	{
		printk(KERN_INFO "daiting---> %d mkdir  %s policy not found. \n", pid, p1->pname);
		return original_sys_mkdir(filename, mode);
	}
		
	printk(KERN_INFO "daiting---> %d mkdir  p %s and pname %s. \n", pid, p->permission, p->pname);

	if (strstr(p->permission, "com.cyanogenmod.trebuchet") && !strstr(p->permission, "+"))
	{
		printk(KERN_INFO "daiting---> mkdir started by launcher. \n");
		return original_sys_mkdir(filename, mode);
	}


		//switch to kernel call from userspace, switch ds:fs
		mm_segment_t oldfs;
	    	oldfs = get_fs();
	   	 	set_fs(get_ds());

		//long new_ret = 	original_sys_mkdir("/storage/sdcard/Android/data/com.example.myapptest", mode);
	char temp_filename[356];
	const char* str = "/storage/sdcard/Android/data/";
	//const char* str2 = "/test.txt";
	strncpy(temp_filename, str, strlen(str));
	strncpy(temp_filename+strlen(str), p->pname, strlen(p->pname));
	//strncpy(temp_filename+strlen(str)+strlen(p->permission), str2, strlen(str2));
	temp_filename[strlen(str)+strlen(p->pname)] = '\0';
	//strncpy(temp_filename, strcat(str, p->permission, ), sizeof(p->permission)+39);
printk(KERN_INFO "daiting  %d mkdir pname p %s %s. \n", pid, p->pname, p->permission);
printk(KERN_INFO "daiting  %d mkdir file name %s. \n", pid, temp_filename);

		original_sys_mkdir(temp_filename, mode);//creating app directory
		
		char real_filename[612];
		char tempstr[1024];
		strncpy(tempstr, filename, strlen(filename));
		tempstr[strlen(filename)] = '\0';	
printk(KERN_INFO "daiting 1 %s. \n", tempstr);
		rm_substr(tempstr, "/storage/sdcard");//asumptions on the directory
printk(KERN_INFO "daiting 2 %s. \n", tempstr);
		
		strncpy(real_filename, temp_filename, strlen(temp_filename));	
		strncpy(real_filename+strlen(temp_filename), tempstr, strlen(tempstr));	
		real_filename[strlen(temp_filename)+strlen(tempstr)] = '\0';
printk(KERN_INFO "daiting 3 %s. \n", real_filename);
		long new_ret = 	original_sys_mkdir(real_filename, mode);

		set_fs(oldfs);

	printk(KERN_INFO "daiting---> %d mkdir ret %s. %08x\n", pid, filename, new_ret);
	return new_ret;
} else {
	return original_sys_mkdir(filename, mode);
	}
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
        printk( "<1>%s: daiting module loaded\n", DEVICE_NAME );

	ret_val = register_chrdev(MAJOR_NUM, DEVICE_NAME, &Fops);

	/* 
	 * Negative values signify an error 
	 */
	if (ret_val < 0) {
		printk(KERN_ALERT "%s daiting failed with %d\n",
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
printk(KERN_INFO "---> daiting vector init ========== \n");

	p = &pelem;
	p1 = &pelem1;

	
	/*
	 * init function pointer in sched.h
	 */	
	//sched_check_valid_pid = check_valid_pid;


	printk(KERN_INFO "---> daiting Changing syscall table pointer ========== \n");
		
	sys_call_table_pointer = (void*) SYS_CALL_TABLE_ADDRESS;

	/*SD card system calls*/

//1
	original_sys_open = sys_call_table_pointer[__NR_open];
original_sys_mkdir = sys_call_table_pointer[__NR_mkdir];

	printk(KERN_INFO "daiting---> Done. \n");
	

	/*SD card system calls*/
//2

    sys_call_table_pointer[__NR_open] = our_sys_open;
sys_call_table_pointer[__NR_mkdir] = our_sys_mkdir;


 	getuid_call = sys_call_table_pointer[__NR_getuid];


	
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
	printk( "<1>%s: daiting module unloaded\n", DEVICE_NAME );

	printk(KERN_INFO  "daiting Clean up syscall_read module!!\n");

	/*SD card system calls*/
//3
	sys_call_table_pointer[__NR_open] = original_sys_open;

sys_call_table_pointer[__NR_mkdir] = original_sys_mkdir;

}

module_init(policy_init)
module_exit(policy_cleanup)
MODULE_LICENSE("GPL");
