/*
 * Copyright (C) 2011 Richard Weinberger <richrd@nod.at>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <asm/page.h>
#include <asm/elf.h>
#include <linux/init.h>

static unsigned int __read_mostly vdso_enabled = 1;
unsigned long um_vdso_addr;

extern unsigned long task_size;
extern char vdso_start[], vdso_end[];

static struct page **vdsop;

static int __init init_vdso(void)
{
	struct page *um_vdso;

	BUG_ON(vdso_end - vdso_start > PAGE_SIZE);

	um_vdso_addr = task_size - PAGE_SIZE;

	vdsop = kmalloc(sizeof(struct page *), GFP_KERNEL);
	if (!vdsop)
		goto oom;

	um_vdso = alloc_page(GFP_KERNEL);
	if (!um_vdso) {
		kfree(vdsop);

		goto oom;
	}

	copy_page(page_address(um_vdso), vdso_start);
	*vdsop = um_vdso;

	return 0;

oom:
	printk(KERN_ERR "Cannot allocate vdso\n");
	vdso_enabled = 0;

	return -ENOMEM;
}
subsys_initcall(init_vdso);

int arch_setup_additional_pages(struct linux_binprm *bprm, int uses_interp)
{
	int err;
	struct mm_struct *mm = current->mm;
#ifdef CONFIG_MMSEM
	int i = 0;
	int j = 0;
#endif

	if (!vdso_enabled)
		return 0;

#ifdef CONFIG_MMSEM
	for (i = 0; i < CONFIG_MMSEM_NODE; i++) {
		if (down_write_killable(&((mm->mmap_sem_node[i]).mmap_sem))) {
			for (j = i-1; j >= 0; j--) {
				up_write(&((mm->mmap_sem_node[j]).mmap_sem));
			}
			return -EINTR;
		}
	}
#else
	if (down_write_killable(&mm->mmap_sem))
		return -EINTR;
#endif

	err = install_special_mapping(mm, um_vdso_addr, PAGE_SIZE,
		VM_READ|VM_EXEC|
		VM_MAYREAD|VM_MAYWRITE|VM_MAYEXEC,
		vdsop);

#ifdef CONFIG_MMSEM
	for (i = CONFIG_MMSEM_NODE-1; i >= 0; i--) {
		up_write(&((mm->mmap_sem_node[i]).mmap_sem));
	}
#else
	up_write(&mm->mmap_sem);
#endif

	return err;
}
