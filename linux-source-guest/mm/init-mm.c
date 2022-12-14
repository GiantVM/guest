// SPDX-License-Identifier: GPL-2.0
#include <linux/mm_types.h>
#include <linux/rbtree.h>
#include <linux/rwsem.h>
#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/cpumask.h>

#include <linux/atomic.h>
#include <linux/user_namespace.h>
#include <asm/pgtable.h>
#include <asm/mmu.h>

#ifndef INIT_MM_CONTEXT
#define INIT_MM_CONTEXT(name)
#endif

struct mm_struct init_mm = {
	.mm_rb		= RB_ROOT,
	.pgd		= swapper_pg_dir,
	.mm_users	= ATOMIC_INIT(2),
	.mm_count	= ATOMIC_INIT(1),
#ifdef CONFIG_MMSEM
	.mmap_sem_node[0].mmap_sem =  __RWSEM_INITIALIZER(init_mm.mmap_sem_node[0].mmap_sem),
	.mmap_sem_node[1].mmap_sem =  __RWSEM_INITIALIZER(init_mm.mmap_sem_node[1].mmap_sem),
	.mmap_sem_node[2].mmap_sem =  __RWSEM_INITIALIZER(init_mm.mmap_sem_node[2].mmap_sem),
	.mmap_sem_node[3].mmap_sem =  __RWSEM_INITIALIZER(init_mm.mmap_sem_node[3].mmap_sem),
	.mmap_sem_node[4].mmap_sem =  __RWSEM_INITIALIZER(init_mm.mmap_sem_node[4].mmap_sem),
	.mmap_sem_node[5].mmap_sem =  __RWSEM_INITIALIZER(init_mm.mmap_sem_node[5].mmap_sem),
	.mmap_sem_node[6].mmap_sem =  __RWSEM_INITIALIZER(init_mm.mmap_sem_node[6].mmap_sem),
	.mmap_sem_node[7].mmap_sem =  __RWSEM_INITIALIZER(init_mm.mmap_sem_node[7].mmap_sem),
#else
	.mmap_sem       = __RWSEM_INITIALIZER(init_mm.mmap_sem),
#endif
	.page_table_lock =  __SPIN_LOCK_UNLOCKED(init_mm.page_table_lock),
	.mmlist		= LIST_HEAD_INIT(init_mm.mmlist),
	.user_ns	= &init_user_ns,
	INIT_MM_CONTEXT(init_mm)
};
