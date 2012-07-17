#include <i-tv/itv_common.h>
#include <i-tv/itv_demux.h>

#include "its.h"

//20110726 koo : ts packet continuity counter debug option
//#define TS_PACKET_CHK_MODE

static int itv_ts_module_debug;
static int itv_ts_module_debug_callstack;

module_param_named(debug, itv_ts_module_debug, int, 0644);
module_param_named(debug_cs, itv_ts_module_debug_callstack, int, 0644);

MODULE_PARM_DESC(debug, "Turn on/off ts debugging (default:off).");
MODULE_PARM_DESC(debug_cs, "Turn on/off ts callstack debugging (default:off).");

#ifdef dprintk
#undef dprintk
#endif

#define dprintk(args...) \
	do { if(itv_ts_module_debug) { printk("[%s] ", __func__); printk(args); } } while (0)

#define DEBUG_CALLSTACK		\
	do { if(itv_ts_module_debug_callstack) { printk("[Call] ----> %s()\n", __func__); } } while (0);

static int itv_ts_write(itv_ringbuffer_t *buffer, const unsigned char *src, size_t len);

#ifdef TS_PACKET_CHK_MODE
typedef	struct {
	struct ts_packet_chk_t* next;
	
	unsigned short pid;
	unsigned char cc;
	
	unsigned long long cnt;
	unsigned int err;
} ts_packet_chk_t;


typedef	struct {
	unsigned long long total_cnt;
	unsigned int total_err;
	unsigned int packet_cnt;

	#define DEBUG_CHK_TIME		10	//sec
	unsigned int cur_time;
	unsigned int prev_time;
	unsigned int debug_time;
	struct timeval time;

  	ts_packet_chk_t* packet;
} ts_packet_chk_info_t;
ts_packet_chk_info_t*	ts_packet_chk_info = NULL;


static void itv_ts_cc_debug(int mod) 
{
	if(ts_packet_chk_info != NULL) {
		if(ts_packet_chk_info->packet != NULL) {
			printk("[total:%llu / err:%d (%d sec)]\n", ts_packet_chk_info->total_cnt, ts_packet_chk_info->total_err, (ts_packet_chk_info->debug_time * DEBUG_CHK_TIME));
			
			if(mod) {
				ts_packet_chk_t* tmp = NULL;
				
				tmp = ts_packet_chk_info->packet;
				do {
					printk("\tpid:0x%04x => cnt:%llu err:%d\n", tmp->pid, tmp->cnt, tmp->err);
					tmp = tmp->next;
				} while(tmp != NULL);
			}
		}
	}
}

static void itv_ts_cc_check(unsigned char* buf)
{
	unsigned short pid;
	unsigned char cc;

	ts_packet_chk_t* tmp = NULL;

	pid = ((*(unsigned char*)(buf + 1) & 0x1f) << 8) | *(unsigned char*)(buf + 2);
	cc 	= *(unsigned char*)(buf + 3) & 0x0f;
	
	if(ts_packet_chk_info != NULL) {
		ts_packet_chk_info->total_cnt++;

		if(ts_packet_chk_info->packet == NULL) {
			tmp = (ts_packet_chk_t*)kmalloc(sizeof(ts_packet_chk_t), GFP_ATOMIC);
			if(tmp == NULL) {
				printk("\t ts_packet_chk_t mem alloc err..\n");
			}

			memset(tmp, 0x0, sizeof(ts_packet_chk_t));
			tmp->pid = pid;
			tmp->cc	= cc;
			tmp->next = NULL;
			tmp->cnt++;
			ts_packet_chk_info->packet = tmp;
			ts_packet_chk_info->packet_cnt++;
			
			//printk("\t>>>> create[%d] : 0x%04x / %02d\n", ts_packet_chk_info->packet_cnt, tmp->pid, tmp->cc);
		} else {
			unsigned char new = 0;
			unsigned int temp;
	
			tmp = ts_packet_chk_info->packet;
			while(1) {
				if(tmp->pid == pid) {
					tmp->cnt++;

					if(tmp->pid != 0x1fff) {
						if(cc > tmp->cc) {
							temp = tmp->err;
							tmp->err += ((cc - tmp->cc + 0xf) % 0xf) - 1;
							if(temp != tmp->err) {
								ts_packet_chk_info->total_err += tmp->err - temp;
								printk("\t(%dmin) pid:0x%04x => cnt:%llu err:%d [%d -> %d]\n", ts_packet_chk_info->debug_time, tmp->pid, tmp->cnt, tmp->err, tmp->cc, cc);
							}
							tmp->cc = cc;
						} else if(cc < tmp->cc) {
							temp = tmp->err;
							tmp->err += (0x10 - tmp->cc + cc) - 1;
							if(temp != tmp->err) {
								ts_packet_chk_info->total_err += tmp->err - temp;
								printk("\t(%dmin) pid:0x%04x => cnt:%llu err:%d [%d -> %d]\n", ts_packet_chk_info->debug_time, tmp->pid, tmp->cnt, tmp->err, tmp->cc, cc);
							}
							tmp->cc = cc;
						}
					}
					break;
				} else {
					tmp = tmp->next;
					
					if(tmp == NULL) {
						new = 1;
						break;
					}
				}
			}

			if(new) {
				ts_packet_chk_t* tmp_ = NULL;
	
				tmp = (ts_packet_chk_t*)kmalloc(sizeof(ts_packet_chk_t), GFP_ATOMIC);
				if(tmp == NULL) {
					printk("\t ts_packet_chk_t mem alloc err..\n");
				}

				memset(tmp, 0x0, sizeof(ts_packet_chk_t));
				tmp->pid = pid;
				tmp->cc	= cc;
				tmp->next = NULL;
				tmp->cnt++;
				
				ts_packet_chk_info->packet_cnt++;
				tmp_ = ts_packet_chk_info->packet;
				do {
					if(tmp_->next == NULL) {
						tmp_->next= tmp;
						break;
					} else {
						tmp_ = tmp_->next;
					}
				} while(1);

				printk("\t>>>> create[%d] : 0x%04x / %02d\n", ts_packet_chk_info->packet_cnt, tmp->pid, tmp->cc);
			}
		}

		do_gettimeofday(&ts_packet_chk_info->time);
		ts_packet_chk_info->cur_time = ((ts_packet_chk_info->time.tv_sec * 1000) & 0x00ffffff) + (ts_packet_chk_info->time.tv_usec / 1000);
		if((ts_packet_chk_info->cur_time - ts_packet_chk_info->prev_time) > (DEBUG_CHK_TIME * 1000)) {
			itv_ts_cc_debug(0);

			ts_packet_chk_info->prev_time = ts_packet_chk_info->cur_time;
			ts_packet_chk_info->debug_time++;
		}
	}
}
#endif	//#ifdef TS_PACKET_CHK_MODE

static inline unsigned short itv_ts_get_pid(const unsigned char *buf)
{
//	DEBUG_CALLSTACK
		
	return ((buf[1] & 0x1f) << 8) + buf[2];
}

static int itv_ts_filter_write_ts(const unsigned char *buffer1, size_t buffer1_len, 
	const unsigned char *buffer2, size_t buffer2_len, itv_ts_filter_t *p_filter)
{	
	int ret;
	//20110224 koo : itv_ts_filter_packets에서 lock/unlock 시킴.
	//unsigned long flags;

	itv_demux_t *idemux = p_filter->priv->idemux;
	itv_ringbuffer_t *buffer;

//	DEBUG_CALLSTACK

	//20110224 koo : itv_ts_filter_packets에서 lock/unlock 시킴.
	//spin_lock_irqsave(&p_filter->priv->lock, flags);
	if(p_filter->output == ITV_DMX_OUT_DECODER)
	{	
		//20110224 koo : itv_ts_filter_packets에서 lock/unlock 시킴.
		//spin_unlock_irqrestore(&p_filter->priv->lock, flags);
		return 0;
	}

	if(p_filter->output == ITV_DMX_OUT_TAP
	    || p_filter->output == ITV_DMX_OUT_TSDEMUX_TAP)
		buffer = &p_filter->buffer;
	else
		buffer = &idemux->stream_buffer;
	
	if(buffer->error)
	{
		//20110224 koo : itv_ts_filter_packets에서 lock/unlock 시킴.
		//spin_unlock_irqrestore(&p_filter->priv->lock, flags);
		wake_up(&buffer->queue);
		return 0;
	}
	
	ret = itv_ts_write(buffer, buffer1, buffer1_len);
	if(ret == buffer1_len)
		ret = itv_ts_write(buffer, buffer2, buffer2_len);
	if(ret < 0)
	{
		itv_ringbuffer_flush(buffer);
		buffer->error = ret;
	}

	//20110224 koo : itv_ts_filter_packets에서 lock/unlock 시킴.
	//spin_unlock_irqrestore(&p_filter->priv->lock, flags);
	wake_up(&buffer->queue);
	
	return 0;
}

static inline void itv_ts_filter_packet_type(itv_ts_filter_t *p_filter, const unsigned char *buf)
{
//	DEBUG_CALLSTACK
		
	switch(p_filter->type)
	{
		case ITV_TS_FILTER_TYPE_TS:
			if(p_filter->ts_type & ITV_TS_PACKET)
			{
				if(p_filter->ts_type & ITV_TS_PAYLOAD_ONLY) 
				{
					//
				}
				else
					itv_ts_filter_write_ts(buf, 188, NULL, 0, p_filter);
			}
			break;
		case ITV_TS_FILTER_TYPE_SEC:
			//
			break;
		default:
			break;
	}
}

static void itv_ts_filter_packet(itv_demux_priv_t *p_priv, const unsigned char *buf)
{	
	unsigned short pid = itv_ts_get_pid(buf);

	itv_ts_filter_t *filter;
	
//	DEBUG_CALLSTACK

	list_for_each_entry(filter, &p_priv->filter_list, list_head)
	{
		if((filter->pid != pid) && (filter->pid != 0x2000))
			continue;

		itv_ts_filter_packet_type(filter, buf);
		break;
	}
}

static void itv_ts_filter_packets(itv_demux_priv_t *p_priv, const unsigned char *buf, size_t count)
{
	unsigned long flags;

//	DEBUG_CALLSTACK

	spin_lock_irqsave(&p_priv->lock, flags);

	while(count--)
	{
		if(buf[0] == 0x47)
			itv_ts_filter_packet(p_priv, buf);
	#if 0
		else
			dprintk("@@## 0x%x\n", buf[0]);
	#endif
		buf += 188;
	}

	spin_unlock_irqrestore(&p_priv->lock, flags);
}

static void itv_ts_irq_handler(void *data)
{
	itv_demux_priv_t *priv = (itv_demux_priv_t *)data;

//	DEBUG_CALLSTACK

	tasklet_schedule(&priv->ts_tasklet);
}

static void itv_ts_irq_p_tsif(unsigned long data)
{
	itv_demux_priv_t *priv = (itv_demux_priv_t *)data;
	itv_demux_t *idemux = priv->idemux;
	itv_ts_buffer_t *buffer = &priv->buffer;
	int iReadPtr = (unsigned int)buffer->bufp - (unsigned int)buffer->buffer;
	int iWritePtr = idemux->tsif_ops->tsif_p_get_pos() - (unsigned int)buffer->buffer;
	unsigned int iBufferSize = buffer->size;
	unsigned char *pucData;
	int iRemain, iPosition;
	int i;
	unsigned char temp[188];
	static int flag = 1;

//	DEBUG_CALLSTACK
	while(1)
	{
		iRemain = (iBufferSize + iWritePtr - iReadPtr) % iBufferSize;
		iPosition = 0;

		if(iRemain < ITV_TS_SIZE * 3)
			break;
#if 0
		pucData = buffer->buffer + iReadPtr;

		if(*pucData == ITV_TS_SYNCBYTE)
		{
			if(iReadPtr + ITV_TS_SIZE >= iBufferSize)
			{
				memcpy(temp, buffer->buffer + iReadPtr, iBufferSize - iReadPtr);
				memcpy(&temp[iBufferSize - iReadPtr], buffer->buffer, (iReadPtr + ITV_TS_SIZE) - iBufferSize);
				
				itv_ts_filter_packets(priv, temp, 1);
			}
			else
			{
				itv_ts_filter_packets(priv, pucData, 1);
			}
			
			iReadPtr = (iReadPtr + ITV_TS_SIZE) % iBufferSize;
		}
		else
#endif
		{
			unsigned int tempPtr;
			for(i=0; i<ITV_TS_SIZE*2; i++)
			{
				pucData = buffer->buffer + ((iReadPtr + iPosition) % iBufferSize);
				if ( *pucData == ITV_TS_SYNCBYTE )
				{
					pucData = buffer->buffer + ((iReadPtr + iPosition + ITV_TS_SIZE) % iBufferSize);
					if( *pucData == ITV_TS_SYNCBYTE )
					{
						pucData = buffer->buffer + ((iReadPtr + iPosition + ITV_TS_SIZE * 2) % iBufferSize);
						if(*pucData == ITV_TS_SYNCBYTE )
						{
							break;
						}
					}
				}
				iPosition ++;
			}

			if(iPosition != 0 && flag)
			{
				dprintk("[%d]=0x%02x\n", iPosition, *pucData);
				flag = 0;
			}

			tempPtr = (iReadPtr + iPosition) % iBufferSize;

			if(iPosition != ITV_TS_SIZE * 2)
			{
				if(tempPtr + ITV_TS_SIZE >= iBufferSize)
				{
					memcpy(temp, buffer->buffer + tempPtr, iBufferSize - tempPtr);
					memcpy(&temp[iBufferSize - tempPtr], buffer->buffer, (tempPtr + ITV_TS_SIZE) - iBufferSize);

					itv_ts_filter_packets(priv, temp, 1);

#ifdef TS_PACKET_CHK_MODE
					itv_ts_cc_check(temp);
#endif
				}
				else
				{
					itv_ts_filter_packets(priv, buffer->buffer + tempPtr, 1);

#ifdef TS_PACKET_CHK_MODE
					itv_ts_cc_check(buffer->buffer + tempPtr);
#endif
				}

				iReadPtr = (iReadPtr + iPosition + ITV_TS_SIZE) % iBufferSize;
			}
			else
				iReadPtr = (iReadPtr + iPosition) % iBufferSize;
		}
	}

	buffer->bufp = buffer->buffer + iReadPtr;
	flag = 1;
}

static inline void itv_ts_filter_state_set(itv_ts_filter_t *filter, int state)
{
//	DEBUG_CALLSTACK
		
	spin_lock_irq(&filter->priv->lock);
	filter->state = state;
	spin_unlock_irq(&filter->priv->lock);
}

static int itv_ts_start(itv_demux_t *p_idemux)
{
	itv_demux_priv_t *priv = p_idemux->priv;
	itv_platform_tsif_operations_t *tsif_ops = p_idemux->tsif_ops;

	DEBUG_CALLSTACK

	if(priv->filtering == 0)
		return 0;

	itv_ringbuffer_flush(&p_idemux->stream_buffer);

#ifdef TS_PACKET_CHK_MODE
	ts_packet_chk_info = (ts_packet_chk_info_t*)kmalloc(sizeof(ts_packet_chk_info_t), GFP_ATOMIC);
	if(ts_packet_chk_info == NULL) {
		printk("\t ts_packet_chk_info_t mem alloc err..\n");
	}
	memset(ts_packet_chk_info, 0x0, sizeof(ts_packet_chk_info_t));
#endif	

//20120320 koo test debug : tsif module test
#if 1
	switch(p_idemux->if_type)
	{
		case ITV_IF_TYPE_TS_PARALLEL:
			priv->buffer.bufp = (unsigned int)priv->buffer.buffer;
			tsif_ops->tsif_p_start();
			break;
		default:
			break;
	}
#endif
	dprintk("demux start\n");

	return 0;
}

static int itv_ts_stop(itv_demux_t *p_idemux)
{
	itv_platform_tsif_operations_t *tsif_ops = p_idemux->tsif_ops;

	DEBUG_CALLSTACK

	switch(p_idemux->if_type)
	{
		case ITV_IF_TYPE_TS_PARALLEL:
			tsif_ops->tsif_p_stop();
			break;
		default:
			break;
	}

	dprintk("demux stop\n");

#ifdef TS_PACKET_CHK_MODE
{
	ts_packet_chk_t* tmp = NULL;
	ts_packet_chk_t* tmp_ = NULL;

	if(ts_packet_chk_info != NULL) {
		if(ts_packet_chk_info->packet != NULL) {

			itv_ts_cc_debug(1);
			
			tmp = ts_packet_chk_info->packet;
			do {
				tmp_ = tmp;
				tmp = tmp->next;
				kfree(tmp_);
			} while(tmp != NULL);
		}
		kfree(ts_packet_chk_info);
		ts_packet_chk_info = NULL;
	}
}
#endif
	
	return 0;
}

static int itv_ts_poll(void *p_filter, struct file *file, poll_table *wait)
{
	unsigned int mask = 0;
	
	itv_ts_filter_t *filter = (itv_ts_filter_t *)p_filter;

	DEBUG_CALLSTACK

	poll_wait(file, &filter->buffer.queue, wait);

	if(filter->state != ITV_TS_FILTER_STATE_SET)
		return 0;

	if(filter->buffer.error)
		mask |= (POLLIN | POLLRDNORM | POLLPRI | POLLERR);

	if(!itv_ringbuffer_empty(&filter->buffer))
		mask |= (POLLIN | POLLRDNORM | POLLPRI);

	return mask;
}

static ssize_t itv_ts_read(void *p_filter, int non_blocking, char __user *buf, size_t count, loff_t *ppos)
{
	size_t todo;
	ssize_t avail;
	ssize_t ret = 0;

	itv_ts_filter_t *filter = (itv_ts_filter_t *)p_filter;
	itv_ringbuffer_t *buffer = &filter->buffer;

	DEBUG_CALLSTACK

	if(mutex_lock_interruptible(&filter->mutex))
		return -ERESTARTSYS;

	if(!buffer->data)
	{
		mutex_unlock(&filter->mutex);
		return 0;
	}

	if(buffer->error)
	{
		ret = buffer->error;
		itv_ringbuffer_flush(buffer);
		mutex_unlock(&filter->mutex);
		return ret;
	}

	for(todo = count; todo > 0; todo -= ret)
	{
		if(non_blocking && itv_ringbuffer_empty(buffer))
		{
			ret = -EWOULDBLOCK;
			break;
		}

		ret = wait_event_interruptible(buffer->queue,
					       !itv_ringbuffer_empty(buffer) ||
					       (buffer->error != 0));
		if(ret < 0) break;

		if(buffer->error)
		{
			ret = buffer->error;
			itv_ringbuffer_flush(buffer);
			break;
		}

		avail = itv_ringbuffer_avail(buffer);
		if(avail > todo)
			avail = todo;

		ret = itv_ringbuffer_read_user(buffer, buf, avail);
		if(ret < 0) break;

		buf += ret;
	}
	mutex_unlock(&filter->mutex);

	return (count - todo) ? (count - todo) : ret;
}

static int itv_ts_write(itv_ringbuffer_t *buffer, const unsigned char *src, size_t len)
{
	ssize_t free;

//	DEBUG_CALLSTACK

	if(!len || !buffer->data)
		return 0;

	free = itv_ringbuffer_free(buffer);
	if(len > free)
	{
		eprintk("buffer overflow\n");
		return -EOVERFLOW;
	}

	return itv_ringbuffer_write(buffer, src, len);
}

static int itv_ts_filter_find(itv_ts_filter_t *p_filter)
{
	itv_ts_filter_t *entry;

//	DEBUG_CALLSTACK

	list_for_each_entry(entry, &p_filter->priv->filter_list, list_head)
		if(entry == p_filter)
			return 1;

	return 0;
}

static void itv_ts_filter_add(itv_ts_filter_t *p_filter)
{
//	DEBUG_CALLSTACK
		
	spin_lock_irq(&p_filter->priv->lock);
	if(itv_ts_filter_find(p_filter))
	{
		wprintk("filter already in list (type=%x state=%x pid=%x)\n", 
			p_filter->type, p_filter->state, p_filter->pid);
		goto out;
	}

	p_filter->priv->filtering++;
	list_add(&p_filter->list_head, &p_filter->priv->filter_list);

	itv_platform_tsif_operations_t *tsif_ops = p_filter->priv->idemux->tsif_ops;
	
	tsif_ops->tsif_p_insert_pid(p_filter->pid);
	
out:
	spin_unlock_irq(&p_filter->priv->lock);
}

static void itv_ts_filter_del(itv_ts_filter_t *p_filter)
{
//	DEBUG_CALLSTACK

	if(p_filter->priv->filtering == 0)
		return;

	spin_lock_irq(&p_filter->priv->lock);
	if (!(itv_ts_filter_find(p_filter)))
	{
		wprintk("filter not in list (type=%x state=%x pid=%x)\n", 
			p_filter->type, p_filter->state, p_filter->pid);
		goto out;
	}

	p_filter->priv->filtering--;
	list_del(&p_filter->list_head);

	itv_platform_tsif_operations_t *tsif_ops = p_filter->priv->idemux->tsif_ops;
	
	tsif_ops->tsif_p_remove_pid(p_filter->pid);

//	if(p_filter->priv->filtering == 0)
//		itv_ts_stop(p_filter->priv->idemux);
out:
	spin_unlock_irq(&p_filter->priv->lock);
}

static inline void itv_ts_filter_invert_mode(itv_demux_filter_t *filter)
{
	int i;

//	DEBUG_CALLSTACK

	for(i = 0; i < ITV_DMX_FILTER_SIZE; i++)
		filter->mode[i] ^= 0xff;
}

static void *itv_ts_filter_get(itv_demux_t *p_idemux)
{
	int i;
	
	itv_demux_priv_t *priv = p_idemux->priv;
	itv_ts_filter_t *filter = NULL;

//	DEBUG_CALLSTACK

	for(i = 0; i < ITV_TS_FILTER_NUM; i++)
	{
		if(priv->filter[i].state == ITV_TS_FILTER_STATE_FREE)
		{
			filter = &priv->filter[i];
			break;
		}
	}
	if(!filter) return NULL;

	mutex_init(&filter->mutex);
	itv_ringbuffer_init(&filter->buffer, NULL, 8192);
	filter->type = ITV_TS_FILTER_TYPE_NONE;
	itv_ts_filter_state_set(filter, ITV_TS_FILTER_STATE_UNSET);

	return (void *)filter;
}

static void itv_ts_filter_put(void *p_filter)
{
	itv_ts_filter_t *filter = (itv_ts_filter_t *)p_filter;
	itv_demux_priv_t *priv = filter->priv;

//	DEBUG_CALLSTACK

	mutex_lock(&filter->mutex);

	if(filter->buffer.data)
	{
		void *mem = filter->buffer.data;

		spin_lock_irq(&priv->lock);
		filter->buffer.data = NULL;
		spin_unlock_irq(&priv->lock);
		vfree(mem);
	}

	itv_ts_filter_del(filter);
	
	itv_ts_filter_state_set(filter, ITV_TS_FILTER_STATE_FREE);
	wake_up(&filter->buffer.queue);

	mutex_unlock(&filter->mutex);
}

static int itv_ts_filter_set(void *p_filter, int type, void *p_params)
{	
	itv_ts_filter_t *filter = (itv_ts_filter_t *)p_filter;
	
//	itv_demux_sec_filter_params_t *sec_params;
	itv_demux_pes_filter_params_t *pes_params;

//	DEBUG_CALLSTACK

	if(mutex_lock_interruptible(&filter->mutex))
	{
		mutex_unlock(&filter->mutex);
		return -ERESTARTSYS;
	}

	switch(type)
	{
		case ITV_DMX_FILTER_TYPE_TS_SEC:
			//
			break;
		case ITV_DMX_FILTER_TYPE_TS_PES:
			pes_params = (itv_demux_pes_filter_params_t *)p_params;
			
			if(pes_params->pes_type > ITV_DMX_PES_OTHER || pes_params->pes_type < 0)
			{
				mutex_unlock(&filter->mutex);
				return -EINVAL;
			}

			if(pes_params->pes_type < ITV_DMX_PES_OTHER)
				filter->type = ITV_TS_FILTER_TYPE_PES;
			else
				filter->type = ITV_TS_FILTER_TYPE_TS;

			filter->output = pes_params->output;
			if(pes_params->output == ITV_DMX_OUT_TS_TAP)
				filter->ts_type = ITV_TS_PACKET;
			else if(pes_params->output == ITV_DMX_OUT_TSDEMUX_TAP)
			{
				// temp
				filter->ts_type = ITV_TS_PACKET;
			}
			else if(pes_params->output == ITV_DMX_OUT_TAP)
			{
				// temp
				filter->ts_type = ITV_TS_PACKET;
			}
			
			filter->pid = pes_params->pid;
			break;
	}
	
	if(!filter->buffer.data)
	{
		void *mem = vmalloc(filter->buffer.size);
		if(!mem)
		{
			mutex_unlock(&filter->mutex);
			return -ENOMEM;
		}
		spin_lock_irq(&filter->priv->lock);
		filter->buffer.data = mem;
		spin_unlock_irq(&filter->priv->lock);
	}
	itv_ringbuffer_flush(&filter->buffer);

	itv_ts_filter_add(filter);

	itv_ts_filter_state_set(filter, ITV_TS_FILTER_STATE_SET);

	mutex_unlock(&filter->mutex);

	return 0;
}

static int itv_ts_activate(itv_object_t *p_this)
{
	int i;
	int bufsize;
	int height_mask;
	
	itv_demux_t *idemux = (itv_demux_t *)p_this;
	itv_demux_priv_t *priv;
	itv_platform_tsif_operations_t *tsif_ops;

	DEBUG_CALLSTACK

	idemux->priv = kzalloc(sizeof(*priv), GFP_KERNEL);
	if(!idemux->priv)
	{
		eprintk("out of memory\n");
		return -1;
	}
	priv = idemux->priv;
	priv->idemux = idemux;

	switch(idemux->if_type)
	{
		case ITV_IF_TYPE_TS_PARALLEL:
			priv->buffer.width = ITV_TS_WIDTH;
			bufsize = ITV_TS_MAX_BUFSIZE;
			height_mask = ITV_TS_HEIGHT_MASK;
			break;
		default:
			// temp
			priv->buffer.width = ITV_TS_WIDTH;
			bufsize = ITV_TS_MAX_BUFSIZE;
			height_mask = ITV_TS_HEIGHT_MASK;
			break;
	}

	priv->buffer.height = bufsize * 1024 / priv->buffer.width;
	if(priv->buffer.height > 0xfff)
	{
		priv->buffer.height /= 2;
		priv->buffer.height &= height_mask;
		priv->buffer.size = 2 * priv->buffer.height * priv->buffer.width;
	}
	else
	{
		priv->buffer.height &= height_mask;
		priv->buffer.size = priv->buffer.height * priv->buffer.width;
	}
	priv->buffer.warning_threshold = priv->buffer.size * 80 / 100;
	priv->buffer.warnings = 0;
	priv->buffer.warning_time = jiffies;

//20120320 koo test debug : tsif module test
#if 1
	tsif_ops = idemux->tsif_ops;
	switch(idemux->if_type)
	{
		case ITV_IF_TYPE_TS_PARALLEL:
			dprintk("ITV_IF_TYPE_TS_PARALLEL\n");
			tasklet_init(&priv->ts_tasklet, itv_ts_irq_p_tsif, (unsigned long)priv);

			//20110413 koo : pkt_intr_cnt는 pid filtering을 사용 여부에 의해 func 내에서 define 된 값을 사용하도록 수정.
			if(tsif_ops->tsif_p_init(&priv->buffer.buffer, &priv->buffer.size, priv->buffer.size, 
				0, 0x04, 0x01, idemux->tsif_mode, itv_ts_irq_handler, (void *)priv))
			{
				eprintk("itv_p_tsif_init() is failed\n");
				tasklet_kill(&priv->ts_tasklet);
				kfree(priv);
				idemux->priv = NULL;
				return -1;
			}
			break;
		default:
			break;
	}
#endif

	priv->filtering = 0;
	INIT_LIST_HEAD(&priv->filter_list);

	spin_lock_init(&priv->lock);
	for(i = 0; i < ITV_TS_FILTER_NUM; i++)
	{
		priv->filter[i].priv = priv;
		priv->filter[i].buffer.data = NULL;
		itv_ts_filter_state_set(&priv->filter[i], ITV_TS_FILTER_STATE_FREE);
		INIT_LIST_HEAD(&priv->filter[i].list_head);
	}

	idemux->demux_start = itv_ts_start;
	idemux->demux_stop = itv_ts_stop;
	idemux->demux_poll = itv_ts_poll;
	idemux->demux_read = itv_ts_read;
	idemux->filter_get = itv_ts_filter_get;
	idemux->filter_put = itv_ts_filter_put;
	idemux->filter_set = itv_ts_filter_set;	
		
	return 0;
}

static void itv_ts_deactivate(itv_object_t *p_this)
{
	itv_demux_t *idemux = (itv_demux_t *)p_this;
	itv_demux_priv_t *priv;
	itv_platform_tsif_operations_t *tsif_ops;

	DEBUG_CALLSTACK

	if(!idemux) return;
	priv = idemux->priv;

	tsif_ops = idemux->tsif_ops;
	switch(idemux->if_type)
	{
		case ITV_IF_TYPE_TS_PARALLEL:
			tsif_ops->tsif_p_deinit();
			break;
		default:
			break;
	}

	tasklet_kill(&priv->ts_tasklet);

	kfree(priv);
	idemux->priv = NULL;
}

itv_module_t itv_ts_module =
{
	.psz_module_name = "ts", 
	.psz_capability = "idemux", 
	.i_score = 20, 
	.pf_activate = itv_ts_activate,
	.pf_deactivate = itv_ts_deactivate,
	.next = NULL, 
	.parent = NULL, 
	.submodule = NULL, 
	.i_submodules = 0
};

static int __init itv_ts_module_init(void)
{
	int retval;

	DEBUG_CALLSTACK

	if((retval = itv_module_register(&itv_ts_module)) != 0)
	{
		eprintk("itv_module_register() is failed\n");
		goto out;
	}

	dprintk("iTV ts module register success\n");

out:
	return retval;
}

static void __exit itv_ts_module_exit(void)
{
	DEBUG_CALLSTACK
		
	itv_module_unregister(&itv_ts_module);

	dprintk("iTV ts module unregister success\n");
}

module_init(itv_ts_module_init);
module_exit(itv_ts_module_exit);

MODULE_AUTHOR("JP");
MODULE_LICENSE("GPL");
