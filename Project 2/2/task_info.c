#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/sched.h>
#include <linux/slab.h>

#define BUFFER_SIZE 128
#define PROC_NAME "pid"

static long l_pid;

static ssize_t proc_read(struct file *file,char __user *usr_buf,size_t count,loff_t *pos);
static ssize_t proc_write(struct file *file,const char __user *usr_buf, size_t count, loff_t *pos);

static const struct proc_ops proc_ops = {
    .proc_read=proc_read,
    .proc_write=proc_write,
 };
 
static int proc_init(void) {
     proc_create(PROC_NAME, 0666, NULL, &proc_ops);

     printk(KERN_INFO "/proc/%s created\n", PROC_NAME);
   
     return 0;
}
 
static void proc_exit(void) {
     remove_proc_entry(PROC_NAME, NULL);

     printk( KERN_INFO "/proc/%s removed\n", PROC_NAME);
}
 
static ssize_t proc_read(struct file *file,char __user *usr_buf,size_t count,loff_t *pos) {
     int rv = 0;
     char buffer[BUFFER_SIZE];
     static int completed = 0;
     struct task_struct *tsk = NULL;

     if (completed) {
            completed = 0;
            return 0;
     }

     tsk = pid_task(find_vpid(l_pid), PIDTYPE_PID);
     if(tsk==NULL){
            printk(KERN_INFO"Invalid PID\n");
            return 0;
     }
     completed = 1;
   
     rv = sprintf(buffer,"command = [%s] pid = [%ld] state = [%ld]\n",tsk->comm,l_pid,tsk->state);
         
     if (copy_to_user(usr_buf, buffer, rv)) {
            rv = -1;
     }

     return rv;
}
 
 

static ssize_t proc_write(struct file *file,const char __user *usr_buf, size_t count, loff_t *pos) {
     char *k_mem;
    
     k_mem=kmalloc(count,GFP_KERNEL);
    
     if (copy_from_user(k_mem, usr_buf, count)) {
	printk( KERN_INFO "Error copying from user\n");
        return -1;
     }
     k_mem[count]=0;
     
     kstrtol(k_mem,10,&l_pid);
    
     kfree(k_mem);
     return count;
}


module_init( proc_init );
module_exit( proc_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Module");
MODULE_AUTHOR("YZC");



