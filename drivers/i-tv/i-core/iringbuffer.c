#include <linux/spinlock.h>
#include <linux/wait.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <i-tv/itv_common.h>
#include <i-tv/itv_ringbuffer.h>

#define PKT_READY		0
#define PKT_DISPOSED	1

void itv_ringbuffer_init(itv_ringbuffer_t *rbuf, void *data, size_t len)
{
	rbuf->pread = rbuf->pwrite = 0;
	rbuf->data = data;
	rbuf->size = len;
	rbuf->error = 0;

	init_waitqueue_head(&rbuf->queue);

	spin_lock_init(&rbuf->lock);
}
EXPORT_SYMBOL(itv_ringbuffer_init);

void itv_ringbuffer_deinit(itv_ringbuffer_t *rbuf)
{
	if(rbuf->data)
		vfree(rbuf->data);

	rbuf->pread = rbuf->pwrite = 0;
	rbuf->data = NULL;
	rbuf->size = 0;
	rbuf->error = 0;
}
EXPORT_SYMBOL(itv_ringbuffer_deinit);

int itv_ringbuffer_empty(itv_ringbuffer_t *rbuf)
{
	return (rbuf->pread == rbuf->pwrite);
}
EXPORT_SYMBOL(itv_ringbuffer_empty);

ssize_t itv_ringbuffer_free(itv_ringbuffer_t *rbuf)
{
	ssize_t free;

	free = rbuf->pread - rbuf->pwrite;
	if(free <= 0)
		free += rbuf->size;
	return free - 1;
}
EXPORT_SYMBOL(itv_ringbuffer_free);

ssize_t itv_ringbuffer_avail(itv_ringbuffer_t *rbuf)
{
	ssize_t avail;

	avail = rbuf->pwrite - rbuf->pread;
	if(avail < 0)
		avail += rbuf->size;
	return avail;
}
EXPORT_SYMBOL(itv_ringbuffer_avail);

void itv_ringbuffer_flush(itv_ringbuffer_t *rbuf)
{
	rbuf->pread = rbuf->pwrite;
	rbuf->error = 0;
}
EXPORT_SYMBOL(itv_ringbuffer_flush);

void itv_ringbuffer_reset(itv_ringbuffer_t *rbuf)
{
	rbuf->pread = rbuf->pwrite = 0;
	rbuf->error = 0;
}
EXPORT_SYMBOL(itv_ringbuffer_reset);

void itv_ringbuffer_flush_spinlock_wakeup(itv_ringbuffer_t *rbuf)
{
	unsigned long flags;

	spin_lock_irqsave(&rbuf->lock, flags);
	itv_ringbuffer_flush(rbuf);
	spin_unlock_irqrestore(&rbuf->lock, flags);

	wake_up(&rbuf->queue);
}
EXPORT_SYMBOL(itv_ringbuffer_flush_spinlock_wakeup);

ssize_t itv_ringbuffer_read_user(itv_ringbuffer_t *rbuf, u8 __user *buf, size_t len)
{
	size_t todo = len;
	size_t split;

	split = (rbuf->pread + len > rbuf->size) ? rbuf->size - rbuf->pread : 0;
	if(split > 0) {
		if(copy_to_user(buf, rbuf->data + rbuf->pread, split))
			return -EFAULT;
		buf += split;
		todo -= split;
		rbuf->pread = 0;
	}
	if(copy_to_user(buf, rbuf->data + rbuf->pread, todo))
		return -EFAULT;

	rbuf->pread = (rbuf->pread + todo) % rbuf->size;

	return len;
}
EXPORT_SYMBOL(itv_ringbuffer_read_user);

void itv_ringbuffer_read(itv_ringbuffer_t *rbuf, u8 *buf, size_t len)
{
	size_t todo = len;
	size_t split;

	split = (rbuf->pread + len > rbuf->size) ? rbuf->size - rbuf->pread : 0;
	if(split > 0) {
		memcpy(buf, rbuf->data + rbuf->pread, split);
		buf += split;
		todo -= split;
		rbuf->pread = 0;
	}
	memcpy(buf, rbuf->data + rbuf->pread, todo);

	rbuf->pread = (rbuf->pread + todo) % rbuf->size;
}
EXPORT_SYMBOL(itv_ringbuffer_read);

ssize_t itv_ringbuffer_write(itv_ringbuffer_t *rbuf, const u8 *buf, size_t len)
{
	size_t todo = len;
	size_t split;

	split = (rbuf->pwrite + len > rbuf->size) ? rbuf->size - rbuf->pwrite : 0;
	if(split > 0) {
		memcpy(rbuf->data + rbuf->pwrite, buf, split);
		buf += split;
		todo -= split;
		rbuf->pwrite = 0;
	}
	memcpy(rbuf->data + rbuf->pwrite, buf, todo);
	rbuf->pwrite = (rbuf->pwrite + todo) % rbuf->size;

	return len;
}
EXPORT_SYMBOL(itv_ringbuffer_write);

ssize_t itv_ringbuffer_pkt_write(itv_ringbuffer_t *rbuf, u8 *buf, size_t len)
{
	int status;
	ssize_t oldpwrite = rbuf->pwrite;

	ITV_RINGBUFFER_WRITE_BYTE(rbuf, len >> 8);
	ITV_RINGBUFFER_WRITE_BYTE(rbuf, len & 0xff);
	ITV_RINGBUFFER_WRITE_BYTE(rbuf, PKT_READY);
	status = itv_ringbuffer_write(rbuf, buf, len);

	if(status < 0) rbuf->pwrite = oldpwrite;
	return status;
}
EXPORT_SYMBOL(itv_ringbuffer_pkt_write);

ssize_t itv_ringbuffer_pkt_read_user(itv_ringbuffer_t *rbuf, size_t idx, int offset, u8 __user *buf, size_t len)
{
	size_t todo;
	size_t split;
	size_t pktlen;

	pktlen = rbuf->data[idx] << 8;
	pktlen |= rbuf->data[(idx + 1) % rbuf->size];
	if(offset > pktlen) return -EINVAL;
	if((offset + len) > pktlen) len = pktlen - offset;

	idx = (idx + ITV_RINGBUFFER_PKTHDRSIZE + offset) % rbuf->size;
	todo = len;
	split = ((idx + len) > rbuf->size) ? rbuf->size - idx : 0;
	if(split > 0) {
		if(copy_to_user(buf, rbuf->data + idx, split))
			return -EFAULT;
		buf += split;
		todo -= split;
		idx = 0;
	}
	if(copy_to_user(buf, rbuf->data + idx, todo))
		return -EFAULT;

	return len;
}
EXPORT_SYMBOL(itv_ringbuffer_pkt_read_user);

ssize_t itv_ringbuffer_pkt_read(itv_ringbuffer_t *rbuf, size_t idx, int offset, u8 *buf, size_t len)
{
	size_t todo;
	size_t split;
	size_t pktlen;

	pktlen = rbuf->data[idx] << 8;
	pktlen |= rbuf->data[(idx + 1) % rbuf->size];
	if(offset > pktlen) return -EINVAL;
	if((offset + len) > pktlen) len = pktlen - offset;

	idx = (idx + ITV_RINGBUFFER_PKTHDRSIZE + offset) % rbuf->size;
	todo = len;
	split = ((idx + len) > rbuf->size) ? rbuf->size - idx : 0;
	if(split > 0) {
		memcpy(buf, rbuf->data + idx, split);
		buf += split;
		todo -= split;
		idx = 0;
	}
	memcpy(buf, rbuf->data + idx, todo);

	return len;
}
EXPORT_SYMBOL(itv_ringbuffer_pkt_read);

void itv_ringbuffer_pkt_dispose(itv_ringbuffer_t *rbuf, size_t idx)
{
	size_t pktlen;

	rbuf->data[(idx + 2) % rbuf->size] = PKT_DISPOSED;

	while(itv_ringbuffer_avail(rbuf) > ITV_RINGBUFFER_PKTHDRSIZE) {
		if(ITV_RINGBUFFER_PEEK(rbuf, 2) == PKT_DISPOSED) {
			pktlen = ITV_RINGBUFFER_PEEK(rbuf, 0) << 8;
			pktlen |= ITV_RINGBUFFER_PEEK(rbuf, 1);
			ITV_RINGBUFFER_SKIP(rbuf, pktlen + ITV_RINGBUFFER_PKTHDRSIZE);
		}
		else {
			break;
		}
	}
}
EXPORT_SYMBOL(itv_ringbuffer_pkt_dispose);

ssize_t itv_ringbuffer_pkt_next(itv_ringbuffer_t *rbuf, size_t idx, size_t *pktlen)
{
	int consumed;
	int curpktlen;
	int curpktstatus;

	if(idx == -1) {
		idx = rbuf->pread;
	}
	else {
		curpktlen = rbuf->data[idx] << 8;
		curpktlen |= rbuf->data[(idx + 1) % rbuf->size];
		idx = (idx + curpktlen + ITV_RINGBUFFER_PKTHDRSIZE) % rbuf->size;
	}

	consumed = (idx - rbuf->pread) % rbuf->size;

	while((itv_ringbuffer_avail(rbuf) - consumed) > ITV_RINGBUFFER_PKTHDRSIZE) {
		curpktlen = rbuf->data[idx] << 8;
		curpktlen |= rbuf->data[(idx + 1) % rbuf->size];
		curpktstatus = rbuf->data[(idx + 2) % rbuf->size];

		if(curpktstatus == PKT_READY) {
			*pktlen = curpktlen;
			return idx;
		}

		consumed += curpktlen + ITV_RINGBUFFER_PKTHDRSIZE;
		idx = (idx + curpktlen + ITV_RINGBUFFER_PKTHDRSIZE) % rbuf->size;
	}

	return -1;
}
EXPORT_SYMBOL(itv_ringbuffer_pkt_next);