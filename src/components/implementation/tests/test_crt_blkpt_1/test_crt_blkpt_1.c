#include <cos_component.h>
#include <cos_defkernel_api.h>
#include <llprint.h>
#include <sl.h>

#include <crt_lock.h>
#include <crt_chan.h>
#include <tinytest.h>
#include <assert.h>


struct cos_compinfo *ci;

int progress_shared = 0;

struct sl_thd *blk_thds1[2] = {
  NULL,
};

struct crt_blkpt blkpt_test_1;

void
blk_thd1(void *d)
{
    struct crt_blkpt_checkpoint chkpt;

    /* set up a checkoint */
    crt_blkpt_checkpoint(&blkpt_test_1, &chkpt);

    crt_blkpt_trigger(&blkpt_test_1, 0);

    crt_blkpt_wait(&blkpt_test_1, 0, &chkpt);

    progress_shared++;

    sl_thd_yield(sl_thd_thdid(blk_thds1[1]));

    while(1);

}

void
blk_thd2(void *d)
{
  assert(1);
  while(1);
}




void
test_blkpt1(void)
{
    int                     i;
    union sched_param_union sps[] = {{.c = {.type = SCHEDP_PRIO, .value = 6}},
                                     {.c = {.type = SCHEDP_PRIO, .value = 7}}};

    progress_shared = 0;
    crt_blkpt_init(&blkpt_test_1);
    printc("Create thread:\n");
    blk_thds1[0] = sl_thd_alloc(blk_thd1, NULL);
    blk_thds1[1] = sl_thd_alloc(blk_thd2, NULL);
    printc("\tcreating thread %d at prio %d\n", sl_thd_thdid(blk_thds1[0]), sps[0].c.value);
    printc("\tcreating thread %d at prio %d\n", sl_thd_thdid(blk_thds1[1]), sps[1].c.value);
    sl_thd_param_set(blk_thds1[0], sps[0].v);
    sl_thd_param_set(blk_thds1[1], sps[1].v);

}

void
cos_init(void)
{
    struct cos_defcompinfo *defci = cos_defcompinfo_curr_get();
    ci                            = cos_compinfo_get(defci);

    printc("Unit-test for the crt blkpt 1 (sl)\n");
    cos_meminfo_init(&(ci->mi), BOOT_MEM_KM_BASE, COS_MEM_KERN_PA_SZ, BOOT_CAPTBL_SELF_UNTYPED_PT);
    cos_defcompinfo_init();
    sl_init(SL_MIN_PERIOD_US);

    test_blkpt1();

    sl_sched_loop_nonblock();

    assert(0);

    return;

}
