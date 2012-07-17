
#ifndef __TCC_LL_H__
#define __TCC_LL_H__

/* *****************************************
 * void *: h_pri (private handle)
 * int   : cmd   (power control command)
 * *****************************************/
typedef int (*power_control_t)(void *, int, void *);

typedef struct data_node data_node_t;
struct data_node {
    unsigned int id;

    /**** customer data ****/
    power_control_t callback;
    void *h_pri;
    /***********************/

    data_node_t *p_next;
};

typedef struct list_handle {
    data_node_t *p_head;
    data_node_t *p_tail;
    data_node_t *p_pos;
    unsigned int node_cnt;
} list_handle_t;


#if 0
extern int init_ll(list_handle_t *h);
extern int insert_ll(list_handle_t *h, data_node_t *p_node);
extern int remove_ll(list_handle_t *h, unsigned int id);
extern void remove_all_ll(list_handle_t *h);
extern data_node_t *find_by_id(list_handle_t *h, unsigned int id);
#endif

#define set_next_node(p_node, p) ((p_node)->p_next = (p))
#define get_next_node(p) ((p)->p_next)
#define get_node_id(p) ((p)->id)
#define get_node_cb(p) ((p)->callback)
#define get_node_pri(p) ((p)->h_pri)

extern int init_pwm_list(void);
extern int insert_pwm_node(unsigned int id, power_control_t func, void *h_pri);
extern int remove_pwm_node(unsigned int id);
extern int callback_pwm_node(unsigned int id, unsigned int cmd, void *p_info);


#endif /* __TCC_LL_H__ */


