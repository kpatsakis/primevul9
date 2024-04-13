static inline void finish_arch_post_lock_switch(void)
{
	struct task_struct *tsk = current;
	struct mm_struct *mm = tsk->mm;

	load_kernel_asce();
	if (mm) {
		preempt_disable();
		while (atomic_read(&mm->context.attach_count) >> 16)
			cpu_relax();

		cpumask_set_cpu(smp_processor_id(), mm_cpumask(mm));
		if (mm->context.flush_mm)
			__tlb_flush_mm(mm);
		preempt_enable();
	}
	set_fs(current->thread.mm_segment);
}