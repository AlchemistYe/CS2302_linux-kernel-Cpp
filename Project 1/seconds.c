#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/jiffies.h>
#include <asm/param.h>

#define BUFFER_SIZE 128
#define PROC_NAME "seconds"
unsigned long int begin;
unsigned long int end;
unsigned long int hz=HZ;

ssize_t proc_read(struct file *file,char __user *usr_buf,size_t count,loff_t *pos);

static const struct proc_ops proc_ops = {
    .proc_read=proc_read,
 };
 
 int proc_init(void) {
    proc_create(PROC_NAME,0666,NULL,&proc_ops);
    begin=jiffies;
    printk(KERN_INFO "proc/seconds is created!");
    printk(KERN_INFO "%lu\n",begin);
    return 0;
 }
 
 void proc_exit(void) {  
    remove_proc_entry(PROC_NAME,NULL);
    printk(KERN_INFO "proc/seconds is deleted!");
    printk(KERN_INFO "%lu\n",end);
 }
 
 ssize_t proc_read(struct file *file,char __user *usr_buf,size_t count,loff_t *pos) {
    int rv=0;
    char buffer[BUFFER_SIZE];
    static int completed=0;
    if(completed){
      completed=0;
      return 0;
    }
    completed=1;
    end = jiffies;
    rv=sprintf(buffer,"%d seconds have elapsed",(end-begin)/hz);
    copy_to_user(usr_buf,buffer,rv);
    return rv; 
 }
 module_init(proc_init);
 module_exit(proc_exit);
 
 MODULE_LICENSE("GPL");
 MODULE_DESCRIPTION("Hello Module");
 MODULE_AUTHOR("YZC");
