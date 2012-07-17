/****************************************************************************
 *	 TCC Version 0.6
 *	 Copyright (c) telechips, Inc.
 *	 ALL RIGHTS RESERVED
 *
****************************************************************************/

#define TRUE   1
#define FALSE  0

#if 0
static ssize_t tccxxx_grp_write(struct file *filp, const char __user *buffer, size_t count, loff_t *ppos);
static ssize_t tccxxx_grp_read(struct file *file, const char __user *buffer, size_t count, loff_t *ppos);
static void tccxxx_grp_vm_open(struct vm_area_struct *vma);
static void tccxxx_grp_vm_close(struct vm_area_struct *vma);
static int tccxxx_grp_mmap(struct file *file, struct vm_area_struct *vma);
static int tccxxx_grp_ioctl(struct inode *inode, struct file *file, unsigned int cmd, void *arg);
static int tccxxx_grp_release(struct inode *inode, struct file *file);
static int tccxxx_grp_open(struct inode *inode, struct file *file);
static irqreturn_t tccxxx_grp_handler(int irq, void *dev_id, struct pt_regs *reg);
#endif//

