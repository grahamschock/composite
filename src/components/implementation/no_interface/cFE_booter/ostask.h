#ifndef OSTASK_H
#define OSTASK_H

#include <sl.h>

#define SCHED_PERIOD_US SL_MIN_PERIOD_US

#define HZ_PAUSE_US (1000*1000) //1 sec

/* We delegate the main thread of execution to a different thread
 * (the main thread needs to run the scheduling loop)
 */
#define MAIN_DELEGATE_THREAD_PRIORITY 1
#define TIMER_THREAD_PRIORITY (MAIN_DELEGATE_THREAD_PRIORITY + 1)
#define LIBRARY_THREAD_PRIORITY (MAIN_DELEGATE_THREAD_PRIORITY + 2)
#define REBOOT_THREAD_PRIORITY (MAIN_DELEGATE_THREAD_PRIORITY + 3)

#define CFE_PSP_SENSOR_BUDGET_USEC   (10000) //10ms
#define CFE_PSP_SENSOR_INTERVAL_USEC (500*1000) //500ms
#define SENSOREMU_USE_HPET
#define CFE_PSP_SENSOR_THDPRIO 4

#define CFE_PRIO_MAXNUM 256
#define CFE_INVERT_PRIO CFE_PRIO_MAXNUM

/*
 * ThreadId overrides for apps
 */
extern thdid_t id_overrides[SL_MAX_NUM_THDS];

void OS_SchedulerStart(cos_thd_fn_t main_delegate);

#endif