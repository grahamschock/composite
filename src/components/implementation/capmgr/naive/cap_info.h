#ifndef CAP_INFO_H
#define CAP_INFO_H

#include <consts.h>
#include <cos_kernel_api.h>
#include <cos_defkernel_api.h>
#include <sl.h>
#include <capmgr.h>
#include <memmgr.h>
#include <bitmap.h>

#define CAP_INFO_COMP_MAX_SUBTREE 8
#define CAP_INFO_COMP_MAX_THREADS (MAX_NUM_THREADS*CAP_INFO_COMP_MAX_SUBTREE)

extern u32_t cap_info_schedbmp[][MAX_NUM_COMP_WORDS];

/* shared memory region information */
struct cap_shmem_glb_info {
	cbuf_t           free_region_id; /* free global identifier */
	unsigned long    total_pages;    /* total number of pages allocated in all shared-mem regions */
	unsigned long    region_npages[MEMMGR_MAX_SHMEM_REGIONS]; /* number of pages allocated per region with array index as the shared-memory identifier */
	cos_channelkey_t region_keys[MEMMGR_MAX_SHMEM_REGIONS]; /* if key > 0, be able to map shmem region with key for a static namespace == key!! */
};

struct cap_channelaep_info {
	struct sl_thd *slaep; /* contains rcvcap, thdid, thdcap. */
	asndcap_t      sndcap[NUM_CPU] CACHE_ALIGNED; /* per-core send-cap */
} cap_channelaeps[CAPMGR_AEPKEYS_MAX];

/* per component shared memory region information */
struct cap_shmem_info {
	struct cos_compinfo *cinfo; /* points to cap_comp_info.defci.ci, to use the same frontier for shared regions */
	unsigned long        total_pages; /* track total pages alloc'ed/mapped to limit shmem usage */
	vaddr_t              shm_addr[MEMMGR_MAX_SHMEM_REGIONS]; /* virtual address mapped in the component with array index as the global shared memory identifier */
};

struct cap_comp_info {
	spdid_t cid;
	int thd_used[NUM_CPU] CACHE_ALIGNED;
	struct cos_defcompinfo defci;
	struct cap_shmem_info shminfo;
	struct sl_thd *thdinfo[NUM_CPU][CAP_INFO_COMP_MAX_THREADS] CACHE_ALIGNED; /* including threads from components in subtree. */
	int initflag;
	/* for core-specific hierarchies */
	u32_t child_bitmap[NUM_CPU][MAX_NUM_COMP_WORDS] CACHE_ALIGNED;
	u32_t child_sched_bitmap[NUM_CPU][MAX_NUM_COMP_WORDS] CACHE_ALIGNED;

	struct cap_comp_info *parent[NUM_CPU] CACHE_ALIGNED;
	int p_thd_iterator[NUM_CPU] CACHE_ALIGNED; /* iterator for parent to get all threads created by capmgr in this component so far! */
	thdcap_t p_initthdcap[NUM_CPU] CACHE_ALIGNED; /* init thread's cap in parent */
	thdid_t  initthdid[NUM_CPU] CACHE_ALIGNED; /* init thread's tid */
};

struct cap_comp_info *cap_info_comp_init(spdid_t spdid, captblcap_t captbl_cap, pgtblcap_t pgtbl_cap, compcap_t compcap,
					 capid_t cap_frontier, vaddr_t heap_frontier, spdid_t sched_spdid);

struct sl_thd *cap_info_thd_init(struct cap_comp_info *rci, struct sl_thd *t, cos_channelkey_t key);
struct sl_thd *cap_info_initthd_init(struct cap_comp_info *rci, struct sl_thd *t, cos_channelkey_t key);

struct cap_comp_info *cap_info_comp_find(spdid_t s);
struct sl_thd        *cap_info_thd_find(struct cap_comp_info *r, thdid_t t);
struct sl_thd        *cap_info_thd_next(struct cap_comp_info *r);
struct sl_thd        *cap_info_initthd(struct cap_comp_info *r);
unsigned int          cap_info_count(void);
void                  cap_info_init(void);

cbuf_t   cap_shmem_region_alloc(struct cap_shmem_info *rcur, cos_channelkey_t key, unsigned long num_pages);
cbuf_t   cap_shmem_region_map(struct cap_shmem_info *rcur, cbuf_t id, cos_channelkey_t key, unsigned long *num_pages);
vaddr_t  cap_shmem_region_vaddr(struct cap_shmem_info *rsh, cbuf_t id);
void     cap_shmem_region_vaddr_set(struct cap_shmem_info *rsh, cbuf_t id, vaddr_t addr);
cbuf_t   cap_shmem_region_find(cos_channelkey_t key);
int      cap_shmem_region_key_set(cbuf_t id, cos_channelkey_t key);

struct cap_channelaep_info *cap_info_channelaep_get(cos_channelkey_t key);
void                        cap_channelaep_set(cos_channelkey_t key, struct sl_thd *t);
asndcap_t                   cap_channelaep_asnd_get(cos_channelkey_t key);

static inline struct cos_compinfo *
cap_info_ci(struct cap_comp_info *r)
{
	return cos_compinfo_get(&(r->defci));
}

static inline struct cos_defcompinfo *
cap_info_dci(struct cap_comp_info *r)
{
	return &(r->defci);
}

static inline struct cap_comp_info *
cap_info_parent(struct cap_comp_info *r)
{
	return r->parent[cos_cpuid()];
}

static inline int
cap_info_is_parent(struct cap_comp_info *r, spdid_t p)
{
	if (cap_info_parent(r)) return (r->parent[cos_cpuid()]->cid == p);

	return 0;
}

static inline int
cap_info_is_sched(spdid_t c)
{
	if (!c) return 1; /* llbooter! */

	return bitmap_check(cap_info_schedbmp[cos_cpuid()], c - 1);
}

static inline int
cap_info_is_child(struct cap_comp_info *r, spdid_t c)
{
	if (!c) return 0;

	return bitmap_check(r->child_bitmap[cos_cpuid()], c - 1);
}

static inline int
cap_info_is_sched_child(struct cap_comp_info *r, spdid_t c)
{
	if (!c) return 0;

	return bitmap_check(r->child_sched_bitmap[cos_cpuid()], c - 1);
}

static inline struct cap_shmem_info *
cap_info_shmem_info(struct cap_comp_info *r)
{
	return &(r->shminfo);
}

static inline struct cos_compinfo *
cap_info_shmem_ci(struct cap_shmem_info *r)
{
	return r->cinfo;
}

static inline int
cap_info_init_check(struct cap_comp_info *r)
{
	return r->initflag;
}

#endif /* CAP_INFO_H */
