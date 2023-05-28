#include <linux/kernel.h>
#include <linux/thread_info.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/list.h>

#define PRINT_DEBUG

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
        // TODO: check if real_parent or parent !
        curr=curr->real_parent;
    }
    //TODO: add idle (pid==0) or not ? right now not added

    return sum;
}

struct task_struct* recurtion_heavist(struct task_struct* curr_task){
    struct task_struct* max = NULL;
    struct task_struct* curr = NULL;
    struct task_struct* hevi_child = NULL;
    struct list_head* child = NULL;
    if (list_empty(&curr_task->children)) {
        return NULL;
    }
    list_for_each(child, &current->children){
        curr = list_entry(child, struct task_struct, sibling);
        hevi_child = recurtion_heavist(curr);
        if (max == NULL){
            max = curr;
        }
        if (hevi_child != NULL){
            if (curr->weight > hevi_child->weight || (curr->weight == hevi_child->weight && curr->pid < hevi_child->pid)){
                hevi_child = curr;
            }
        }
        else {
            hevi_child = curr;
        }
        if (hevi_child->weight > max->weight || (hevi_child->weight == max->weight && hevi_child->pid <= max->pid)){
            max = hevi_child;
        }
    }
    return max;
}


asmlinkage long sys_get_heaviest_descendant(void){

    if(list_empty(&current->children)){
        return -ECHILD;
    }

    return recurtion_heavist(current)->pid;
}

























