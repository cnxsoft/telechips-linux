#ifndef __ITV_RINGBUFFER_H__
#define __ITV_RINGBUFFER_H__

typedef struct itv_ringbuffer_t {
	unsigned char 	*data;
	ssize_t			size;
	ssize_t			pread;
	ssize_t			pwrite;
	int				error;

	wait_queue_head_t	queue;
	spinlock_t			lock;
} itv_ringbuffer_t;

#define ITV_RINGBUFFER_PKTHDRSIZE	3

#define ITV_RINGBUFFER_PEEK(rbuf, offs)	\
	(rbuf)->data[((rbuf)->pread + (offs)) % (rbuf)->size]

#define ITV_RINGBUFFER_SKIP(rbuf, num)	\
	(rbuf)->pread = ((rbuf)->pread + (num)) % (rbuf)->size

#define ITV_RINGBUFFER_WRITE_BYTE(rbuf, byte)					\
	{															\
		(rbuf)->data[(rbuf)->pwrite] = (byte);					\
		(rbuf)->pwrite = ((rbuf)->pwrite + 1) % (rbuf)->size;	\
	}

ITV_EXPORT(void, itv_ringbuffer_init, (itv_ringbuffer_t *, void *, size_t));
ITV_EXPORT(void, itv_ringbuffer_deinit, (itv_ringbuffer_t *));
ITV_EXPORT(int, itv_ringbuffer_empty, (itv_ringbuffer_t *));
ITV_EXPORT(ssize_t, itv_ringbuffer_free, (itv_ringbuffer_t *));
ITV_EXPORT(ssize_t, itv_ringbuffer_avail, (itv_ringbuffer_t *));
ITV_EXPORT(void, itv_ringbuffer_reset, (itv_ringbuffer_t *));
ITV_EXPORT(void, itv_ringbuffer_flush, (itv_ringbuffer_t *));
ITV_EXPORT(void, itv_ringbuffer_flush_spinlock_wakeup, (itv_ringbuffer_t *));
ITV_EXPORT(ssize_t, itv_ringbuffer_read_user, (itv_ringbuffer_t *, u8 __user *, size_t));
ITV_EXPORT(void, itv_ringbuffer_read, (itv_ringbuffer_t *, u8 *, size_t));
ITV_EXPORT(ssize_t, itv_ringbuffer_write, (itv_ringbuffer_t *, const u8 *, size_t));
ITV_EXPORT(ssize_t, itv_ringbuffer_pkt_write, (itv_ringbuffer_t *, u8 *, size_t));
ITV_EXPORT(ssize_t, itv_ringbuffer_pkt_read_user, (itv_ringbuffer_t *, size_t, int, u8 __user *, size_t));
ITV_EXPORT(ssize_t, itv_ringbuffer_pkt_read, (itv_ringbuffer_t *, size_t, int, u8 *, size_t));
ITV_EXPORT(void, itv_ringbuffer_pkt_dispose, (itv_ringbuffer_t *, size_t));
ITV_EXPORT(ssize_t, itv_ringbuffer_pkt_next, (itv_ringbuffer_t *, size_t, size_t *));

#endif	/* __ITV_RINGBUFFER_H__ */
