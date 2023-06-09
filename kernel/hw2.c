#include <linux/kernel.h>
#include <linux/thread_info.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/list.h>

//#define PRINT_DEBUG

asmlinkage long sys_hello(void){
    printk("Hello, World!\n");
    return 0;
}

asmlinkage long sys_set_weight(int weight){
#ifdef PRINT_DEBUG
    printk("old weight is: %d\n", current->weight);
#endif
    if(weight<0){
        return -EINVAL;
    }
    current->weight = weight;
#ifdef PRINT_DEBUG
    printk("new weight is: %d\n", current->weight);
#endif
    return 0;
}

asmlinkage long sys_get_weight(void){
#ifdef PRINT_DEBUG
    printk("current weight is: %d\n", current->weight);
#endif
    return 	current->weight;
}

asmlinkage long sys_get_ancestor_sum(void){
    int sum=0;
    struct task_struct* curr = current;
    while(curr->pid != 0){
#ifdef PRINT_DEBUG
        printk("current_process pid is: %d and weight is: %d\n", curr->pid ,curr->weight);
#endif
        sum+=curr->weight;
        curr=curr->real_parent;
    }
    return sum;
}


struct task_struct* recursion_heaviest(struct task_struct* curr_task, bool first){
    struct task_struct* max = first ? NULL : curr_task;
    struct task_struct* temp = NULL;
    struct list_head* child = NULL;
    if (list_empty(&curr_task->children)) {
        return max;
    }
    list_for_each(child, &curr_task->children){
        temp = list_entry(child, struct task_struct, sibling);
        temp = recursion_heaviest(temp, false);
        if (max == NULL){
            max = temp;
        }
        else if (temp->weight > max->weight || (temp->weight == max->weight && temp->pid < max->pid)){
            max = temp;
        }
    }
    return max;
}


asmlinkage long sys_get_heaviest_descendant(void){
    if(list_empty(&current->children)){
        return -ECHILD;
    }
    return recursion_heaviest(current, true)->pid;
}