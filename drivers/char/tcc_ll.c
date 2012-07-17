/*
 * linux/drivers/char/tcc_ll.c 
 *
 * Author:  <linux@telechips.com>
 * Created: 21th August, 2009 
 * Description: Telechips linked list 
 *
 * Copyright (c) Telechips, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>

#include <linux/tcc_ll.h>

#define jprintk printk

static void copy_data_node(data_node_t *dst, data_node_t *src)
{
    if (dst && src) {
        dst->id = get_node_id(src);
        dst->callback = get_node_cb(src);
        dst->h_pri = get_node_pri(src);
    }
}

/* data_node_t functions */
static data_node_t *new_node(data_node_t *p_node)
{
    data_node_t *p = NULL;

    if (p_node) {
        p = (data_node_t *)kmalloc(sizeof(data_node_t), GFP_KERNEL);
        if (p) {
            memset(p, 0, sizeof(data_node_t));
            copy_data_node(p, p_node);
        }
    }
    return p;
}

static data_node_t *free_node(data_node_t *p_node)
{
    if (p_node) {
        kfree(p_node);
    }
    return NULL;
}

/************************************/


int init_ll(list_handle_t *h)
{
    if (h) {
        memset(h, 0, sizeof(list_handle_t));
        return 0;
    }
    return -1;
}

static void set_pos_node(list_handle_t *h, int pos)
{
    if (h) {
        int i = 0;
        for (h->p_pos = h->p_head; h->p_pos; h->p_pos = get_next_node(h->p_pos)) {
            i++;
            if (i > pos) { break; }
        }
    }
}

int insert_ll(list_handle_t *h, data_node_t *p_node)
{
    if (h) {
        data_node_t *p = new_node(p_node);

        if (p) {
            if (!(h->p_head)) {
                h->p_head = h->p_tail = h->p_pos = p;
            } else {
                set_next_node(h->p_tail, p);
                h->p_tail = p;
            }
            (h->node_cnt)++;
            return 0;
        }
    }
    return -1;
}

int remove_internal_ll(list_handle_t *h, data_node_t *p_pre, data_node_t *p)
{
    if (h && p) {
        if ((p == h->p_head) || (p == h->p_tail)) {
            if (p == h->p_head) {
                h->p_head = get_next_node(p);
            }
            if (p == h->p_tail) {
                h->p_tail = p_pre;
                if (h->p_tail) {
                    set_next_node(h->p_tail, NULL);
                }
            }
        } else {
            set_next_node(p_pre, get_next_node(p));
        }

        if (p == h->p_pos) {
            h->p_pos =  get_next_node(h->p_pos);
            if (h->p_pos == NULL) {
                h->p_pos = h->p_head;
            }
        }
        p = free_node(p);

        if (h->node_cnt > 0) {
            h->node_cnt--;
        } else {
            printk("h->node_cnt error ...\n");
        }
        return 0;
    }
    return -1;
}

int remove_ll(list_handle_t *h, unsigned int id)
{
    int ret = -1;
    if (h) {
        data_node_t *p, *p_pre = NULL;
        for (p = h->p_head; p; p = get_next_node(p)) {
            if (p->id == id) {
                ret = remove_internal_ll(h, p_pre, p);
                break;
            }
            p_pre = p;
        }
    }
    return ret;
}

void remove_all_ll(list_handle_t *h)
{
    if (h) {
        for (h->p_tail = h->p_head; h->p_tail; h->p_tail = h->p_head) {
            h->p_head = get_next_node(h->p_head);
            free_node(h->p_tail);
        }
        h->p_head = h->p_tail = h->p_pos = NULL;
        h->node_cnt = 0;
    }
}

data_node_t *find_by_id(list_handle_t *h, unsigned int id)
{
    data_node_t *p = NULL;

    if (h) {
        set_pos_node(h, 0);
        for (p = h->p_pos; p; p = get_next_node(p)) {
            if (p->id == id) { return p; }
        }
    }
    return NULL;
}



typedef struct power_man {
    list_handle_t tcc_pwm_list;
    struct mutex mutex;
} power_man_t;

static power_man_t pwr_man;

int init_pwm_list(void)
{
    int ret = -1;
    memset(&pwr_man, 0, sizeof(pwr_man));
    ret = init_ll(&(pwr_man.tcc_pwm_list));
    mutex_init(&(pwr_man.mutex));
    return ret;
}
EXPORT_SYMBOL(init_pwm_list);

int insert_pwm_node(unsigned int id, power_control_t func, void *h_pri)
{
    int ret = -1;
    list_handle_t *h = &(pwr_man.tcc_pwm_list);
    if (h && func) {
        mutex_lock(&(pwr_man.mutex));
        if (!find_by_id(h, id)) {
            data_node_t node;

            memset(&node, 0, sizeof(node));
            node.id = id;
            node.callback = func;
            node.h_pri = h_pri;

            ret = insert_ll(h, &node);
        } else {
            printk("--- err --- already exist dev-node !!! -> id[%d]\n", id);
        }
        mutex_unlock(&(pwr_man.mutex));
    }
    return ret;
}
EXPORT_SYMBOL(insert_pwm_node);

int remove_pwm_node(unsigned int id)
{
    int ret = -1;
    list_handle_t *h = &(pwr_man.tcc_pwm_list);
    mutex_lock(&(pwr_man.mutex));
    ret = remove_ll(h, id);
    mutex_unlock(&(pwr_man.mutex));
    return ret;
}
EXPORT_SYMBOL(remove_pwm_node);

//int callback_pwm_node(stpwrioctl *p_arg)
int callback_pwm_node(unsigned int id, unsigned int cmd, void *p_info)
{
    int ret = -1;
    list_handle_t *h = &(pwr_man.tcc_pwm_list);
    if (h) {
        data_node_t *p_node = NULL;

        mutex_lock(&(pwr_man.mutex));
        p_node = find_by_id(h, id);
        if (p_node) {
            jprintk("call dev-callback -> id[%d]\n", p_node->id);
            if (p_node->callback) {
                ret = p_node->callback(get_node_pri(p_node), cmd, p_info);
            } else {
                printk("callback is NULL !!!  -> id[%d]\n", p_node->id);
            }
        } else {
            printk("--- err --- cannot find dev-node !!! -> id[%d]\n", id);
        }
        mutex_unlock(&(pwr_man.mutex));
    }
    return ret;
}
EXPORT_SYMBOL(callback_pwm_node);


