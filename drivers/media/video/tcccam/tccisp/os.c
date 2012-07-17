#include "os.h"
        
void OS_SleepMs(UINT32 u32Msec)
{
	#if 0
	if ( xTaskGetSchedulerState() == taskSCHEDULER_RUNNING ) {
	    // we can use the scheduler to do the delay
	    vTaskDelay((portTickType)u32Msec/portTICK_RATE_MS);
	}
	else
	{
	    // we have to use the CPU waisting methode
	    while ( u32Msec-- )
	    {
	        OS_SleepUs(1000);
	    }
	}
	#endif
}

void OS_SleepUs(UINT32 u32Usec) //range: 0..1000us via OS_ASSERT()
{
	//OS_ASSERT( u32Usec <= 1000 );

	//LowLevelSleepUs(u32Usec);
		{
			int i;
			for (i=0; i<u32Usec*0x100; i++)	{	/*__asm { nop; }*/	}
		}
}

UINT32 OS_GetTick(void)
{
    //return xTaskGetTickCount();
	return 0;
}

UINT32 OS_GetFrequency(void)
{
    //return configTICK_RATE_HZ;
	return 200000000;	//200mhz
}

