static inline void switch_mm(struct mm_struct *prev, struct mm_struct *next,
			     struct task_struct *tsk)
{
	int cpu = smp_processor_id();

	S390_lowcore.user_asce = next->context.asce_bits | __pa(next->pgd);
	if (prev == next)
		return;
	if (MACHINE_HAS_TLB_LC)
		cpumask_set_cpu(cpu, &next->context.cpu_attach_mask);
	/* Clear old ASCE by loading the kernel ASCE. */
	__ctl_load(S390_lowcore.kernel_asce, 1, 1);
	__ctl_load(S390_lowcore.kernel_asce, 7, 7);
	atomic_inc(&next->context.attach_count);
	atomic_dec(&prev->context.attach_count);
	if (MACHINE_HAS_TLB_LC)
		cpumask_clear_cpu(cpu, &prev->context.cpu_attach_mask);
}