int init_new_context(struct task_struct *tsk, struct mm_struct *mm)
{
	int index;

	if (radix_enabled())
		index = radix__init_new_context(mm);
	else
		index = hash__init_new_context(mm);

	if (index < 0)
		return index;

	mm->context.id = index;

	mm->context.pte_frag = NULL;
	mm->context.pmd_frag = NULL;
#ifdef CONFIG_SPAPR_TCE_IOMMU
	mm_iommu_init(mm);
#endif
	atomic_set(&mm->context.active_cpus, 0);
	atomic_set(&mm->context.copros, 0);

	return 0;
}