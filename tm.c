#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/sched/signal.h>
#include <linux/uaccess.h>

#define PROC_NAME "task_manager"

// Structure to hold process information
struct process_info {
    pid_t pid;
    char comm[TASK_COMM_LEN];
    unsigned long user_time;
    unsigned long system_time;
    unsigned long num_threads;
    unsigned long vm_rss; // Resident Set Size
    unsigned long vm_size; // Virtual memory size
};

// Function to display process information in /proc
static int show_task_manager(struct seq_file *m, void *v) {
    struct task_struct *task;
    struct process_info pinfo;

    seq_printf(m, "PID\tProcess Name\tUser Time\tSystem Time\tThreads\tVmRSS\tVmSize\n");

    for_each_process(task) {
        pinfo.pid = task->pid;
        get_task_comm(pinfo.comm, task);
        pinfo.user_time = task->utime;
        pinfo.system_time = task->stime;
        pinfo.num_threads = get_nr_threads(task);
        
        if (task->mm) {
    		pinfo.vm_rss = atomic_long_read(&task->mm->rss_stat.count[MM_FILEPAGES]) << PAGE_SHIFT;
    		pinfo.vm_size = task->mm->total_vm << PAGE_SHIFT;
	} else {
    		pinfo.vm_rss = 0;
    		pinfo.vm_size = 0;
	}


        seq_printf(m, "%d\t%s\t%lu\t%lu\t%lu\t%lu\t%lu\n",
                   pinfo.pid, pinfo.comm, pinfo.user_time, pinfo.system_time,
                   pinfo.num_threads, pinfo.vm_rss, pinfo.vm_size);
    }
    return 0;
}

// Open function for /proc file
static int task_manager_open(struct inode *inode, struct file *file) {
    return single_open(file, show_task_manager, NULL);
}

// File operations structure for /proc file
static const struct proc_ops task_manager_fops = {
    .proc_open = task_manager_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

// Module initialization function
static int __init task_manager_init(void) {
    proc_create(PROC_NAME, 0, NULL, &task_manager_fops);
    printk(KERN_INFO "Task Manager Module Loaded\n");
    return 0;
}

// Module cleanup function
static void __exit task_manager_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "Task Manager Module Unloaded\n");
}

module_init(task_manager_init);
module_exit(task_manager_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("AS");
MODULE_DESCRIPTION("A task manager module that shows extended process information.");

