/*****************************************************************************/
/*!
 *  \file        os.h \n
 *  \version     1.0 \n
 *  \author      Vahlsing \n
 *  \brief       Generic operating system abstraction layer.
 *               Included by \ref os.h and the target OS implementations. \n
 *
 *  \revision    $Revision: 714 $ \n
 *               $Author: vahlsing $ \n
 *               $Date: 2009-11-28 00:32:26 +0100 (Sa, 28 Nov 2009) $ \n
 *               $Id: os.h 714 2009-11-27 23:32:26Z vahlsing $ \n
 */
/*  Copyright(c) Silicon Image GmbH, 2009.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted without any limitations.
 *
 *  Parts of this software are provided under individual licenses with more stringend
 *  terms and conditions. Those licenses include but are not limited to:
 *  - the GNU General Public License (multiple versions) 
 *  - the GNU General Public License (version 2) modified by the FreeRTOS exception
 *  - the BSD License or derived licenses.
 *  Detailed copyright and licensing information of those software parts is keept
 *  along with those software parts as required by their respective licenses.
 *
 *  This software is provided by the copyright holders and contributors "as is" and
 *  any express or implied warranties, including, but not limited to, the implied
 *  warranties of merchantability and fitness for a particular purpose are disclaimed.
 *  In no event shall the copyright holder or contributors be liable for any direct,
 *  indirect, incidental, special, exemplary, or consequential damages (including,
 *  but not limited to, procurement of substitute goods or services; loss of use,
 *  data, or profits; or business interruption) however caused and on any theory of
 *  liability, whether in contract, strict liability, or tort (including negligence
 *  or otherwise) arising in any way out of the use of this software, even if advised
 *  of the possibility of such damage.
 */
/*****************************************************************************/

#ifndef _OS_H
#define _OS_H

#include <stdarg.h>

#include "def.h"
#include "os_config.h"


#ifdef __cplusplus
extern "C"
{
#endif


//!@defgroup OS_LAYER OS layer interface
//!@{

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
//!@defgroup OS_LAYER_VER OS layer version
//!@{
#define OS_LAYER_VER_MAJOR  1 //!< major version of OS layer interface; use \REF OS_GetVerMajor() to read
#define OS_LAYER_VER_MINOR  0 //!< minor version of OS layer interface; use \REF OS_GetVerMinor() to read
#define OS_LAYER_VER_BUILD  6 //!< build number of OS layer interface; use \REF OS_GetVerBuild() to read

/*****************************************************************************/
/*
 *          OS_GetVerMajor()
 *//*!
 *  \BRIEF  Return the major version number of OS layer interface.
 *
 *  \RETURN                 Major version number.
 */
/*****************************************************************************/
extern UINT32 OS_GetVerMajor ( void );

/*****************************************************************************/
/*
 *          OS_GetVerMinor()
 *//*!
 *  \BRIEF  Return the minor version number of OS layer interface.
 *
 *  \RETURN                 Minor version number.
 */
/*****************************************************************************/
extern UINT32 OS_GetVerMinor ( void );

/*****************************************************************************/
/*
 *          OS_GetVerBuild()
 *//*!
 *  \BRIEF  Return the build number of OS layer interface.
 *
 *  \RETURN                 Build number.
 */
/*****************************************************************************/
extern UINT32 OS_GetVerBuild ( void );

/*****************************************************************************/
/*
 *          OS_GetVerInfo()
 *//*!
 *  \BRIEF  Return if the OS layer library is a debug or release build.
 *
 *  \RETURN                 'D'/'R' = Debug/Release build.
 */
/*****************************************************************************/
extern CHAR OS_GetVerInfo ( void );
//!@} defgroup OS_LAYER_VER

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
//!@defgroup OS_LAYER_GLOBAL OS layer globals
//!@{

/*****************************************************************************/
/*!
 *  \DESC   Status codes the OS abstraction layer functions may return.
 *
 *  \NOTE   Negative values mark errors, positive values mark warnings,
 *          zero marks OK.
 */
/*****************************************************************************/
typedef enum _OS_STATUS
{
    OS_OK                    = 0L,  //!< success
    // errors (negative)
    OS_ERROR                 =-1L,  //!< general error; e.g. not enough memory to create object
    OS_TIMEOUT               =-2L,  //!< operation failed due to elapsed timeout
    OS_INVALID_PARAM         =-3L,  //!< invalid parameter supplied
    OS_NOT_INITIALIZED       =-4L,  //!< resource object is not initialized
    OS_OPERATION_FAILED      =-5L,  //!< operation failed (due to unexpected problems in underlying OS)
    // warnings (positive)
    OS_WARNING               =+1L,  //!< general warning
    // dummy to force 32bit size
    OS_DUMMY                 =0x7fffffffL  //!< DUMMY, DON'T USE
} OS_STATUS;

/*****************************************************************************/
/*!
 *  \DESC   Returns zero terminated string of OS_STATUS given.
 */
/*****************************************************************************/
char* OS_Status2String(OS_STATUS osStatus);

/*****************************************************************************/
/*!
 *  \DESC   Special timeout value to be used with OS_xxxWait() functions.
 */
/*****************************************************************************/
#define OS_INFINITE_TIME    0xffffffffUL //!< Request infinite timeout (fully blocking operation).

/*****************************************************************************/
/*!
 *  \DESC   Special timeout value to be used with OS_xxxWait() functions.
 */
/*****************************************************************************/
#define OS_NONBLOCK_TIME    0x00000000UL //!< Request immediate timeout (non blocking operation).

//!@} defgroup OS_LAYER_GLOBAL

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
//!@defgroup OS_EVENT OS layer events
//!@{
#ifdef OS_EVENT

/*****************************************************************************/
/*!
 *  \DESC   Anonymous handle to OS layer events.
 */
/*****************************************************************************/
typedef struct _OS_Event* OS_hEvent;

/*****************************************************************************/
/*
 *          OS_EventCreate()
 *//*!
 *  \BRIEF  Create and initialize an event object.
 *
 *          Create an event. 'Automatic reset mode' as well as 'initial state'
 *          could be set. Automatic reset means OS_EventReset() needn't be
 *          called to set the event state back to non-signaled (reset) once it
 *          was signaled and a thread successfully waited for it. A manual reset
 *          event would have to be discretely reset by calling \REF OS_EventReset().
 *          While automatic reset events are more likely to be used for tight
 *          synchronisation between two threads or to wakeup a thread from an IRQ
 *          handler, will manual reset events be more like used for loose coupling
 *          of e.g. a source thread (setting the event), multiple sink threads
 *          waiting for the event in parallel and the source thread finally resetting
 *          the event after a certain period of time in which the sink threads
 *          should have taken notice of the event.
 *
 *  \NOTE   Just one of multiple threads waiting will be woken up for an automatic
 *          reset event. That thread will be either the one with the highest priority
 *          or an arbitrary one of all threads sharing the same highest priority.
 *          For a manual (=non automatic reset) event all threads waiting for it
 *          will be resumed once it gets signaled. This will happen in descending
 *          order with regard to the threads' priorities and in arbitrary order
 *          for threads sharing the same priority.
 *
 *  \PARAM  phEvent             Reference to an event handle. On success a valid
 *                              handle will be stored there, otherwise NULL.
 *  \PARAM  bAutomatic          Set automatic reset mode.
 *  \PARAM  bInitState          Set initial state to signaled/reset (=TRUE/FALSE)
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Event successfully created and initialized.
 *  \RETVAL OS_ERROR            Event could not be created.
 *  \RETVAL OS_NOT_INITIALIZED  Tried to signal/reset a not initialized event.
 *                              See \REF OS_EventSignal() or \REF OS_EventReset()
 *                              for more details.
 *  \RETVAL OS_OPERATION_FAILED Event could not be initialized.
 *                              See \REF OS_EventSignal() or \REF OS_EventReset()
 *                              for more details.
 */
/*****************************************************************************/
extern OS_STATUS OS_EventCreate (
    OUT OS_hEvent   *phEvent,
    IN  BOOL        bAutomatic,
    IN  BOOL        bInitState
);

/*****************************************************************************/
/*
 *          OS_EventDestroy()
 *//*!
 *  \BRIEF  Destroy an event object.
 *
 *          Destroy an event and free resources associated with the event object.
 *
 *  \NOTE   No internal reference count is kept. The caller has to make sure,
 *          that no other thread is waiting for the event anymore!
 *
 *  \PARAM  phEvent             Reference to an event handle. The handle will
 *                              get set to NULL once all resources are freed.
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Event successfully destroyed.
 */
/*****************************************************************************/
extern OS_STATUS OS_EventDestroy (
    INOUT OS_hEvent *phEvent
);

/*****************************************************************************/
/*
 *          OS_EventSignal()
 *//*!
 *  \BRIEF  Set the event state to signaled.
 *
 *          Set the state of the event object to signaled and thus wakeup
 *          suspended thread(s) waiting for the event.
 *
 *  \NOTE   See \REF OS_EventCreate().
 *
 *  \PARAM  hEvent              Handle of an event object.
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Event successfully signaled.
 *  \RETVAL OS_NOT_INITIALIZED  Tried to signal a not initialized event.
 *  \RETVAL OS_OPERATION_FAILED Event could not be signaled.
 */
/*****************************************************************************/
extern OS_STATUS OS_EventSignal (
    IN  OS_hEvent   hEvent
);

/*****************************************************************************/
/*
 *          OS_EventSignalIrq()
 *//*!
 *  \BRIEF  Set the event state to signaled from within IRQ context.
 *
 *          Set the state of the event object to signaled and thus wakeup
 *          suspended thread(s) waiting for the event from within an IRQ
 *          handler routine running in IRQ context.
 *
 *  \NOTE   See \REF OS_EventCreate().
 *
 *  \PARAM  hEvent              Handle of an event object.
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Event successfully signaled.
 *  \RETVAL OS_NOT_INITIALIZED  Tried to signal a not initialized event.
 */
/*****************************************************************************/
extern OS_STATUS OS_EventSignalIrq (
    IN  OS_hEvent   hEvent
);

/*****************************************************************************/
/*
 *          OS_EventReset()
 *//*!
 *  \BRIEF  Set the event state to reset.
 *
 *          Set the state of the event object to reset.
 *
 *  \NOTE   See \REF OS_EventCreate().
 *
 *  \PARAM  hEvent              Handle of an event object.
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Event successfully reset.
 *  \RETVAL OS_NOT_INITIALIZED  Tried to reset a not initialized event.
 *  \RETVAL OS_OPERATION_FAILED Event could not be reset.
 */
/*****************************************************************************/
extern OS_STATUS OS_EventReset (
    IN  OS_hEvent   hEvent
);

/*****************************************************************************/
/*
 *          OS_EventResetIrq()
 *//*!
 *  \BRIEF  Set the event state to reset from within IRQ context.
 *
 *          Set the state of the event object to reset from within an IRQ
 *          handler routine running in IRQ context. \n
 *          \b Hint: Having to reset an event from within an IRQ handler will
 *          almost ever be an indication for a general mistake in the design
 *          of your code.
 *
 *  \NOTE   See \REF OS_EventCreate().
 *
 *  \PARAM  hEvent              Handle of an event object.
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Event successfully reset.
 *  \RETVAL OS_NOT_INITIALIZED  Tried to reset a not initialized event.
 */
/*****************************************************************************/
extern OS_STATUS OS_EventResetIrq (
    IN  OS_hEvent   hEvent
);

/*****************************************************************************/
/*
 *          OS_EventPulse()
 *//*!
 *  \BRIEF  Set the event state to signaled and immediately reset it.
 *
 *          Set the state of the event object to signaled and thus wakeup
 *          suspended thread(s) waiting for the event followed by an immediate
 *          reset of the event.
 *
 *  \NOTE   See \REF OS_EventCreate().
 *
 *  \PARAM  hEvent              Handle of an event object.
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Event successfully pulsed.
 *  \RETVAL OS_NOT_INITIALIZED  Tried to pulse a not initialized event.
 *  \RETVAL OS_OPERATION_FAILED Event could not be pulsed.
 */
/*****************************************************************************/
extern OS_STATUS OS_EventPulse (
    IN  OS_hEvent   hEvent
);

/*****************************************************************************/
/*
 *          OS_EventWait()
 *//*!
 *  \BRIEF  Wait for an event to be signaled.
 *
 *          Wait for the state of the event object to get signaled while
 *          infinitely blocking the calling thread. The function internally
 *          calls \REF OS_EventTimedWait() with timeout parameter set to
 *          \REF OS_INFINITE_TIME.
 *
 *  \NOTE   See \REF OS_EventCreate().
 *
 *  \PARAM  hEvent              Handle of an event object.
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL                     See \REF OS_EventTimedWait().
 */
/*****************************************************************************/
extern OS_STATUS OS_EventWait (
    IN  OS_hEvent   hEvent
);

/*****************************************************************************/
/*
 *          OS_EventTimedWait()
 *//*!
 *  \BRIEF  Wait with timeout for an event to be signaled.
 *
 *          Wait for the state of the event object to get signaled while
 *          blocking the calling thread. The function call returns immediatly,
 *          if the event is already in the signaled state. If the specified
 *          timeout elapses, the function returns with an apropriate error
 *          code. Depending on the auto reset mode, the event is either
 *          reset automatically, or has to be reset later on using the
 *          \REF OS_EventReset() or \REF OS_EventResetIrq() functions.
 *  \NOTE   See \REF OS_EventCreate().
 *
 *  \PARAM  hEvent              Handle of an event object.
 *  \PARAM  u32MSec             Time in msec to wait for the event to get
 *                              signaled.
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Event was signaled within timeout period.
 *  \RETVAL OS_TIMEOUT          Event wasn't signaled within timeout period.
 *  \RETVAL OS_NOT_INITIALIZED  Tried to wait for a not initialized event.
 *  \RETVAL OS_OPERATION_FAILED Could not wait for the event.
 */
/*****************************************************************************/
extern OS_STATUS OS_EventTimedWait (
    IN  OS_hEvent   hEvent,
    IN  UINT32      u32MSec
);

#endif // OS_EVENT
//!@} defgroup OS_EVENT

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
//!@defgroup OS_MUTEX OS layer mutexes
//!@{
#ifdef OS_MUTEX

/*****************************************************************************/
/*!
 *  \BRIEF  Anonymous handle to OS layer mutexes.
 */
/*****************************************************************************/
typedef struct _OS_Mutex* OS_hMutex;

/*****************************************************************************/
/*
 *          OS_MutexCreate()
 *//*!
 *  \BRIEF  Create and initialize a mutex object.
 *
 *          Create a mutex and set its initial state to unlocked.
 *
 *  \NOTE   In contrast to a semaphore, a mutex must be unlocked by the same
 *          thread that did originally lock it. \n
 *          If a mutex is held by thread A and another, higher prioritized
 *          thread B tries to get hold of the mutex, the priority of thread A
 *          gets raised to that of thread B until thread A proceeds and
 *          finally releases the mutex. This mechanism is called 'Priority
 *          Inheritance' and is required to aviod situations where a high priority
 *          thread is blocked by a low priority thread from accessing shared
 *          resources and that low priority thread itself is blocked from execution
 *          by a medium priority thread consuming the processing power - a situation
 *          called 'Priority Inversion'.
 *
 *  \PARAM  phMutex             Reference to a mutex handle. On success a valid
 *                              handle will be stored there, otherwise NULL.
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Mutex successfully created and initialized.
 *  \RETVAL OS_ERROR            Mutex could not be created.
 */
/*****************************************************************************/
extern OS_STATUS OS_MutexCreate (
    OUT OS_hMutex   *phMutex
);

/*****************************************************************************/
/*
 *          OS_MutexDestroy()
 *//*!
 *  \BRIEF  Destroy a mutex object.
 *
 *          Destroy a mutex and free resources associated with the mutex object.
 *
 *  \NOTE   No internal reference count is kept. The caller has to make sure,
 *          that no other thread is waiting for the mutex anymore!
 *
 *  \PARAM  phMutex             Reference to a mutex handle. The handle will
 *                              get set to NULL once all resources are freed.
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Mutex successfully destroyed.
 */
/*****************************************************************************/
extern OS_STATUS OS_MutexDestroy (
    IN  OS_hMutex   *phMutex
);

/*****************************************************************************/
/*
 *          OS_MutexTryLock()
 *//*!
 *  \BRIEF  Try to lock a mutex object.
 *
 *          Try to lock a mutex but without blocking the calling thread.
 *          The function will always return immediately.
 *
 *  \NOTE   See \REF OS_MutexCreate().
 *
 *  \PARAM  hMutex              Handle of a mutex object.
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Mutex successfully locked.
 *  \RETVAL OS_NOT_INITIALIZED  Tried to lock a not initialized mutex.
 *  \RETVAL OS_OPERATION_FAILED Mutex is already locked by someone else.
 */
/*****************************************************************************/
extern OS_STATUS OS_MutexTryLock (
    IN  OS_hMutex   hMutex
);

/*****************************************************************************/
/*
 *          OS_MutexLock()
 *//*!
 *  \BRIEF  Lock a mutex object.
 *
 *          Lock a mutex. The thread will get blocked if the mutex is already
 *          locked until it gets unlocked by the current holder again.
 *
 *  \NOTE   See \REF OS_MutexCreate().
 *
 *  \PARAM  hMutex              Handle of a mutex object.
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Mutex successfully locked.
 *  \RETVAL OS_NOT_INITIALIZED  Tried to lock a not initialized mutex.
 *  \RETVAL OS_OPERATION_FAILED Mutex couldn't be locked.
 */
/*****************************************************************************/
extern OS_STATUS OS_MutexLock (
    IN  OS_hMutex   hMutex
);

/*****************************************************************************/
/*
 *          OS_MutexUnlock()
 *//*!
 *  \BRIEF  Unlock a mutex object.
 *
 *          Unlock a mutex previously locked by this thread.
 *
 *  \NOTE   See \REF OS_MutexCreate().
 *
 *  \PARAM  hMutex              Handle of a mutex object.
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Mutex successfully unlocked.
 *  \RETVAL OS_NOT_INITIALIZED  Tried to unlock a not initialized mutex.
 *  \RETVAL OS_OPERATION_FAILED Mutex couldn't be unlocked.
 */
/*****************************************************************************/
extern OS_STATUS OS_MutexUnlock (
    IN  OS_hMutex   hMutex
);

#endif // OS_MUTEX
//!@} defgroup OS_MUTEX

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
//!@defgroup OS_SEMAPHORE OS layer semaphores
//!@{
#ifdef OS_SEMAPHORE

/*****************************************************************************/
/*!
 *  \BRIEF  Anonymous handle to OS layer semaphores.
 */
/*****************************************************************************/
typedef struct _OS_Semaphore* OS_hSemaphore;

/*****************************************************************************/
/*
 *          OS_SemaphoreCreate()
 *//*!
 *  \BRIEF  Create and initialize a semaphore object.
 *
 *          Create a counting semaphore and set its initial count as specified.
 *
 *  \NOTE   In contrast to a mutex, a semaphore may be (repeatedly) taken by
 *          one thread and be given back by another thread. No priority
 *          inheritance will take place. Depending on the way the semaphore
 *          is going to be used, e.g. either a) like a counter for events that
 *          occurred or are to be processed or b) like a counter for remaining
 *          or available resources, the initial count may be used to preload
 *          the semaphore count accordingly, e.g. set it to 0 (zero) for a) or
 *          set it to whatever suits for b). The inital and maximum count is
 *          limited to a maximum of 0x7fffffffL.
 *
 *  \PARAM  phSemaphore         Reference to a semaphore handle. On success a
 *                              valid handle will be stored there, otherwise NULL.
 *  \PARAM  u32InitCount        Initial count of the semaphore.

 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Semaphore successfully created and initialized.
 *  \RETVAL OS_ERROR            Semaphore could not be created.
 *  \RETVAL OS_INVALID_PARAM    Inital semaphore count exceeded maximum allowed.
 */
/*****************************************************************************/
extern OS_STATUS OS_SemaphoreCreate(
    OUT OS_hSemaphore   *phSemaphore,
    IN  UINT32          u32InitCount
);

/*****************************************************************************/
/*
 *          OS_SemaphoreDestroy()
 *//*!
 *  \BRIEF  Destroy a semaphore object.
 *
 *          Destroy a semaphore and free resources associated with the
 *          semaphore object.
 *
 *  \NOTE   No internal reference count is kept. The caller has to make sure,
 *          that no other thread is waiting for the semaphore anymore!
 *
 *  \PARAM  phSemaphore         Reference to a semaphore handle. The handle will
 *                              get set to NULL once all resources are freed.
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Semaphore successfully destroyed.
 */
/*****************************************************************************/
extern OS_STATUS OS_SemaphoreDestroy (
    IN  OS_hSemaphore   *phSemaphore
);

/*****************************************************************************/
/*
 *          OS_SemaphorePost()
 *//*!
 *  \BRIEF  Post a semaphore object.
 *
 *          Post a semaphore by increasing the semaphore count.
 *
 *  \NOTE   See \REF OS_SemaphoreCreate().
 *
 *  \PARAM  hSemaphore          Handle of a semaphore object.
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Semaphore successfully posted.
 *  \RETVAL OS_NOT_INITIALIZED  Tried to post a not initialized Semaphore.
 *  \RETVAL OS_OPERATION_FAILED Semaphore couldn't be posted.
 */
/*****************************************************************************/
extern OS_STATUS OS_SemaphorePost (
    IN  OS_hSemaphore   hSemaphore
);

/*****************************************************************************/
/*
 *          OS_SemaphoreTryWait()
 *//*!
 *  \BRIEF  Try to get a semaphore.
 *
 *          Try to get a semaphore without blocking the calling thread. The
 *          function internally calls \REF OS_SemaphoreTimedWait() with timeout
 *          parameter set to \REF OS_NONBLOCK_TIME.
 *
 *  \NOTE   See \REF OS_SemaphoreCreate().
 *
 *  \PARAM  hSemaphore          Handle of a semaphore object.
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL                     See \REF OS_SemaphoreTimedWait().
 */
/*****************************************************************************/
extern OS_STATUS OS_SemaphoreTryWait (
    IN  OS_hSemaphore   hSemaphore
);

/*****************************************************************************/
/*
 *          OS_SemaphoreWait()
 *//*!
 *  \BRIEF  Wait for a semaphore to get available.
 *
 *          Wait for a semaphore to get available while inifitely blocking the
 *          calling thread. The function internally calls
 *          \REF OS_SemaphoreTimedWait() with timeout parameter set to
 *          \REF OS_INFINITE_TIME.
 *
 *  \NOTE   See \REF OS_SemaphoreCreate().
 *
 *  \PARAM  hSemaphore          Handle of a semaphore object.
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL                     See \REF OS_SemaphoreTimedWait().
 */
/*****************************************************************************/
extern OS_STATUS OS_SemaphoreWait (
    IN  OS_hSemaphore   hSemaphore
);

/*****************************************************************************/
/*
 *          OS_SemaphoreTimedWait()
 *//*!
 *  \BRIEF  Wait with timeout for a semaphore to get available.
 *
 *          Wait for a semaphore to get available while blocking the calling
 *          thread. The function call returns immediatly, if a semaphore is
 *          available. If the specified timeout elapses, the function returns
 *          with an apropriate error code.
 *
 *  \NOTE   See \REF OS_SemaphoreCreate().
 *
 *  \PARAM  hSemaphore          Handle of a semaphore object.
 *  \PARAM  u32MSec             Time in msec to wait for the semaphore to get
 *                              available.
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Semaphore was available within timeout period.
 *  \RETVAL OS_TIMEOUT          Semaphore wasn't available within timeout period.
 *  \RETVAL OS_NOT_INITIALIZED  Tried to wait for a not initialized semaphore.
 *  \RETVAL OS_OPERATION_FAILED Could not wait for the semaphore.
 */
/*****************************************************************************/
extern OS_STATUS OS_SemaphoreTimedWait (
    IN  OS_hSemaphore   hSemaphore,
    IN  UINT32          u32MSec
);

#endif // OS_SEMAPHORE
//!@} defgroup OS_SEMAPHORE

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
//!@defgroup OS_QUEUE OS layer queues
//!@{
#ifdef OS_QUEUE

/*****************************************************************************/
/*
 *  \BRIEF  Anonymous handle to OS layer queues.
 */
/*****************************************************************************/
typedef struct _OS_Queue* OS_hQueue;

/*****************************************************************************/
/*
 *          OS_QueueCreate()
 *//*!
 *  \BRIEF  Create and initialize an queue object.
 *
 *          Create a queue. A contiguous block of at least u32MaxItems*u32ItemSize
 *          bytes of memory will be allocated on the heap for data storage.
 *          No special alignment of data items in memory, neither base address
 *          nor size, is required, but may make sense, in order to speed up the
 *          copy process, which takes place two times for each data item (write
 *          to and read back from the queue).
 *
 *  \NOTE   Its perfectly OK for multiple threads trying to read or write a
 *          single queue simultaneously, as long its OK for your application
 *          as well ;-). \n
 *          If a queue gets full/empty the threads trying to write/read that
 *          queue will block until the queue gets non-full/non-empty again.
 *          But then, just one of the waiting write/read threads will be woken
 *          up for each item read/written from/to the queue. The thread woken
 *          up will be either the one with the highest priority or an arbitrary
 *          one of all threads sharing the same highest priority.
 *
 *  \PARAM  phQueue             Reference to a queue handle. On success a valid
 *                              handle will be stored there, otherwise NULL.
 *  \PARAM  u32MaxItems         Number of items the queue can hold.
 *  \PARAM  u32ItemSize         Size of an item in number of bytes.
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Queue successfully created and initialized.
 *  \RETVAL OS_ERROR            Queue could not be created.
 */
/*****************************************************************************/
extern OS_STATUS OS_QueueCreate (
    OUT OS_hQueue   *phQueue,
    IN  UINT32      u32MaxItems,
    IN  UINT32      u32ItemSize
);

/*****************************************************************************/
/*
 *          OS_QueueDestroy()
 *//*!
 *  \BRIEF  Destroy a queue object.
 *
 *          Destroy a queue and free resources associated with the queue object.
 *
 *  \NOTE   No internal reference count is kept. The caller has to make sure,
 *          that no other thread is waiting to access the queue anymore!
 *
 *  \PARAM  phQueue             Reference to a queue handle. The handle will
 *                              get set to NULL once all resources are freed.
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Queue successfully destroyed.
 */
/*****************************************************************************/
extern OS_STATUS OS_QueueDestroy (
    IN  OS_hQueue   *phQueue
);

/*****************************************************************************/
/*
 *          OS_QueueSend()
 *//*!
 *  \BRIEF  Send an item to a queue.
 *
 *          Send an item to a queue. The function will return immediately if
 *          either the queue isn't full so that the write could be performed
 *          or if a timeout value of \REF OS_NONBLOCK_TIME is given. For other
 *          timeout values, the function will block the calling thread until
 *          either queue space gets available or the timeout expires. For a
 *          timeout value of \REF OS_INFINITE_TIME the block would last
 *          infinitely if no space gets avaialble.
 *
 *  \NOTE   See \REF OS_QueueCreate().
 *
 *  \PARAM  hQueue              Handle of a queue object.
 *  \PARAM  pItemToQueue        Pointer to the item to be send to the queue.
 *  \PARAM  u32MSec             Time in msec to wait for queue space to get
 *                              available.
 *  \PARAM  bFront              Where to put the item to the queue
 *                              (front/back = TRUE/FALSE).
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Item successfully send to queue.
 *  \RETVAL OS_TIMEOUT          Couldn't send item within timeout period.
 *  \RETVAL OS_NOT_INITIALIZED  Tried to write to a not initialized queue.
 *  \RETVAL OS_OPERATION_FAILED Queue could not be written.
 */
/*****************************************************************************/
extern OS_STATUS OS_QueueSend (
    IN  OS_hQueue   hQueue,
    IN  const void  *pItemToQueue,
    IN  UINT32      u32MSec,
    IN  BOOL        bFront
);

/*****************************************************************************/
/*
 *          OS_QueueSendIrq()
 *//*!
 *  \BRIEF  Send an item to a queue from within IRQ context.
 *
 *          Send an item to a queue from within an IRQ handler routine running
 *          in IRQ context. The function will always return immediately.
 *
 *  \NOTE   See \REF OS_QueueCreate().
 *
 *  \PARAM  hQueue              Handle of a queue object.
 *  \PARAM  pItemToQueue        Pointer to the item to be send to the queue.
 *  \PARAM  bFront              Where to put the item to the queue
 *                              (front/back = TRUE/FALSE).
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Item successfully send to queue.
 *  \RETVAL OS_TIMEOUT          Queue could not be written (it is full).
 *  \RETVAL OS_NOT_INITIALIZED  Tried to write to a not initialized queue.
 */
/*****************************************************************************/
extern OS_STATUS OS_QueueSendIrq (
    IN  OS_hQueue   hQueue,
    IN  const void  *pItemToQueue,
    IN  BOOL        bFront
);

/*****************************************************************************/
/*
 *          OS_QueueReceive()
 *//*!
 *  \BRIEF  Receive an item from a queue.
 *
 *          Receive an item from a queue. The function will return immediately
 *          if either the queue isn't empty so that the read could be performed
 *          or if a timeout value of \REF OS_NONBLOCK_TIME is given. For other
 *          timeout values, the function will block the calling thread until
 *          either new data gets available or the timeout expires. For a
 *          timeout value of \REF OS_INFINITE_TIME the block would last
 *          infinitely if no data gets available. Depending on the selected
 *          read mode the item is either removed from the queue or kept in
 *          the queue.
 *
 *  \NOTE   See \REF OS_QueueCreate().
 *
 *  \PARAM  hQueue              Handle of a queue object.
 *  \PARAM  pItemToReceive      Pointer to the item to be received from the queue.
 *  \PARAM  u32MSec             Time in msec to wait for data to get available.
 *  \PARAM  bPeek               How to read the item from the queue
 *                              (peek/remove = TRUE/FALSE).
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Item successfully received from queue.
 *  \RETVAL OS_TIMEOUT          Couldn't receive item within timeout period.
 *  \RETVAL OS_NOT_INITIALIZED  Tried to read from a not initialized queue.
 *  \RETVAL OS_OPERATION_FAILED Queue could not be read.
 */
/*****************************************************************************/
extern OS_STATUS OS_QueueReceive (
    IN  OS_hQueue   hQueue,
    OUT void        *pItemToReceive,
    IN  UINT32      u32MSec,
    IN  BOOL        bPeek
);

/*****************************************************************************/
/*
 *          OS_QueueReceiveIrq()
 *//*!
 *  \BRIEF  Receive an item from a queue from within IRQ context.
 *
 *          Receive an item from a queue from within an IRQ handler routine
 *          running in IRQ context. The function will always return immediately.
 *          Depending on the selected read mode the item is either removed from
 *          the queue or kept in the queue.
 *
 *  \NOTE   See \REF OS_QueueCreate().
 *
 *  \PARAM  hQueue              Handle of a queue object.
 *  \PARAM  pItemToReceive      Pointer to the item to be received from the queue.
 *  \PARAM  bPeek               How to read the item from the queue
 *                              (peek/remove = TRUE/FALSE).
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Item successfully received from queue.
 *  \RETVAL OS_TIMEOUT          Queue could not be read (it is empty).
 *  \RETVAL OS_NOT_INITIALIZED  Tried to read from a not initialized queue.
 */
/*****************************************************************************/
extern OS_STATUS OS_QueueReceiveIrq (
    IN  OS_hQueue   hQueue,
    OUT void        *pItemToReceive,
    IN  BOOL        bPeek
);
#endif // OS_QUEUE
//!@} defgroup OS_QUEUE

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
//!@defgroup OS_THREAD OS layer threads
//!@{
#ifdef OS_THREAD

/*****************************************************************************/
/*!
 *  \DESC   Anonymous handle to OS layer threads.
 */
/*****************************************************************************/
typedef struct _OS_Thread* OS_hThread;

/*****************************************************************************/
/*!
 *  \DESC   The prototype for OS layer thread functions.
 */
/*****************************************************************************/
typedef void OS_ThreadFunc (void *pArg);

/*****************************************************************************/
/*!
 *  \BRIEF  Special thread stack size.
 *          Forces a target system specific default stack size to be used for
 *          the thread to be created.
 */
/*****************************************************************************/
#define OS_USE_DEFAULT_STACK_SIZE 0

/*****************************************************************************/
/*!
 *  \DESC   Possible execution priorities for OS abstraction layer threads.
 */
/*****************************************************************************/
typedef enum _OS_THREAD_PRIO {
    OS_THREAD_PRIO_HIGHEST  =4, //!< e.g. used for deferred interrupt handlers
    OS_THREAD_PRIO_HIGH     =3, //!< e.g. used for communication protocol drivers
    OS_THREAD_PRIO_NORMAL   =2, //!< e.g. used for normal application code
    OS_THREAD_PRIO_LOW      =1, //!< e.g. used for (graphical) user interface
    OS_THREAD_PRIO_LOWEST   =0  //!< e.g. used for low priority housekeeping
} OS_THREAD_PRIO;

/*****************************************************************************/
/*
 *          OS_ThreadCreate()
 *//*!
 *  \BRIEF  Create a new thread.
 *
 *          Create a new thread with given stack size and priority.
 *
 *  \NOTE   Threads may terminate themselves simply by returning from their
 *          thread function. Upon termination an internal event gets signaled.
 *          Use \REF OS_ThreadTimedWait() to wait for that event being signaled.
 *
 *  \PARAM  phThread            Reference to a thread handle. On success a valid
 *                              handle will be stored there, otherwise NULL.
 *                              If reference is NULL, no handle will be returned.
 *  \PARAM  pThreadFunc         The thread function to be executed.
 *  \PARAM  pArg                Reference to the thread function argument.
 *  \PARAM  u32StackSizeBytes   The required stack size.
 *                              See \REF OS_USE_DEFAULT_STACK_SIZE.
 *  \PARAM  Priority            The initial execution priority.
 *  \PARAM  szThreadName        A name for the thread.
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Thread successfully created.
 *  \RETVAL OS_ERROR            Thread could not be created.
 *  \RETVAL OS_INVALID_PARAM    Illegal parameter specified.
 */
/*****************************************************************************/
extern OS_STATUS OS_ThreadCreate (
    OUT OS_hThread      *phThread,
    IN  OS_ThreadFunc   *pThreadFunc,
    IN  void            *pArg,
    IN  UINT32          u32StackSizeBytes,
    IN  OS_THREAD_PRIO  Priority,
    IN  CHAR            *szThreadName
);

/*****************************************************************************/
/*
 *          OS_ThreadDestroy()
 *//*!
 *  \BRIEF  Destroy a thread object.
 *
 *          Destroy a thread and free resources associated with the thread
 *          object. If the associated thread is still running when this function
 *          is called, it get's terminated first.
 *
 *  \NOTE   No internal reference count is kept. The caller has to make sure,
 *          that no other thread is waiting for the thread anymore!
 *
 *  \PARAM  phThread            Reference to a thread handle. The handle will
 *                              get set to NULL once all resources are freed.
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Thread successfully destroyed.
 */
/*****************************************************************************/
extern OS_STATUS OS_ThreadDestroy (
    IN  OS_hThread      *phThread
);

/*****************************************************************************/
/*
 *          OS_ThreadTimedWait()
 *//*!
 *  \BRIEF  Wait for the thread to have terminated.
 *
 *          Wait for the state of the thread exit event to get signaled while
 *          blocking the calling thread. The function internally calls
 *          \REF OS_EventTimedWait().
 *
 *  \NOTE   See \REF OS_EventTimedWait().
 *
 *  \PARAM  hThread             Handle of a thread object.
 *  \PARAM  u32MSec             Time in msec to wait for a thread to have
 *                              terminated. See \REF OS_INFINITE_TIME or
 *                              \REF OS_NONBLOCK_TIME.
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL                     See \REF OS_EventTimedWait().
 */
/*****************************************************************************/
extern OS_STATUS OS_ThreadTimedWait (
    IN  OS_hThread      hThread,
    IN  UINT32          u32MSec
);

/*****************************************************************************/
/*
 *          OS_ThreadSetPriority()
 *//*!
 *  \BRIEF  Set thread execution priority.
 *
 *          Change thread execution priority and return the previous priority.
 *
 *  \PARAM  hThread             Handle of a thread object. Maybe NULL to set
 *                              own priority.
 *  \PARAM  NewPriority         New thread priority.
 *  \PARAM  pOldPriority        Reference to old priority. Maybe NULL if old
 *                              priority isn't required e.g. to restore it
 *                              later on. Only valid on success.
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Priority successfully changed.
 *  \RETVAL OS_INVALID_PARAM    Illegal priority specified.
 *  \RETVAL OS_NOT_INITIALIZED  Tried to set priority of a not initialized thread.
 */
/*****************************************************************************/
extern OS_STATUS OS_ThreadSetPriority (
    IN  OS_hThread      hThread,
    IN  OS_THREAD_PRIO  NewPriority,
    OUT OS_THREAD_PRIO  *pOldPriority
);

/*****************************************************************************/
/*!
 *  \DESC   Argument structure passed to application's main thread function
 *          as required by the OS abstraction layer.
 */
/*****************************************************************************/
typedef struct _OS_MainThreadArgs {
    int     argc;           //!< forwarded as given to main()
    char    **argv;         //!< forwarded as given to main()
    void    *pReserved;     //!< you may guess it ;-)
    void    *pUnused;       //!< you may guess it ;-)
} OS_MainThreadArgs;

/*****************************************************************************/
/*!
 *  \DESC   Type of the application's main thread function to be registered
 *          as such as required by the OS abstraction layer.
 */
/*****************************************************************************/
typedef void AppMainThreadFunc (OS_MainThreadArgs *pArgs);

/*****************************************************************************/
/*!
 *  \DESC   Layout of structure used internally for registering the
 *          application's main thread as required by the OS abstraction layer.
 */
/*****************************************************************************/
typedef struct __OS_MainThreadContext {
    OS_hThread          hThread;
    AppMainThreadFunc   *pThreadFunc;
    OS_MainThreadArgs   ThreadArgs;
    UINT32              u32MinStackSize;
    OS_THREAD_PRIO      Priority;
    CHAR                *szThreadName;
} _OS_MainThreadContext;

/*****************************************************************************/
/*!
 *  \DESC   Internal structure used for registering the application's main
 *          thread as required by the OS abstraction layer.
 */
/*****************************************************************************/
extern _OS_MainThreadContext _OS_MainThread;

/*****************************************************************************/
/*!
 *  \BRIEF  Macro to be used to register the application's main thread.
 */
/*****************************************************************************/
#define OS_REGISTER_MAIN_THREAD(                \
    /*IN AppMainThreadFunc* */ ThreadFunc,      \
    /*IN UINT32             */ u32MinStackSize, \
    /*IN OS_THREAD_PRIO     */ Priority,        \
    /*IN CHAR*              */ szThreadName     \
)                                               \
_OS_MainThreadContext _OS_MainThread =          \
{                                               \
    NULL,                                       \
    ThreadFunc,                                 \
    {0, NULL},      /* set at runtime */        \
    u32MinStackSize,                            \
    Priority,                                   \
    szThreadName                                \
};

#endif // OS_THREAD
//!@} defgroup OS_THREAD

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
//!@defgroup OS_FILE OS layer files
//!@{
#ifdef OS_FILE

/*****************************************************************************/
/*!
 *  \BRIEF  Possible file access modes for OS layer files.
 *
 *          The modes are exclusive. They specify the way the file shall be
 *          accessed and whether the file must already exist or will be created
 *          if necessary.
 *
 *  \NOTE   All files are accessed as plain binary files, no special text mode
 *          processing is performed.
 */
/*****************************************************************************/
typedef enum _OS_FILE_ACCESS {
    OS_FILE_ACCESS_READ_EXISTING,       //!< ANSI-Mode "rb" : random read, file must exist.
    OS_FILE_ACCESS_READWRITE_EXISTING,  //!< ANSI-Mode "rb+": random read/write, file must exist.
    OS_FILE_ACCESS_WRITE_CREATENEW,     //!< ANSI-Mode "wb" : random write, if file exists the previous contents are discarded, otherwise a new file is created.
    OS_FILE_ACCESS_READWRITE_CREATENEW, //!< ANSI-Mode "wb+": random read/write, if file exists the previous contents are discarded, otherwise a new file is created.
    OS_FILE_ACCESS_APPEND_EXISTING,     //!< ANSI-Mode "ab" : always write to the end (seek is ignored for writing), file must exist.
    OS_FILE_ACCESS_READAPPEND_EXISTING  //!< ANSI-Mode "ab+": random read, always write to the end (seek is ignored for writing), file must exist.
} OS_FILE_ACCESS;

/*****************************************************************************/
/*!
 *  \BRIEF  Special file seek positions.
 */
/*****************************************************************************/
typedef enum _OS_FILE_POSITION {
    OS_FILE_SEEK_BEGIN  =0L,    //!< Seeks to the beginning of the file.
    OS_FILE_SEEK_END    =-1L    //!< Seeks to the end of the file.
} OS_FILE_SEEK_POSITION;

/*****************************************************************************/
/*
 *  Some predefined drive prefixes.
 */
/*****************************************************************************/
//!@defgroup OS_FILE_DRIVE_PREFIXES OS layer predefined drive prefixes
//!@{

#define OS_DRIVE_SEMI       "semi://"   /*!< Predefined drive prefix for semihosting.
                                             \NOTE Only available if \REF OS_FILE_WANT_ARMSEMI is defined in \REF os_config.h.
                                             \NOTE Extended path semantics apply.*/
#ifndef OS_FILE_WANT_ARMSEMI            // we want this to always be documented by doxygen
#undef OS_DRIVE_SEMI                    // but compiled in only if OS_FILE_WANT_ARMSEMI is defined
#endif                                  // that's why we have this unusual way in here

#define OS_DRIVE_TFTP       "tftp://"   /*!< Predefined drive prefix for TFTP access.
                                             \NOTE Only supported by \REF OS_FileLoad() & \REF OS_FileSave().
                                             \NOTE Gets substituted with \REF OS_DRIVE_TRX by other OS_FileXxx() functions
                                                   to achieve compatibility with the bootloader when booting via TFTP. */

#define OS_DRIVE_TRX        "trx://"    /*!< Predefined drive prefix for Trillian extended remote I/O. */

#define OS_DRIVE_USB        "usb://"    /*!< Predefined drive prefix for USB storage. */
//!@} defgroup OS_FILE_DRIVE_PREFIXES

/*****************************************************************************/
/*!
 *  \BRIEF  Maximum path length of OS layer file names including drive prefix.
 */
/*****************************************************************************/
#define OS_FILE_MAX_PATH 255

/*****************************************************************************/
/*!
 *  @defgroup OS_FILE_NAMES OS layer file names
 *
 *          OS layer file names are build according to the following scheme:
 *
 *          [ {prot://} ] [ {serverip:} ] [ {path/} ] {filename}.
 *
 *          Where:
 *          - {prot://}     \n
 *                          one of the \REF OS_FILE_DRIVE_PREFIXES; if omitted a
 *                          default drive prefix will be taken from environment
 *                          variable 'boot_path' or if it's empty or not existent
 *                          'boot_path_dbg'; the selected variable must hold a
 *                          valid prefix then; despite the prefix both variables
 *                          may contain {serverip:} and/or {path/} as well to
 *                          create a virtual root which the application can only
 *                          escape by specifying a fully qualified path including
 *                          drive prefix, serverip (optional), path (optional) and
 *                          filename
 *          - {serverip:}   \n
 *                          if a remote protocol is chosen via the drive prefix,
 *                          the IP address of the server to be used to access the
 *                          given remote file may be specified; if omitted the
 *                          IP adress will be taken from environment variable
 *                          'serverip'; that variable must then hold a valid IP
 *                          address in the well known IPv4 form a.b.c.d
 *          - {path/}       \n
 *                          if the file resides in a directory below the server's
 *                          or device's root directory the path down the directory
 *                          hierarchy is to be given; single forward slashes '/'
 *                          are used as delimiters, device drivers and servers
 *                          may have to adopt this according to their needs; the
 *                          path shall not start with a leading slash nor contain
 *                          dots '.' or '..' to traverse directories in order to
 *                          make sure that only files below a virtual root can be
 *                          accessed; this virtual root is the device's root or
 *                          the server's configured root directory; pathes breaking
 *                          this convention will be rejected; for valid characters
 *                          see {filename}
 *          - {filename}    \n
 *                          the name of the file including the extension (if any);
 *                          valid characters for the filename (and the path as well)
 *                          are given below; names breaking this convention will be
 *                          rejected;\n
 *                          - 'a'..'z'
 *                          - 'A'..'Z'
 *                          - '0'..'9'
 *                          - '_', '.', '-', '+'
 *
 *  \NOTE   If \REF OS_DRIVE_SEMI is selected some less restrictive path semantics apply.
 *          This is due to the AXD debugger setting it's virtual root (better: current
 *          semihosting working dir) to where the .axd file is located. The path may
 *          begin with any number of '../' to traverse upwards in the directory hierarchy
 *          or begin with a slash '/' to start traversing at root dir level. It may as well
 *          begin with a Windows style drive prefix like 'c:' if a Windows host is connected.
 *          Nevertheless must slashes '/' be used as separators and not the Windows
 *          back-slashes '\'!
 *
 *          EXAMPLE: Fully qualified path
 *              - 'serverip'=ignored
 *              - 'boot_path'=ignored
 *              - 'boot_path_dbg'=ignored
 *              - given filename=usb://path/to/my.file
 *              .
 *              Resulting path is: usb://path/to/my.file
 *
 *          EXAMPLE: File with path, full virtual root path on default server
 *              - 'serverip'=192.168.11.1
 *              - 'boot_path'=tftp://virtual/root
 *              - 'boot_path_dbg'=ignored
 *              - given filename=path/to/my.file
 *              .
 *              Resulting path is: tftp://192.168.11.1:virtual/root/path/to/my.file
 *
 *          EXAMPLE: File with path, default protocol but on other server
 *              - 'serverip'=ignored
 *              - 'boot_path'=tftp://192.168.22.2:virtual/root
 *              - 'boot_path_dbg'=ignored
 *              - given filename=192.168.33.3:path/to/my.file
 *              .
 *              Resulting path is: tftp://192.168.33.3:path/to/my.file
 *
 *          EXAMPLE: File with path, default server
 *              - 'serverip'=192.168.11.1
 *              - 'boot_path'=trx://
 *              - 'boot_path_dbg'=ignored
 *              - given filename=path/to/my.file
 *              .
 *              Resulting path is: trx://192.168.11.1:path/to/my.file
 *
 *          EXAMPLE: File with path, running from debugger
 *              - 'serverip'=ignored
 *              - 'boot_path'= (empty)
 *              - 'boot_path_dbg'=usb://virtual/root
 *              - given filename=path/to/my.file
 *              .
 *              Resulting path is: usb://virtual/root/path/to/my.file
 *
 *          EXAMPLE: File with path, running semihosted from debugger (extended path semantics)
 *              - 'serverip'=ignored
 *              - 'boot_path'= (empty)
 *              - 'boot_path_dbg'=semi://../../new/virtual/root (relative to .axd file)
 *              - given filename=path/to/my.file
 *              .
 *              Resulting path is: semi://../../new/virtual/root/path/to/my.file \n
 *              AXD semihosting path is:  ../../new/virtual/root/path/to/my.file
 */
/*****************************************************************************/
//!\ingroup OS_FILE_NAMES
typedef CHAR* OS_szFileName;

/*****************************************************************************/
/*!
 *  \BRIEF  Anonymous handle to OS layer files.
 */
/*****************************************************************************/
typedef struct _OS_File* OS_hFile;

/*****************************************************************************/
/*!
 *  Some predefined file handles.
*/
/*****************************************************************************/
//!@defgroup OS_FILE_STDHANDLES OS layer predefined stdio file handles
//!@{
extern OS_hFile OS_STDIN ; //!< Predefined file handle for standard input.
extern OS_hFile OS_STDOUT; //!< Predefined file handle for standard output.
extern OS_hFile OS_STDERR; //!< Predefined file handle for standard error output.
#ifdef OS_HWDEP
extern OS_hFile OS_STDLCD; //!< Predefined file handle for LCD output. Only available with OS_HWDEP (\REF OS_HWDEP).
extern OS_hFile OS_STDKEY; //!< Predefined file handle for Keypad input. Only available with OS_HWDEP (\REF OS_HWDEP).
#endif // OS_HWDEP
//!@} defgroup OS_FILE_STDHANDLES

/*****************************************************************************/
/*
 *          OS_FileOpen()
 *//*!
 *  \BRIEF  Open a file.
 *
 *          Open a file using the given drive/path/name and access mode.
 *
 *  \NOTE   Drive \REF OS_DRIVE_TFTP will be redirected to \REF OS_DRIVE_TRX
 *          to achieve compatibility with bootloader when booting via TFTP.
 *          See \REF OS_FileLoad() or \REF OS_FileSave() if native TFTP access
 *          is required.
 *
 *  \PARAM  phFile              Reference to a file handle. On success a valid
 *                              handle will be stored there, otherwise NULL.
 *  \PARAM  szFileName          Zero terminated string specifying the file to
 *                              be opended including its path.
 *                              See \REF OS_FILE_NAMES for details on file name
 *                              conventions.
 *  \PARAM  AccessMethod        Specifies how the file is going to be accessed
 *                              and whether the file must already exist or if
 *                              a new file should be created.
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               File successfully opened.
 *  \RETVAL OS_ERROR            File could not be opened, e.g. doesn't exist but
 *                              AccessMethod was e.g. OS_FILE_ACCESS_READ_EXISTING
 *  \RETVAL OS_INVALID_PARAM    An invalid file handle reference or file name or
 *                              access method was given.
 *  \RETVAL OS_OPERATION_FAILED Opening the file failed due to internal error,
 *                              e.g. TFTP or TRIXI server not present or
 *                              communication timeout, usb device not ready or
 *                              not connected.
 */
/*****************************************************************************/
extern OS_STATUS OS_FileOpen (
    OUT OS_hFile            *phFile,
    IN  OS_szFileName       szFileName,
    IN  OS_FILE_ACCESS      AccessMethod
);

/*****************************************************************************/
/*
 *          OS_FileClose()
 *//*!
 *  \BRIEF  Close a file.
 *
 *          Close a file referred to by the given file handle.
 *
 *  \PARAM  phFile              Reference to a file handle. The handle will
 *                              get set to NULL once the file is closed and
 *                              any internal ressources are freed.
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               File successfully closed.
 *  \RETVAL OS_INVALID_PARAM    An invalid file handle reference was given.
 *  \RETVAL OS_OPERATION_FAILED Closing the file failed due to internal error,
 *                              e.g. TFTP or TRIXI server not present anymore or
 *                              communication timeout, usb device not ready or
 *                              not connected.
 */
/*****************************************************************************/
extern OS_STATUS OS_FileClose (
    IN  OS_hFile    *phFile
);

/*****************************************************************************/
/*
 *          OS_FileRead()
 *//*!
 *  \BRIEF  Read from a file.
 *
 *          Read given number of bytes from the file referred to by the given
 *          file handle.
 *
 *  \PARAM  hFile               File handle.
 *  \PARAM  pBuffer             Pointer to buffer to store the data into.
 *  \PARAM  u32ReadCount        Number of bytes to read.
 *  \PARAM  pu32ReadCount       Pointer to variable to store the number of bytes
 *                              that have actually been read.
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Data successfully read. Doesn't imply that
 *                              *pu32ReadCount == u32ReadCount holds true, but
 *                              at least one byte was read.
 *  \RETVAL OS_ERROR            File could not be read, e.g. EOF reached.
 *  \RETVAL OS_INVALID_PARAM    An invalid file handle was given.
 *  \RETVAL OS_OPERATION_FAILED Reading from the file failed due to internal error,
 *                              e.g. TFTP or TRIXI server not present anymore or
 *                              communication timeout, usb device not ready or
 *                              not connected anymore.
 */
/*****************************************************************************/
extern OS_STATUS OS_FileRead (
    IN  OS_hFile    hFile,
    IN  void        *pBuffer,
    IN  UINT32      u32ReadCount,
    OUT UINT32      *pu32ReadCount
);

/*****************************************************************************/
/*
 *          OS_FileWrite()
 *//*!
 *  \BRIEF  Write to a file.
 *
 *          Write given number of bytes to the file referred to by the given
 *          file handle.
 *
 *  \PARAM  hFile               File handle.
 *  \PARAM  pBuffer             Pointer to buffer holding the data to be written.
 *  \PARAM  u32WriteCount       Number of bytes to write.
 *  \PARAM  pu32WrittenCount    Pointer to variable to store the number of bytes
 *                              that have actually been written.
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Data successfully written. Implies that
 *                              *pu32WrittenCount == u32WriteCount holds true.
 *  \RETVAL OS_ERROR            File could not be written, e.g. drive full.
 *  \RETVAL OS_INVALID_PARAM    An invalid file handle was given.
 *  \RETVAL OS_OPERATION_FAILED Writing to the file failed due to internal error,
 *                              e.g. TFTP or TRIXI server not present anymore or
 *                              communication timeout, usb device not ready or
 *                              not connected anymore.
 */
/*****************************************************************************/
extern OS_STATUS OS_FileWrite (
    IN  OS_hFile    hFile,
    IN  void        *pBuffer,
    IN  UINT32      u32WriteCount,
    OUT UINT32      *pu32WrittenCount
);

/*****************************************************************************/
/*
 *          OS_FileFlush()
 *//*!
 *  \BRIEF  Flush internally buffered write data to a file.
 *
 *          Writes any internally buffered write data to the file referred to
 *          by the given file handle.
 *
 *  \PARAM  hFile               File handle.
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Buffered write data successfully written to storage.
 *  \RETVAL OS_ERROR            Buffered data could not be written, e.g. drive full.
 *  \RETVAL OS_INVALID_PARAM    An invalid file handle was given.
 *  \RETVAL OS_OPERATION_FAILED Writing to the file failed due to internal error,
 *                              e.g. TFTP or TRIXI server not present anymore or
 *                              communication timeout, usb device not ready or
 *                              not connected anymore.
 */
/*****************************************************************************/
extern OS_STATUS OS_FileFlush (
    IN  OS_hFile    hFile
);

/*****************************************************************************/
/*
 *          OS_FileSeek()
 *//*!
 *  \BRIEF  Seek in a file.
 *
 *          Seek to a given position in the file referred to by the given
 *          file handle.
 *
 *  \PARAM  hFile               File handle.
 *  \PARAM  u32NewFilePosition  Position to seek to.
 *                              See \REF OS_FILE_SEEK_POSITION as well.
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Successfully seek to given position.
 *  \RETVAL OS_ERROR            File could not be seeked, e.g. file open for
 *                              reading and EOF reached or file is a TTY.
 *  \RETVAL OS_INVALID_PARAM    An invalid file handle was given.
 *  \RETVAL OS_OPERATION_FAILED Seeking the file failed due to internal error,
 *                              e.g. TFTP or TRIXI server not present anymore or
 *                              communication timeout, usb device not ready or
 *                              not connected anymore.
 */
/*****************************************************************************/
extern OS_STATUS OS_FileSeek (
    IN  OS_hFile    hFile,
    IN  UINT32      u32NewFilePosition
);

/*****************************************************************************/
/*
 *          OS_FileTell()
 *//*!
 *  \BRIEF  Get current read/write position in a file.
 *
 *          Return the current read/write position in the file referred to
 *          by the given file handle.
 *
 *  \PARAM  hFile               File handle.
 *  \PARAM  pu32CurFilePosition Pointer to variable to store the current file
 *                              position in. Only valid on success.
 *                              \Note The real byte position is returned, no
 *                              matching to any of \REF OS_FILE_SEEK_POSITION is
 *                              done!
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Successfully returned the current position.
 *  \RETVAL OS_ERROR            File position could not be retrieved, e.g. file
 *                              open for reading+appending or file is a TTY.
 *  \RETVAL OS_INVALID_PARAM    An invalid file handle was given.
 *  \RETVAL OS_OPERATION_FAILED Retrieving the file position failed due to internal
 *                              error, e.g. TFTP or TRIXI server not present anymore
 *                              or communication timeout, usb device not ready or
 *                              not connected anymore.
 */
/*****************************************************************************/
extern OS_STATUS OS_FileTell (
    IN  OS_hFile    hFile,
    OUT UINT32      *pu32CurFilePosition
);

/*****************************************************************************/
/*
 *          OS_FileEOF()
 *//*!
 *  \BRIEF  Check if EOF was reached.
 *
 *          Return whether EOF in the file referred to by the given file handle
 *          was reached or not.
 *
 *  \PARAM  hFile               File handle.
 *  \PARAM  pbIsEOF             Pointer to variable to store if EOF was reached
 *                              in the current file or not. Only valid on success.
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Successfully returned the EOF state.
 *  \RETVAL OS_ERROR            EOF could not be checked, e.g. file open for
 *                              writing only or file is a TTY.
 *  \RETVAL OS_INVALID_PARAM    An invalid file handle was given.
 *  \RETVAL OS_OPERATION_FAILED Checking the EOF state of the file failed due to
 *                              internal error, e.g. TFTP or TRIXI server not
 *                              present anymore or communication timeout, usb
 *                              device not ready or not connected anymore.
 */
/*****************************************************************************/
extern OS_STATUS OS_FileEOF (
    IN  OS_hFile    hFile,
    OUT BOOL        *pbIsEOF
);

/*****************************************************************************/
/*
 *          OS_FileLoad()
 *//*!
 *  \BRIEF  Load a file.
 *
 *          Load a file at once using the given drive/path/name.
 *
 *  \NOTE   Supports \REF OS_DRIVE_TFTP as well.
 *
 *  \PARAM  szFileName          Zero terminated string specifying the file to
 *                              be opended including its path.
 *                              See \REF OS_FILE_NAMES for details on file name
 *                              conventions. AccessMethod for non-TFTP drives
 *                              is \REF OS_FILE_ACCESS_READ_EXISTING.
 *  \PARAM  pBuffer             Pointer to buffer to load the file data into.
 *  \PARAM  u32BufferSize       Buffer size in bytes.
 *  \PARAM  pu32ReadCount       Pointer to variable to store the number of bytes
 *                              that have actually been read into the buffer.
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               File successfully loaded.
 *  \RETVAL OS_ERROR            File could not be opened or read, e.g. doesn't
 *                              exist or provided buffer was to small. Depending
 *                              on the drive the buffer size may be checked in
 *                              advance or the buffer may be filled until the
 *                              end is reached.
 *  \RETVAL OS_INVALID_PARAM    An invalid file name or buffer size was given.
 *  \RETVAL OS_OPERATION_FAILED Opening the file failed due to internal error,
 *                              e.g. TFTP or TRIXI server not present or
 *                              communication timeout, usb device not ready or
 *                              not connected.
 */
/*****************************************************************************/
extern OS_STATUS OS_FileLoad (
    IN  OS_szFileName   szFileName,
    IN  void            *pBuffer,
    IN  UINT32          u32BufferSize,
    OUT UINT32          *pu32ReadCount
);

/*****************************************************************************/
/*
 *          OS_FileSave()
 *//*!
 *  \BRIEF  Save a file.
 *
 *          Save a file at once using the given drive/path/name.
 *
 *  \NOTE   Supports \REF OS_DRIVE_TFTP as well.
 *
 *  \PARAM  szFileName          Zero terminated string specifying the file to
 *                              be opended including its path.
 *                              See \REF OS_FILE_NAMES for details on file name
 *                              conventions. AccessMethod for non-TFTP drives
 *                              is \REF OS_FILE_ACCESS_WRITE_CREATENEW.
 *  \PARAM  pBuffer             Pointer to buffer holding the file data to be saved.
 *  \PARAM  u32BufferSize       Number of bytes to write.
 *  \PARAM  pu32WrittenCount    Pointer to variable to store the number of bytes
 *                              that have actually been written.
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               File successfully saved.
 *  \RETVAL OS_ERROR            File could not be opened or written, e.g. couldn't
 *                              be created or drive is full.
 *  \RETVAL OS_INVALID_PARAM    An invalid file name or buffer size was given.
 *  \RETVAL OS_OPERATION_FAILED Opening the file failed due to internal error,
 *                              e.g. TFTP or TRIXI server not present or
 *                              communication timeout, usb device not ready or
 *                              not connected.
 */
/*****************************************************************************/
extern OS_STATUS OS_FileSave (
    IN  OS_szFileName   szFileName,
    IN  void            *pBuffer,
    IN  UINT32          u32BufferSize,
    OUT UINT32          *pu32WrittenCount
);

/*****************************************************************************/
/*
 *          OS_Printf()
 *//*!
 *  \BRIEF  OS layer interface to libc's printf.
 *
 *          Internally uses \REF OS_VFPrintf() to print formatted data to
 *          standard output.
 *
 *  \NOTE   See \REF OS_VFPrintf().
 *
 *  \PARAM  szFormat            Pointer to a zero terminated format string.
 *  \PARAM  ...                 Optional data arguments as specified by the
 *                              format string.
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL                     See \REF OS_VFPrintf().
 */
/*****************************************************************************/
extern OS_STATUS OS_Printf (
    IN  const CHAR  *szFormat,
    IN  ...
);

/*****************************************************************************/
/*
 *          OS_FPrintf()
 *//*!
 *  \BRIEF  OS layer interface to libc's fprintf.
 *
 *          Internally uses \REF OS_VFPrintf() to print formatted data to
 *          the given file.
 *
 *  \NOTE   See \REF OS_VFPrintf().
 *
 *  \PARAM  hFile               Handle to OS layer file to be used for output.
 *  \PARAM  szFormat            Pointer to a zero terminated format string.
 *  \PARAM  ...                 Optional data arguments as specified by the
 *                              format string.
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL                     See \REF OS_VFPrintf().
 */
/*****************************************************************************/
extern OS_STATUS OS_FPrintf (
    IN  OS_hFile    hFile,
    IN  const CHAR  *szFormat,
    IN  ...
);

/*****************************************************************************/
/*
 *          OS_VPrintf()
 *//*!
 *  \BRIEF  OS layer interface to libc's vprintf.
 *
 *          Internally uses OS_VFPrintf() to print formatted data to the given file.
 *
 *  \NOTE   See \REF OS_VFPrintf().
 *
 *  \PARAM  szFormat            Pointer to a zero terminated format string.
 *  \PARAM  vargs               Variable list of arguments as defined in stdarg.h.
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Data successfully printed into given file.
 *  \RETVAL OS_ERROR            Data couldn't be printed into given file.
 *  \RETVAL OS_ILLEGAL_PARAM    Illegal or missing format string.
 */
/*****************************************************************************/
extern OS_STATUS OS_VPrintf (
    IN  const CHAR  *szFormat,
    IN  va_list     vargs
);

/*****************************************************************************/
/*
 *          OS_VFPrintf()
 *//*!
 *  \BRIEF  OS layer interface to libc's vfprintf.
 *
 *          Internally uses OS_VFPrintf() to print formatted data to the given file.
 *
 *  \NOTE   As the underlying libc's vfprintf is used the supported format
 *          specifiers and other implementation specific detail may depend
 *          on the underlying libc. For compatibility reasons it is strongly
 *          recommended to stick to common format specifiers and avoid any
 *          fancy stuff your libc may support. \n
 *          In the future it may make sense to implement a generic version of
 *          vfprintf() to avoid these caveats, for now we have to live with it!\n
 *          The FreeRTOS implementation uses OSprv_vfprintf() instead, which
 *          in turn uses vsnprintf() along with a static buffer to finally
 *          write the output to the DBGUnit to avoid the shortcomings of semihosting
 *          using SWIs with FreeRTOS scheduler using SWIs for task switching.
 *
 *  \PARAM  hFile               Handle to OS layer file to be used for output.
 *  \PARAM  szFormat            Pointer to a zero terminated format string.
 *  \PARAM  vargs               Variable list of arguments as defined in stdarg.h.
 *
 *  \RETURN                     Status of operation.
 *  \RETVAL OS_OK               Data successfully printed into given file.
 *  \RETVAL OS_ERROR            Data couldn't be printed into given file.
 *  \RETVAL OS_ILLEGAL_PARAM    Illegal or missing format string.
 */
/*****************************************************************************/
extern OS_STATUS OS_VFPrintf (
    IN  OS_hFile    hFile,
    IN  const CHAR  *szFormat,
    IN  va_list     vargs
);

#else // OS_FILE

/*****************************************************************************/
/*
 *  Empty stubs to allow building an application that uses OS_Printf() family
 *  with OS_FILE and thus print output effectively turned off.
 */
/*****************************************************************************/
#define OS_Printf(...)
#define OS_FPrintf(...)
#define OS_VPrintf(...)
#define OS_VFPrintf(...)

#endif // OS_FILE
//!@} defgroup OS_FILE

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
//!@defgroup OS_DEBUG OS layer debug/assert
//!@{
#ifdef OS_DEBUG

/*****************************************************************************/
/*!
 *  \BRIEF  OS layer debug facility control.
 *
 *          The OS layer debug facility control enums can be split into two
 *          sub groups:
 *          - the ones controlling the level and
 *          - the ones controlling the modules.
 *
 *          For debug output either one of the level controls or one of the level
 *          controls combined (ORed together) with one of the module controls
 *          has to be passed as parameter to the OS_DebugOut() family of functions.
 *          While the first variant produces output solely depending on the
 *          configured debug level, the second variant requires the selected
 *          module to be activated for debug output as well. \n
 *          Examples:
 *          - OS_DebugOut(DBG_INFO, "Message printed depending on current debug level");
 *          - OS_DebugOut(DBG_INFO|DBG_MODULE_6, "Message printed depending on current debug level & module");
 *
 *          For debug control either one of the level controls or one (or multiple)
 *          of the module controls (combined (ORed together)) has (/have) to be
 *          passed to the OS_DebugSetXxx() family of functions, depending on what
 *          subtype of facility control the function expects. \n
 *          Examples:
 *          - OS_DebugSetLevel(DBG_INFO);
 *          - OS_DebugSetModules(DBG_MODULE_6);
 *          - OS_DebugSetModules(DBG_MODULE_6|DBG_MODULE_12);
 *
 *  \NOTE   For user convenience application specific named module controls
 *          should be used instead of these generic named ones.
 *          Example my_dbg.h:\n
 *          <pre>
 *          \#define DBG_MAIN_THREAD         DBG_MODULE_1
 *          \#define DBG_CMD_THREAD          DBG_MODULE_2
 *          \#define DBG_NETCOM_THREAD       DBG_MODULE_3
 *          </pre>
 */
/*****************************************************************************/
typedef enum _OS_DEBUG_FACILITY {
    // level
    DBG_DEBUG_MAX     = (int)0x00000000UL,
    DBG_DEBUG_MORE    = (int)0x00000001UL,
    DBG_DEBUG_MIN     = (int)0x00000002UL,
    DBG_INFO          = (int)0x00000003UL,
    DBG_WARNING       = (int)0x00000004UL,
    DBG_ERROR         = (int)0x00000005UL,
    DBG_FATAL         = (int)0x00000006UL,
    DBG_ALWAYS        = (int)0x00000007UL,
    DBG_MODE_MASK     = (int)0x00000007UL,
    // modules
    DBG_MODULE_OSTEST = (int)0x00000040UL, // for OS layer test debugging
    DBG_MODULE_OS     = (int)0x00000080UL, // for OS layer debugging
    DBG_MODULE_1      = (int)0x00000100UL,
    DBG_MODULE_2      = (int)0x00000200UL,
    DBG_MODULE_3      = (int)0x00000400UL,
    DBG_MODULE_4      = (int)0x00000800UL,
    DBG_MODULE_5      = (int)0x00001000UL,
    DBG_MODULE_6      = (int)0x00002000UL,
    DBG_MODULE_7      = (int)0x00004000UL,
    DBG_MODULE_8      = (int)0x00008000UL,
    DBG_MODULE_9      = (int)0x00010000UL,
    DBG_MODULE_10     = (int)0x00020000UL,
    DBG_MODULE_11     = (int)0x00040000UL,
    DBG_MODULE_12     = (int)0x00080000UL,
    DBG_MODULE_13     = (int)0x00100000UL,
    DBG_MODULE_14     = (int)0x00200000UL,
    DBG_MODULE_15     = (int)0x00400000UL,
    DBG_MODULE_16     = (int)0x00800000UL,
    DBG_MODULE_17     = (int)0x01000000UL,
    DBG_MODULE_18     = (int)0x02000000UL,
    DBG_MODULE_19     = (int)0x04000000UL,
    DBG_MODULE_20     = (int)0x08000000UL,
    DBG_MODULE_21     = (int)0x10000000UL,
    DBG_MODULE_22     = (int)0x20000000UL,
    DBG_MODULE_23     = (int)0x40000000UL,
    DBG_MODULE_24     = (int)0x80000000UL,
    DBG_MODULE_ALL    = (int)0xffffff00UL, // all except OS layer modules
    DBG_MODULE_MASK   = (int)0xfffffff0UL
} OS_DEBUG_FACILITY;

/*****************************************************************************/
/*
 *          OS_Assert()
 *//*!
 *  \BRIEF  OS layer assertion checker.
 *
 *          Internally uses \REF OS_DebugOut() to print formatted data if
 *          assertion fails.
 *
 *  \NOTE   Do not use directly but via \REF OS_ASSERT macro to be able to
 *          compile with OS_DEBUG turned off.
 *
 *  \PARAM  bCondition          If FALSE, the assertion has failed and thus
 *                              the message is printed and execution stopped.
 *  \PARAM  szMessage           Message to be printed on assertion failure.
 *  \PARAM  szFile              File name, used in printout.
 *  \PARAM  u32Line             Line number, used in printout.
 *  \PARAM  szFunction          Function name, used in printout.
 */
/*****************************************************************************/
void OS_Assert(
    IN  BOOL    bCondition,
    IN  CHAR    *szMessage,
    IN  CHAR    *szFile,
    IN  UINT32  u32Line,
    IN  CHAR    *szFunction
);

/*****************************************************************************/
/*
 *          OS_DebugOut()
 *//*!
 *  \BRIEF  OS layer debug output.
 *
 *          Prints formatted debug data if the given debug level is at or above
 *          the currently configured one. If a module was given as well, that
 *          module must be currently configured for debug output, too.
 *          Internally uses \REF OS_FPrintf() etc to print formatted data.
 *
 *  \NOTE   Do not use directly but via \REF OS_DBGOUT macro to be able to
 *          compile with OS_DEBUG turned off.
 *
 *  \PARAM  Facility            Specifies level of information and module at once.
 *                              If no module is specified only the level will be
 *                              evaluated.
 *  \PARAM  szFormat            Format string of message to be printed.
 *  \PARAM  ...                 Optional params according to format specified.
 */
/*****************************************************************************/
void OS_DebugOut(
    IN  OS_DEBUG_FACILITY   Facility,
    IN  const CHAR          *szFormat,
    IN  ...
);

/*****************************************************************************/
/*
 *          OS_DebugVOut()
 *//*!
 *  \BRIEF  OS layer debug output.
 *
 *          Prints formatted debug data if the given debug level is at or above
 *          the currently configured one. If a module was given as well, that
 *          module must be currently configured for debug output, too.
 *          Internally uses \REF OS_VFPrintf() etc to print formatted data.
 *
 *  \NOTE   Do not use directly but via \REF OS_DBGVOUT macro to be able to
 *          compile with OS_DEBUG turned off.
 *
 *  \PARAM  Facility            Specifies level of information and module at once.
 *                              If no module is specified only the level will be
 *                              evaluated.
 *  \PARAM  szFormat            Format string of message to be printed.
 *  \PARAM  vargs               Variable list of arguments as defined in stdarg.h.
 */
/*****************************************************************************/
void OS_DebugVOut(
    IN  OS_DEBUG_FACILITY   Facility,
    IN  const CHAR          *szFormat,
    IN  va_list             vargs
);

/*****************************************************************************/
/*
 *          OS_DebugSetLevel()
 *//*!
 *  \BRIEF  Set new OS layer debug level.
 *
 *  \NOTE   Do not use directly but via \REF OS_DBG_SET_LEVEL macro to be
 *          able to compile with OS_DEBUG turned off.
 *
 *  \PARAM  FacilityLevel       Specifies the new minimum level for debug
 *                              information to get printed.
 *
 *  \RETURN                     Previous value of debug level.
 */
/*****************************************************************************/
OS_DEBUG_FACILITY OS_DebugSetLevel(
    IN  OS_DEBUG_FACILITY   FacilityLevel
);

/*****************************************************************************/
/*
 *          OS_DebugSetModules()
 *//*!
 *  \BRIEF  Set new OS layer module debug mask.
 *
 *  \NOTE   Do not use directly but via \REF OS_DBG_SET_MODULES macro to
 *          be able to compile with OS_DEBUG turned off.
 *
 *  \PARAM  FacilityModules     Specifies the new set of modules whose debug
 *                              information gets printed if the minimum debug
 *                              level is met.
 *
 *  \RETURN                     Previous module debug mask.
 */
/*****************************************************************************/
OS_DEBUG_FACILITY OS_DebugSetModules(
    IN  OS_DEBUG_FACILITY   FacilityModules
);

/*****************************************************************************/
/*
 *          OS_DebugSetModulesOn()
 *//*!
 *  \BRIEF  Turn on debugging information printout for a given set of modules.
 *
 *  \NOTE   Do not use directly but via \REF OS_DBG_SET_MODULES macro to
 *          be able to compile with OS_DEBUG turned off.
 *
 *  \PARAM  FacilityModules     Specifies a further set of modules whose debug
 *                              information should get printed from now on.
 *
 *  \RETURN                     Previous module debug mask.
 */
/*****************************************************************************/
OS_DEBUG_FACILITY OS_DebugSetModulesOn(
    IN  OS_DEBUG_FACILITY   FacilityModules
);

/*****************************************************************************/
/*
 *          OS_DebugSetModulesOff()
 *//*!
 *  \BRIEF  Turn off debugging information printout for a given set of modules.
 *
 *  \NOTE   Do not use directly but via \REF OS_DBG_SET_MODULES macro to
 *          be able to compile with OS_DEBUG turned off.
 *
 *  \PARAM  FacilityModules     Specifies the set of modules whose debug
 *                              information shouldn't get printed anymore.
 *
 *  \RETURN                     Previous module debug mask.
 */
/*****************************************************************************/
OS_DEBUG_FACILITY OS_DebugSetModulesOff(
    IN  OS_DEBUG_FACILITY   FacilityModules
);

/*****************************************************************************/
/*
 *          OS_DebugSetFile()
 *//*!
 *  \BRIEF  Set new OS layer debug output file handle to send debug output
 *          to a user selected file instead of the default OS_STDOUT.
 *
 *  \NOTE   Do not use directly but via \REF OS_DBG_SET_FILE macro to be able
 *          to compile with OS_DEBUG turned off.
 *
 *  \PARAM  hNewDebugFile       Handle to open, writeable file to be used for debug
 *                              output. Specifying NULL restores default OS_STDOUT.
 *
 *  \RETURN                     Old debug output file handle.
 */
/*****************************************************************************/
OS_hFile OS_DebugSetFile(
    IN  OS_hFile            hNewDebugFile
);

// forward to implementations
#define OS_ASSERT(condition)                OS_Assert((condition), #condition, __FILE__, __LINE__, __FUNCTION__)
#define OS_DBGOUT(facility, ...)            OS_DebugOut((OS_DEBUG_FACILITY)(facility), ##__VA_ARGS__)
#define OS_DBGVOUT(facility, format, vargs) OS_DebugVOut((OS_DEBUG_FACILITY)(facility), format, vargs)
#define OS_DBG_SET_LEVEL(level)             OS_DebugSetLevel(level)
#define OS_DBG_SET_MODULES(modules)         OS_DebugSetModules(modules)
#define OS_DBG_SET_MODULES_ON(modules)      OS_DebugSetModulesOn(modules)
#define OS_DBG_SET_MODULES_OFF(modules)     OS_DebugSetModulesOff(modules)
#define OS_DBG_SET_FILE(filehandle)         OS_DebugSetFile(filehandle)

#else // OS_DEBUG

// define as empty macros
#define OS_ASSERT(condition)
#define OS_DBGOUT(facility, ...)
#define OS_DBGVOUT(facility, format, vargs)
#define OS_DBG_SET_LEVEL(level)
#define OS_DBG_SET_MODULES(modules)
#define OS_DBG_SET_MODULES_ON(modules)
#define OS_DBG_SET_MODULES_OFF(modules)
#define OS_DBG_SET_FILE(filehandle)

#endif // OS_DEBUG
//!@} defgroup OS_DEBUG

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
//!@defgroup OS_MISC OS layer miscellaneous stuff
//!@{
#ifdef OS_MISC

/*****************************************************************************/
/*
 *          OS_SleepMs()
 *//*!
 *  \BRIEF  Sleep some time.
 *
 *          Sleeps for at least the specified amount of time by blocking the
 *          current thread.
 *
 *  \NOTE   There is a timing uncertainty of +[0..1]msec for the FreeRTOS based
 *          OS abstraction layer if no other thread is ready for execution. For
 *          other base OSes this might be significantly higher.
 *          Specifying 0 (zero) cooperatively yields execution if other
 *          threads are ready to execute.
 *
 *  \PARAM  u32Msec         Minimum time to sleep in msec.
 */
/*****************************************************************************/
extern void OS_SleepMs (
    IN  UINT32  u32Msec
);

/*****************************************************************************/
/*
 *          OS_SleepUs()
 *//*!
 *  \BRIEF  Sleep some time.
 *
 *          Delay for about the specified amount of time by \b waisting CPU time.
 *
 *  \NOTE   There is an unknown timing uncertainty mainly for small times due
 *          e.g. to code cache misses or IRQ load which can't be predicted nor
 *          avoided in most circumstances. The scheduler isn't suspended by
 *          this function, so the current thread may be swapped out and be
 *          resumed much later leading to unexpected delay times. If precise
 *          timing is required bewteen two actions, a critical region may help.
 *          See \REF OS_CriticalSectionBegin() and \REF OS_CriticalSectionEnd()
 *          for details.\n
 *          The valid input range is arbitrarily limited. Use the (possibly)
 *          better suited and much more cooperative \REF OS_SleepMs() function
 *          for longer delays or call this function in a loop to achieve longer
 *          delay times.
 *
 *  \PARAM  u32Usec         Time to sleep in usec. Valid range is [0..1000]us.
 */
/*****************************************************************************/
extern void OS_SleepUs (
    IN  UINT32  u32Usec
);

/*****************************************************************************/
/*
 *          OS_Malloc()
 *//*!
 *  \BRIEF  Allocate memory on the heap.
 *
 *          Allocates at least the given amount of contiguous memory on the heap.
 *          The minimum alignment of the base address is 4 bytes.
 *
 *  \NOTE   May alias to libc's malloc(). \n
 *          A size of 0 (zero) bytes may be a valid request depending on the
 *          underlying libc. In that case a value != NULL would be returned
 *          which in turn would have to be freed again using \REF OS_Free().
 *
 *  \PARAM  size            Amount of memory to allocate on the heap.
 *
 *  \RETURN                 Pointer to allocated memory. NULL on error.
 */
/*****************************************************************************/
void* OS_Malloc(
    IN  UINT32 size
);

/*****************************************************************************/
/*
 *          OS_Free()
 *//*!
 *  \BRIEF  Free previously allocated memory.
 *
 *          Free a previously allocated block of memory.
 *
 *  \NOTE   May alias to libc's free(). \n
 *
 *  \PARAM  pMem            Pointer to memory block to free on the heap.
 */
/*****************************************************************************/
extern void OS_Free (
    IN  void    *pMem
);

/*****************************************************************************/
/*
 *          OS_CriticalSectionBegin()
 *//*!
 *  \BRIEF  Marks the beginning of a critical region in the code.
 *
 *          Stops preemptive thread switching until \REF OS_CriticalSectionEnd()
 *          is called.
 *
 *  \NOTE   IRQs may still occur in a critical section and events or queues can
 *          be modified from within the IRQ handlers, but no thread switching
 *          will occur until the critical section ends.
 */
/*****************************************************************************/
void OS_CriticalSectionBegin ( void );

/*****************************************************************************/
/*
 *          OS_CriticalSectionEnd()
 *//*!
 *  \BRIEF  Marks the end of a critical region in the code.
 *
 *          Reenables preemptive thread switching after it was disbaled by
 *          calling \REF OS_CriticalSectionBegin().
 *
 *  \NOTE   See \REF OS_CriticalSectionBegin().
 */
/*****************************************************************************/
void OS_CriticalSectionEnd ( void );

/*****************************************************************************/
/*
 *          OS_CriticalSectionIRQBegin()
 *//*!
 *  \BRIEF  Marks the beginning of a critical region in the code.
 *
 *          Stops preemptive thread switching and disables IRQs as well
 *          until \REF OS_CriticalSectionIRQEnd() is called.
 */
/*****************************************************************************/
void OS_CriticalSectionIRQBegin ( void );

/*****************************************************************************/
/*
 *          OS_CriticalSectionIRQEnd()
 *//*!
 *  \BRIEF  Marks the end of a critical region in the code.
 *
 *          Reenables preemptive thread switching and IRQs as well after they
 *          were disbaled by calling \REF OS_CriticalSectionIRQBegin().
 */
/*****************************************************************************/
void OS_CriticalSectionIRQEnd ( void );

/*****************************************************************************/
/*
 *          OS_GetTick()
 *//*!
 *  \BRIEF  Return the system time.
 *
 *          Return the current system tick counter value. The offset to
 *          whatever point in time is unspecified.
 *
 *  \NOTE   The resolution of the system tick can be retrieved via
 *          \REF OS_GetFrequency(). To convert e.g. a system time difference
 *          into seconds, divide that difference by the value returned from
 *          \REF OS_GetFrequency().
 *
 *  \RETURN                 Current system tick counter value.
 */
/*****************************************************************************/
extern UINT32 OS_GetTick ( void );

/*****************************************************************************/
/*
 *          OS_GetFrequency()
 *//*!
 *  \BRIEF  Return the system time resolution in [Hz].
 *
 *          Return the resolution of the system tick counter.
 *
 *  \NOTE   See \REF OS_GetTick().
 *
 *  \RETURN                 System tick counter resolution in [Hz].
 */
/*****************************************************************************/
extern UINT32 OS_GetFrequency ( void );

/*****************************************************************************/
/*
 *          OS_GetNow()
 *//*!
 *  \BRIEF  Return the system time since start in [us].
 *
 *          Return the system time since start in high resolution of 1 usec.
 *
 *  \RETURN                 Current system time in [us].
 */
/*****************************************************************************/
extern UINT32 OS_GetNow ( void );

#endif // OS_MISC
//!@} defgroup OS_MISC

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
//!@defgroup OS_HWDEP OS layer non portable, Trillian hardware dependent stuff
//!@{
#ifdef OS_HWDEP

/*****************************************************************************/
/*
 *          OS_KeypadPressed()
 *//*!
 *  \BRIEF  Return if a key was pressed on the keypad.
 *
 *  \RETURN                 TRUE if a key or pen action can be read via
 *                          \REF OS_KeypadGetKey(), FALSE otherwise.
 *
 *          <b>This function is a NOT PORTABLE part of the OS layer! Use only for
 *          demonstration/validation software to be run on Trillian HW!</b>
 */
/*****************************************************************************/
extern BOOL OS_KeypadPressed ( void );

/*****************************************************************************/
/*
 *          OS_KeypadGetKey()
 *//*!
 *  \BRIEF  Waits for a key being pressed on the keypad and returns it.
 *
 *  \RETURN                 Pressed key ('0'..'9','#','*') or pen action
 *                          (\REF OS_PenDown / \REF OS_PenMoved for new/next
 *                          coordinates available). Blocks until a key or pen
 *                          action is available. Use \REF OS_KeypadGetPenCoord()
 *                          to read pen coordinates.
 *
 *          <b>This function is a NOT PORTABLE part of the OS layer! Use only for
 *          demonstration/validation software to be run on Trillian HW!</b>
 */
/*****************************************************************************/
extern CHAR OS_KeypadGetKey ( void );

/*****************************************************************************/
/*
 *          OS_KeypadGetPenCoord()
 *//*!
 *  \BRIEF  Waits for a key being pressed on the keypad and returns it.
 *
 *  \PARAM  plX, plY        Pointer to variables to store the pen coordinates in.
 *
 *  \RETURN                 New/next pen coordinates. Coordiantes are already
 *                          translated into display coordinates. Call only after
 *                          \REF OS_PenDown or \REF OS_PenMoved was received via
 *                          \REF OS_KeypadGetKey() before.
 *
 *          <b>This function is a NOT PORTABLE part of the OS layer! Use only for
 *          demonstration/validation software to be run on Trillian HW!</b>
 */
/*****************************************************************************/
extern void OS_KeypadGetPenCoord ( INT32 *plX, INT32 *plY );

/*****************************************************************************/
/*!
 *  \DESC   Special key value to indicate that new pen coordinates available.
 */
/*****************************************************************************/
#define OS_PenDown    0xfd //!< New pen coordinates available.

/*****************************************************************************/
/*!
 *  \DESC   Special key value to indicate that moved pen coordinates available.
 */
/*****************************************************************************/
#define OS_PenMoved    0xfe //!< Next pen coordinates available.

#endif // OS_HWDEP
//!@} defgroup OS_HWDEP

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
//!@} defgroup OS_LAYER

#ifdef __cplusplus
}
#endif

#endif // _OS_H
