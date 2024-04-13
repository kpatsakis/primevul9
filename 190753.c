void destroy_context(struct mm_struct *mm)
{
#ifdef CONFIG_SPAPR_TCE_IOMMU
	WARN_ON_ONCE(!list_empty(&mm->context.iommu_group_mem_list));
#endif
	if (radix_enabled())
		WARN_ON(process_tb[mm->context.id].prtb0 != 0);
	else
		subpage_prot_free(mm);
	destroy_contexts(&mm->context);
	mm->context.id = MMU_NO_CONTEXT;
}