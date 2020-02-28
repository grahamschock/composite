#ifndef CRT_LOCK_H
#define CRT_LOCK_H

/***
 * Simple blocking lock. Uses blockpoints to enable the blocking and
 * waking of contending threads. This has little to no intelligence,
 * for example, not expressing dependencies for PI.
 */

#include <cos_component.h>
#include <crt_blkpt.h>


struct crt_sem {
  unsigned long count;
  unsigned long max_threads;
  struct crt_blkpt blkpt;
};

struct crt_lock {
    unsigned long owner;
    struct crt_blkpt blkpt;
};


static inline void
crt_sem_up(struct crt_sem *s)
{
  struct crt_blkpt_checkpoint chkpt;

  long unsigned int thd_num_cur = ps_faa(&s->count, 1);

  while(1)
    {
      crt_blkpt_checkpoint(&s->blkpt, &chkpt);

      if(s->count >= s->max_threads)
        {
          return;
        }
      crt_blkpt_wait(&s->blkpt, 0, &chkpt);
    }
}

static inline void
crt_sem_alloc(struct crt_sem *s, int size)
{

  s->max_threads = size;
  s->count = 0;
}

static inline void
crt_sem_down(struct crt_sem *s)
{
  struct crt_blkpt_checkpoint chkpt;

  long unsigned int thd_num_cur = ps_faa(&s->count, -1);

  crt_blkpt_trigger(&s->blkpt, 0);

}

static inline int
crt_lock_init(struct crt_lock *l)
{
    l->owner = 0;

    return crt_blkpt_init(&l->blkpt);
    unsigned long owner;
    struct crt_blkpt blkpt;
};

static inline int
crt_lock_init(struct crt_lock *l)
{
    l->owner = 0;

    return crt_blkpt_init(&l->blkpt);
>>>>>>> 4b2fb9044f05189207d30686c81a2293027875f9
}

static inline int
crt_lock_teardown(struct crt_lock *l)
{
<<<<<<< HEAD
    assert(l->owner == 0);

    return crt_blkpt_teardown(&l->blkpt);
=======
    assert(l->owner == 0);

    return crt_blkpt_teardown(&l->blkpt);
>>>>>>> 4b2fb9044f05189207d30686c81a2293027875f9
}

static inline void
crt_lock_take(struct crt_lock *l)
{
<<<<<<< HEAD
    struct crt_blkpt_checkpoint chkpt;

    while (1) {
        crt_blkpt_checkpoint(&l->blkpt, &chkpt);

        if (ps_cas(&l->owner, 0, (unsigned long)cos_thdid())) {
            return;	/* success! */
        }
        /* failure: try and block */
        crt_blkpt_wait(&l->blkpt, 0, &chkpt);
    }
=======
    struct crt_blkpt_checkpoint chkpt;

    while (1) {
        crt_blkpt_checkpoint(&l->blkpt, &chkpt);

        if (ps_cas(&l->owner, 0, (unsigned long)cos_thdid())) {
            return;	/* success! */
        }
        /* failure: try and block */
        crt_blkpt_wait(&l->blkpt, 0, &chkpt);
    }
>>>>>>> 4b2fb9044f05189207d30686c81a2293027875f9
}

static inline void
crt_lock_release(struct crt_lock *l)
{
<<<<<<< HEAD
    assert(l->owner == cos_thdid());
    l->owner = 0;
    /* if there are blocked threads, wake 'em up! */
    crt_blkpt_trigger(&l->blkpt, 0);
=======
    assert(l->owner == cos_thdid());
    l->owner = 0;
    /* if there are blocked threads, wake 'em up! */
    crt_blkpt_trigger(&l->blkpt, 0);
>>>>>>> 4b2fb9044f05189207d30686c81a2293027875f9
}

#endif /* CRT_LOCK_H */
